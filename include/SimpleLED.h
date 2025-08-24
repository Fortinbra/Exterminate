// SimpleLED.h - Minimal LED control (init + on/off) without using LEDController

#pragma once

#include <cstdint>

namespace Exterminate::SimpleLED {

// Initialize the board's default LED (no-op if unavailable)
bool initialize();

// Turn LED on/off (no-op if unavailable or not initialized)
void on();
void off();

// Query availability based on board macros
bool isAvailable();

// Returns the LED pin if known at compile time; otherwise -1
int getPin();

// Initialize an LED connected to a specific GPIO pin (returns false if pin conflicts with I2S pins)
bool initializePin(unsigned int pin);

// Turn a specific GPIO pin on/off (no-op if never initialized). These are stateless and assume output mode is already set.
void onPin(unsigned int pin);
void offPin(unsigned int pin);

// Optional: Initialize PWM on a specific pin for brightness control (returns false if not supported)
bool initializePwmPin(unsigned int pin, uint16_t wrap = 255, float clkdiv = 4.0f);

// Set PWM brightness (0.0 to 1.0) on a specific pin initialized with initializePwmPin
void setBrightnessPin(unsigned int pin, float brightness);

// LED Status patterns for system state indication
enum class LEDStatus {
    OFF,              // LED off
    ON,               // LED solid on
    BREATHING,        // Slow breathing pattern (pairing mode)
    FAST_BLINK,       // Fast blinking (error state)
    SLOW_BLINK        // Slow blinking (warning state)
};

// LED Status controller for automatic patterns
class LEDStatusController {
public:
    // Initialize with specific pin (uses PWM for breathing effect)
    bool initialize(unsigned int pin);
    
    // Set the LED status pattern
    void setStatus(LEDStatus status);
    
    // Get current status
    LEDStatus getStatus() const { return m_currentStatus; }
    
    // Update the LED pattern (call this regularly from main loop)
    void update();
    
    // Check if initialized
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized = false;
    unsigned int m_pin = 0;
    LEDStatus m_currentStatus = LEDStatus::OFF;
    uint32_t m_lastUpdate = 0;
    float m_brightness = 0.0f;
    bool m_direction = true; // For breathing pattern
};

}
