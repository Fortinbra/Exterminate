#include "AudioController.h"
#include "audio/audio_index.h"
#include "pico/multicore.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdio.h>

namespace Exterminate {

// Static instance for audio callbacks
AudioController* AudioController::instance_ = nullptr;

AudioController::AudioController(const Config& config)
    : config_(config)
    , playbackState_(PlaybackState::Stopped)
    , volume_(1.0f)
    , audioIntensity_(0.0f)
    , bufferPool_(nullptr)
    , initialized_(false)
    , currentAudioData_(nullptr)
    , currentAudioSize_(0)
    , currentAudioPosition_(0)
{
    printf("AudioController: Created with Pico Extras I2S - data_pin=%u, clock_base=%u, sample_rate=%u\n",
           config_.dataPin, config_.clockPinBase, config_.sampleRate);
}

AudioController::~AudioController() {
    shutdown();
    instance_ = nullptr;
    printf("AudioController: Destroyed\n");
}

bool AudioController::initialize() {
    if (initialized_) {
        printf("AudioController: Already initialized\n");
        return true;
    }

    printf("AudioController: Initializing Pico Extras I2S audio system...\n");
    
    // Set static instance for callbacks
    instance_ = this;

    // Configure audio format for our embedded PCM data
    // All our audio files are 22050Hz, mono, 16-bit
    audioFormat_ = {
        .sample_freq = config_.sampleRate,
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .channel_count = 1  // Mono
    };

    printf("AudioController: Audio format - %uHz, %u channels, 16-bit PCM\n", 
           audioFormat_.sample_freq, audioFormat_.channel_count);

    // Configure I2S pins
    i2sConfig_ = {
        .data_pin = config_.dataPin,
        .clock_pin_base = config_.clockPinBase,
        .dma_channel = 0,  // Let the library choose
        .pio_sm = 0        // Let the library choose
    };

    printf("AudioController: I2S config - data_pin=%u, clock_base=%u\n",
           i2sConfig_.data_pin, i2sConfig_.clock_pin_base);

    // Create audio buffer pool - we need a producer pool that we can fill
    audio_buffer_format_t bufferFormat = {
        .format = &audioFormat_,
        .sample_stride = 2  // 16-bit = 2 bytes per sample
    };
    
    bufferPool_ = audio_new_producer_pool(&bufferFormat, config_.bufferCount, config_.samplesPerBuffer);
    if (!bufferPool_) {
        printf("AudioController: ERROR - Failed to create buffer pool\n");
        return false;
    }

    printf("AudioController: Created producer buffer pool - %u buffers, %u samples each\n",
           config_.bufferCount, config_.samplesPerBuffer);

    // Initialize I2S with the audio format
    const audio_format_t* producerFormat = audio_i2s_setup(&audioFormat_, &i2sConfig_);
    if (!producerFormat) {
        printf("AudioController: ERROR - Failed to setup I2S\n");
        // TODO: Add proper buffer pool cleanup when we find the right function
        bufferPool_ = nullptr;
        return false;
    }

    printf("AudioController: I2S setup successful\n");

    // Connect our producer pool to the I2S consumer
    bool connect_ok = audio_i2s_connect(bufferPool_);
    if (!connect_ok) {
        printf("AudioController: ERROR - Failed to connect buffer pool to I2S\n");
        // TODO: Add proper buffer pool cleanup when we find the right function
        bufferPool_ = nullptr;
        return false;
    }

    printf("AudioController: Connected producer pool to I2S consumer\n");

    // Enable I2S output
    audio_i2s_set_enabled(true);

    printf("AudioController: I2S enabled\n");

    initialized_ = true;
    printf("AudioController: Initialization complete!\n");
    return true;
}

void AudioController::shutdown() {
    if (!initialized_) {
        return;
    }

    printf("AudioController: Shutting down...\n");
    
    stopAudio();
    
    // Disable I2S
    audio_i2s_set_enabled(false);
    
    if (bufferPool_) {
        // TODO: Add proper buffer pool cleanup when we find the right function
        bufferPool_ = nullptr;
    }
    
    initialized_ = false;
    printf("AudioController: Shutdown complete\n");
}

bool AudioController::playAudio(Audio::AudioIndex audioIndex) {
    if (!initialized_) {
        printf("AudioController: ERROR - Not initialized\n");
        return false;
    }

    // Get audio file info
    const Audio::AudioFile* audioFile = Audio::getAudioFile(audioIndex);
    if (!audioFile) {
        printf("AudioController: ERROR - Invalid audio index %d\n", static_cast<int>(audioIndex));
        return false;
    }

    printf("AudioController: Playing audio file '%s' - %zu samples, %u Hz\n",
           audioFile->name, audioFile->sample_count, audioFile->sample_rate);

    // Stop any current playback
    stopAudio();

    // Set up new audio data
    currentAudioData_ = audioFile->data;
    currentAudioSize_ = audioFile->sample_count;
    currentAudioPosition_ = 0;

    // Start playback
    playbackState_ = PlaybackState::Playing;
    
    // Start the audio worker loop
    startAudioWorker();
    
    printf("AudioController: Playback started\n");
    return true;
}

void AudioController::startAudioWorker() {
    // Launch a simple audio worker on core 1
    multicore_launch_core1([]() {
        AudioController* controller = AudioController::instance_;
        if (!controller) return;
        
        printf("AudioController: Audio worker started on core 1\n");
        
        while (controller->playbackState_ != PlaybackState::Stopped) {
            // Get a buffer from the pool
            audio_buffer_t* buffer = take_audio_buffer(controller->bufferPool_, false);
            if (buffer) {
                // Fill the buffer
                controller->fillAudioBuffer(buffer);
                
                // Send it to the I2S output
                give_audio_buffer(controller->bufferPool_, buffer);
            } else {
                // No buffer available, yield
                sleep_ms(1);
            }
        }
        
        printf("AudioController: Audio worker stopped\n");
    });
}

bool AudioController::stopAudio() {
    if (playbackState_ == PlaybackState::Stopped) {
        return true;
    }

    printf("AudioController: Stopping audio playback\n");
    
    playbackState_ = PlaybackState::Stopped;
    currentAudioData_ = nullptr;
    currentAudioSize_ = 0;
    currentAudioPosition_ = 0;
    audioIntensity_ = 0.0f;
    
    return true;
}

bool AudioController::pauseAudio() {
    if (playbackState_ != PlaybackState::Playing) {
        return false;
    }

    printf("AudioController: Pausing audio playback\n");
    playbackState_ = PlaybackState::Paused;
    return true;
}

bool AudioController::resumeAudio() {
    if (playbackState_ != PlaybackState::Paused) {
        return false;
    }

    printf("AudioController: Resuming audio playback\n");
    playbackState_ = PlaybackState::Playing;
    return true;
}

void AudioController::setVolume(float volume) {
    // Clamp volume to valid range
    volume = std::max(0.0f, std::min(1.0f, volume));
    volume_ = volume;
    printf("AudioController: Volume set to %.2f\n", volume);
}

size_t AudioController::fillAudioBuffer(audio_buffer_t* buffer) {
    if (!buffer || playbackState_ != PlaybackState::Playing || !currentAudioData_) {
        // Fill with silence
        memset(buffer->buffer->bytes, 0, buffer->max_sample_count * 2);
        buffer->sample_count = buffer->max_sample_count;
        return 0;
    }

    size_t samplesRemaining = currentAudioSize_.load() - currentAudioPosition_.load();
    size_t samplesToWrite = std::min(samplesRemaining, static_cast<size_t>(buffer->max_sample_count));
    
    if (samplesToWrite == 0) {
        // End of audio reached
        playbackState_ = PlaybackState::Stopped;
        memset(buffer->buffer->bytes, 0, buffer->max_sample_count * 2);
        buffer->sample_count = buffer->max_sample_count;
        printf("AudioController: End of audio reached\n");
        return 0;
    }

    // Copy audio data with volume control
    int16_t* bufferSamples = (int16_t*)buffer->buffer->bytes;
    const int16_t* audioSamples = currentAudioData_ + currentAudioPosition_.load();
    float vol = volume_.load();
    
    for (size_t i = 0; i < samplesToWrite; ++i) {
        // Apply volume and clamp to prevent overflow
        float sample = static_cast<float>(audioSamples[i]) * vol;
        sample = std::max(-32768.0f, std::min(32767.0f, sample));
        bufferSamples[i] = static_cast<int16_t>(sample);
    }
    
    // Fill remaining buffer with silence if needed
    if (samplesToWrite < buffer->max_sample_count) {
        memset(&bufferSamples[samplesToWrite], 0, 
               (buffer->max_sample_count - samplesToWrite) * 2);
    }
    
    buffer->sample_count = buffer->max_sample_count;
    
    // Update position
    currentAudioPosition_ = currentAudioPosition_.load() + samplesToWrite;
    
    // Calculate audio intensity for LED effects
    updateAudioIntensity(bufferSamples, samplesToWrite);
    
    return samplesToWrite;
}

void AudioController::updateAudioIntensity(const int16_t* samples, size_t sampleCount) {
    if (!samples || sampleCount == 0) {
        audioIntensity_ = 0.0f;
        return;
    }
    
    // Calculate RMS (Root Mean Square) for audio intensity
    float sum = 0.0f;
    for (size_t i = 0; i < sampleCount; ++i) {
        float sample = static_cast<float>(samples[i]) / 32768.0f;  // Normalize to [-1, 1]
        sum += sample * sample;
    }
    
    float rms = sqrtf(sum / static_cast<float>(sampleCount));
    
    // Apply some smoothing and scaling for LED effects
    float intensity = std::min(1.0f, rms * 3.0f);  // Scale up for better LED response
    
    // Simple low-pass filter for smoother LED transitions
    float current = audioIntensity_.load();
    audioIntensity_ = current * 0.7f + intensity * 0.3f;
}

// Audio callback - called by the I2S system when it needs data
void AudioController::audioCallback(void* userData) {
    AudioController* controller = static_cast<AudioController*>(userData);
    if (!controller || !controller->initialized_) {
        return;
    }
    
    // Get a buffer to fill
    audio_buffer_t* buffer = take_audio_buffer(controller->bufferPool_, false);
    if (buffer) {
        // Fill the buffer with audio data
        controller->fillAudioBuffer(buffer);
        
        // Give the buffer back to the audio system
        give_audio_buffer(controller->bufferPool_, buffer);
    }
}

} // namespace Exterminate
