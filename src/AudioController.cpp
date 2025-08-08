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
    , actualI2SFormat_(nullptr)
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

    // Configure audio format for I2S output
    // I2S typically expects stereo output, so we'll use 2 channels
    // Our mono audio files will be duplicated to both channels
    audioFormat_ = {
        .sample_freq = config_.sampleRate,
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .channel_count = 2  // Stereo for I2S compatibility
    };

    printf("AudioController: Audio format - %uHz, %u channels, 16-bit PCM (mono files converted to stereo)\n", 
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

    // Initialize I2S first to get the actual format it will use
    const audio_format_t* actualFormat = audio_i2s_setup(&audioFormat_, &i2sConfig_);
    if (!actualFormat) {
        printf("AudioController: ERROR - Failed to setup I2S\n");
        return false;
    }

    // Store the actual format for later use
    actualI2SFormat_ = actualFormat;

    printf("AudioController: I2S setup successful\n");
    printf("AudioController: I2S actual format - %uHz, %u channels\n", 
           actualFormat->sample_freq, actualFormat->channel_count);

    // Now create the buffer pool with the exact format I2S expects
    audio_buffer_format_t bufferFormat = {
        .format = actualFormat,
        .sample_stride = static_cast<uint16_t>(actualFormat->channel_count * 2)  // channels * 2 bytes per 16-bit sample
    };
    
    bufferPool_ = audio_new_producer_pool(&bufferFormat, config_.bufferCount, config_.samplesPerBuffer);
    if (!bufferPool_) {
        printf("AudioController: ERROR - Failed to create buffer pool\n");
        return false;
    }

    printf("AudioController: Created producer buffer pool - %u buffers, %u samples each\n",
           config_.bufferCount, config_.samplesPerBuffer);

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
    if (!buffer || playbackState_ != PlaybackState::Playing || !currentAudioData_ || !actualI2SFormat_) {
        // Fill with silence - use actual format to determine bytes per sample
        size_t bytesPerSample = actualI2SFormat_->channel_count * 2; // 2 bytes per 16-bit sample per channel
        memset(buffer->buffer->bytes, 0, buffer->max_sample_count * bytesPerSample);
        buffer->sample_count = buffer->max_sample_count;
        return 0;
    }

    size_t samplesRemaining = currentAudioSize_.load() - currentAudioPosition_.load();
    size_t monoSamplesToRead = std::min(samplesRemaining, static_cast<size_t>(buffer->max_sample_count));
    
    if (monoSamplesToRead == 0) {
        // End of audio reached
        playbackState_ = PlaybackState::Stopped;
        size_t bytesPerSample = actualI2SFormat_->channel_count * 2;
        memset(buffer->buffer->bytes, 0, buffer->max_sample_count * bytesPerSample);
        buffer->sample_count = buffer->max_sample_count;
        printf("AudioController: End of audio reached\n");
        return 0;
    }

    // Convert mono to the actual I2S format
    int16_t* bufferSamples = (int16_t*)buffer->buffer->bytes;
    const int16_t* audioSamples = currentAudioData_ + currentAudioPosition_.load();
    float vol = volume_.load();
    
    for (size_t i = 0; i < monoSamplesToRead; ++i) {
        // Apply volume and clamp to prevent overflow
        float sample = static_cast<float>(audioSamples[i]) * vol;
        sample = std::max(-32768.0f, std::min(32767.0f, sample));
        int16_t outputSample = static_cast<int16_t>(sample);
        
        // Duplicate mono sample to all channels based on actual format
        for (uint16_t ch = 0; ch < actualI2SFormat_->channel_count; ++ch) {
            bufferSamples[i * actualI2SFormat_->channel_count + ch] = outputSample;
        }
    }
    
    // Fill remaining buffer with silence if needed
    if (monoSamplesToRead < buffer->max_sample_count) {
        size_t remainingSamples = buffer->max_sample_count - monoSamplesToRead;
        size_t bytesToClear = remainingSamples * actualI2SFormat_->channel_count * 2;
        memset(&bufferSamples[monoSamplesToRead * actualI2SFormat_->channel_count], 0, bytesToClear);
    }
    
    buffer->sample_count = buffer->max_sample_count;
    
    // Update position (in mono samples)
    currentAudioPosition_ = currentAudioPosition_.load() + monoSamplesToRead;
    
    // Calculate audio intensity for LED effects (use first channel)
    updateAudioIntensity(bufferSamples, monoSamplesToRead);
    
    return monoSamplesToRead;
}

void AudioController::updateAudioIntensity(const int16_t* samples, size_t monoSampleCount) {
    if (!samples || monoSampleCount == 0 || !actualI2SFormat_) {
        audioIntensity_ = 0.0f;
        return;
    }
    
    // Calculate RMS (Root Mean Square) for audio intensity
    // Since we duplicated mono samples to all channels, analyze first channel only
    float sum = 0.0f;
    for (size_t i = 0; i < monoSampleCount; ++i) {
        float sample = static_cast<float>(samples[i * actualI2SFormat_->channel_count]) / 32768.0f;  // First channel, normalize to [-1, 1]
        sum += sample * sample;
    }
    
    float rms = sqrtf(sum / static_cast<float>(monoSampleCount));
    
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
