# Exterminate

> **✅ Development Status**
> 
> This project has been tested on physical hardware (Pimoroni Pico LiPo 2 XL W) and verified to run core features including Bluetooth gamepad control, differential drive movement, and I2S audio playback. The codebase was developed with assistance from GitHub Copilot and refined through hands-on testing by the author and contributors.
> 
> **Key Dependencies**: Built upon [BluePad32](https://github.com/ricardoquesada/bluepad32) for Bluetooth gamepad support and [rp2040_i2s_example](https://github.com/malacalypse/rp2040_i2s_example) for PIO-based I2S audio implementation.
> 
> **Notes**: While the project is tested and functioning, additional edge cases and hardware revisions may surface further issues. Contributions, bug reports, and validation against different hardware revisions are welcome.

A C++ project for the Pimoroni Pico LiPo 2 XL W that brings an animatronic Dalek to life with Bluetooth gamepad control, differential drive movement, and iconic audio playback.

## 🤖 Project Overview

Exterminate is a Dalek control system that enables wireless gamepad control of a mobile Dalek replica. The project features differential drive movement and I2S audio playback for the iconic "Exterminate!" sound effects. It demonstrates modern C++ best practices, clean architecture, and SOLID principles while interfacing with embedded hardware.

> **Note**: This is an educational project created for learning purposes. The "Dalek" name and associated content are the intellectual property of the BBC. See the [Copyright Disclaimer](#️-copyright-disclaimer) section for full details.

### Key Features

- **Bluetooth Gamepad Support**: Uses BluePad32 library for wireless controller connectivity with visual status indication
- **Differential Drive Control**: Sophisticated two-wheel movement with forward/reverse and turning
- **Pimoroni Motor SHIM for Pico (DRV8833-based)**: Motor SHIM breakout providing a DRV8833 dual H-bridge with convenient headers and power wiring (see [Pimoroni Motor SHIM for Pico](https://shop.pimoroni.com/products/motor-shim-for-pico))
- **PIO-Based I2S Audio**: High-quality 44.1kHz PCM audio output using pico-extras I2S library
- **Audio-Reactive LEDs**: Real-time LED visualization that pulses with audio intensity for authentic Dalek head lighting
- **Controller Status LED**: Visual feedback showing controller connection status (solid when paired, flashing when waiting)
- **Modern C++17**: Clean, maintainable code following SOLID principles and RAII patterns
- **Hardware Abstraction**: Modular design allowing easy hardware component swapping

## 🛠️ Hardware Requirements

### Core Components

- **Pimoroni Pico LiPo 2 XL W** (RP2350-based board; this is the board used for testing)
- **Pimoroni Motor SHIM for Pico** (DRV8833-based dual H-bridge for differential drive movement). See: [Pimoroni Motor SHIM for Pico](https://shop.pimoroni.com/products/motor-shim-for-pico)
- **Two DC Motors** (geared motors recommended for mobile Dalek base)
- **I2S Audio Amplifier** (Adafruit MAX98357A I2S amplifier breakout — see [Adafruit MAX98357A Breakout (Product 3006)](https://www.adafruit.com/product/3006))
- **PWM-Compatible LEDs** (4x LEDs for audio visualization, 1x LED for controller status)
- **Speaker** (4-8Ω speaker, 3W recommended for MAX98357A)
- **Bluetooth Gamepad** (PS3, PS4, Xbox, or compatible controller)

### Wiring Diagram

```text
Pico W -> Pimoroni Motor SHIM (Movement)
GPIO 6 -> AIN1 (Left Motor Direction 1)
GPIO 7 -> AIN2 (Left Motor Direction 2)
GPIO 27 -> BIN1 (Right Motor Direction 1)
GPIO 26 -> BIN2 (Right Motor Direction 2)
3V3    -> VCC
GND    -> GND

Motor SHIM (DRV8833) -> Motors
AOUT1/AOUT2 -> Left Motor
BOUT1/BOUT2 -> Right Motor



Pico W -> I2S Audio Amplifier (PIO-based)
-----------------------------
GPIO 32 -> I2S BCLK (Bit Clock)
GPIO 33 -> I2S LRCLK (Left/Right Clock)
GPIO 34 -> I2S DOUT (Data Output)
5V      -> VIN (Power Input)
GND     -> GND

I2S Amplifier -> Speaker
------------------------
Speaker+ -> Speaker Positive Terminal
Speaker- -> Speaker Negative Terminal

Pico W -> Audio Visualization LEDs
----------------------------------
GPIO 13 -> External Red LED 1 (PWM for intensity control)
GPIO 14 -> External Red LED 2 (PWM for intensity control)
GND     -> LED Cathodes (via current limiting resistors)

Pico W -> Controller Status LED (Eye Stalk)
-------------------------------
GPIO 44 -> Blue eye LED (Status: BREATHING = pairing, SOLID = paired, FAST BLINK = error)
GND     -> LED Cathode (via current limiting resistor)

I2S Amplifier -> Speaker
------------------------
Speaker+ -> Speaker Positive Terminal
Speaker- -> Speaker Negative Terminal
```

## 🚀 Quick Start

### Prerequisites

1. **Pico SDK v2.1.1** installed and configured
2. **CMake 3.13+** and **Ninja** build system
3. **ARM GCC Toolchain** for cross-compilation
4. **Git** with submodule support

### Building the Project

1. **Clone the repository**:

   ```bash
   git clone --recursive https://github.com/yourusername/Exterminate.git
   cd Exterminate
   ```

2. **Initialize submodules** (if not cloned with `--recursive`):

   ```bash
   git submodule update --init --recursive
   ```

3. **Configure the build**:

   ```bash
   mkdir build
   cd build
   cmake .. -G Ninja
   ```
   - Copy `build/Exterminate.uf2` to the mounted drive
   - Or use: `picotool load Exterminate.uf2 -fx`

## 🎮 Usage

### Gamepad Pairing

1. **Put Pico W in pairing mode**: The system automatically enters pairing mode on startup
2. **Pair your gamepad**: Follow standard Bluetooth pairing procedure for your controller
3. **Verify connection**: Controller status LED on GPIO 15 will show solid light when connected, flashing when waiting for pairing

### Controls

- **Left Joystick Y-Axis**: Forward/Backward movement
- **Left Joystick X-Axis**: Steering (differential drive)
- **Right Trigger/Button**: Play "Exterminate!" audio
- **Face Buttons**: Additional sound effects (if implemented)
- **Deadzone**: 10% deadzone prevents drift from centered joysticks


```text
Movement:     Left stick Y -> Forward/Backward drive
Steering:     Left stick X -> Differential turning
Audio Trigger: Right trigger -> "Exterminate!" sound effect
```

## 🏗️ Project Structure

```text
Exterminate/
├── .github/
│   └── copilot-instructions.md    # Coding guidelines and best practices
├── include/                       # Public header files
│   ├── exterminate_platform.h     # Platform interface declarations
│   ├── MotorController.h          # Motor driver class interface
│   ├── AudioController.h          # Audio controller class interface
│   ├── SimpleLED.h                # Minimal LED helper (PWM)
│   ├── I2S.h                      # PIO-based I2S audio interface
│   └── audio/                     # Generated audio headers (from MP3s)
│       ├── 00001.h                # "Exterminate!" audio data
│       ├── 00002.h                # Additional sound effects
│       ├── ...                    # More audio files
│       └── audio_index.h          # Audio file registry
├── src/                          # Source implementation files
│   ├── main.cpp                  # Application entry point
│   ├── exterminate_platform.cpp  # BluePad32 platform implementation
│   ├── MotorController.cpp       # Motor control implementation
│   ├── AudioController.cpp       # Audio control implementation
│   ├── SimpleLED.cpp             # Minimal LED helper implementation
│   ├── I2S.cpp                   # PIO-based I2S implementation
│   ├── i2s.pio                   # PIO assembly for I2S state machines
│   ├── btstack_config.h          # BTstack configuration
│   └── sdkconfig.h               # BluePad32 configuration
├── lib/                          # External libraries
│   └── bluepad32/                # BluePad32 submodule
├── misc/                         # Audio source files (excluded from git)
│   ├── 00001.mp3                # "Exterminate!" audio sample (source)
│   ├── 00002.mp3                # Additional Dalek sound effects (source)
│   └── ...                      # More audio samples (source)
├── tools/                        # Build and conversion tools
│   ├── mp3_to_header.py         # MP3 to C++ header converter
│   ├── convert_audio.ps1        # PowerShell conversion script
│   └── convert_audio.bat        # Batch conversion script
├── docs/                         # Project & build documentation
│   ├── audio_system.md          # Audio system documentation
│   └── build/                   # Build + assembly guides (see README there)
├── tests/                        # Unit tests (future)
├── build/                        # Build output (generated)
├── CMakeLists.txt               # Build system configuration
├── COPYRIGHT.md                 # Copyright and IP information
└── README.md                    # This file
```

## 🧩 Architecture

### Design Principles

The project follows **SOLID principles** and modern C++ best practices:

- **Single Responsibility**: Each class has one clear purpose
- **Open/Closed**: Extensible design through interfaces
- **Liskov Substitution**: Proper inheritance hierarchies
- **Interface Segregation**: Small, focused interfaces
- **Dependency Inversion**: Abstractions over implementations

### Key Classes

#### `MotorController`

- **Purpose**: Hardware abstraction for DRV8833-based motor driver
- **Features**: PWM speed control, differential drive algorithms, RAII resource management
- **Interface**: `setMotorSpeed()`, `setDifferentialDrive()`, `stopAllMotors()`

#### `AudioController`

- **Purpose**: Pico-extras I2S audio output with LED integration
- **Features**: 44.1kHz PCM audio playback, real-time audio intensity calculation, LED synchronization, resource discovery pattern
- **Interface**: `playAudio()`, `stopAudio()`, `getAudioIntensity()`, `initialize()` methods

#### `SimpleLED`

- **Purpose**: Lightweight PWM helper for external LEDs
- **Features**: Pin init, PWM init, and brightness setting; audio-driven updates done in `main.cpp`
- **Interface**: `initializePwmPin()`, `setBrightnessPin()`

#### `exterminate_platform`

- **Purpose**: BluePad32 platform integration with controller status indication
- **Features**: Gamepad event handling, joystick normalization, audio triggers, controller status LED management
- **Interface**: Standard BluePad32 platform callbacks with LED status integration

### Data Flow

```text
Gamepad Input → BluePad32 → Platform Handler → Motor Controller → Drive Motors (Movement)
                                            └→ Audio Controller → Pico-Extras I2S → Speaker
                                            └→ LED Controller → PWM LEDs → Audio Visualization
                                            └→ Status LED → Controller Connection Feedback
```

## ⚙️ Configuration

### Motor Controller Settings

Edit the configuration in `src/exterminate_platform.cpp`:

```cpp
// Current MotorController configuration for movement
MotorController::Config motorConfig = {
    .leftMotorPin1 = 2,   // GPIO pins for DRV8833
    .leftMotorPin2 = 3,
    .rightMotorPin1 = 4,
    .rightMotorPin2 = 5,
    .pwmFrequency = 10000 // 10kHz PWM frequency
};

// AudioController configuration for sound effects
AudioController::Config audioConfig = {
    .bclkPin = 6,              // I2S bit clock (PIO-based)
    .lrclkPin = 8,             // I2S left/right clock
    .dinPin = 9,               // I2S data input
    .sampleRate = 22050,       // 22.05kHz audio
    .bitsPerSample = 16        // 16-bit PCM audio
};

// SimpleLED setup for audio visualization (example)
Exterminate::SimpleLED::initializePwmPin(11, 255, 4.0f);
Exterminate::SimpleLED::initializePwmPin(12, 255, 4.0f);

// Controller status LED configuration
const uint controllerStatusPin = 15;  // GPIO 15 for status indication
```

### Audio File Conversion

The project uses embedded audio files converted from MP3s:

> **Copyright Notice**: Users are responsible for ensuring they have appropriate rights to any audio files used. This project does not include copyrighted Dalek audio content - it only provides the technical framework for audio playback.

```bash
# Convert MP3 files to C++ headers (Windows)
.\tools\convert_audio.ps1

# Or use batch file
tools\convert_audio.bat

# Manual conversion
python tools\mp3_to_header.py misc include\audio
```

**Audio Usage:**

```cpp
#include "AudioController.h"
#include "audio/audio_index.h"

// Play "Exterminate!" sound
audioController.playAudio(Audio::AudioIndex::AUDIO_00001);
```

### Deadzone and Sensitivity

Adjust gamepad sensitivity for movement and eye stalk control in `exterminate_platform_on_controller_data()`:

```cpp
const float DEADZONE = 0.1f;  // 10% deadzone for both movement and eye stalk
// Modify as needed for smooth operation of both drive and eye stalk
```

## 🔧 Development

### Code Style

The project uses **clang-format** for consistent code formatting. Key conventions:

- **Classes**: `PascalCase`
- **Functions/Variables**: `camelCase`  
- **Constants**: `UPPER_CASE_SNAKE_CASE`
- **Namespaces**: `Exterminate`
- **Headers**: `#pragma once`

### Building for Development

Enable debug output and additional logging:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Testing

While embedded testing is complex, the modular design supports unit testing:

- Hardware interfaces are abstracted
- Business logic is separated from hardware specifics
- Mock implementations can be provided for testing

## 🐛 Troubleshooting

### Common Issues

1. **Build Failures**:
   - Ensure Pico SDK v2.1.1 is properly installed
   - Check CMake and Ninja versions
   - Verify git submodules are initialized

1. **Gamepad Not Connecting**:
   - Check gamepad compatibility with BluePad32
   - Verify Bluetooth functionality on Pico W
   - Try resetting gamepad to pairing mode

1. **Motors Not Responding**:
   - Check wiring connections to DRV8833
   - Verify power supply to motor driver
   - Test PWM output with oscilloscope/logic analyzer

 

1. **No Audio Output**:
   - Check I2S amplifier wiring and connections
   - Verify MAX98357A power supply (5V required)
   - Ensure speaker impedance is 4-8Ω
   - Check SD pin is tied to 3V3 (shutdown control)
   - Test I2S signals with oscilloscope or logic analyzer

1. **Compilation Errors**:
   - Ensure C++17 standard is enabled
   - Check include paths for BluePad32
   - Verify btstack configuration files

1. **DMA Channel Conflicts** (Critical Fix Applied):
   - **Symptoms**: Runtime panic "DMA channel X is already claimed", LED staying solid, gamepad not pairing
   - **Root Cause**: Pico-extras I2S library expects to manage its own resources internally
   - **Solution**: Resource discovery pattern implemented in AudioController::initialize()
   - **Reference**: See `docs/troubleshooting_dma_conflicts.md` for detailed explanation

### Debug Output

Enable USB serial output for debugging:

```cpp
// In CMakeLists.txt (already configured)
pico_enable_stdio_usb(Exterminate 1)
```

Monitor output:

```bash
# Windows
# Use PuTTY or similar terminal emulator

# Linux/macOS  
screen /dev/ttyACM0 115200
```

## 📚 Dependencies

### Core Libraries

- **Pico SDK v2.1.1**: Raspberry Pi Pico development framework
- **BluePad32**: Bluetooth gamepad support library ([GitHub](https://github.com/ricardoquesada/bluepad32))
- **BTstack**: Bluetooth protocol stack
- **CYW43**: WiFi/Bluetooth driver for Pico W

### Hardware Libraries

-- **hardware_pwm**: PWM generation for motor control
- **hardware_i2s**: I2S audio output with MAX98357A amplifier
- **hardware_gpio**: GPIO pin control and configuration
- **pico_stdlib**: Standard Pico functionality

### Key Inspirations and References

- **I2S Implementation**: Based on [malacalypse/rp2040_i2s_example](https://github.com/malacalypse/rp2040_i2s_example) - Excellent reference for PIO-based I2S audio output
- **BluePad32 Integration**: Comprehensive Bluetooth gamepad library by Ricardo Quesada
- **Printables Animated Dalek (Rick100)**: This project was inspired by and is a remix of Rick100's "Animated Dalek with Sound" model on Printables — [Animated Dalek with Sound](https://www.printables.com/model/1351261-animated-dalek-with-sound) (credit: Rick100)

## 🤝 Contributing

**Hardware Testing Needed!** This project is currently awaiting hardware validation. If you build and test this system, your feedback is invaluable!

### Priority Contributions

1. **Hardware Testing Reports**: Document what works, what doesn't, and any required modifications
2. **Bug Fixes**: Issues discovered during real-world testing
3. **Performance Optimizations**: Improvements based on actual hardware behavior
4. **Documentation Updates**: Corrections or additions based on practical experience

### Standard Contribution Process

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/amazing-feature`
3. **Follow coding guidelines**: See `.github/copilot-instructions.md`
4. **Write tests** for new functionality
5. **Commit changes**: `git commit -m 'Add amazing feature'`
6. **Push to branch**: `git push origin feature/amazing-feature`
7. **Open a Pull Request**

### Code Review Checklist

- [ ] Follows SOLID principles
- [ ] Includes proper error handling
- [ ] Uses RAII for resource management
- [ ] Maintains consistent code style
- [ ] Includes appropriate documentation

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## ⚠️ Copyright Disclaimer

### Important Notice Regarding Intellectual Property

The "Dalek" name, character design, and associated audio content (including "Exterminate!" and other phrases) are the intellectual property of the BBC and are protected by copyright. This project uses these elements purely for:

- **Educational purposes** - Teaching embedded systems programming and robotics
- **Entertainment and hobbyist use** - Personal projects and learning experiences
- **Non-commercial applications** - Not intended for sale or commercial distribution

**No Copyright Infringement Intended**: This project is a fan-made educational tool created out of appreciation for the Doctor Who series. All rights to the Dalek character, design, and audio remain with the BBC and the estate of Terry Nation.

**Audio Content**: Users are responsible for ensuring they have appropriate rights to any audio files they use with this system. The project provides tools for audio conversion but does not include copyrighted audio content.

**For Educational Use**: This project is intended to help students and hobbyists learn about:

- Embedded C++ programming
- Real-time audio processing
- Robotics and motor control
- Bluetooth communication protocols
- Hardware interfacing and design

If you are affiliated with the BBC or hold rights to the Dalek intellectual property and have concerns about this project, please contact the repository maintainer.

**For detailed copyright information, see [COPYRIGHT.md](COPYRIGHT.md).**

## 🎯 Future Enhancements

### Planned Features

- **Multiple Audio Samples**: Expand library of Dalek sound effects and phrases
- **Voice Modulation**: Real-time voice processing to sound like a Dalek
-- **Eye Stalk Lighting**: LED control for head illumination effects
- **Motion Sensors**: Trigger audio/movement based on proximity detection
- **Remote Web Interface**: Control Dalek via web browser for demonstrations

-### Hardware Expansions

- **LED Matrix/Displays**: Add visual effects and status indicators
-- **Proximity Sensors**: Automatic target detection and response
-- **Camera Module**: First-person view from the Dalek's perspective
-- **Voice Recognition**: Respond to specific voice commands
- **LED Matrix/Displays**: Add visual effects and status indicators
- **Proximity Sensors**: Automatic target detection and response
- **Camera Module**: First-person view from the Dalek's perspective
- **Voice Recognition**: Respond to specific voice commands

## 📞 Support

- **Issues**: Report bugs via GitHub Issues
- **Discussions**: Join project discussions  
- **Documentation**: Additional docs in `docs/` directory
- **Community**: BluePad32, Raspberry Pi Pico, and animatronics communities

---

Built with ❤️ for the Dalek and animatronics community
