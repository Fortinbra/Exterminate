// Custom platform implementation for Exterminate project
#include <stddef.h>
#include <string.h>

#include <pico/cyw43_arch.h>
#include <pico/time.h>
#include <uni.h>

#include "sdkconfig.h"

// Sanity check
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

// Declarations
static void trigger_event_on_gamepad(uni_hid_device_t* d);

//
// Platform Overrides
//
static void exterminate_platform_init(int argc, const char** argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    logi("Exterminate platform: init()\n");
}

static void exterminate_platform_on_init_complete(void) {
    logi("Exterminate platform: on_init_complete()\n");
    
    // Turn off LED to indicate initialization is complete
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

static void exterminate_platform_on_device_connected(uni_hid_device_t* d) {
    logi("Exterminate platform: device connected: %p\n", d);
}

static void exterminate_platform_on_device_disconnected(uni_hid_device_t* d) {
    logi("Exterminate platform: device disconnected: %p\n", d);
}

static uni_error_t exterminate_platform_on_device_ready(uni_hid_device_t* d) {
    logi("Exterminate platform: device ready: %p\n", d);
    // You can reject the connection by returning UNI_ERROR_IGNORE_DEVICE
    return UNI_ERROR_SUCCESS;
}

static void exterminate_platform_on_controller_data(uni_hid_device_t* d, uni_controller_t* ctl) {
    static uint8_t leds = 0;
    static uint8_t enabled = true;
    static uni_controller_t prev;
    uni_gamepad_t* gp;

    if (memcmp(&prev, ctl, sizeof(*ctl)) == 0) {
        return;
    }
    prev = *ctl;
    // Print controller data...
    uni_controller_dump(ctl);

    // Example: if "A" button is pressed, toggle LED
    switch (ctl->klass) {
        case UNI_CONTROLLER_CLASS_GAMEPAD:
            gp = &ctl->gamepad;
            
            // Turn on LED when A button is pressed
            if (gp->buttons & BUTTON_A) {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            } else {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            }
            
            // Log button presses for debugging
            if (gp->buttons) {
                logi("Buttons pressed: 0x%04x\n", gp->buttons);
            }
            break;
        default:
            break;
    }
}

static const uni_property_t* exterminate_platform_get_property(uni_property_idx_t key) {
    ARG_UNUSED(key);
    return NULL;
}

//
// Helpers
//
static void trigger_event_on_gamepad(uni_platform_oob_event_t event, void* data) {
    ARG_UNUSED(event);
    ARG_UNUSED(data);
    
    logi("Exterminate platform: OOB event received\n");
}

//
// Entry Point
//
static const struct uni_platform exterminate_platform = {
    .name = "Exterminate",
    .init = exterminate_platform_init,
    .on_init_complete = exterminate_platform_on_init_complete,
    .on_device_discovered = NULL,
    .on_device_connected = exterminate_platform_on_device_connected,
    .on_device_disconnected = exterminate_platform_on_device_disconnected,
    .on_device_ready = exterminate_platform_on_device_ready,
    .on_gamepad_data = NULL,
    .on_controller_data = exterminate_platform_on_controller_data,
    .get_property = exterminate_platform_get_property,
    .on_oob_event = trigger_event_on_gamepad,
    .device_dump = NULL,
    .register_console_cmds = NULL,
};

extern "C" {
struct uni_platform* get_exterminate_platform(void) {
    return (struct uni_platform*)&exterminate_platform;
}
}
