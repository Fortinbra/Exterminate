// SimpleLED.cpp - Minimal LED control without using LEDController

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "SimpleLED.h"

namespace Exterminate::SimpleLED {

namespace {
    // Track if we have a usable LED
    bool g_initialized = false;
    bool g_available = false;
    uint g_pin = 0;
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

} // namespace Exterminate::SimpleLED
