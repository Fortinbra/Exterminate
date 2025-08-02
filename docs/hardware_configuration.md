# Hardware Configuration Guide

## Overview

This guide covers the complete hardware setup for the Exterminate Dalek project, including pin assignments, wiring diagrams, and component specifications.

## Core Hardware

### Raspberry Pi Pico W

The Raspberry Pi Pico W serves as the main microcontroller with built-in WiFi and Bluetooth capabilities.

**Key Features:**
- **CPU**: Dual-core ARM Cortex-M0+ @ 133MHz
- **Memory**: 264KB SRAM, 2MB Flash
- **I/O**: 26 GPIO pins, 3 ADC inputs
- **Connectivity**: 802.11n WiFi, Bluetooth 5.2
- **Power**: 1.8-5.5V input, 3.3V logic levels

## Pin Assignment Map

### GPIO Pin Allocations

| GPIO | Function | Component | Direction | Notes |
|------|----------|-----------|-----------|-------|
| **0** | UART TX | Debug Console | Output | USB Serial |
| **1** | UART RX | Debug Console | Input | USB Serial |
| **2** | AIN1 | Left Motor Pin 1 | Output | DRV8833 PWM |
| **3** | AIN2 | Left Motor Pin 2 | Output | DRV8833 PWM |
| **4** | BIN1 | Right Motor Pin 1 | Output | DRV8833 PWM |
| **5** | BIN2 | Right Motor Pin 2 | Output | DRV8833 PWM |
| **6** | I2S BCLK | Audio Bit Clock | Output | PIO I2S |
| **7** | *Reserved* | Future Expansion | - | Available |
| **8** | I2S LRCLK | Audio Word Select | Output | PIO I2S |
| **9** | I2S DIN | Audio Data Out | Output | PIO I2S |
| **10** | *Reserved* | Future Expansion | - | Available |
| **11** | LED PWM | Audio Viz LED 1 | Output | PWM Control |
| **12** | LED PWM | Audio Viz LED 2 | Output | PWM Control |
| **13** | LED PWM | Audio Viz LED 3 | Output | PWM Control |
| **14** | LED PWM | Audio Viz LED 4 | Output | PWM Control |
| **15** | Status LED | Controller Status | Output | Digital Control |
| **16** | Servo PWM | Eye Stalk Servo | Output | 50Hz PWM |
| **17-25** | *Reserved* | Future Expansion | - | Available |

### Power Pins

| Pin | Function | Voltage | Current |
|-----|----------|---------|---------|
| **VSYS** | System Power In | 1.8-5.5V | 500mA max |
| **VBUS** | USB Power In | 5V | 500mA max |
| **3V3** | 3.3V Power Out | 3.3V | 300mA max |
| **GND** | Ground | 0V | - |

## Component Wiring

### DRV8833 Motor Driver

**Power Connections:**
```
Pico W      DRV8833     Function
------      -------     --------
3V3    -->  VCC         Logic Power (3.3V)
GND    -->  GND         Ground
VSYS   -->  VMOT        Motor Power (5V from USB)
```

**Control Connections:**
```
Pico W      DRV8833     Function
------      -------     --------
GPIO 2 -->  AIN1        Left Motor Direction 1
GPIO 3 -->  AIN2        Left Motor Direction 2
GPIO 4 -->  BIN1        Right Motor Direction 1
GPIO 5 -->  BIN2        Right Motor Direction 2
```

**Motor Connections:**
```
DRV8833     Motor       Function
-------     -----       --------
AO1    -->  Left +      Left Motor Terminal 1
AO2    -->  Left -      Left Motor Terminal 2
BO1    -->  Right +     Right Motor Terminal 1
BO2    -->  Right -     Right Motor Terminal 2
```

### Eye Stalk Servo

**Standard Servo Wiring:**
```
Servo Wire  Pico W      Function
----------  ------      --------
Red    -->  VSYS        Power (5V)
Brown  -->  GND         Ground
Orange -->  GPIO 16     PWM Signal
```

**PWM Signal Requirements:**
- **Frequency**: 50Hz (20ms period)
- **Pulse Width**: 1-2ms (1ms = 0°, 1.5ms = 90°, 2ms = 180°)
- **Logic Level**: 3.3V (compatible with most 5V servos)

### I2S Audio Amplifier

**MAX98357A I2S Amplifier Module:**

```text
MAX98357A Pin Pico W      Function
------------- ------      --------
VIN      -->  5V          Power (5V from VSYS)
GND      -->  GND         Ground
BCLK     -->  GPIO 6      Bit Clock (PIO-generated)
LRCLK    -->  GPIO 8      Left/Right Clock (PIO-generated)
DIN      -->  GPIO 9      Data Input (PIO-generated)
GAIN     -->  GND         Gain Setting (9dB when tied to GND)
SD       -->  3V3         Shutdown Control (3V3 = enabled)
```

**Speaker Connection:**
```text
Amplifier     Speaker
---------     -------
Speaker+ -->  Speaker Positive Terminal (4-8Ω, 3W max)
Speaker- -->  Speaker Negative Terminal
```

**MAX98357A Advantages:**
- **Integrated Solution**: DAC + Class D amplifier in one module
- **High Efficiency**: Class D amplifier design for low power consumption
- **Direct Speaker Drive**: No external amplifier required
- **I2S Interface**: Clean digital audio interface
- **Compact**: Small form factor suitable for embedded projects

### Audio Visualization LEDs

**LED Array for Dalek Head Lighting:**
```text
LED Pin      Pico W      Function
-------      ------      --------
LED 1 +  --> GPIO 11     PWM Control (Audio Intensity)
LED 2 +  --> GPIO 12     PWM Control (Audio Intensity)
LED 3 +  --> GPIO 13     PWM Control (Audio Intensity)
LED 4 +  --> GPIO 14     PWM Control (Audio Intensity)
All -    --> GND         Common Ground (via 220Ω resistors)
```

**LED Requirements:**
- **Type**: Standard 5mm or 3mm LEDs (red/orange for Dalek aesthetic)
- **Current**: ~20mA per LED maximum
- **Resistors**: 220Ω current limiting resistors for 3.3V operation
- **Effects**: Real-time audio-reactive intensity control via PWM

### Controller Status LED

**Status Indication LED:**
```text
Status LED   Pico W      Function
----------   ------      --------
LED +   -->  GPIO 15     Digital Control (On/Off/Flash)
LED -   -->  GND         Ground (via 220Ω resistor)
```

**Status Patterns:**
- **Solid On**: Controller connected and paired
- **Flashing**: Waiting for controller connection (500ms interval)
- **Off**: System starting up or error state
```
DAC Module  Pico W      Function
----------  ------      --------
VCC    -->  3V3         Power (3.3V)
GND    -->  GND         Ground
BCLK   -->  GPIO 18     Bit Clock
DOUT   -->  GPIO 19     Data Output
LRCLK  -->  GPIO 20     Left/Right Clock
```

**Audio Output:**
```
DAC Module  Output      Function
----------  ------      --------
LOUT   -->  Speaker +   Left Audio Channel
ROUT   -->  Speaker +   Right Audio Channel (or bridge)
GND    -->  Speaker -   Audio Ground
```

## Power Distribution

### Power Requirements

| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| Pico W | 3.3V | 150mA | 0.5W |
| DRV8833 Logic | 3.3V | 10mA | 0.03W |
| Motors (2x) | 5-6V | 1A each | 5-6W |
| Servo | 5V | 500mA | 2.5W |
| Audio DAC | 3.3V | 50mA | 0.17W |
| **Total** | - | **~2.7A** | **~8.2W** |

### Recommended Power Supply

**For USB Development:**
- **USB Power**: 5V @ 500mA (limited)
- **Suitable for**: Testing, programming, light motors
- **Limitations**: May brownout with full motor load

**For Full Operation:**
- **External Supply**: 6V @ 3A minimum
- **Connection**: Via VSYS pin or external power distribution
- **Protection**: Fuse or current limiting recommended

### Power Distribution Schematic

```
External 6V Supply
       |
    [FUSE]
       |
   +---+---+
   |       |
VSYS      DRV8833 VMOT
(Pico)    (Motor Power)
   |
   +--- Servo Power
```

## Assembly Guidelines

### PCB Layout Considerations

**Signal Routing:**
- Keep PWM traces short and direct
- Separate analog and digital grounds where possible
- Use ground planes for EMI reduction
- Route I2S signals with controlled impedance

**Power Routing:**
- Use thick traces for motor power (≥20 mil)
- Add bulk capacitance near DRV8833 (100µF+)
- Decouple all IC power pins (0.1µF ceramic)
- Consider separate power planes for logic and motors

### Mechanical Mounting

**Pico W Mounting:**
- Use M2.5 standoffs to prevent board flex
- Ensure adequate ventilation around CYW43 chip
- Keep antenna area clear of metal objects

**Motor Mounting:**
- Secure motors to prevent vibration
- Use flexible wires to accommodate movement
- Add encoder feedback if precise positioning needed

**Servo Mounting:**
- Mount servo horn securely to eye stalk mechanism
- Ensure full range of motion without binding
- Use appropriate torque limits to prevent damage

## Development Setup

### Programming Interface

**Built-in USB:**
```
Pico W USB-C --> Computer USB-A/C
```

**Debug Interface (Optional SWD):**
```
Pico W      Debugger    Function
------      --------    --------
SWCLK  -->  SWCLK       SWD Clock
SWDIO  -->  SWDIO       SWD Data
GND    -->  GND         Ground
```

### Development Tools Required

**Hardware:**
- USB-C cable for programming
- Breadboard or PCB for prototyping
- Multimeter for debugging
- Oscilloscope (optional, for PWM verification)

**Software:**
- Pico SDK toolchain
- VS Code with Pico extension
- Ninja build system
- Serial terminal (minicom, PuTTY, etc.)

## Safety Considerations

### Electrical Safety

**Power Supply:**
- Never exceed 5.5V on VSYS pin
- Use current limiting to prevent overcurrent
- Add reverse polarity protection
- Ensure adequate heat dissipation

**Motor Safety:**
- Motors can generate back-EMF when stopped suddenly
- DRV8833 includes protection diodes
- Add external capacitors if using long motor leads
- Monitor motor current to prevent overheating

### Mechanical Safety

**Moving Parts:**
- Ensure all rotating parts are properly guarded
- Use appropriate servo torque limits
- Implement software position limits
- Add manual emergency stop capability

**Structural:**
- Mount all components securely
- Use appropriate fasteners for loads
- Consider vibration and shock resistance
- Plan for cable management

## Troubleshooting

### Common Hardware Issues

**No Power:**
1. Check USB connection and cable
2. Verify power supply voltage and current capacity
3. Look for short circuits or component damage
4. Measure voltages at key test points

**Motors Not Working:**
1. Verify DRV8833 power and ground connections
2. Check PWM signals with oscilloscope/multimeter
3. Test motor continuity and resistance
4. Ensure adequate power supply current

**Servo Not Responding:**
1. Check 5V power supply to servo
2. Verify PWM signal frequency (50Hz)
3. Test servo with known-good controller
4. Check for mechanical binding

**Audio Issues:**
1. Verify I2S DAC power and connections
2. Check I2S signal timing with oscilloscope
3. Test with known-good audio source
4. Verify speaker connections and impedance

### Debug Test Points

**Power Rails:**
- VSYS: Should be 5V (USB) or external supply voltage
- 3V3: Should be 3.3V ±5%
- DRV8833 VCC: Should match 3V3

**PWM Signals:**
- GPIO 2-5: Should show PWM when motors active
- GPIO 16: Should show 50Hz PWM when servo active
- GPIO 18-20: Should show I2S signals when audio playing

This hardware configuration provides a robust foundation for the Exterminate Dalek project with clear upgrade paths and comprehensive safety considerations.
