# Documentation Index

## Welcome to Exterminate Dalek Documentation

This documentation provides comprehensive information for building, programming, and operating an animatronic Dalek using the Raspberry Pi Pico W platform.

## Quick Navigation

### 🚀 New to the Project?

**[Getting Started Guide](getting_started.md)** - Start here for your first build

- Complete hardware shopping list
- 30-minute quick start tutorial  
- Basic assembly instructions
- Initial testing and troubleshooting

### 🏗️ System Documentation

**[System Architecture](system_architecture.md)** - High-level project overview

- Complete system design and component relationships
- Real-time constraints and performance requirements
- Error handling strategies and safety systems
- Memory management and resource allocation

**[Hardware Configuration](hardware_configuration.md)** - Complete wiring guide

- Pin assignments and GPIO allocations
- Component wiring diagrams with part numbers
- Power distribution and electrical requirements
- Assembly guidelines and mounting specifications

### 🎮 Control Systems

**[Gamepad Control](gamepad_control.md)** - Bluetooth controller integration

- Supported gamepad types (PS4/PS5, Xbox, Nintendo Switch)
- Pairing procedures and connection troubleshooting
- Control mapping and customization options
- Safety features and emergency stop functionality


**[Motor Control](motor_control.md)** - Movement system

- Pimoroni Motor Shim integration and configuration
- Differential drive mathematics and implementation

- PWM signal generation and timing requirements

### 🔊 Audio & Visual Systems

**[Audio System](audio_system.md)** - PIO-based I2S audio implementation

- PIO-based I2S audio generation architecture
- MAX98357A I2S amplifier integration and configuration
- Audio buffer management and real-time constraints
- LED integration for audio-reactive visual effects

**[Embedded Audio Usage](embedded_audio_usage.md)** - Audio file management

- MP3 to header file conversion process
- Audio file organization and naming conventions
- Build system integration for audio resources
- Runtime audio playback and control

**[LED System](led_system.md)** - Audio visualization and status indication

- Audio-reactive LED controller for Dalek head lighting effects
- Controller status LED for connection feedback
- PWM-based intensity control and effect patterns
- Hardware configuration and wiring specifications

### 🛠️ Development

**[Development Guide](development_guide.md)** - Complete development workflow

- Development environment setup and configuration
- Build system usage (CMake, Ninja, VS Code)
- Debugging techniques and profiling tools
- Testing strategies and continuous integration

**[Troubleshooting: DMA Conflicts](troubleshooting_dma_conflicts.md)** - Critical system fixes

- Resolution for "DMA channel already claimed" runtime panics
- LED staying solid and gamepad pairing issues  
- Resource discovery pattern implementation
- Prevention strategies for library integration conflicts

## Documentation Structure

### By Experience Level

**Beginner (First Time Building):**
1. [Getting Started Guide](getting_started.md) - Essential first steps
2. [Hardware Configuration](hardware_configuration.md) - Basic wiring
3. [Development Guide](development_guide.md) - Software setup

**Intermediate (Modifying and Extending):**
1. [System Architecture](system_architecture.md) - Understanding design
2. [Motor Control](motor_control.md) - Movement customization
3. [Audio System](audio_system.md) - Sound effect integration

**Advanced (Core Development):**
1. [Development Guide](development_guide.md) - Full development workflow
2. [System Architecture](system_architecture.md) - Design patterns
3. All module-specific documentation for deep implementation details

### By System Component

**Movement System:**
- [Motor Control](motor_control.md) - Pimoroni Motor Shim integration
- [Hardware Configuration](hardware_configuration.md) - Wiring details
- [Gamepad Control](gamepad_control.md) - Input handling

**Audio System:**
- [Audio System](audio_system.md) - I2S implementation  
- [Embedded Audio Usage](embedded_audio_usage.md) - File management
- [Hardware Configuration](hardware_configuration.md) - DAC wiring

**Control System:**
- [Gamepad Control](gamepad_control.md) - BluePad32 integration
- [System Architecture](system_architecture.md) - Control flow
- [Development Guide](development_guide.md) - Debugging controls

**Development Workflow:**
- [Development Guide](development_guide.md) - Complete workflow
- [Getting Started Guide](getting_started.md) - Initial setup
- [System Architecture](system_architecture.md) - Design understanding

## Project Features

### Hardware Capabilities

- **Microcontroller**: Raspberry Pi Pico W (RP2350) with dual-core Cortex-M33
- **Wireless**: Built-in Wi-Fi and Bluetooth 5.2 with BluePad32 gamepad support
- **Movement**: Differential drive with Pimoroni Motor Shim
- **Audio**: I2S DAC output with embedded MP3 sound effects  
- **Servo Control**: (removed from project scope)
- **Expansion**: Multiple GPIO pins available for additional features

### Software Architecture

- **Real-Time Control**: 1ms motor control loop with safety monitoring
- **Concurrent Audio**: Independent I2S audio generation using PIO
- **Event-Driven Input**: BluePad32 gamepad event processing
- **Resource Management**: RAII-based hardware resource management
- **Safety Systems**: Emergency stop and fault detection throughout

### Development Tools

- **IDE Integration**: Full VS Code support with IntelliSense and debugging
- **Build System**: CMake with Ninja for fast compilation
- **Audio Pipeline**: Automated MP3 to C header conversion
- **Testing Framework**: Unit testing with hardware abstraction
- **Documentation**: Comprehensive guides with code examples

## File Organization

### Source Code Structure

```
src/
├── main.cpp                 # Main application entry point
├── AudioController.cpp      # I2S audio system implementation  
├── MotorController.cpp      # Motor control
├── exterminate_platform.cpp # BluePad32 platform integration
├── btstack_config.h         # Bluetooth stack configuration
└── sdkconfig.h             # SDK configuration options
```

### Header Files

```
include/
├── AudioController.h        # Audio system interface
├── MotorController.h        # Motor control interface
├── exterminate_platform.h  # Platform definitions
└── audio/                  # Generated audio headers (24 files)
    ├── audio_index.h       # Audio file index and metadata
    ├── 00001.h → 00024.h   # Individual audio file data
    └── ...
```

### Documentation Files

```
docs/
├── README.md               # This index file
├── getting_started.md      # New user tutorial
├── system_architecture.md  # High-level system design
├── hardware_configuration.md # Complete wiring guide
├── motor_control.md        # Movement system details
├── gamepad_control.md      # Input system documentation
├── audio_system.md         # I2S audio implementation
├── embedded_audio_usage.md # Audio file management
└── development_guide.md    # Complete development workflow
```

### Build and Tools

```
tools/
├── mp3_to_header.py        # Audio conversion script
├── convert_audio.ps1       # PowerShell automation
└── convert_audio.bat       # Windows batch automation

CMakeLists.txt              # Main build configuration
pico_sdk_import.cmake       # Pico SDK integration
.clang-format              # Code style configuration
```

## Getting Help

### Common Questions

**Q: Where do I start if I'm new to embedded development?**

A: Begin with the [Getting Started Guide](getting_started.md). It walks through everything from buying components to your first successful build.

**Q: How do I add my own sound effects?**

A: See [Embedded Audio Usage](embedded_audio_usage.md) for the complete process of converting MP3 files and integrating them into the build.

**Q: My motors aren't working correctly. What should I check?**

A: The [Motor Control](motor_control.md) documentation has a comprehensive troubleshooting section covering wiring, power, and software issues.

**Q: Can I use a different gamepad than the ones mentioned?**

A: The [Gamepad Control](gamepad_control.md) guide covers all supported controllers and includes information about compatibility with other Bluetooth gamepads.

**Q: How do I modify the code for my specific needs?**

A: Start with [System Architecture](system_architecture.md) to understand the overall design, then refer to the [Development Guide](development_guide.md) for the development workflow.

### Troubleshooting Resources

1. **Build Issues**: [Development Guide - Troubleshooting](development_guide.md#troubleshooting)
2. **Hardware Problems**: [Hardware Configuration - Safety](hardware_configuration.md#safety-considerations)  
3. **Audio Problems**: [Audio System - Troubleshooting](audio_system.md#troubleshooting)
4. **Motor Issues**: [Motor Control - Troubleshooting](motor_control.md#troubleshooting)
5. **Control Issues**: [Gamepad Control - Troubleshooting](gamepad_control.md#troubleshooting)

### Community and Support

- **Project Repository**: [GitHub Repository](https://github.com/your-username/Exterminate)
- **Issue Tracking**: Use GitHub Issues for bug reports and feature requests
- **Discussions**: GitHub Discussions for questions and community support
- **Wiki**: Additional community-contributed documentation and examples

## Version Information

**Documentation Version**: 1.0  
**Project Version**: See `CMakeLists.txt` for current version  
**Last Updated**: January 2025  
**Pico SDK Version**: 2.0+  
**BluePad32 Version**: Latest from submodule  

## Contributing to Documentation

We welcome improvements to the documentation! Here's how to contribute:

1. **Small Fixes**: Edit files directly and submit a pull request
2. **Major Changes**: Open an issue first to discuss the proposed changes
3. **New Sections**: Follow the existing documentation structure and style
4. **Code Examples**: Test all code examples before submitting

**Documentation Standards**:
- Use clear, concise language suitable for beginners
- Include practical examples and code snippets
- Provide troubleshooting information for common issues
- Cross-reference related documentation sections
- Follow markdown formatting guidelines

---

**Ready to Build Your Dalek?**

Start with the [Getting Started Guide](getting_started.md) and begin your journey into embedded robotics!

For quick reference, bookmark this index page and the documentation section most relevant to your current development phase.

**EXTERMINATE... with proper documentation!** 🤖📚
