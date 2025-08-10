# Motor Control System

## Overview

The Motor Control System provides differential drive control for the Exterminate Dalek robot using a DRV8833 dual H-bridge motor driver. This system enables forward/backward movement, turning, and rotation in place.

## Hardware Setup

### DRV8833 Motor Driver

The DRV8833 is a dual H-bridge motor driver that can control two DC motors with speeds up to 1.2A continuous current per motor.

**Pin Connections:**
```
Pico Pin  | DRV8833 Pin | Function
----------|-------------|----------
GPIO 3    | AIN1        | Left Motor Direction 1
GPIO 2    | AIN2        | Left Motor Direction 2
GPIO 4    | BIN1        | Right Motor Direction 1
GPIO 5    | BIN2        | Right Motor Direction 2
3.3V      | VCC         | Logic Power Supply
GND       | GND         | Ground
VBAT      | VMOT        | Motor Power Supply (6-10.8V)
```

### Motor Connections

Connect your DC motors to the motor output terminals:
- **Motor A (Left)**: AO1 and AO2 terminals
- **Motor B (Right)**: BO1 and BO2 terminals

**Motor Polarity**: If a motor spins in the wrong direction, swap the two wires for that motor.

## Software Interface

### Basic Usage

```cpp
#include "MotorController.h"

// Configure motor controller
Exterminate::MotorController::Config config{};
config.leftMotorPin1 = 3;    // GPIO 3 -> AIN1
config.leftMotorPin2 = 2;    // GPIO 2 -> AIN2
config.rightMotorPin1 = 4;   // GPIO 4 -> BIN1
config.rightMotorPin2 = 5;   // GPIO 5 -> BIN2
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

### Current Limiting

The DRV8833 includes built-in current limiting and thermal protection:

- **Peak Current**: 2A per motor (brief)
- **Continuous Current**: 1.2A per motor
- **Thermal Shutdown**: Automatic at ~150°C

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
