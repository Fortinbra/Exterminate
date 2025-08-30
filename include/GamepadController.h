#pragma once

#include "SimpleLED.h"

extern "C" {
    #include <uni.h>
    #include <btstack_run_loop.h>
}

namespace Exterminate {

// Forward declarations
class MotorController;
class AudioController;
class MosfetDriver;

/**
 * @brief Bluetooth connection states for LED status indication
 */
enum class BluetoothState {
    INITIALIZING,     // System starting up
    PAIRING,          // Ready to accept connections, scanning
    CONNECTED,        // Device connected but not ready
    PAIRED,           // Device fully paired and ready
    ERROR             // Connection error or initialization failure
};

/**
 * @brief C++ wrapper for BluePad32 gamepad controller functionality
 * 
 * This class provides a C++ interface to the BluePad32 library for handling
 * Bluetooth gamepad connections. It follows SOLID principles by encapsulating
 * all BluePad32-specific functionality behind a clean C++ interface.
 */
class GamepadController {
public:
    /**
     * @brief Initialize the gamepad controller system
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start the BluePad32 event loop (blocking call)
     * This method does not return under normal operation
     */
    void startEventLoop();

    /**
     * @brief Check if the system is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

    /**
     * @brief Get current Bluetooth connection state
     * @return Current BluetoothState
     */
    BluetoothState getBluetoothState() const { return m_bluetoothState; }

    /**
     * @brief Set the LED status controller for automatic status indication
     * @param ledController Pointer to LED controller (nullptr to disable)
     */
    void setLEDController(SimpleLED::LEDStatusController* ledController);

    /**
     * @brief Set the motor controller for tank-style steering
     * @param motorController Pointer to motor controller (nullptr to disable)
     */
    void setMotorController(MotorController* motorController);

    /**
     * @brief Set the audio controller for sound effects
     * @param audioController Pointer to audio controller (nullptr to disable)
     */
    void setAudioController(AudioController* audioController);
    /**
     * @brief Set the MOSFET driver for simple on/off control via gamepad
     */
    void setMosfetDriver(MosfetDriver* mosfetDriver);
        void processMosfetControls(const uni_gamepad_t* gp);

    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static GamepadController& getInstance();

    // Prevent copy and assignment (singleton pattern)
    GamepadController(const GamepadController&) = delete;
    GamepadController& operator=(const GamepadController&) = delete;

private:
    GamepadController() = default;
    ~GamepadController() = default;

    bool m_initialized = false;
    BluetoothState m_bluetoothState = BluetoothState::INITIALIZING;
    SimpleLED::LEDStatusController* m_ledController = nullptr;
    MotorController* m_motorController = nullptr;
    AudioController* m_audioController = nullptr;
        MosfetDriver* m_mosfetDriver = nullptr;

    // C callback functions that interface with BluePad32
    static void platformInit(int argc, const char** argv);
    static void platformOnInitComplete();
    static uni_error_t platformOnDeviceDiscovered(bd_addr_t addr, const char* name, uint16_t cod, uint8_t rssi);
    static void platformOnDeviceConnected(uni_hid_device_t* d);
    static void platformOnDeviceDisconnected(uni_hid_device_t* d);
    static uni_error_t platformOnDeviceReady(uni_hid_device_t* d);
    static void platformOnControllerData(uni_hid_device_t* d, uni_controller_t* ctl);
    static const uni_property_t* platformGetProperty(uni_property_idx_t idx);
    static void platformOnOobEvent(uni_platform_oob_event_t event, void* data);

    // Helper methods
    static void logGamepadData(uni_hid_device_t* d, const uni_gamepad_t* gp);
    static void logControllerData(uni_hid_device_t* d, uni_controller_t* ctl);
    void processTankSteering(const uni_gamepad_t* gp);
    void processAudioControls(const uni_gamepad_t* gp);
    
    // LED status management
    void updateLEDStatus();
    static void ledUpdateTimerCallback(btstack_timer_source_t* timer);
    
    // Platform structure for BluePad32
    static struct uni_platform s_platform;
    
    // Timer for LED updates
    btstack_timer_source_t m_ledUpdateTimer;
};

} // namespace Exterminate
