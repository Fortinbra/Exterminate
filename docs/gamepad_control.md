# Gamepad Control System

## Overview

The Gamepad Control System integrates Bluetooth gamepad input with the Exterminate Dalek's movement and control systems. It uses the BluePad32 library to provide wireless control via various gaming controllers.

## Supported Controllers

The system supports a wide range of Bluetooth gamepads through BluePad32:

- **Sony PlayStation Controllers**: PS3, PS4, PS5 DualSense
- **Microsoft Xbox Controllers**: Xbox One, Xbox Series X/S
- **Nintendo Controllers**: Joy-Con, Pro Controller, Wii Remote
- **Generic Controllers**: Most Bluetooth HID gamepad devices
- **Retro Controllers**: 8BitDo, RetroFlag, and other retro-style controllers

## Hardware Setup

### Bluetooth Configuration

The Raspberry Pi Pico W's CYW43 wireless chip provides Bluetooth connectivity:

```cpp
// Bluetooth is automatically enabled when CYW43_ENABLE_BLUETOOTH == 1
if (cyw43_arch_init()) {
    printf("Failed to initialize CYW43 wireless\n");
    return -1;
}
```

**No additional hardware required** - the Pico W has built-in Bluetooth.

## Pairing Controllers

### Automatic Pairing Mode

The Exterminate system automatically enters pairing mode on startup:

1. **Power on the Dalek**
2. **Put your controller in pairing mode**:
   - **PS4/PS5**: Hold Share + PS button until light flashes
   - **Xbox**: Hold Xbox + Menu buttons until light flashes rapidly
   - **Joy-Con**: Hold side button for 3 seconds
3. **Wait for connection** - LED will turn off when connected

### Controller Status

- **LED ON**: Controller connected and active
- **LED OFF**: No controller connected or system ready
- **LED BLINKING**: Controller connecting/disconnecting

## Control Mapping

### Movement Controls

| Control | Function | Range |
|---------|----------|-------|
| **Left Stick Y** | Forward/Backward | -1.0 to 1.0 |
| **Left Stick X** | Turn Left/Right | -1.0 to 1.0 |
| **Button A** | LED Indicator | Press/Release |
| **Button B** | Emergency Stop | Press |

### Advanced Controls

```cpp
// Left stick controls differential drive
float forward = leftStick.y;  // Forward/backward speed
float turn = leftStick.x;     // Turning rate

// Differential drive calculation:
// Left Motor  = forward - turn
// Right Motor = forward + turn
```

### Control Characteristics

- **Deadzone**: 10% stick deadzone prevents controller drift
- **Response**: Real-time response (~1ms latency)
- **Smoothing**: Raw stick values, no additional filtering
- **Range**: Full -1.0 to +1.0 speed range available

## Safety Features

### Automatic Safety Systems

1. **Disconnect Safety**: Motors automatically stop when controller disconnects
2. **Emergency Stop**: B button immediately stops all motors
3. **System Button**: Home/PS button triggers emergency stop
4. **Startup Safety**: Motors remain stopped until controller input received

### Failsafe Behavior

```cpp
// Emergency stop scenarios
void emergencyStop() {
    motorController->stopAllMotors();
    logi("Emergency stop activated!\n");
}

// Triggered by:
// - Controller disconnection
// - B button press  
// - System button press
// - Communication timeout
```

## System Integration

### Platform Architecture

The gamepad system is implemented as a custom BluePad32 platform:

```cpp
// Platform callbacks handle all gamepad events
static const struct uni_platform exterminate_platform = {
    .name = "Exterminate",
    .on_device_connected = exterminate_platform_on_device_connected,
    .on_device_disconnected = exterminate_platform_on_device_disconnected,
    .on_controller_data = exterminate_platform_on_controller_data,
    // ... other callbacks
};
```

### Real-time Processing

```cpp
// Controller data processed in real-time callback
void exterminate_platform_on_controller_data(uni_controller_t* ctl) {
    // Get normalized joystick values
    float forward = -static_cast<float>(ctl->gamepad.axis_y) / 512.0f;
    float turn = static_cast<float>(ctl->gamepad.axis_x) / 512.0f;
    
    // Apply deadzone
    if (abs(forward) < 0.1f) forward = 0.0f;
    if (abs(turn) < 0.1f) turn = 0.0f;
    
    // Control motors
    motorController->setDifferentialDrive(forward, turn);
}
```

## Configuration

### Controller Sensitivity

Adjust sensitivity by modifying the normalization factors:

```cpp
// Standard sensitivity (default)
float forward = -axis_y / 512.0f;    // Full range
float turn = axis_x / 512.0f;        // Full range

// Reduced sensitivity
float forward = -axis_y / 1024.0f;   // Half sensitivity
float turn = axis_x / 1024.0f;       // Half sensitivity

// Increased sensitivity (careful!)
float forward = -axis_y / 256.0f;    // Double sensitivity
float turn = axis_x / 256.0f;        // Double sensitivity
```

### Deadzone Adjustment

```cpp
// Conservative deadzone (less sensitive)
const float deadzone = 0.2f;  // 20% deadzone

// Aggressive deadzone (more sensitive)  
const float deadzone = 0.05f; // 5% deadzone

// No deadzone (may drift)
const float deadzone = 0.0f;  // No deadzone
```

## Troubleshooting

### Connection Issues

**Controller won't pair:**
1. Ensure controller is in pairing mode
2. Check controller battery level
3. Try power cycling the Pico W
4. Verify Bluetooth is enabled in build configuration

**Controller connects but doesn't work:**
1. Check serial output for error messages
2. Verify motor controller initialization
3. Test with known-good controller
4. Check for GPIO pin conflicts

### Performance Issues

**Laggy response:**
1. Check for interference from other 2.4GHz devices
2. Reduce distance between controller and Pico W
3. Ensure adequate power supply
4. Monitor serial output for communication errors

**Inconsistent movement:**
1. Adjust deadzone settings
2. Check motor power supply stability
3. Verify controller stick calibration
4. Test with different controller

### Debug Information

Enable debug output for troubleshooting:

```cpp
// In exterminate_platform.cpp, uncomment:
uni_controller_dump(ctl);  // Prints all controller data

// This will show:
// - Button states
// - Joystick positions  
// - Trigger values
// - Connection status
```

## Extending Controller Support

### Adding Custom Button Mappings

```cpp
// Add new button functions
if (gp->buttons & BUTTON_X) {
    // Trigger audio playback
    audioController->playSound(SOUND_EXTERMINATE);
}

// (Servo support removed from this version)
```

### Multiple Controller Support

BluePad32 supports up to 4 simultaneous controllers:

```cpp
// Handle multiple controllers
void exterminate_platform_on_controller_data(uni_hid_device_t* d, uni_controller_t* ctl) {
    int controllerIndex = uni_hid_device_get_idx(d);
    
    switch (controllerIndex) {
        case 0: // Primary controller - movement
            handleMovementController(ctl);
            break;
        case 1: // Secondary controller - audio/effects
            handleEffectsController(ctl);
            break;
    }
}
```

## Performance Specifications

- **Latency**: <10ms from controller input to motor response
- **Update Rate**: 100Hz+ (limited by controller polling rate)
- **Range**: ~10m typical Bluetooth range
- **Battery Life**: Controller-dependent (typically 8-40 hours)
- **Concurrent Controllers**: Up to 4 supported
- **Memory Usage**: ~2KB RAM for controller state

The gamepad control system provides responsive, reliable wireless control that brings the Exterminate Dalek to life with intuitive, game-like controls.
