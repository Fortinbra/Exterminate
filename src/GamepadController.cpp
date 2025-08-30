#include "GamepadController.h"
#include "MotorController.h"
#include "AudioController.h"
#include "MosfetDriver.h"
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cmath>

extern "C" {
    #include "sdkconfig.h"
}

namespace Exterminate {

// Static member definitions
struct uni_platform GamepadController::s_platform = {
    .name = "Exterminate Dalek Platform",
    .init = GamepadController::platformInit,
    .on_init_complete = GamepadController::platformOnInitComplete,
    .on_device_discovered = GamepadController::platformOnDeviceDiscovered,
    .on_device_connected = GamepadController::platformOnDeviceConnected,
    .on_device_disconnected = GamepadController::platformOnDeviceDisconnected,
    .on_device_ready = GamepadController::platformOnDeviceReady,
    .on_gamepad_data = nullptr,  // deprecated
    .on_controller_data = GamepadController::platformOnControllerData,
    .get_property = GamepadController::platformGetProperty,
    .on_oob_event = GamepadController::platformOnOobEvent,
    .device_dump = nullptr,  // optional
    .register_console_cmds = nullptr,  // optional
};

GamepadController& GamepadController::getInstance() {
    static GamepadController instance;
    return instance;
}

bool GamepadController::initialize() {
    if (m_initialized) {
        return true;
    }

    printf("GamepadController: Initializing BluePad32 system...\n");
    m_bluetoothState = BluetoothState::INITIALIZING;

    // Initialize CYW43 driver architecture (enables Bluetooth)
    if (cyw43_arch_init()) {
        printf("GamepadController: Failed to initialize cyw43_arch\n");
        m_bluetoothState = BluetoothState::ERROR;
        return false;
    }

    // Turn on LED during initialization
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // Must be called before uni_init()
    uni_platform_set_custom(&s_platform);

    // Initialize BP32
    uni_init(0, nullptr);

    m_initialized = true;
    printf("GamepadController: BluePad32 initialized successfully\n");
    
    // Start LED update timer
    m_ledUpdateTimer.process = &GamepadController::ledUpdateTimerCallback;
    btstack_run_loop_set_timer(&m_ledUpdateTimer, 50); // Update every 50ms
    btstack_run_loop_add_timer(&m_ledUpdateTimer);
    
    return true;
}

void GamepadController::startEventLoop() {
    if (!m_initialized) {
        printf("GamepadController: Error - not initialized! Call initialize() first.\n");
        return;
    }

    printf("GamepadController: Starting BluePad32 event loop...\n");
    
    // This call blocks and does not return under normal operation
    btstack_run_loop_execute();
}

// Platform callback implementations
void GamepadController::setLEDController(SimpleLED::LEDStatusController* ledController) {
    m_ledController = ledController;
    updateLEDStatus(); // Update LED immediately when controller is set
}

void GamepadController::setMotorController(MotorController* motorController) {
    m_motorController = motorController;
    if (m_motorController) {
        printf("GamepadController: Motor controller connected for tank steering\n");
        printf("DEBUG: Motor controller initialized: %s\n", 
               m_motorController->isInitialized() ? "YES" : "NO");
    }
}

void GamepadController::setAudioController(AudioController* audioController) {
    m_audioController = audioController;
    if (m_audioController) {
        printf("GamepadController: Audio controller connected for sound effects\n");
        printf("DEBUG: Audio controller initialized: %s\n", 
               m_audioController->isInitialized() ? "YES" : "NO");
    }
}

void GamepadController::setMosfetDriver(MosfetDriver* mosfetDriver) {
    m_mosfetDriver = mosfetDriver;
    if (m_mosfetDriver) {
        printf("GamepadController: MOSFET driver registered\n");
    }
}

void GamepadController::updateLEDStatus() {
    if (!m_ledController) return;
    
    switch (m_bluetoothState) {
        case BluetoothState::INITIALIZING:
            m_ledController->setStatus(SimpleLED::LEDStatus::SLOW_BLINK);
            break;
        case BluetoothState::PAIRING:
            m_ledController->setStatus(SimpleLED::LEDStatus::BREATHING);
            break;
        case BluetoothState::CONNECTED:
            m_ledController->setStatus(SimpleLED::LEDStatus::SLOW_BLINK);
            break;
        case BluetoothState::PAIRED:
            m_ledController->setStatus(SimpleLED::LEDStatus::ON);
            break;
        case BluetoothState::ERROR:
            m_ledController->setStatus(SimpleLED::LEDStatus::FAST_BLINK);
            break;
    }
}

void GamepadController::ledUpdateTimerCallback(btstack_timer_source_t* timer) {
    (void)timer;
    
    GamepadController& instance = getInstance();
    
    // Update LED pattern
    if (instance.m_ledController) {
        instance.m_ledController->update();
    }
    
    // Reschedule timer
    btstack_run_loop_set_timer(&instance.m_ledUpdateTimer, 50); // 50ms = 20Hz update rate
    btstack_run_loop_add_timer(&instance.m_ledUpdateTimer);
}

void GamepadController::platformInit(int argc, const char** argv) {
    (void)argc;
    (void)argv;
    
    printf("GamepadController: Platform init callback\n");
}

void GamepadController::platformOnInitComplete() {
    printf("GamepadController: Platform initialization complete\n");

    // Start scanning and autoconnect to supported controllers
    uni_bt_start_scanning_and_autoconnect_unsafe();

    // Delete stored BT keys for fresh start (useful during development)
    uni_bt_del_keys_unsafe();

    // Turn off LED once init is done
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    // Update state to pairing mode
    getInstance().m_bluetoothState = BluetoothState::PAIRING;
    getInstance().updateLEDStatus();

    printf("GamepadController: Ready to accept gamepad connections\n");
    printf("GamepadController: All gamepad inputs will be logged to UART console\n");
}

uni_error_t GamepadController::platformOnDeviceDiscovered(bd_addr_t addr, const char* name, uint16_t cod, uint8_t rssi) {
    printf("GamepadController: Device discovered - Name: %s, RSSI: %d dBm\n", 
           name ? name : "Unknown", rssi);

    // Filter out keyboards as an example
    if (((cod & UNI_BT_COD_MINOR_MASK) & UNI_BT_COD_MINOR_KEYBOARD) == UNI_BT_COD_MINOR_KEYBOARD) {
        printf("GamepadController: Ignoring keyboard device\n");
        return UNI_ERROR_IGNORE_DEVICE;
    }

    return UNI_ERROR_SUCCESS;
}

void GamepadController::platformOnDeviceConnected(uni_hid_device_t* d) {
    printf("GamepadController: Device connected (ptr: %p, idx: %d)\n", 
           d, uni_hid_device_get_idx_for_instance(d));
    
    // Update state to connected (but not ready yet)
    getInstance().m_bluetoothState = BluetoothState::CONNECTED;
    getInstance().updateLEDStatus();
}

void GamepadController::platformOnDeviceDisconnected(uni_hid_device_t* d) {
    printf("GamepadController: Device disconnected (ptr: %p, idx: %d)\n", 
           d, uni_hid_device_get_idx_for_instance(d));
    
    // Return to pairing mode when device disconnects
    getInstance().m_bluetoothState = BluetoothState::PAIRING;
    getInstance().updateLEDStatus();
}

uni_error_t GamepadController::platformOnDeviceReady(uni_hid_device_t* d) {
    printf("GamepadController: Device ready (ptr: %p, idx: %d)\n", 
           d, uni_hid_device_get_idx_for_instance(d));
    
    // Update state to fully paired and ready
    getInstance().m_bluetoothState = BluetoothState::PAIRED;
    getInstance().updateLEDStatus();
    
    // Accept all ready devices
    return UNI_ERROR_SUCCESS;
}

void GamepadController::platformOnControllerData(uni_hid_device_t* d, uni_controller_t* ctl) {
    // Get the singleton instance to access member variables
    GamepadController& instance = getInstance();
    
    // Log all controller data to UART console
    logControllerData(d, ctl);
    
    // Process gamepad controls if we have a gamepad
    if (ctl->klass == UNI_CONTROLLER_CLASS_GAMEPAD) {
        // Process audio controls (A button for sound effects)
        if (instance.m_audioController) {
            instance.processAudioControls(&ctl->gamepad);
        }
        
        // Process tank steering if we have a motor controller
        if (instance.m_motorController) {
            instance.processTankSteering(&ctl->gamepad);
        }
        // Process MOSFET controls (Y button toggles MOSFET)
        if (instance.m_mosfetDriver) {
            instance.processMosfetControls(&ctl->gamepad);
        }
    }
}

const uni_property_t* GamepadController::platformGetProperty(uni_property_idx_t idx) {
    (void)idx;
    return nullptr;
}

void GamepadController::platformOnOobEvent(uni_platform_oob_event_t event, void* data) {
    switch (event) {
        case UNI_PLATFORM_OOB_GAMEPAD_SYSTEM_BUTTON:
            printf("GamepadController: System button pressed on device %p\n", data);
            break;
        case UNI_PLATFORM_OOB_BLUETOOTH_ENABLED:
            printf("GamepadController: Bluetooth enabled: %s\n", 
                   (bool)data ? "true" : "false");
            break;
        default:
            printf("GamepadController: Unsupported OOB event: 0x%04x\n", event);
            break;
    }
}

void GamepadController::logControllerData(uni_hid_device_t* d, uni_controller_t* ctl) {
    int device_idx = uni_hid_device_get_idx_for_instance(d);
    
    switch (ctl->klass) {
        case UNI_CONTROLLER_CLASS_GAMEPAD:
            printf("GAMEPAD[%d]: ", device_idx);
            logGamepadData(d, &ctl->gamepad);
            break;
            
        case UNI_CONTROLLER_CLASS_BALANCE_BOARD:
            printf("BALANCE_BOARD[%d]: ", device_idx);
            uni_balance_board_dump(&ctl->balance_board);
            break;
            
        case UNI_CONTROLLER_CLASS_MOUSE:
            printf("MOUSE[%d]: ", device_idx);
            uni_mouse_dump(&ctl->mouse);
            break;
            
        case UNI_CONTROLLER_CLASS_KEYBOARD:
            printf("KEYBOARD[%d]: ", device_idx);
            uni_keyboard_dump(&ctl->keyboard);
            break;
            
        default:
            printf("UNKNOWN_CONTROLLER[%d]: Unsupported class: %d\n", 
                   device_idx, ctl->klass);
            break;
    }
}

void GamepadController::logGamepadData(uni_hid_device_t* d, const uni_gamepad_t* gp) {
    (void)d; // Suppress unused parameter warning
    
    // Log main buttons if any are pressed
    if (gp->buttons != 0) {
        printf("Buttons: 0x%04x [", gp->buttons);
        
        // Check individual main buttons
        if (gp->buttons & BUTTON_A) printf("A ");
        if (gp->buttons & BUTTON_B) printf("B ");
        if (gp->buttons & BUTTON_X) printf("X ");
        if (gp->buttons & BUTTON_Y) printf("Y ");
        if (gp->buttons & BUTTON_SHOULDER_L) printf("L1 ");
        if (gp->buttons & BUTTON_SHOULDER_R) printf("R1 ");
        if (gp->buttons & BUTTON_TRIGGER_L) printf("L2 ");
        if (gp->buttons & BUTTON_TRIGGER_R) printf("R2 ");
        if (gp->buttons & BUTTON_THUMB_L) printf("L3 ");
        if (gp->buttons & BUTTON_THUMB_R) printf("R3 ");
        
        printf("] ");
    }

    // Log misc buttons if any are pressed (separate field!)
    if (gp->misc_buttons != 0) {
        printf("MiscButtons: 0x%02x [", gp->misc_buttons);
        
        if (gp->misc_buttons & MISC_BUTTON_SYSTEM) printf("HOME ");
        if (gp->misc_buttons & MISC_BUTTON_SELECT) printf("SELECT ");  
        if (gp->misc_buttons & MISC_BUTTON_START) printf("START ");
        if (gp->misc_buttons & MISC_BUTTON_CAPTURE) printf("CAPTURE ");
        
        printf("] ");
    }

    // Log D-pad if pressed
    if (gp->dpad != 0) {
        printf("D-pad: ");
        if (gp->dpad & DPAD_UP) printf("UP ");
        if (gp->dpad & DPAD_DOWN) printf("DOWN ");
        if (gp->dpad & DPAD_LEFT) printf("LEFT ");
        if (gp->dpad & DPAD_RIGHT) printf("RIGHT ");
    }

    // Log analog sticks (only if significantly moved from center)
    const int16_t deadzone = 50; // Small deadzone to avoid spam from stick drift
    
    if (abs(gp->axis_x) > deadzone || abs(gp->axis_y) > deadzone) {
        printf("LeftStick: X=%d Y=%d ", gp->axis_x, gp->axis_y);
    }
    
    if (abs(gp->axis_rx) > deadzone || abs(gp->axis_ry) > deadzone) {
        printf("RightStick: X=%d Y=%d ", gp->axis_rx, gp->axis_ry);
    }

    // Log triggers if pressed
    if (gp->brake > 10 || gp->throttle > 10) {
        printf("Triggers: L2=%d R2=%d ", gp->brake, gp->throttle);
    }

    printf("\n");
}

void GamepadController::processTankSteering(const uni_gamepad_t* gp) {
    if (!m_motorController) {
        printf("DEBUG: No motor controller set!\n");
        return;
    }
    
    if (!m_motorController->isInitialized()) {
        printf("DEBUG: Motor controller not initialized!\n");
        return;
    }
    
    // Tank steering using left analog stick
    // Y-axis controls throttle (forward/reverse)
    // X-axis controls steering (left/right turn)
    
    // Convert from BluePad32's axis range to normalized values
    // BluePad32 uses signed 16-bit values (-512 to 511 typically)
    static constexpr float AXIS_SCALE = 1.0f / 512.0f;
    static constexpr int16_t DEADZONE = 50;  // Deadzone to prevent drift
    
    // Get raw axis values
    int16_t rawThrottle = gp->axis_y;  // Y-axis for forward/backward
    int16_t rawSteering = gp->axis_x;  // X-axis for left/right
    
    // Debug: Always print raw values to see what we're getting
    static int debugCounter = 0;
    if (debugCounter++ % 50 == 0) { // Print every 50 calls to avoid spam
        printf("DEBUG: Raw stick values - X=%d Y=%d\n", rawSteering, rawThrottle);
    }
    
    // Apply deadzone
    int16_t throttle = (abs(rawThrottle) > DEADZONE) ? rawThrottle : 0;
    int16_t steering = (abs(rawSteering) > DEADZONE) ? rawSteering : 0;
    
    // Convert to normalized values (-1.0 to 1.0)
    float normalizedThrottle = static_cast<float>(throttle) * AXIS_SCALE;
    float normalizedSteering = static_cast<float>(steering) * AXIS_SCALE;
    
    // Invert Y-axis since gamepad Y is typically inverted
    // (up on stick should be forward motion)
    normalizedThrottle = -normalizedThrottle;
    
    // Clamp to valid range
    normalizedThrottle = std::max(-1.0f, std::min(1.0f, normalizedThrottle));
    normalizedSteering = std::max(-1.0f, std::min(1.0f, normalizedSteering));
    
    // Apply tank steering to motors
    m_motorController->setDifferentialDrive(normalizedThrottle, normalizedSteering);
    
    // Optional: Log motor commands when there's significant input
    if (abs(throttle) > DEADZONE || abs(steering) > DEADZONE) {
        printf("TankSteering: Raw(X=%d,Y=%d) -> Throttle=%.2f Steering=%.2f\n", 
               rawSteering, rawThrottle, normalizedThrottle, normalizedSteering);
    }
}

void GamepadController::processAudioControls(const uni_gamepad_t* gp) {
    if (!m_audioController) {
        return;
    }
    
    if (!m_audioController->isInitialized()) {
        printf("DEBUG: Audio controller not initialized!\n");
        return;
    }
    
    // Static variables to track button states (prevent retriggering while held)
    static bool previousAButton = false;
    
    // Check if A button is currently pressed
    bool currentAButton = (gp->buttons & BUTTON_A) != 0;
    
    // Trigger audio on A button press (not release)
    if (currentAButton && !previousAButton) {
        printf("A button pressed - triggering random audio!\n");
        
        // Play a random audio file
        if (m_audioController) {
            bool success = m_audioController->playRandomAudio();
            if (success) {
                printf("GamepadController: Random audio playback started\n");
            } else {
                printf("GamepadController: Failed to start random audio playback\n");
            }
        } else {
            printf("GamepadController: No audio controller available\n");
        }
    }
    
    // Update previous button state
    previousAButton = currentAButton;
}

void GamepadController::processMosfetControls(const uni_gamepad_t* gp) {
    if (!m_mosfetDriver) return;

    static bool previousYButton = false;
    bool currentYButton = (gp->buttons & BUTTON_Y) != 0;

    if (currentYButton && !previousYButton) {
        // Button pressed -> turn MOSFET on (full speed)
        m_mosfetDriver->set(true);
    } else if (!currentYButton && previousYButton) {
        // Button released -> turn MOSFET off
        m_mosfetDriver->set(false);
    }

    previousYButton = currentYButton;
}

} // namespace Exterminate
