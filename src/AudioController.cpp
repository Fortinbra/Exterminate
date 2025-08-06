#include "AudioController.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <cstdint>
#include <functional>
#include <stdio.h>  // For printf functions

namespace Exterminate {

AudioController::AudioController(const Config& config)
    : config_(config)
    , playbackState_(PlaybackState::Stopped)
    , volume_(1.0f)
    , currentPCMData_(nullptr)
    , currentPCMData32_(nullptr)
    , using32BitData_(false)
    , currentSampleCount_(0)
    , currentSamplePosition_(0)
    , currentAudioIntensity_(0.0f)
{
    // Create audio processor
    audioProcessor_ = std::make_unique<PCMAudioProcessor>(this);
}

AudioController::~AudioController()
{
    stopAudio();
}

bool AudioController::initialize()
{
    printf("AudioController::initialize() - Starting initialization\n");
    
    if (i2sController_) {
        printf("AudioController::initialize() - Already initialized\n");
        return true;  // Already initialized
    }

    try {
        printf("AudioController::initialize() - Creating I2S configuration\n");
        
        // Create I2S configuration
        I2SConfig i2sConfig;
        i2sConfig.dataOutPin = config_.dataOutPin;
        i2sConfig.dataInPin = 7;           // GPIO 7 for input (unused but required)
        i2sConfig.clockPinBase = config_.clockPinBase;
        i2sConfig.systemClockPin = config_.systemClockPin;
        i2sConfig.sampleRate = config_.sampleRate;
        i2sConfig.systemClockMult = 256;   // Standard multiplier
        i2sConfig.bitDepth = 32;           // 32-bit audio for MAX98357A compatibility
        i2sConfig.enableSystemClock = config_.enableSystemClock;
        
        printf("AudioController::initialize() - I2S Config: dataOut=%d, dataIn=%d, clockBase=%d, sysClock=%d, rate=%d, bitDepth=%d\n",
             i2sConfig.dataOutPin, i2sConfig.dataInPin, i2sConfig.clockPinBase, 
             i2sConfig.systemClockPin, i2sConfig.sampleRate, i2sConfig.bitDepth);
        
        // Create I2S controller with our audio processor
        printf("AudioController::initialize() - Creating I2S controller\n");
        i2sController_ = std::make_unique<I2SController>(i2sConfig, 
            std::make_unique<PCMAudioProcessor>(this));
        
        if (!i2sController_) {
            printf("ERROR: AudioController::initialize() - Failed to create I2S controller\n");
            return false;
        }
        
        printf("AudioController::initialize() - I2S controller created, initializing hardware\n");
        
        // Initialize I2S hardware
        bool result = i2sController_->initialize();
        
        if (result) {
            printf("AudioController::initialize() - I2S hardware initialization successful\n");
        } else {
            printf("ERROR: AudioController::initialize() - I2S hardware initialization failed\n");
        }
        
        return result;
    }
    catch (const std::exception& e) {
        printf("ERROR: AudioController::initialize() - Exception during initialization: %s\n", e.what());
        return false;
    }
    catch (...) {
        printf("ERROR: AudioController::initialize() - Unknown exception during initialization\n");
        return false;
    }
}

bool AudioController::playAudio(Audio::AudioIndex audioIndex)
{
    if (!i2sController_) {
        return false;
    }

    // Get audio file info
    const Audio::AudioFile* audioFile = getAudioFileInfo(audioIndex);
    if (!audioFile) {
        return false;
    }

    // Use the new PCM playback method
    return playPCMAudioData(audioFile->data, audioFile->sample_count);
}

bool AudioController::playPCMAudioData(const int16_t* data, size_t sampleCount)
{
    if (!i2sController_ || !data || sampleCount == 0) {
        return false;
    }

    // Stop current playback if any
    stopAudio();

    // Set up new PCM playback (16-bit)
    currentPCMData_ = data;
    currentPCMData32_ = nullptr;
    using32BitData_ = false;
    currentSampleCount_ = sampleCount;
    currentSamplePosition_ = 0;
    playbackState_ = PlaybackState::Playing;

    // Start I2S output
    i2sController_->start();
    
    return true;
}

bool AudioController::playPCMAudioData(const int32_t* data, size_t sampleCount)
{
    if (!i2sController_ || !data || sampleCount == 0) {
        return false;
    }

    // Stop current playback if any
    stopAudio();

    // Set up new PCM playback (32-bit)
    currentPCMData_ = nullptr;
    currentPCMData32_ = data;
    using32BitData_ = true;
    currentSampleCount_ = sampleCount;
    currentSamplePosition_ = 0;
    playbackState_ = PlaybackState::Playing;

    // Start I2S output
    i2sController_->start();
    
    return true;
}

void AudioController::stopAudio()
{
    if (i2sController_) {
        i2sController_->stop();
    }
    
    playbackState_ = PlaybackState::Stopped;
    currentPCMData_ = nullptr;
    currentSampleCount_ = 0;
    currentSamplePosition_ = 0;
}

void AudioController::pauseAudio()
{
    if (playbackState_ == PlaybackState::Playing && i2sController_) {
        i2sController_->stop();
        playbackState_ = PlaybackState::Paused;
    }
}

void AudioController::resumeAudio()
{
    if (playbackState_ == PlaybackState::Paused && i2sController_) {
        i2sController_->start();
        playbackState_ = PlaybackState::Playing;
    }
}

bool AudioController::isPlaying() const
{
    return playbackState_ == PlaybackState::Playing;
}

void AudioController::setVolume(float volume)
{
    volume_ = constrain(volume, 0.0f, 1.0f);
}

float AudioController::getVolume() const
{
    return volume_;
}

size_t AudioController::getAudioFileCount()
{
    return Audio::AUDIO_FILE_COUNT;
}

const Audio::AudioFile* AudioController::getAudioFileInfo(Audio::AudioIndex audioIndex)
{
    size_t index = static_cast<size_t>(audioIndex);
    if (index >= Audio::AUDIO_FILE_COUNT) {
        return nullptr;
    }
    
    return &Audio::AUDIO_FILES[index];
}

// PCMAudioProcessor implementation
void AudioController::PCMAudioProcessor::processAudio(const int32_t* input, int32_t* output, size_t frameCount)
{
    if (!controller_ || controller_->playbackState_ != PlaybackState::Playing) {
        // Fill with silence
        for (size_t i = 0; i < frameCount * 2; ++i) {
            output[i] = 0;
        }
        // Update LED intensity for silence
        controller_->updateLEDIntensity();
        return;
    }

    const int16_t* pcmData = controller_->currentPCMData_;
    const int32_t* pcmData32 = controller_->currentPCMData32_;
    bool using32Bit = controller_->using32BitData_;
    size_t& position = controller_->currentSamplePosition_;
    size_t sampleCount = controller_->currentSampleCount_;
    
    for (size_t frame = 0; frame < frameCount; ++frame) {
        int32_t sample32 = 0;
        
        if (position < sampleCount) {
            if (using32Bit && pcmData32) {
                // Use 32-bit data directly
                sample32 = pcmData32[position++];
            } else if (!using32Bit && pcmData) {
                // Convert 16-bit to 32-bit
                int16_t sample16 = pcmData[position++];
                sample16 = controller_->applyVolume(sample16);
                sample32 = static_cast<int32_t>(sample16) << 16;
            }
        }
        
        // Convert 16-bit PCM to 32-bit I2S format (left and right channels)
        // MAX98357A expects data in upper 16 bits, shifted left by 16
        // with proper sign extension for negative values
        output[frame * 2] = sample32;      // Left channel
        output[frame * 2 + 1] = sample32;  // Right channel (mono to stereo)
        
        // Check if playback finished
        if (position >= sampleCount) {
            controller_->playbackState_ = PlaybackState::Stopped;
            break;
        }
    }
    
    // Update LED intensity based on current audio
    controller_->updateLEDIntensity();
}

int16_t AudioController::applyVolume(int16_t sample) const
{
    if (volume_ >= 1.0f) {
        return sample;
    }
    
    if (volume_ <= 0.0f) {
        return 0;
    }
    
    // Apply volume scaling
    float scaled = static_cast<float>(sample) * volume_;
    return static_cast<int16_t>(constrain(scaled, -32768.0f, 32767.0f));
}

float AudioController::getAudioIntensity() const
{
    return currentAudioIntensity_;
}

void AudioController::setLEDIntensityCallback(std::function<void(float)> callback)
{
    ledIntensityCallback_ = callback;
}

float AudioController::calculateAudioIntensity(const int16_t* samples, size_t count) const
{
    if (!samples || count == 0) {
        return 0.0f;
    }
    
    // Calculate RMS (Root Mean Square) for audio intensity
    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        float sample = static_cast<float>(samples[i]) / 32768.0f; // Normalize to [-1, 1]
        sum += sample * sample;
    }
    
    float rms = std::sqrt(sum / static_cast<float>(count));
    
    // Apply some scaling and smoothing for LED visualization
    float intensity = std::min(rms * 3.0f, 1.0f); // Scale up for better LED response
    
    return intensity;
}

void AudioController::updateLEDIntensity()
{
    if (ledIntensityCallback_ && currentPCMData_ && playbackState_ == PlaybackState::Playing) {
        // Analyze a small window of audio around current position
        const size_t windowSize = 256; // Small analysis window
        size_t startPos = currentSamplePosition_;
        size_t endPos = std::min(startPos + windowSize, currentSampleCount_);
        size_t analyzeCount = endPos - startPos;
        
        if (analyzeCount > 0) {
            currentAudioIntensity_ = calculateAudioIntensity(
                currentPCMData_ + startPos, analyzeCount);
            ledIntensityCallback_(currentAudioIntensity_);
        }
    } else if (ledIntensityCallback_ && playbackState_ != PlaybackState::Playing) {
        // No audio playing, set intensity to 0
        currentAudioIntensity_ = 0.0f;
        ledIntensityCallback_(0.0f);
    }
}

float AudioController::constrain(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

} // namespace Exterminate
