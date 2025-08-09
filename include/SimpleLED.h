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

}
