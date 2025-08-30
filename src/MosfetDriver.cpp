#include <cstdint>
#include "MosfetDriver.h"
#include "hardware/gpio.h"

namespace Exterminate {

MosfetDriver::MosfetDriver(uint8_t mosfetPin)
    : pin_(mosfetPin), initialized_(false) {}

void MosfetDriver::initialize() {
    if (!initialized_) {
        gpio_init(pin_);
        gpio_set_dir(pin_, GPIO_OUT);
        gpio_put(pin_, 0);
        initialized_ = true;
    }
}

void MosfetDriver::set(bool on) {
    if (initialized_) {
        gpio_put(pin_, on ? 1 : 0);
    }
}

} // namespace Exterminate
