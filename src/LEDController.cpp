/*
 * LEDController.cpp - LED Audio Visualization Controller Implementation
 * 
 * This file is part of the Exterminate project.
 * 
 * Implements LED lighting effects that sync with audio playback,
 * simulating the iconic Dalek head lights that flash when speaking.
 */

#include "LEDController.h"
#include "hardware/clocks.h"
#include <cmath>
#include <algorithm>
#include "pico/time.h"
#include <cmath>
#include <algorithm>

namespace Exterminate {

LEDController::LEDController(const Config& config)
    : config_(config)
    , currentPattern_(EffectPattern::Pulse)
    , enabled_(false)
    , initialized_(false)
    , audioIntensity_(0.0f)
    , smoothedIntensity_(0.0f)
    , lastUpdateTime_(0)
    , effectCounter_(0)
    , effectPhase_(0.0f)
{
    // Initialize arrays
    currentBrightness_.fill(config_.minBrightness);
    targetBrightness_.fill(config_.minBrightness);
    pwmSlices_.fill(0);
    pwmChannels_.fill(0);
}

LEDController::~LEDController()
{
    turnOff();
    
    // Release PWM resources
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        if (initialized_) {
            pwm_set_enabled(pwmSlices_[i], false);
        }
    }
}

bool LEDController::initialize()
{
    if (initialized_) {
        return true;
    }

    // Validate configuration
    if (config_.numLEDs == 0 || config_.numLEDs > 4) {
        return false;
    }

    // Initialize each LED's PWM
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        if (!initializeLEDPWM(i)) {
            return false;
        }
    }

    // Set initial state
    turnOff();
    lastUpdateTime_ = to_ms_since_boot(get_absolute_time());
    
    initialized_ = true;
    enabled_ = true;
    
    return true;
}

bool LEDController::initializeLEDPWM(uint8_t ledIndex)
{
    if (ledIndex >= config_.numLEDs) {
        return false;
    }

    uint8_t pin = config_.ledPins[ledIndex];
    
    // Set GPIO function to PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);
    
    // Get PWM slice and channel for this pin
    uint slice = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);
    
    pwmSlices_[ledIndex] = slice;
    pwmChannels_[ledIndex] = channel;
    
    // Configure PWM
    pwm_config pwm_cfg = pwm_get_default_config();
    
    // Calculate clock divider for desired frequency
    // PWM frequency = clock_freq / (div * (wrap + 1))
    // For 125MHz clock and 1kHz PWM with 16-bit resolution:
    // div = 125MHz / (1000Hz * 65536) ≈ 1.9
    float clockDiv = static_cast<float>(clock_get_hz(clk_sys)) / 
                     (config_.pwmFrequency * 65536.0f);
    
    pwm_config_set_clkdiv(&pwm_cfg, clockDiv);
    pwm_config_set_wrap(&pwm_cfg, 65535); // 16-bit resolution
    
    // Initialize PWM slice
    pwm_init(slice, &pwm_cfg, false); // Don't start yet
    
    // Set initial duty cycle (off)
    pwm_set_chan_level(slice, channel, 0);
    
    return true;
}

void LEDController::updateAudioIntensity(float audioIntensity)
{
    // Constrain input
    audioIntensity_ = constrain(audioIntensity, 0.0f, 1.0f);
    
    // Apply smoothing to avoid rapid flickering
    const float smoothingFactor = 0.15f; // Adjust for responsiveness
    smoothedIntensity_ = smoothedIntensity_ * (1.0f - smoothingFactor) + 
                        audioIntensity_ * smoothingFactor;
}

void LEDController::setEffectPattern(EffectPattern pattern)
{
    currentPattern_ = pattern;
    effectCounter_ = 0;
    effectPhase_ = 0.0f;
    
    if (pattern == EffectPattern::Off) {
        turnOff();
    }
}

void LEDController::setBrightness(uint16_t brightness)
{
    brightness = constrain(brightness, static_cast<uint16_t>(0), config_.maxBrightness);
    
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        targetBrightness_[i] = brightness;
    }
}

void LEDController::setLEDBrightness(uint8_t ledIndex, uint16_t brightness)
{
    if (ledIndex >= config_.numLEDs) {
        return;
    }
    
    brightness = constrain(brightness, static_cast<uint16_t>(0), config_.maxBrightness);
    targetBrightness_[ledIndex] = brightness;
}

void LEDController::turnOff()
{
    enabled_ = false;
    
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        currentBrightness_[i] = 0;
        targetBrightness_[i] = 0;
        
        if (initialized_) {
            uint16_t pwmValue = config_.invertOutput ? (65535 - 0) : 0;
            pwm_set_chan_level(pwmSlices_[i], pwmChannels_[i], pwmValue);
            pwm_set_enabled(pwmSlices_[i], false);
        }
    }
}

void LEDController::turnOn()
{
    if (!initialized_) {
        return;
    }
    
    enabled_ = true;
    
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        pwm_set_enabled(pwmSlices_[i], true);
    }
}

void LEDController::update()
{
    if (!initialized_ || !enabled_) {
        return;
    }
    
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
    uint32_t deltaTime = currentTime - lastUpdateTime_;
    lastUpdateTime_ = currentTime;
    
    // Update effect counter and phase
    effectCounter_ += deltaTime;
    effectPhase_ += deltaTime * 0.001f; // Convert to seconds
    
    // Update effects based on current pattern
    updateEffects();
    
    // Apply brightness smoothing
    updateBrightnessSmoothing();
    
    // Apply to hardware
    applyBrightness();
}

void LEDController::updateEffects()
{
    if (currentPattern_ == EffectPattern::Off) {
        for (uint8_t i = 0; i < config_.numLEDs; ++i) {
            targetBrightness_[i] = 0;
        }
        return;
    }
    
    // Calculate target brightness for each LED based on pattern
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        uint16_t brightness = 0;
        
        switch (currentPattern_) {
            case EffectPattern::Pulse:
                brightness = calculatePulseEffect(i, smoothedIntensity_);
                break;
                
            case EffectPattern::Alternate:
                brightness = calculateAlternateEffect(i, smoothedIntensity_);
                break;
                
            case EffectPattern::Wave:
                brightness = calculateWaveEffect(i, smoothedIntensity_);
                break;
                
            case EffectPattern::Breathe:
                brightness = calculateBreatheEffect(i, smoothedIntensity_);
                break;
                
            default:
                brightness = config_.minBrightness;
                break;
        }
        
        targetBrightness_[i] = brightness;
    }
}

void LEDController::updateBrightnessSmoothing()
{
    // Smooth brightness transitions for each LED
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        int32_t diff = targetBrightness_[i] - currentBrightness_[i];
        
        if (diff != 0) {
            // Calculate step size based on fade speed
            int32_t step = (diff * config_.fadeSpeed) / 256;
            
            // Ensure minimum step size for movement
            if (step == 0) {
                step = (diff > 0) ? 1 : -1;
            }
            
            currentBrightness_[i] += step;
            
            // Clamp to target if we've overshot
            if ((step > 0 && currentBrightness_[i] > targetBrightness_[i]) ||
                (step < 0 && currentBrightness_[i] < targetBrightness_[i])) {
                currentBrightness_[i] = targetBrightness_[i];
            }
        }
    }
}

void LEDController::applyBrightness()
{
    for (uint8_t i = 0; i < config_.numLEDs; ++i) {
        uint16_t brightness = currentBrightness_[i];
        
        // Apply inversion if needed
        uint16_t pwmValue = config_.invertOutput ? (65535 - brightness) : brightness;
        
        // Set PWM duty cycle
        pwm_set_chan_level(pwmSlices_[i], pwmChannels_[i], pwmValue);
    }
}

uint16_t LEDController::calculatePulseEffect(uint8_t ledIndex, float intensity)
{
    // All LEDs pulse together based on audio intensity
    float baseIntensity = intensity * 0.8f + 0.2f; // Scale to 20-100%
    uint16_t brightness = static_cast<uint16_t>(
        config_.minBrightness + 
        (config_.maxBrightness - config_.minBrightness) * baseIntensity
    );
    
    return constrain(brightness, config_.minBrightness, config_.maxBrightness);
}

uint16_t LEDController::calculateAlternateEffect(uint8_t ledIndex, float intensity)
{
    // LEDs alternate in intensity with a phase offset
    float phase = effectPhase_ * 2.0f + ledIndex * M_PI; // 2 Hz base frequency
    float sine = (std::sin(phase) + 1.0f) * 0.5f; // 0-1 range
    
    float modulation = sine * intensity * 0.6f + intensity * 0.3f + 0.1f;
    uint16_t brightness = static_cast<uint16_t>(
        config_.minBrightness + 
        (config_.maxBrightness - config_.minBrightness) * modulation
    );
    
    return constrain(brightness, config_.minBrightness, config_.maxBrightness);
}

uint16_t LEDController::calculateWaveEffect(uint8_t ledIndex, float intensity)
{
    // Wave effect across LEDs
    float wavePhase = effectPhase_ * 1.5f + ledIndex * M_PI * 0.5f; // 1.5 Hz with LED offset
    float wave = (std::sin(wavePhase) + 1.0f) * 0.5f; // 0-1 range
    
    float modulation = wave * intensity * 0.7f + intensity * 0.2f + 0.1f;
    uint16_t brightness = static_cast<uint16_t>(
        config_.minBrightness + 
        (config_.maxBrightness - config_.minBrightness) * modulation
    );
    
    return constrain(brightness, config_.minBrightness, config_.maxBrightness);
}

uint16_t LEDController::calculateBreatheEffect(uint8_t ledIndex, float intensity)
{
    // Gentle breathing effect with audio modulation
    float breathPhase = effectPhase_ * 0.5f; // 0.5 Hz breathing
    float breath = (std::sin(breathPhase) + 1.0f) * 0.5f; // 0-1 range
    
    // Combine breathing with audio intensity
    float modulation = breath * 0.4f + intensity * 0.5f + 0.1f;
    uint16_t brightness = static_cast<uint16_t>(
        config_.minBrightness + 
        (config_.maxBrightness - config_.minBrightness) * modulation
    );
    
    return constrain(brightness, config_.minBrightness, config_.maxBrightness);
}

} // namespace Exterminate
