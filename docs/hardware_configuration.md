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

### Board Pinout: Pico LiPo 2 XL W

The project uses the Pico LiPo 2 XL W form-factor board. Refer to the following pinout diagram when wiring peripherals and mapping GPIOs:

![Pico LiPo 2 XL W Pinout Diagram](https://cdn.shopify.com/s/files/1/0174/1800/files/ppico_lipo_2_xl_w_pinout_diagram.png?v=1747918618)

#### Notes

- The GPIO numbering in this documentation refers to the RP2350/RP2040 GPIO numbers shown on the diagram.
- Physical pad locations differ from the standard Raspberry Pi Pico; double‑check the board silkscreen when wiring.
- Power rails and special pins (VBUS, VSYS, 3V3, GND) are labeled on the diagram; ensure polarity and voltage limits are respected.

## Pin Assignment Map

### GPIO Pin Allocations

| GPIO | Function | Component | Direction | Notes |
|------|----------|-----------|-----------|-------|
| **0** | UART TX | Debug Console | Output | USB Serial |
| **1** | UART RX | Debug Console | Input | USB Serial |
| **2** | *Reserved* | Future Expansion | - | Available |
| **3** | *Reserved* | Future Expansion | - | Available |
| **4** | *Reserved* | Future Expansion | - | Available |
| **5** | *Reserved* | Future Expansion | - | Available |
| **6** | AIN1 | Left Motor Pin 1 (Motor Shim) | Output | PWM Control |
| **7** | AIN2 | Left Motor Pin 2 (Motor Shim) | Output | PWM Control |
| **6** | *Reserved* | Future Expansion | - | Available |
| **7** | *Reserved* | Future Expansion | - | Available |
| **8** | *Reserved* | Future Expansion | - | Available |
| **9** | *Reserved* | Future Expansion | - | Available |
| **10** | *Reserved* | Future Expansion | - | Available |
| **11** | LED PWM | Dome Red LED 1 (Audio Viz) | Output | PWM Control |
| **12** | LED PWM | Dome Red LED 2 (Audio Viz) | Output | PWM Control |
| **13** | *Reserved* | Future Expansion | - | Available |
| **14** | *Reserved* | Future Expansion | - | Available |
| **15** | Blue Status LED | Eye Stalk Bluetooth Status | Output | Digital Control |
| **16** | *Reserved* | Future expansion | - | - |
| **17-25** | *Reserved* | Future Expansion | - | Available |
| **26** | BIN2 | Right Motor Pin 2 (Motor Shim) | Output | PWM Control |
| **27** | BIN1 | Right Motor Pin 1 (Motor Shim) | Output | PWM Control |
| **28-31** | *Reserved* | Future Expansion | - | Available |
| **32** | I2S BCLK | Audio Bit Clock | Output | PIO I2S |
| **33** | I2S LRCLK | Audio Word Select | Output | PIO I2S |
| **34** | I2S DIN | Audio Data Out | Output | PIO I2S |
| **35** | *Reserved* | Future Expansion | - | Available |

### Power Pins

| Pin | Function | Voltage | Current |
|-----|----------|---------|---------|
| **VSYS** | System Power In | 1.8-5.5V | 500mA max |
| **VBUS** | USB Power In | 5V | 500mA max |
| **3V3** | 3.3V Power Out | 3.3V | 300mA max |
| **GND** | Ground | 0V | - |

## Component Wiring

### Pimoroni Motor Shim for Pico

The Pimoroni Motor Shim stacks directly onto the Pico LiPo 2 XL W board. Motor power and connections are handled by the shim.

**GPIO Mapping:**

```text
Pico GPIO   Motor Shim   Function
---------   ----------   -------------------------------
GPIO 6      AIN1         Left motor direction 1 (PWM)
GPIO 7      AIN2         Left motor direction 2 (PWM)
GPIO 27     BIN1         Right motor direction 1 (PWM)
GPIO 26     BIN2         Right motor direction 2 (PWM)
```

**Power & Motors:**

```text
Motor Shim   Function
----------   -----------------------------------------
VMOTOR       Motor supply input (per shim spec)
MOTOR A +/-  Left motor terminals (connect to motor)
MOTOR B +/-  Right motor terminals (connect to motor)
```

#### Notes

- The shim is soldered as a stack; no separate breadboard wiring for motor pins is required.
- Provide adequate motor supply per Pimoroni specs; do not power motors from Pico 3V3.
- Keep 20 kHz PWM to move switching out of audible range.

 

### I2S Audio Amplifier

**MAX98357A I2S Amplifier Module:** (Adafruit MAX98357A Breakout: [Adafruit product 3006](https://www.adafruit.com/product/3006))

 
```text
MAX98357A Pin Pico W      Function
------------- ------      --------
VIN      -->  5V          Power (5V from VSYS)
GND      -->  GND         Ground
BCLK     -->  GPIO 32     Bit Clock (I2S)
LRCLK    -->  GPIO 33     Left/Right Clock (I2S)
DIN      -->  GPIO 34     Data Input (I2S)
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

### Audio Visualization LEDs (Dome, Red)

**LED Array for Dalek Head Lighting (2x Red):**

```text
LED Pin      Pico W      Function
-------      ------      --------
LED 1 +  --> GPIO 37     PWM Control (Audio Intensity)
LED 2 +  --> GPIO 38     PWM Control (Audio Intensity)
All -    --> GND         Common Ground
```

**LED Requirements and Resistors (3.3 V GPIO):**

- Dome LEDs (red): 150Ω recommended (≈7–9 mA each). Use 100Ω if higher brightness is needed (≈11–13 mA), staying within GPIO limits.

- One series resistor per LED.
- Red LED (Vf ≈ 2.0–2.2 V):
   - 150Ω → ~7–9 mA (recommended default)
   - 100Ω → ~11–13 mA (upper end of per‑pin drive)
- Blue LED (Vf ≈ 3.0–3.2 V):
   - 100–150Ω → ~1–3 mA (limited headroom at 3.3 V)
- For true ~20 mA brightness:
   - Red at 3.3 V: use a transistor/MOSFET driver and 62–68Ω.
   - Blue: use a 5 V LED supply with a driver and 100Ω (common ground with Pico).

**Effects:**

- Two red dome LEDs flash in sync with audio intensity via PWM to represent speech.

### Bluetooth Status LED (Eye Stalk, Blue)

**Wiring:**

```text
Eye Stalk LED   Pico W      Function
--------------  ------      --------
LED +       -->  GPIO 36     Digital Control (Status)
LED -       -->  GND         Ground (via series resistor: blue 100–150Ω)
```

**Status Patterns:**

- **Blinking**: Bluetooth pairing/connecting
- **Solid**: Bluetooth paired/connected


## Power Distribution

### Power Requirements

| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| Pico W | 3.3V | 150mA | 0.5W |
| Motor Shim Logic | 3.3V | 10mA | 0.03W |
| Motors (2x) | 5-6V | 1A each | 5-6W |
 
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



```text
External 6V Supply
       |
    [FUSE]
       |
   +---+---+
   |       |
VSYS      Motor Shim VMOTOR
(Pico)    (Motor Power)
   |
   
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
- Add bulk capacitance near motor driver input (100µF+ at VMOTOR)
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

 

## Development Setup

### Programming Interface

**Built-in USB:**

```text
Pico W USB-C --> Computer USB-A/C
```

**Debug Interface (Optional SWD):**

```text
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
- Motor driver includes protection diodes (per Motor Shim spec)
- Add external capacitors if using long motor leads
- Monitor motor current to prevent overheating

### Mechanical Safety

**Moving Parts:**

- Ensure all rotating parts are properly guarded
 
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

1. Verify Motor Shim VMOTOR power and ground connections
2. Check PWM signals with oscilloscope/multimeter
3. Test motor continuity and resistance
4. Ensure adequate power supply current

 

**Audio Issues:**

1. Verify I2S DAC power and connections
2. Check I2S signal timing with oscilloscope
3. Test with known-good audio source
4. Verify speaker connections and impedance

### Debug Test Points

**Power Rails:**

- VSYS: Should be 5V (USB) or external supply voltage
- 3V3: Should be 3.3V ±5%
- Motor Shim logic VCC: Should match 3V3

**PWM Signals:**

- GPIO 6,7,26,27: Should show PWM when motors active
 
- GPIO 32-34: Should show I2S signals when audio playing

This hardware configuration provides a robust foundation for the Exterminate Dalek project with clear upgrade paths and comprehensive safety considerations.
