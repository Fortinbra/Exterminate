#pragma once

#include "audio/audio_index.h"
#include "pico/audio_i2s.h"
#include "pico/multicore.h"
#include <cstdint>
#include <memory>
#include <atomic>

namespace Exterminate {

/**
 * @brief Clean I2S Audio Controller using Pico Extras audio_i2s
 * 
 * Simplified implementation focused on reliable I2S audio output
 * using the proven Pico Extras library. Supports embedded PCM audio
 * with real-time streaming and LED visualization integration.
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
     * @brief I2S Configuration structure
     */
    struct Config {
        uint8_t dataPin;        ///< I2S data output pin (DOUT)
        uint8_t clockPinBase;   ///< Base pin for clocks (BCK = base, LRCLK = base+1)
        uint32_t sampleRate;    ///< Audio sample rate (Hz)
        uint bufferCount;       ///< Number of audio buffers to use
        uint samplesPerBuffer;  ///< Samples per audio buffer
        
        static Config getDefault() {
            return Config{
                .dataPin = 9,          // GPIO 9 = I2S DOUT
                .clockPinBase = 6,     // GPIO 6 = BCK, GPIO 7 = LRCLK (per documentation)
                .sampleRate = 44100,   // Match our embedded audio files (they are 44.1kHz)
                .bufferCount = 3,      // Triple buffering for smooth playback
                .samplesPerBuffer = 256 // Small buffers for low latency
            };
        }
    };

    /**
     * @brief Construct Audio Controller
     * 
     * @param config I2S configuration
     */
    explicit AudioController(const Config& config = Config::getDefault());

    /**
     * @brief Destroy Audio Controller
     */
    ~AudioController();

    // Disable copy operations
    AudioController(const AudioController&) = delete;
    AudioController& operator=(const AudioController&) = delete;

    /**
     * @brief Initialize I2S audio system
     * 
     * @return true if successful
     */
    bool initialize();

    /**
     * @brief Shutdown audio system
     */
    void shutdown();

    /**
     * @brief Play embedded audio by index
     * 
     * @param audioIndex Audio file to play
     * @return true if playback started
     */
    bool playAudio(Audio::AudioIndex audioIndex);

    /**
     * @brief Stop current audio playback
     * 
     * @return true if stopped successfully
     */
    bool stopAudio();

    /**
     * @brief Pause current audio playback
     * 
     * @return true if paused successfully
     */
    bool pauseAudio();

    /**
     * @brief Resume paused audio playback
     * 
     * @return true if resumed successfully
     */
    bool resumeAudio();

    /**
     * @brief Set audio volume
     * 
     * @param volume Volume level (0.0 to 1.0)
     */
    void setVolume(float volume);

    /**
     * @brief Get current volume
     * 
     * @return float Current volume (0.0 to 1.0)
     */
    float getVolume() const { return volume_.load(); }

    /**
     * @brief Get current playback state
     * 
     * @return PlaybackState Current state
     */
    PlaybackState getPlaybackState() const { return playbackState_.load(); }

    /**
     * @brief Check if audio is playing
     * 
     * @return true if currently playing
     */
    bool isPlaying() const { return getPlaybackState() == PlaybackState::Playing; }

    /**
     * @brief Get current audio intensity for LED effects
     * 
     * @return float Normalized intensity (0.0 to 1.0)
     */
    float getAudioIntensity() const { return audioIntensity_.load(); }

private:
    Config config_;
    
    // Thread-safe state variables
    std::atomic<PlaybackState> playbackState_;
    std::atomic<float> volume_;
    std::atomic<float> audioIntensity_;
    
    // Pico Extras I2S components
    audio_buffer_pool_t* bufferPool_;
    audio_format_t audioFormat_;
    const audio_format_t* actualI2SFormat_;  // Store the actual format returned by I2S
    audio_i2s_config_t i2sConfig_;
    bool initialized_;
    
    // Current audio data
    const int16_t* currentAudioData_;
    std::atomic<size_t> currentAudioSize_;
    std::atomic<size_t> currentAudioPosition_;
    
    // Multicore synchronization
    static AudioController* instance_;
    
    /**
     * @brief Audio callback function for buffer filling
     * 
     * Called by the audio system when buffers need data
     */
    static void audioCallback(void* userData);
    
    /**
     * @brief Fill audio buffer with current PCM data
     * 
     * @param buffer Buffer to fill
     * @return Number of samples written
     */
    size_t fillAudioBuffer(audio_buffer_t* buffer);
    
    /**
     * @brief Calculate audio intensity for LED effects
     * 
     * @param samples PCM samples to analyze
     * @param sampleCount Number of samples
     */
    void updateAudioIntensity(const int16_t* samples, size_t sampleCount);
    
    /**
     * @brief Start the audio worker on core 1
     */
    void startAudioWorker();
};

} // namespace Exterminate
