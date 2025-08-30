# Getting Started Guide

## Project Overview

Welcome to the Exterminate Dalek project! This guide will help you get started with building and programming your own animatronic Dalek using a Raspberry Pi Pico W.

**What You're Building:**

- A Bluetooth-controlled animatronic Dalek
- Differential drive movement system
- Sound effects with I2S audio output  
- Eye stalk servo control
- Responsive gamepad controls

## What You Need

### Essential Hardware

**Microcontroller:**

- 1x Raspberry Pi Pico W (RP2350 recommended)
- 1x USB-C cable for programming

**Motor System:**

- 1x Pimoroni Motor SHIM for Pico (DRV8833-based dual motor driver board). See: [Pimoroni Motor SHIM for Pico](https://shop.pimoroni.com/products/motor-shim-for-pico)
- 2x DC geared motors (6V, up to 1A each)
- 1x Servo motor (standard 3-wire, 5V)
- Wheels and chassis materials

**Audio System:**

- 1x I2S DAC module (PCM5102A recommended)
- 1x Small speaker (4-8Ω, 3-5W)
- Audio amplifier (optional for louder output)

**Power Supply:**

- Battery pack (6-12V, 2A minimum)
- Voltage regulator for 5V rail
- Power switch and indicator LED

**Assembly Materials:**

- Breadboard or custom PCB
- Jumper wires and connectors
- Mounting hardware and chassis
- 3D printed or fabricated Dalek shell

### Development Tools

**Required Software:**

- Windows 10/11, macOS, or Linux
- Visual Studio Code
- Git version control

**Gamepad Support:**

- PlayStation 4/5 controller, or
- Xbox One/Series controller, or  
- Nintendo Switch Pro controller, or
- Generic Bluetooth gamepad

## Quick Start (30 Minutes)

### Step 1: Get the Code

**Clone the Repository:**

```bash
git clone https://github.com/your-username/Exterminate.git
cd Exterminate
git submodule update --init --recursive
```

**Open in VS Code:**

```bash
code .
```

### Step 2: Install Development Environment

**VS Code Extensions:**

When you open the project, VS Code will prompt to install recommended extensions:

- Raspberry Pi Pico (essential)
- C/C++ Extension Pack
- CMake Tools

Click "Install All" when prompted.

**Pico SDK Installation:**

The Raspberry Pi Pico extension will automatically download and configure the SDK. This may take a few minutes.

### Step 3: Build the Project

**Using VS Code:**

1. Open Command Palette (Ctrl+Shift+P)
2. Type "CMake: Configure" and press Enter
3. Select "Pico ARM GCC" when prompted
4. Type "CMake: Build" and press Enter

**Expected Output:**
```
[build] Building folder: Exterminate
[build] Build finished with exit code 0
```

### Step 4: Basic Hardware Setup

**Minimal Test Setup:**

For initial testing, you only need:

1. Raspberry Pi Pico W
2. USB cable connected to computer
3. Optional: LED on GPIO 25 (built-in LED)

**Test Flash:**

1. Hold BOOTSEL button on Pico W
2. Connect USB cable
3. In VS Code: Ctrl+Shift+P → "Raspberry Pi Pico: Flash"
4. Release BOOTSEL button

**Verify Success:**

- Pico W should reboot automatically
- Built-in LED should start blinking
- Check VS Code output for success message

## Hardware Assembly

### Power System

**Power Distribution:**

```
Battery (+) ──→ Power Switch ──→ Voltage Regulator ──→ 5V Rail
                    │
                    └──→ Motor Driver VIN (6-12V)

5V Rail ──→ Pico W VIN (Pin 39)
       ├──→ Servo Power (Red wire)
       └──→ I2S DAC VCC
```

**Important Notes:**

- Never exceed 12V input to motor driver
- Pico W can accept 1.8-5.5V on VIN pin
- Use common ground for all components

### Motor Connections

**Motor SHIM (DRV8833-based) Wiring:**

```
Pico W GPIO    Motor SHIM Pin    Function
------------   -----------    --------
GPIO 2      →  AIN1          Motor A Direction 1
GPIO 3      →  AIN2          Motor A Direction 2  
GPIO 4      →  BIN1          Motor B Direction 1
GPIO 5      →  BIN2          Motor B Direction 2
GND         →  GND           Ground
5V          →  VCC           Logic Power
Battery+    →  VIN           Motor Power
```

**Motor Wiring:**

```
Motor SHIM (DRV8833)        Left Motor     Right Motor
-------        ----------     -----------
AOUT1       →  Motor + (A)
AOUT2       →  Motor - (A)
BOUT1       →                 Motor + (B)
BOUT2       →                 Motor - (B)
```

### Audio System

**I2S DAC Connections:**

```
Pico W GPIO    PCM5102A Pin   Function
------------   ------------   --------
GPIO 18     →  BCK           Bit Clock
GPIO 19     →  DIN           Data Input
GPIO 20     →  LCK           Left/Right Clock
5V          →  VCC           Power
GND         →  GND           Ground
            →  OUT+/OUT-     To speaker/amplifier
```

**Speaker Connection:**

- Connect DAC output to speaker directly (low volume), or
- Connect through audio amplifier for higher volume
- Use 4-8Ω speaker rated for 3-5W

### Servo Control

**Servo Wiring:**

```
Servo Wire     Pico W Pin     Function
----------     ----------     --------
Red         →  5V (Pin 40)   Power
Black/Brown →  GND           Ground  
Yellow/White→  GPIO 16       PWM Signal
```

## Software Configuration

### Audio Setup

**Adding Sound Effects:**

1. Place MP3 files in `misc/` directory
2. Name them numerically: `00001.mp3`, `00002.mp3`, etc.
3. Run conversion script:

```powershell
# Windows PowerShell
.\tools\convert_audio.ps1

# Windows Command Prompt
tools\convert_audio.bat
```

This converts MP3 files to C header files for embedding.

**Rebuild After Audio Changes:**

After adding new audio files:

1. Run conversion script
2. Build project again (Ctrl+Shift+P → "CMake: Build")
3. Flash updated firmware

### Gamepad Pairing

**Pairing Process:**

1. Power on your Dalek (Pico W running firmware)
2. Put your gamepad in pairing mode:
   - **PS4/PS5:** Hold Share + PS button until light bar flashes
   - **Xbox:** Hold Xbox + Connect button until button flashes
   - **Switch Pro:** Hold Sync button until LEDs race
3. Wait for automatic connection (should take 10-30 seconds)
4. Test basic movement with analog sticks

**Troubleshooting Connection:**

- Ensure gamepad is charged
- Try different gamepad if available
- Check serial output for BluePad32 messages
- Restart Pico W if connection fails

## Basic Operation

### Control Mapping

**Movement Controls:**

- **Left Stick Y-Axis:** Forward/backward movement
- **Left Stick X-Axis:** Turning (differential steering)
- **Right Stick Y-Axis:** Eye stalk up/down

**Audio Controls:**

- **Face Buttons (A/B/X/Y):** Play different sound effects
- **D-Pad:** Additional sound effects
- **Shoulder Buttons:** Special audio commands

**Safety Controls:**

- **Start/Menu Button:** Emergency stop (disables motors)
- **Select/View Button:** Reset to normal operation

### Testing Each System

**Motor Test:**

1. Ensure motors are properly mounted and wheels attached
2. Use left analog stick to test movement
3. Verify both wheels turn in correct directions
4. Test emergency stop functionality

**Audio Test:**

1. Connect speaker to I2S DAC output
2. Press different gamepad buttons
3. Verify sound effects play correctly
4. Adjust volume if using amplifier

**Servo Test:**

1. Connect servo to eye stalk mechanism
2. Use right analog stick Y-axis
3. Verify smooth movement up and down
4. Check for proper center position

## Common Issues and Solutions

### Build Problems

**"CMake configuration failed":**

- Ensure Pico SDK is properly installed
- Try closing and reopening VS Code
- Check that all git submodules are initialized

**"Compilation errors":**

- Verify all required extensions are installed
- Check that you're using the correct compiler (Pico ARM GCC)
- Try cleaning and rebuilding (Ctrl+Shift+P → "CMake: Clean")

### Hardware Problems

**Motors not responding:**

1. Check all wiring connections
2. Verify power supply voltage (should be 6-12V)
3. Test motors directly with battery
4. Check DRV8833 power LED

**No audio output:**

1. Verify I2S wiring (especially clock signals)
2. Check speaker connections
3. Test with known-good audio source
4. Verify DAC power supply

**Gamepad won't connect:**

1. Try pairing process again
2. Ensure gamepad is in pairing mode
3. Check for other Bluetooth interference
4. Try different gamepad if available

### Software Issues

**Pico W won't flash:**

1. Hold BOOTSEL button while connecting USB
2. Verify Pico W appears as USB drive
3. Try different USB cable
4. Check for driver issues

**Erratic behavior:**

1. Check power supply stability
2. Verify ground connections
3. Look for loose wiring
4. Check serial output for error messages

## Next Steps

### Enhance Your Dalek

**Mechanical Improvements:**

- Design and 3D print custom Dalek shell
- Add LED effects for eyestalk and dome lights
- Implement head rotation mechanism
- Add sensor arrays (ultrasonic, camera)

**Software Features:**

- Add autonomous movement patterns
- Implement voice recognition
- Create preset movement sequences
- Add remote monitoring capabilities

**Advanced Audio:**

- Record custom Dalek voice samples
- Add real-time voice modulation
- Implement spatial audio effects
- Add background ambient sounds

### Learning Resources

**Documentation:**

- Read detailed module documentation in `docs/` folder
- Study source code for implementation details
- Review hardware schematics and pin assignments

**Community:**

- Join Raspberry Pi Pico community forums
- Share your build photos and videos
- Contribute improvements back to the project

**Further Projects:**

- Adapt code for other robot platforms
- Explore other Pico W capabilities
- Learn about embedded systems design
- Study robotics and control systems

## Safety and Best Practices

### Electrical Safety

- Always disconnect power when making wiring changes
- Use proper fuses and circuit protection
- Verify connections before applying power
- Keep workspace clean and organized

### Mechanical Safety

- Ensure all moving parts are properly secured
- Test emergency stop functionality regularly
- Use appropriate materials for structural components
- Consider weight distribution and stability

### Software Safety

- Test thoroughly before deploying
- Implement proper error handling
- Use version control for code changes
- Keep backups of working configurations

**Ready to Start Building?**

Follow this guide step by step, and you'll have a working Dalek in no time! Remember that robotics projects are iterative - start simple, test frequently, and add features gradually.

For detailed technical information, refer to the comprehensive documentation in the `docs/` folder.

**Exterminate... Responsibly!** 🤖
