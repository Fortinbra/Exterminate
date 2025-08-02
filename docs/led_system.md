# LED System Documentation

This document describes the LED controller system in the Exterminate project, including audio visualization LEDs and controller status indication.

## Overview

The LED system provides visual feedback through two distinct subsystems:

1. **Audio Visualization LEDs**: Real-time audio-reactive lighting for Dalek head effects
2. **Controller Status LED**: Visual indication of gamepad connection status

## Audio Visualization LEDs

### Purpose

The audio visualization LEDs simulate the iconic flashing lights on a Dalek's head that pulse in sync with speech and sound effects.

### Hardware Configuration

- **GPIO Pins**: 11, 12, 13, 14 (4 LEDs total)
- **Control Method**: PWM-based intensity control
- **LED Type**: Standard 5mm red/orange LEDs with 220Ω current limiting resistors
- **Update Rate**: Real-time synchronization with audio playback

### LED Effects

The `LEDController` class supports multiple effect patterns:

#### PULSE Effect
- All LEDs pulse together with audio intensity
- Suitable for single-voice Dalek speech
- Intensity based on real-time audio level

#### ALTERNATE Effect  
- LEDs alternate in pairs (1,3 vs 2,4)
- Creates dynamic back-and-forth lighting
- Good for dramatic sound effects

#### WAVE Effect
- Sequential LED activation creating a wave pattern
- LEDs light up in sequence: 1→2→3→4→1...
- Adds movement to the lighting display

#### BREATHE Effect
- Smooth fade in/out pattern independent of audio
- Creates atmospheric "breathing" effect
- Can run continuously as background lighting

### Configuration Example

```cpp
#include "LEDController.h"

// Configure LED controller
LEDController::Config config = {
    .ledPins = {11, 12, 13, 14},  // GPIO pins for LEDs
    .numLEDs = 4,                 // Number of LEDs
    .pwmFrequency = 1000          // 1kHz PWM frequency
};

LEDController ledController;
ledController.begin(config);

// Set audio-reactive effect
ledController.setEffect(LEDController::Effect::PULSE);
ledController.setBrightness(0.8f);  // 80% maximum brightness

// In main loop - update with audio intensity
float audioIntensity = audioController.getAudioIntensity();
ledController.updateLEDs(audioIntensity);
```

### Audio Integration

The LED system integrates with the `AudioController` to provide real-time audio visualization:

```cpp
// Real-time audio intensity calculation
float intensity = audioController.getAudioIntensity();

// Update LEDs with current audio level
ledController.updateLEDs(intensity);

// The intensity value ranges from 0.0 (silent) to 1.0 (maximum)
```

## Controller Status LED

### Purpose

Provides immediate visual feedback about Bluetooth controller connection status, essential for user experience.

### Hardware Configuration

- **GPIO Pin**: 15
- **Control Method**: Digital on/off with software-controlled flashing
- **LED Type**: Standard indicator LED with 220Ω current limiting resistor
- **Update Rate**: 500ms flash interval when waiting for connection

### Status Patterns

| LED State | Controller Status | Description |
|-----------|------------------|-------------|
| **Solid On** | Connected | Controller paired and actively connected |
| **Flashing** | Waiting | System ready, waiting for controller pairing |
| **Off** | Disconnected | System starting up or controller disconnected |

### Implementation

The controller status LED is managed in `exterminate_platform.cpp`:

```cpp
// GPIO pin definition
const uint CONTROLLER_STATUS_LED_PIN = 15;

// Status tracking
static bool controller_connected = false;
static bool led_flash_state = false;
static absolute_time_t last_flash_time;

// Initialize status LED
void init_controller_status_led() {
    gpio_init(CONTROLLER_STATUS_LED_PIN);
    gpio_set_dir(CONTROLLER_STATUS_LED_PIN, GPIO_OUT);
    gpio_put(CONTROLLER_STATUS_LED_PIN, false);
    last_flash_time = get_absolute_time();
}

// Update LED based on controller status
void update_controller_status_led() {
    if (controller_connected) {
        // Solid on when connected
        gpio_put(CONTROLLER_STATUS_LED_PIN, true);
    } else {
        // Flash when waiting for connection
        absolute_time_t current_time = get_absolute_time();
        if (absolute_time_diff_us(last_flash_time, current_time) >= 500000) {
            led_flash_state = !led_flash_state;
            gpio_put(CONTROLLER_STATUS_LED_PIN, led_flash_state);
            last_flash_time = current_time;
        }
    }
}

// Called when controller connects
void on_controller_connected() {
    controller_connected = true;
    update_controller_status_led();
}

// Called when controller disconnects  
void on_controller_disconnected() {
    controller_connected = false;
    update_controller_status_led();
}
```

## Wiring Diagrams

### Audio Visualization LEDs

```text
Pico W          LED Array
------          ---------
GPIO 11  -----> LED 1 Anode
GPIO 12  -----> LED 2 Anode  
GPIO 13  -----> LED 3 Anode
GPIO 14  -----> LED 4 Anode
GND      -----> All LED Cathodes (via 220Ω resistors)
```

### Controller Status LED

```text
Pico W          Status LED
------          ----------
GPIO 15  -----> LED Anode
GND      -----> LED Cathode (via 220Ω resistor)
```

### Audio System Integration

```text
Pico W          MAX98357A I2S Amplifier
------          -----------------------
GPIO 6   -----> BCLK (Bit Clock)
GPIO 8   -----> LRCLK (Left/Right Clock)
GPIO 9   -----> DIN (Data Input)
5V       -----> VIN (Power)
GND      -----> GND
3V3      -----> SD (Shutdown - Active High)
```

## Power Considerations

### Current Draw

- **Audio LEDs**: ~20mA × 4 = 80mA maximum (all LEDs at full brightness)
- **Status LED**: ~20mA maximum
- **Total LED Current**: ~100mA maximum

### PWM Frequency

- **Audio LEDs**: 1kHz PWM frequency for smooth intensity control
- **Status LED**: Simple digital on/off (no PWM required)

### Resistor Calculations

For 3.3V GPIO output with standard LEDs (2.0V forward voltage):

```text
Resistor Value = (3.3V - 2.0V) / 0.020A = 65Ω minimum
Recommended: 220Ω for safety margin and longer LED life
```

## Troubleshooting

### Audio LEDs Not Responding

1. **Check Wiring**: Verify GPIO connections and resistor values
2. **Audio Integration**: Ensure `getAudioIntensity()` returns valid values
3. **PWM Configuration**: Verify PWM slice initialization
4. **Effect Selection**: Confirm correct effect is set

### Controller Status LED Issues

1. **GPIO Configuration**: Verify GPIO 15 is properly initialized as output
2. **BluePad32 Integration**: Check controller callback functions
3. **Timing**: Verify flash timing logic for waiting state
4. **Connection Logic**: Test controller pairing process

### Performance Issues

1. **Update Rate**: Don't update LEDs faster than necessary (~30-60Hz)
2. **PWM Frequency**: Keep PWM frequency high enough to avoid visible flicker
3. **Intensity Calculation**: Optimize audio intensity calculations for real-time performance

## Future Enhancements

### Planned Features

- **Color LED Support**: RGB LEDs for multi-color effects
- **Advanced Patterns**: More sophisticated audio visualization algorithms
- **Brightness Control**: Automatic brightness adjustment based on ambient light
- **Remote Control**: Web interface for LED effect configuration

### Hardware Expansions

- **LED Strips**: Support for addressable LED strips (WS2812B)
- **Multiple Zones**: Independent control of different LED groups
- **Sensors**: Light sensors for automatic brightness adjustment
- **Effects Engine**: More complex pattern generation and sequencing
