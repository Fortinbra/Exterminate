/*
 * LEDController.h - LED Audio Visualization Controller
 * 
 * This file is part of the Exterminate project.
 * 
 * Provides LED lighting effects that sync with audio playback,
 * simulating the iconic Dalek head lights that flash when speaking.
 * 
 * Uses PWM for smooth brightness control and supports multiple
 * LED configurations including single LED and LED strip effects.
 */

#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <cstdint>
#include <array>

namespace Exterminate {

/**
 * @brief LED Audio Visualization Controller
 * 
 * Controls LEDs that pulse in sync with audio intensity,
 * creating the classic Dalek speaking effect.
 */
class LEDController {
public:
    /**
     * @brief LED configuration structure
     */
    struct Config {
        std::array<uint8_t, 4> ledPins;  ///< GPIO pins for LEDs (up to 4 LEDs)
        uint8_t numLEDs;                 ///< Number of active LEDs (1-4)
        uint32_t pwmFrequency;           ///< PWM frequency in Hz (default: 1000Hz)
        uint16_t maxBrightness;          ///< Maximum brightness (0-65535)
        uint16_t minBrightness;          ///< Minimum brightness when idle
        uint16_t fadeSpeed;              ///< Fade speed factor (higher = faster fade)
        bool invertOutput;               ///< Invert PWM output (for common anode LEDs)
        
        static Config getDefaultConfig() {
            return Config{
                .ledPins = {11, 12, 13, 14},  // GPIO pins for LEDs
                .numLEDs = 2,                 // Use 2 LEDs for classic Dalek effect
                .pwmFrequency = 1000,         // 1kHz PWM frequency
                .maxBrightness = 65535,       // Full brightness
                .minBrightness = 1000,        // Dim when idle (not completely off)
                .fadeSpeed = 8,               // Moderate fade speed
                .invertOutput = false         // Normal PWM (common cathode LEDs)
            };
        }
    };

    /**
     * @brief LED effect patterns
     */
    enum class EffectPattern {
        Pulse,          ///< All LEDs pulse together
        Alternate,      ///< LEDs alternate in intensity
        Wave,           ///< Wave effect across LEDs
        Breathe,        ///< Gentle breathing effect
        Off             ///< LEDs off
    };

    /**
     * @brief Construct LED Controller
     * 
     * @param config LED configuration
     */
    explicit LEDController(const Config& config = Config::getDefaultConfig());

    /**
     * @brief Destructor - cleanup PWM resources
     */
    ~LEDController();

    // Disable copy constructor and assignment operator
    LEDController(const LEDController&) = delete;
    LEDController& operator=(const LEDController&) = delete;

    /**
     * @brief Initialize LED PWM hardware
     * 
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Update LED brightness based on audio intensity
     * 
     * @param audioIntensity Audio intensity (0.0 = silent, 1.0 = max volume)
     */
    void updateAudioIntensity(float audioIntensity);

    /**
     * @brief Set LED effect pattern
     * 
     * @param pattern Effect pattern to use
     */
    void setEffectPattern(EffectPattern pattern);

    /**
     * @brief Set all LEDs to specific brightness
     * 
     * @param brightness Brightness level (0-65535)
     */
    void setBrightness(uint16_t brightness);

    /**
     * @brief Set individual LED brightness
     * 
     * @param ledIndex LED index (0-3)
     * @param brightness Brightness level (0-65535)
     */
    void setLEDBrightness(uint8_t ledIndex, uint16_t brightness);

    /**
     * @brief Turn all LEDs off
     */
    void turnOff();

    /**
     * @brief Turn all LEDs on at current brightness
     */
    void turnOn();

    /**
     * @brief Update LED effects (call regularly from main loop)
     */
    void update();

    /**
     * @brief Get current effect pattern
     */
    EffectPattern getEffectPattern() const { return currentPattern_; }

    /**
     * @brief Check if LEDs are enabled
     */
    bool isEnabled() const { return enabled_; }

private:
    Config config_;
    EffectPattern currentPattern_;
    bool enabled_;
    bool initialized_;

    // PWM state
    std::array<uint, 4> pwmSlices_;
    std::array<uint, 4> pwmChannels_;
    
    // Current LED states
    std::array<uint16_t, 4> currentBrightness_;
    std::array<uint16_t, 4> targetBrightness_;
    
    // Audio processing
    float audioIntensity_;
    float smoothedIntensity_;
    uint32_t lastUpdateTime_;
    
    // Effect state
    uint32_t effectCounter_;
    float effectPhase_;

    /**
     * @brief Initialize PWM for a single LED
     */
    bool initializeLEDPWM(uint8_t ledIndex);

    /**
     * @brief Apply current brightness values to PWM hardware
     */
    void applyBrightness();

    /**
     * @brief Update effect patterns
     */
    void updateEffects();

    /**
     * @brief Smooth brightness transitions
     */
    void updateBrightnessSmoothing();

    /**
     * @brief Calculate pulse effect
     */
    uint16_t calculatePulseEffect(uint8_t ledIndex, float intensity);

    /**
     * @brief Calculate alternate effect
     */
    uint16_t calculateAlternateEffect(uint8_t ledIndex, float intensity);

    /**
     * @brief Calculate wave effect
     */
    uint16_t calculateWaveEffect(uint8_t ledIndex, float intensity);

    /**
     * @brief Calculate breathe effect
     */
    uint16_t calculateBreatheEffect(uint8_t ledIndex, float intensity);

    /**
     * @brief Constrain value to range
     */
    template<typename T>
    T constrain(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
};

} // namespace Exterminate
