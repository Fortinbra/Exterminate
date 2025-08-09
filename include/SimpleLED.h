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

}
