// Custom platform implementation for Exterminate project
#include <stddef.h>
#include <string.h>
#include <cmath>

#include <pico/cyw43_arch.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <uni.h>

#include "sdkconfig.h"
#include "MotorController.h"

// Sanity check
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

// Motor controller instance
static Exterminate::MotorController* motorController = nullptr;

// Controller status LED configuration
#define CONTROLLER_STATUS_LED_PIN 15
static bool controller_connected = false;
static absolute_time_t last_flash_time = 0;
static bool flash_state = false;
static const uint32_t FLASH_INTERVAL_MS = 500; // Flash every 500ms when waiting

// Declarations
static void trigger_event_on_gamepad(uni_hid_device_t* d);
static void init_controller_status_led(void);
static void update_controller_status_led(void);

//
// Platform Overrides
//
static void exterminate_platform_init(int argc, const char** argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    logi("Exterminate platform: init()\n");
    
    // Initialize controller status LED
    init_controller_status_led();
    
    // Initialize motor controller
    // Configure for DRV8833 - adjust these pins according to your wiring
    Exterminate::MotorController::Config motorConfig = {
        .leftMotorPin1 = 2,    // GPIO2 - Left motor direction pin 1 (AIN1)
        .leftMotorPin2 = 3,    // GPIO3 - Left motor direction pin 2 (AIN2)
        .rightMotorPin1 = 4,   // GPIO4 - Right motor direction pin 1 (BIN1)
        .rightMotorPin2 = 5,   // GPIO5 - Right motor direction pin 2 (BIN2)
        .pwmFrequency = 10000  // 10kHz PWM frequency
    };
    
    motorController = new Exterminate::MotorController(motorConfig);
    
    if (motorController && motorController->initialize()) {
        logi("Exterminate platform: Motor controller initialized successfully\n");
    } else {
        loge("Exterminate platform: Failed to initialize motor controller\n");
        delete motorController;
        motorController = nullptr;
    }
}

static void exterminate_platform_on_init_complete(void) {
    logi("Exterminate platform: on_init_complete()\n");
    
    // Turn off the onboard LED to indicate initialization is complete
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    
    // Start flashing the controller status LED (waiting for controller)
    controller_connected = false;
    last_flash_time = get_absolute_time();
    flash_state = false;
    logi("Controller status LED: FLASHING (waiting for controller)\n");
}

static void exterminate_platform_on_device_connected(uni_hid_device_t* d) {
    logi("Exterminate platform: device connected: %p\n", d);
    
    // Turn on status LED to indicate controller is connected
    controller_connected = true;
    gpio_put(CONTROLLER_STATUS_LED_PIN, 1);
    logi("Controller status LED: ON (controller connected)\n");
}

static void exterminate_platform_on_device_disconnected(uni_hid_device_t* d) {
    logi("Exterminate platform: device disconnected: %p\n", d);
    
    // Start flashing LED to indicate waiting for controller
    controller_connected = false;
    last_flash_time = get_absolute_time();
    flash_state = false;
    logi("Controller status LED: FLASHING (waiting for controller)\n");
    
    // Stop motors when controller disconnects for safety
    if (motorController) {
        motorController->stopAllMotors();
        logi("Motors stopped due to controller disconnect\n");
    }
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

    // Update controller status LED (handles flashing when waiting)
    update_controller_status_led();

    if (memcmp(&prev, ctl, sizeof(*ctl)) == 0) {
        return;
    }
    prev = *ctl;
    
    // Print controller data for debugging (comment out for production)
    // uni_controller_dump(ctl);

    // Handle gamepad input
    switch (ctl->klass) {
        case UNI_CONTROLLER_CLASS_GAMEPAD:
        {
            gp = &ctl->gamepad;
            
            // Get joystick values and normalize them
            // Left stick Y-axis for forward/backward movement
            float forward = static_cast<float>(gp->axis_y) / 512.0f;  // Normalize from -512..512 to -1..1
            
            // Left stick X-axis for steering
            float turn = static_cast<float>(gp->axis_x) / 512.0f;     // Normalize from -512..512 to -1..1
            
            // Invert forward axis (controller Y is typically inverted)
            forward = -forward;
            
            // Apply deadzone to prevent drift
            const float deadzone = 0.1f;
            if (std::abs(forward) < deadzone) forward = 0.0f;
            if (std::abs(turn) < deadzone) turn = 0.0f;
            
            // Control motors if motor controller is available
            if (motorController && motorController->isInitialized()) {
                motorController->setDifferentialDrive(forward, turn);
                
                // Log movement for debugging
                if (forward != 0.0f || turn != 0.0f) {
                    logi("Movement: forward=%.2f, turn=%.2f\n", forward, turn);
                }
            }
            
            // LED feedback for A button press (keep original functionality)
            if (gp->buttons & BUTTON_A) {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            } else {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            }
            
            // Emergency stop on B button
            if (gp->buttons & BUTTON_B) {
                if (motorController) {
                    motorController->stopAllMotors();
                    logi("Emergency stop activated!\n");
                }
            }
            
            break;
        }
        default:
            // Stop motors for non-gamepad controllers
            if (motorController) {
                motorController->stopAllMotors();
            }
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
    
    // Handle cleanup if needed
    if (event == UNI_PLATFORM_OOB_GAMEPAD_SYSTEM_BUTTON) {
        // System button pressed - could be used for emergency stop or mode change
        if (motorController) {
            motorController->stopAllMotors();
            logi("System button pressed - motors stopped\n");
        }
    }
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

//
// Controller Status LED Functions
//
static void init_controller_status_led(void) {
    // Initialize the controller status LED GPIO pin
    gpio_init(CONTROLLER_STATUS_LED_PIN);
    gpio_set_dir(CONTROLLER_STATUS_LED_PIN, GPIO_OUT);
    gpio_put(CONTROLLER_STATUS_LED_PIN, 0); // Start with LED off
    
    // Initialize timing for flashing
    last_flash_time = get_absolute_time();
    flash_state = false;
    controller_connected = false;
    
    logi("Controller status LED initialized on GPIO %d\n", CONTROLLER_STATUS_LED_PIN);
}

static void update_controller_status_led(void) {
    if (controller_connected) {
        // Controller is connected - keep LED solid on
        gpio_put(CONTROLLER_STATUS_LED_PIN, 1);
    } else {
        // No controller connected - flash LED
        absolute_time_t current_time = get_absolute_time();
        uint64_t elapsed_ms = absolute_time_diff_us(last_flash_time, current_time) / 1000;
        
        if (elapsed_ms >= FLASH_INTERVAL_MS) {
            flash_state = !flash_state;
            gpio_put(CONTROLLER_STATUS_LED_PIN, flash_state ? 1 : 0);
            last_flash_time = current_time;
        }
    }
}

extern "C" {
struct uni_platform* get_exterminate_platform(void) {
    return (struct uni_platform*)&exterminate_platform;
}
}
