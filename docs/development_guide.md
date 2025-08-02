# Development Guide

## Overview

This guide covers setting up the development environment, building the project, flashing firmware, and debugging the Exterminate Dalek system.

## Prerequisites

### Required Software

**Development Environment**:
- **Visual Studio Code** (recommended IDE)
- **Git** for version control
- **Python 3.6+** for audio conversion tools

**Pico SDK Toolchain**:
- **CMake** 3.13+ build system
- **Ninja** build tool  
- **ARM GCC Toolchain** for cross-compilation
- **Pico SDK** 2.0+ development libraries

### Hardware Requirements

**Development Board**:
- Raspberry Pi Pico W (RP2350 recommended)
- USB-C cable for programming and power
- Breadboard or development PCB

**Optional Debug Hardware**:
- SWD debugger (Raspberry Pi Debug Probe)
- Logic analyzer for signal debugging
- Oscilloscope for PWM verification

## Environment Setup

### VS Code Extension Installation

Install the following VS Code extensions:

```bash
# Core development extensions
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension raspberrypi.raspberry-pi-pico

# Additional helpful extensions
code --install-extension ms-python.python
code --install-extension ms-vscode.hexeditor
code --install-extension github.copilot
```

### Pico SDK Setup

**Automatic Setup (Recommended)**:
The Raspberry Pi Pico extension will automatically install the SDK when you open the project.

**Manual Setup**:
1. Download Pico SDK from GitHub
2. Set environment variable: `PICO_SDK_PATH=/path/to/pico-sdk`
3. Install toolchain following official Pico documentation

## Project Structure

```
Exterminate/
├── .vscode/                 # VS Code configuration
│   ├── c_cpp_properties.json
│   ├── launch.json
│   └── tasks.json
├── build/                   # Build output (generated)
├── docs/                    # Documentation
├── include/                 # Header files
│   ├── audio/              # Generated audio headers
│   ├── AudioController.h
│   ├── MotorController.h
│   └── exterminate_platform.h
├── lib/                     # External libraries
│   └── bluepad32/          # BluePad32 submodule
├── misc/                    # MP3 source files (not in git)
├── src/                     # Source code
│   ├── main.cpp
│   ├── AudioController.cpp
│   ├── MotorController.cpp
│   └── exterminate_platform.cpp
├── tools/                   # Build and development tools
│   ├── mp3_to_header.py
│   ├── convert_audio.ps1
│   └── convert_audio.bat
├── CMakeLists.txt           # Main build configuration
├── pico_sdk_import.cmake    # SDK import script
└── README.md               # Project overview
```

## Building the Project

### Quick Build (VS Code)

1. **Open Project**: Open the Exterminate folder in VS Code
2. **Select Kit**: Choose "Pico ARM GCC" when prompted
3. **Build**: Use Ctrl+Shift+P → "CMake: Build" or click Build button
4. **Flash**: Use Ctrl+Shift+P → "Raspberry Pi Pico: Flash"

### Command Line Build

```bash
# Configure build system
cmake -B build -G Ninja

# Build project
ninja -C build

# Or use CMake wrapper
cmake --build build
```

### Build Targets

Available build targets:

```bash
# Build main executable
ninja -C build Exterminate

# Build and create UF2 file for flashing
ninja -C build Exterminate.uf2

# Clean build
ninja -C build clean

# Rebuild everything
ninja -C build clean && ninja -C build
```

## Configuration Options

### CMake Build Options

Configure build with CMake options:

```bash
# Enable debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Configure motor PWM frequency
cmake -B build -DMOTOR_PWM_FREQUENCY=20000

# Enable verbose build output
cmake -B build -DCMAKE_VERBOSE_MAKEFILE=ON

# Target specific Pico variant
cmake -B build -DPICO_BOARD=pico_w
```

### Audio System Configuration

**Converting Audio Files**:
1. Place MP3 files in `misc/` directory
2. Run conversion script:

```powershell
# Windows PowerShell
.\tools\convert_audio.ps1

# Windows Command Prompt  
tools\convert_audio.bat

# Manual Python execution
python tools\mp3_to_header.py misc include\audio
```

**Audio Build Integration**:
Audio headers are automatically included in the build. No additional configuration needed.

## Flashing Firmware

### UF2 Flashing (Recommended)

**Using VS Code**:
1. Hold BOOTSEL button while connecting Pico W
2. Use "Raspberry Pi Pico: Flash" command
3. VS Code will automatically build and flash

**Manual UF2 Flashing**:
1. Hold BOOTSEL button while connecting Pico W
2. Copy `build/Exterminate.uf2` to RPI-RP2 drive
3. Pico will automatically reboot and run firmware

### SWD Flashing (Advanced)

**With Raspberry Pi Debug Probe**:
```bash
# Flash via SWD (requires OpenOCD)
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
  -c "program build/Exterminate.elf verify reset exit"
```

**SWD Wiring**:
```
Debug Probe    Pico W      Function
-----------    ------      --------
SWCLK     -->  SWCLK       SWD Clock
SWDIO     -->  SWDIO       SWD Data  
GND       -->  GND         Ground
VTREF     -->  3V3         Target Voltage Reference
```

## Debugging

### Serial Debug Output

**Enable Debug Output**:
Debug output is sent via USB serial (UART0):

```cpp
// In main.cpp
stdio_init_all();  // Enables USB serial

// Use standard printf for debug output
printf("Debug message: %d\n", value);
```

**View Debug Output**:
```bash
# Windows (PowerShell)
# Find COM port in Device Manager, then:
python -m serial.tools.miniterm COM3 115200

# Linux/macOS
python -m serial.tools.miniterm /dev/ttyACM0 115200

# Or use VS Code Serial Monitor extension
```

### VS Code Debugging

**Launch Configuration** (`.vscode/launch.json`):
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Pico Debug",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "executable": "${workspaceFolder}/build/Exterminate.elf",
            "configFiles": [
                "interface/cmsis-dap.cfg",
                "target/rp2040.cfg"
            ]
        }
    ]
}
```

**Debug Features**:
- Breakpoints in C++ code
- Variable inspection
- Call stack examination
- Memory viewer
- Register inspection

### Common Debug Scenarios

**Motor Not Working**:
1. Check serial output for initialization messages
2. Verify PWM signals with oscilloscope/logic analyzer
3. Test motor controller in isolation
4. Check power supply stability

**Bluetooth Connection Issues**:
1. Monitor BluePad32 debug output
2. Verify controller pairing process
3. Check for interference
4. Test with known-good controller

**Audio Problems**:
1. Verify I2S signals with logic analyzer
2. Check audio file conversion process
3. Test with simple sine wave generation
4. Verify DAC connections and power

## Performance Profiling

### Built-in Profiling

Enable profiling in build configuration:

```cmake
# Enable profiling
set(ENABLE_PROFILING ON)
```

**Profiling Macros**:
```cpp
#include "profiling.h"

void critical_function() {
    PROFILE_START(critical_function);
    
    // Function implementation
    do_important_work();
    
    PROFILE_END(critical_function);
}
```

### Performance Monitoring

**Key Metrics to Monitor**:
- Motor control loop timing (<1ms)
- Audio buffer underruns
- Bluetooth event processing time
- Memory usage and fragmentation

**Monitoring Code**:
```cpp
// Monitor timing critical functions
uint32_t start_time = time_us_32();
motor_controller.update();
uint32_t execution_time = time_us_32() - start_time;

if (execution_time > 1000) {  // >1ms is concerning
    printf("WARN: Motor update took %lu us\n", execution_time);
}
```

## Testing

### Unit Testing Setup

**Test Framework**: Minimal custom framework for embedded testing

**Running Tests**:
```bash
# Build test target
cmake -B build -DBUILD_TESTING=ON
ninja -C build tests

# Run tests on host (cross-platform code only)
./build/tests/unit_tests
```

**Test Structure**:
```cpp
// Example test file
#include "test_framework.h"
#include "MotorController.h"

TEST(MotorController, SpeedClamping) {
    MockMotorHardware hardware;
    MotorController controller(test_config, &hardware);
    
    // Test speed clamping
    controller.setSpeed(1.5f);  // Over maximum
    ASSERT_EQ(hardware.getLastPWM(), 1.0f);
    
    controller.setSpeed(-1.5f); // Under minimum  
    ASSERT_EQ(hardware.getLastPWM(), -1.0f);
}
```

### Integration Testing

**Hardware-in-Loop Testing**:
1. Connect real hardware components
2. Run automated test sequences
3. Verify expected behavior
4. Log results for analysis

**Test Scenarios**:
- Gamepad input → motor response
- Emergency stop functionality
- Audio playback verification
- Power supply variations

## Troubleshooting

### Build Issues

**CMake Configuration Fails**:
1. Verify Pico SDK installation and path
2. Check CMake and Ninja versions
3. Clear build directory and reconfigure
4. Check for missing dependencies

**Compilation Errors**:
1. Verify all submodules are initialized
2. Check for missing include paths
3. Verify C++ standard compatibility
4. Check for conflicting library versions

**Linking Errors**:
1. Verify all source files are in CMakeLists.txt
2. Check for missing library dependencies
3. Verify symbol names and namespaces
4. Check for multiple definitions

### Runtime Issues

**System Won't Boot**:
1. Verify power supply and connections
2. Check for flash corruption
3. Try flashing bootloader
4. Use SWD debugger to check startup

**Unexpected Behavior**:
1. Enable verbose debug output
2. Use debugger to step through code
3. Check for race conditions
4. Verify hardware connections

**Memory Issues**:
1. Monitor stack usage
2. Check for memory leaks
3. Verify audio buffer sizes
4. Consider reducing feature set

## Contributing

### Code Style

**Formatting**: Use `.clang-format` configuration:
```bash
# Format all source files
clang-format -i src/*.cpp include/*.h
```

**Naming Conventions**:
- Classes: `PascalCase` (e.g., `MotorController`)
- Functions: `camelCase` (e.g., `setMotorSpeed`)
- Variables: `snake_case` (e.g., `motor_speed_`)
- Constants: `UPPER_CASE` (e.g., `MAX_SPEED`)

### Git Workflow

**Branch Strategy**:
```bash
# Create feature branch
git checkout -b feature/new-audio-system

# Make changes and commit
git add .
git commit -m "feat: Add new audio system with I2S support"

# Push and create pull request
git push origin feature/new-audio-system
```

**Commit Message Format**:
```
type(scope): description

[optional body]

[optional footer]
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

This development guide provides everything needed to successfully build, debug, and extend the Exterminate Dalek project.
