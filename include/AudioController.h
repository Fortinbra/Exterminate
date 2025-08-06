#pragma once

#include "I2S.h"
#include "audio/audio_index.h"
#include <cstdint>
#include <memory>
#include <functional>

namespace Exterminate {

/**
 * @brief Controller for I2S audio playback with embedded PCM support
 * 
 * This class provides control for I2S audio output and manages
 * embedded audio resources converted from audio files to PCM format.
 * Supports direct playback without requiring real-time decoding.
 * 
 * Integrates with the modern I2S PIO implementation for reliable audio output.
 */
class AudioController {
public:
    /**
     * @brief Audio playback states
     */
    enum class PlaybackState {
        Stopped,
        Playing,
        Paused
    };

    /**
     * @brief Configuration structure for audio controller
     */
    struct Config {
        uint8_t dataOutPin;     ///< I2S data output pin (to DAC)
        uint8_t clockPinBase;   ///< Base pin for BCK and LRCK (consecutive pins)
        uint8_t systemClockPin; ///< System clock output pin (optional)
        uint32_t sampleRate;    ///< Sample rate in Hz (default: 22050 to match PCM files)
        bool enableSystemClock; ///< Whether to generate system clock output
        
        static Config getDefaultConfig() {
            return Config{
                .dataOutPin = 6,
                .clockPinBase = 8,      // Uses pins 8 (BCK) and 9 (LRCK)
                .systemClockPin = 10,
                .sampleRate = 44100,    // Match our PCM audio files
                .enableSystemClock = false  // Disabled by default for most I2S DACs
            };
        }
    };

    /**
     * @brief Construct a new Audio Controller
     * 
     * @param config Configuration for the audio controller
     */
    explicit AudioController(const Config& config = Config::getDefaultConfig());

    /**
     * @brief Destroy the Audio Controller (RAII cleanup)
     */
    ~AudioController();

    // Disable copy constructor and assignment operator
    AudioController(const AudioController&) = delete;
    AudioController& operator=(const AudioController&) = delete;

    /**
     * @brief Initialize the audio controller
     * 
     * @return true if initialization was successful
     * @return false if initialization failed
     */
    bool initialize();

    /**
     * @brief Play an embedded audio file by index
     * 
     * @param audioIndex Index from AudioIndex enum
     * @return true if playback started successfully
     * @return false if playback failed to start
     */
    bool playAudio(Audio::AudioIndex audioIndex);

    /**
     * @brief Play PCM audio data directly (16-bit)
     * 
     * @param data Pointer to 16-bit PCM sample data
     * @param sampleCount Number of samples to play
     * @return true if playback started successfully
     * @return false if playback failed to start
     */
    bool playPCMAudioData(const int16_t* data, size_t sampleCount);

    /**
     * @brief Play PCM audio data directly (32-bit)
     * 
     * @param data Pointer to 32-bit PCM sample data
     * @param sampleCount Number of samples to play
     * @return true if playback started successfully
     * @return false if playback failed to start
     */
    bool playPCMAudioData(const int32_t* data, size_t sampleCount);

    /**
     * @brief Stop audio playback
     */
    void stopAudio();

    /**
     * @brief Pause audio playback
     */
    void pauseAudio();

    /**
     * @brief Resume paused audio playback
     */
    void resumeAudio();

    /**
     * @brief Check if audio is currently playing
     * 
     * @return true if audio is playing
     * @return false if audio is stopped or paused
     */
    bool isPlaying() const;

    /**
     * @brief Set audio volume level
     * 
     * @param volume Volume level (0.0 to 1.0)
     */
    void setVolume(float volume);

    /**
     * @brief Get current audio volume level
     * 
     * @return float Current volume (0.0 to 1.0)
     */
    float getVolume() const;

    /**
     * @brief Get current audio intensity for LED effects
     * 
     * @return float Current audio intensity (0.0 = silent, 1.0 = max)
     */
    float getAudioIntensity() const;

    /**
     * @brief Set LED intensity callback for real-time audio visualization
     * 
     * @param callback Function to call with audio intensity updates
     */
    void setLEDIntensityCallback(std::function<void(float)> callback);

    /**
     * @brief Get total number of available audio files
     * 
     * @return size_t Number of audio files
     */
    static size_t getAudioFileCount();

    /**
     * @brief Get audio file information by index
     * 
     * @param audioIndex Audio file index
     * @return const Audio::AudioFile* Pointer to audio file info, or nullptr if invalid
     */
    static const Audio::AudioFile* getAudioFileInfo(Audio::AudioIndex audioIndex);

private:
    /**
     * @brief Audio processor implementation for PCM playback
     */
    class PCMAudioProcessor : public IAudioProcessor {
    private:
        AudioController* controller_;
        
    public:
        explicit PCMAudioProcessor(AudioController* controller) 
            : controller_(controller) {}
        
        void processAudio(const int32_t* input, int32_t* output, size_t frameCount) override;
    };

    Config config_;
    std::unique_ptr<I2SController> i2sController_;
    std::unique_ptr<PCMAudioProcessor> audioProcessor_;
    
    PlaybackState playbackState_;
    float volume_;
    
    // Current playback state
    const int16_t* currentPCMData_;
    const int32_t* currentPCMData32_;  // For 32-bit audio data
    bool using32BitData_;              // Flag to track which data type is being used
    size_t currentSampleCount_;
    size_t currentSamplePosition_;
    
    // LED integration
    std::function<void(float)> ledIntensityCallback_;
    float currentAudioIntensity_;

    /**
     * @brief Apply volume scaling to audio sample
     * 
     * @param sample Input audio sample
     * @return int16_t Volume-adjusted sample
     */
    int16_t applyVolume(int16_t sample) const;

    /**
     * @brief Calculate current audio intensity from PCM data
     * 
     * @param samples Pointer to current PCM samples
     * @param count Number of samples to analyze
     * @return float Audio intensity (0.0 to 1.0)
     */
    float calculateAudioIntensity(const int16_t* samples, size_t count) const;

    /**
     * @brief Update LED intensity based on current audio
     */
    void updateLEDIntensity();

    /**
     * @brief Constrain value within range
     * 
     * @param value Value to constrain
     * @param min Minimum value
     * @param max Maximum value
     * @return float Constrained value
     */
    static float constrain(float value, float min, float max);
};

} // namespace Exterminate
