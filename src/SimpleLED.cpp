// SimpleLED.cpp - Minimal LED control without using LEDController

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "SimpleLED.h"
#include <cmath>

namespace Exterminate::SimpleLED {

namespace {
    // Track if we have a usable LED
    bool g_initialized = false;
    bool g_available = false;
    uint g_pin = 0;

    // Store PWM wrap per pin so we can compute brightness without querying hardware.
    // Increased to support higher GPIO numbers (e.g., 35-47 range used for external LEDs).
    constexpr uint MAX_PIN_INDEX = 64; // ample for current RP2/RP23xx families
    uint16_t g_pwmWrapByPin[MAX_PIN_INDEX] = {0};
}

bool isAvailable() {
    // Available only if board defines LED and it doesn't conflict with I2S pins
#ifdef PICO_DEFAULT_LED_PIN
    uint pin = PICO_DEFAULT_LED_PIN;
    return !(pin == 6 || pin == 7 || pin == 9);
#else
    return false;
#endif
}

bool initialize() {
#ifdef PICO_DEFAULT_LED_PIN
    g_pin = PICO_DEFAULT_LED_PIN;
    // Avoid conflicting with I2S default pins used by AudioController (BCK=6, LRCLK=7, DOUT=9)
    if (g_pin == 6 || g_pin == 7 || g_pin == 9) {
        g_available = false;
        g_initialized = false;
        return false;
    }
    gpio_init(g_pin);
    gpio_set_dir(g_pin, GPIO_OUT);
    g_available = true;
    g_initialized = true;
    return true;
#else
    g_available = false;
    g_initialized = false;
    return false;
#endif
}

void on() {
    if (!(g_initialized && g_available)) return;
    gpio_put(g_pin, 1);
}

void off() {
    if (!(g_initialized && g_available)) return;
    gpio_put(g_pin, 0);
}

int getPin() {
#ifdef PICO_DEFAULT_LED_PIN
    return PICO_DEFAULT_LED_PIN;
#else
    return -1;
#endif
}

bool initializePin(unsigned int pin) {
    // Avoid conflicting with I2S default pins used by AudioController
    if (pin == 6 || pin == 7 || pin == 9) {
        return false;
    }
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    return true;
}

void onPin(unsigned int pin) {
    gpio_put(pin, 1);
}

void offPin(unsigned int pin) {
    gpio_put(pin, 0);
}

bool initializePwmPin(unsigned int pin, uint16_t wrap, float clkdiv) {
    // Avoid conflicting with I2S default pins used by AudioController
    if (pin == 6 || pin == 7 || pin == 9) {
        return false;
    }
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, wrap);
    pwm_set_clkdiv(slice, clkdiv);
    pwm_set_enabled(slice, true);
    if (pin < MAX_PIN_INDEX) {
        g_pwmWrapByPin[pin] = wrap;
    }
    return true;
}

void setBrightnessPin(unsigned int pin, float brightness) {
    uint16_t top = 255;
    if (pin < MAX_PIN_INDEX && g_pwmWrapByPin[pin] != 0) {
        top = g_pwmWrapByPin[pin];
    }
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;
    uint16_t level = static_cast<uint16_t>(brightness * top);
    pwm_set_gpio_level(pin, level);
}

// LEDStatusController implementation
bool LEDStatusController::initialize(unsigned int pin) {
    m_pin = pin;
    
    // Initialize PWM for smooth breathing effects
    if (!initializePwmPin(pin, 255, 4.0f)) {
        return false;
    }
    
    m_initialized = true;
    m_currentStatus = LEDStatus::OFF;
    m_lastUpdate = to_ms_since_boot(get_absolute_time());
    m_brightness = 0.0f;
    m_direction = true;
    
    // Start with LED off
    setBrightnessPin(m_pin, 0.0f);
    
    return true;
}

void LEDStatusController::setStatus(LEDStatus status) {
    if (!m_initialized) return;
    
    m_currentStatus = status;
    m_lastUpdate = to_ms_since_boot(get_absolute_time());
    
    // Reset state variables for new pattern
    switch (status) {
        case LEDStatus::OFF:
            setBrightnessPin(m_pin, 0.0f);
            break;
        case LEDStatus::ON:
            setBrightnessPin(m_pin, 1.0f);
            break;
        case LEDStatus::BREATHING:
        case LEDStatus::FAST_BLINK:
        case LEDStatus::SLOW_BLINK:
            m_brightness = 0.0f;
            m_direction = true;
            break;
    }
}

void LEDStatusController::update() {
    if (!m_initialized) return;
    
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
    uint32_t elapsed = currentTime - m_lastUpdate;
    
    switch (m_currentStatus) {
        case LEDStatus::OFF:
        case LEDStatus::ON:
            // Static states, nothing to update
            break;
            
        case LEDStatus::BREATHING: {
            // Breathing pattern - slow sine wave (4 second cycle)
            const uint32_t BREATHING_PERIOD_MS = 4000;
            float phase = (float)(currentTime % BREATHING_PERIOD_MS) / BREATHING_PERIOD_MS;
            // Use sine wave for smooth breathing effect
            m_brightness = 0.1f + 0.9f * (1.0f + sinf(2.0f * M_PI * phase)) / 2.0f;
            setBrightnessPin(m_pin, m_brightness);
            break;
        }
        
        case LEDStatus::FAST_BLINK: {
            // Fast blink - 200ms on, 200ms off
            const uint32_t BLINK_PERIOD_MS = 400;
            bool on = (currentTime % BLINK_PERIOD_MS) < (BLINK_PERIOD_MS / 2);
            setBrightnessPin(m_pin, on ? 1.0f : 0.0f);
            break;
        }
        
        case LEDStatus::SLOW_BLINK: {
            // Slow blink - 800ms on, 800ms off
            const uint32_t BLINK_PERIOD_MS = 1600;
            bool on = (currentTime % BLINK_PERIOD_MS) < (BLINK_PERIOD_MS / 2);
            setBrightnessPin(m_pin, on ? 1.0f : 0.0f);
            break;
        }
    }
}

} // namespace Exterminate::SimpleLED
