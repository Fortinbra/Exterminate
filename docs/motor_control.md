# Motor Control System

## Overview

The Motor Control System provides differential drive control for the Exterminate Dalek robot using the Pimoroni Motor Shim for Pico (stacked on the Pico LiPo 2 XL W). This system enables forward/backward movement, turning, and rotation in place.

## Hardware Setup

### Pimoroni Motor Shim for Pico

The Motor Shim is a dual H-bridge driver board that stacks on the Pico and exposes motor terminals and VMOTOR input. It routes control pins to Pico GPIO as follows:

```text
Pico GPIO | Shim Signal | Function
----------|-------------|---------
GPIO 6    | AIN1        | Left Motor Direction 1 (PWM)
GPIO 7    | AIN2        | Left Motor Direction 2 (PWM)
GPIO 27   | BIN1        | Right Motor Direction 1 (PWM)
GPIO 26   | BIN2        | Right Motor Direction 2 (PWM)
```

Power and motors connect directly to the shim:

```text
Motor Shim   Function
----------   -----------------------------------------
VMOTOR       Motor supply input (per shim spec)
MOTOR A +/-  Left motor terminals (connect to motor)
MOTOR B +/-  Right motor terminals (connect to motor)
```

### Motor Connections

Connect your DC motors to the shim’s motor output terminals:

- **Motor A (Left)**: MOTOR A +/−
- **Motor B (Right)**: MOTOR B +/−

**Motor Polarity**: If a motor spins in the wrong direction, swap the two wires for that motor.

## Software Interface

### Basic Usage

```cpp
#include "MotorController.h"

// Configure motor controller
Exterminate::MotorController::Config config{};
config.leftMotorPin1 = 6;    // GPIO 6 -> AIN1 (Motor Shim)
config.leftMotorPin2 = 7;    // GPIO 7 -> AIN2 (Motor Shim)
config.rightMotorPin1 = 27;  // GPIO 27 -> BIN1 (Motor Shim)
config.rightMotorPin2 = 26;  // GPIO 26 -> BIN2 (Motor Shim)
config.pwmFrequency = 20000; // 20 kHz PWM (default used in code)

// Create motor controller instance
Exterminate::MotorController motors(config);

// Initialize hardware
if (!motors.initialize()) {
    // Handle initialization error
}

// Basic movement examples using the public API
motors.setMotorSpeed(Exterminate::MotorController::Motor::LEFT, 0.5f);   // Left forward 50%
motors.setMotorSpeed(Exterminate::MotorController::Motor::RIGHT, 0.5f);  // Right forward 50%

// Stop all motors
motors.stopAllMotors();
```

### Individual Motor Control

```cpp
// Control motors independently
motors.setMotorSpeed(Exterminate::MotorController::Motor::LEFT, 0.7f);   // Left motor 70% forward
motors.setMotorSpeed(Exterminate::MotorController::Motor::RIGHT, -0.5f); // Right motor 50% backward
```

### Differential Drive Control

```cpp
// Advanced differential drive
// forward: forward/backward speed (-1.0 to 1.0)
// turn: rotation rate (-1.0 to 1.0)
motors.setDifferentialDrive(0.5f, 0.2f);  // Forward with slight right turn
```

## Motor Control Theory

### PWM Control

The motor controller uses PWM (Pulse Width Modulation) to control motor speed:

- **Frequency**: 10kHz (configurable)
- **Duty Cycle**: 0-100% controls speed
- **Direction**: Controlled by which pins are PWM vs static

### Direction Control Modes

| AIN1 | AIN2 | Motor A Action |
|------|------|----------------|
| PWM  | 0    | Forward (PWM speed) |
| 0    | PWM  | Reverse (PWM speed) |
| 0    | 0    | Coast (free spin) |
| 1    | 1    | Brake (short circuit) |

### Differential Drive Kinematics

For a two-wheeled robot:

```text
Left Motor Speed  = Linear Speed - Angular Speed
Right Motor Speed = Linear Speed + Angular Speed
```

This allows:

- **Forward**: Both motors same speed, same direction
- **Backward**: Both motors same speed, opposite direction  
- **Turn**: Motors different speeds
- **Rotate**: Motors same speed, opposite directions

## Safety Features

### Current and Thermal Considerations

Refer to the Pimoroni Motor Shim specifications for peak and continuous current limits and thermal behavior. Ensure your motors and supply are within ratings.

### Software Notes

```cpp
// Speed clamping (automatic within [-1.0, 1.0])
motors.setMotorSpeed(Exterminate::MotorController::Motor::LEFT, 1.5f);  // Clamped to 1.0f

// Stop all motors
motors.stopAllMotors();
```

## Troubleshooting

### Common Issues

**Motor doesn't move:**

1. Check power supply to VMOT (6-10.8V)
2. Verify motor connections
3. Check GPIO pin assignments
4. Ensure sufficient current capacity

**Motor moves wrong direction:**

1. Swap the two motor wires
2. Or invert the speed value in software

**Jerky movement:**

1. Lower PWM frequency (try 1kHz)
2. Add motor capacitors if not present
3. Check power supply stability

**Motors overheat:**

1. Reduce continuous speed
2. Add heatsink to DRV8833
3. Check for mechanical binding
4. Verify current draw

### Debug Output

Enable debug output to monitor motor state:

```cpp
#define MOTOR_DEBUG 1  // In your build configuration

// This will print motor speeds and directions to UART
```

## Performance Specifications

- **Response Time**: <1ms for speed changes
- **Speed Resolution**: 16-bit PWM (65,536 levels)
- **Update Rate**: Real-time (limited only by PWM frequency)
- **Memory Usage**: ~200 bytes RAM
- **CPU Usage**: Minimal (hardware PWM)

## Integration with Gamepad

The motor controller integrates seamlessly with the gamepad system:

```cpp
// In your main control loop
void handleGamepadInput(const GamepadState& gamepad) {
    // Left stick for movement
    float linear = gamepad.leftStick.y;   // Forward/backward
    float angular = gamepad.leftStick.x;  // Left/right
    
    motors.setDifferentialDrive(linear, angular);
    
    // Right trigger for turbo mode
    if (gamepad.rightTrigger > 0.5f) {
        motors.setSpeedMultiplier(1.5f);  // 150% speed
    } else {
        motors.setSpeedMultiplier(1.0f);  // Normal speed
    }
}
```

This motor control system provides precise, responsive control perfect for bringing your Exterminate Dalek to life with smooth, realistic movement patterns.
