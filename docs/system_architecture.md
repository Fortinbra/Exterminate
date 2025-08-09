# System Architecture

## Overview

The Exterminate Dalek project implements a modular, real-time control system for an animatronic Dalek robot. The architecture follows modern C++ principles with RAII resource management, clear separation of concerns, and hardware abstraction.

## High-Level Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Bluetooth     │    │   USB Debug     │    │  Physical I/O   │
│   Gamepad       │◄──►│   Console       │◄──►│   Hardware      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
          │                       │                       │
          ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                     Pico W Hardware                              │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ │
│  │   CYW43     │ │    ARM      │ │     PIO     │ │    PWM      │ │
│  │ WiFi/BT     │ │  Cortex-M0+ │ │  I2S/LEDs   │ │ Motors/LEDs │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘ │
└─────────────────────────────────────────────────────────────────┘
          │                       │                       │
          ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   BluePad32     │    │  Exterminate    │    │   Hardware      │
│   Platform      │◄──►│   Platform      │◄──►│   Controllers   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                               │
                               ▼
                    ┌─────────────────┐
                    │  Motor Control  │
                    │  I2S Audio      │
                    │  LED System     │
                    │  Servo Control  │
                    └─────────────────┘
```

## Core Components

### 1. Main Application Layer

**File**: `src/main.cpp`

**Responsibilities**:
- System initialization and startup
- CYW43 wireless subsystem setup
- BluePad32 library initialization
- Main event loop coordination

**Key Functions**:
```cpp
int main() {
    // Initialize hardware and peripherals
    stdio_init_all();
    cyw43_arch_init();
    
    // Configure custom platform
    uni_platform_set_custom(get_exterminate_platform());
    
    // Start BluePad32 event loop
    uni_init(0, NULL);
    btstack_run_loop_execute();  // Never returns
}
```

### 2. Platform Integration Layer

**Files**: `src/exterminate_platform.cpp`, `include/exterminate_platform.h`

**Responsibilities**:
- BluePad32 custom platform implementation
- Gamepad event handling and processing
- Motor control integration
- Safety systems and emergency stops

**Architecture Pattern**: Observer pattern for gamepad events

```cpp
// Platform callbacks handle all external events
static const struct uni_platform exterminate_platform = {
    .on_device_connected = handle_controller_connect,
    .on_device_disconnected = handle_controller_disconnect,
    .on_controller_data = handle_controller_input,
    .on_oob_event = handle_system_events,
};
```

### 3. Motor Control Subsystem

**Files**: `src/MotorController.cpp`, `include/MotorController.h`

**Responsibilities**:
- PWM-based motor speed control
- Differential drive kinematics
- Motor safety and protection
- Hardware abstraction for DRV8833

**Design Patterns**:
- **RAII**: Automatic resource management
- **Strategy**: Configurable control algorithms
- **Facade**: Simplified interface for complex motor operations

```cpp
class MotorController {
    // RAII ensures proper initialization/cleanup
    explicit MotorController(const Config& config);
    ~MotorController();
    
    // High-level control interface
    void setDifferentialDrive(float linear, float angular);
    void moveForward(float speed);
    void emergencyStop();
};
```

### 4. I2S Audio Subsystem

**Files**: `src/I2S.cpp`, `include/I2S.h`, `src/i2s.pio`

**Responsibilities**:
- PIO-based I2S protocol implementation
- Precise audio timing and clock generation
- DMA-based audio streaming
- Hardware-level audio output

**Architecture**:
- **PIO State Machines**: Custom assembly for I2S timing
- **DMA Integration**: Double-buffered continuous audio
- **Hardware Abstraction**: Clean C++ interface over PIO complexity
- **RAII Design**: Automatic resource management

```cpp
class I2S {
    // Configure PIO-based I2S
    explicit I2S(const Config& config);
    ~I2S();
    
    // Audio streaming interface
    void start();
    void writeAudio(const int16_t* data, size_t samples);
    void stop();
};
```

### 5. Audio Controller Subsystem

**Files**: `src/AudioController.cpp`, `include/AudioController.h`, `include/audio/*`

**Responsibilities**:
- High-level audio playback management
- Embedded audio file handling
- LED integration and audio intensity calculation
- Volume control and audio mixing

**Architecture**:
- **Embedded Resources**: MP3 files converted to PCM byte arrays
- **I2S Integration**: Uses I2S class for hardware output
- **LED Synchronization**: Real-time audio intensity for LED effects
- **Resource Management**: Compile-time audio file registry

```cpp
class AudioController {
    // Configure audio system with LED integration
    explicit AudioController(const Config& config);
    
    // High-level audio interface
    void playAudio(AudioIndex audioFile);
    float getAudioIntensity() const;  // For LED synchronization
    void setVolume(float volume);
};
```

### 6. LED Subsystem (SimpleLED)

**Files**: `src/SimpleLED.cpp`, `include/SimpleLED.h`

**Responsibilities**:
- Initialize PWM on external LED pins
- Set per-pin brightness from 0.0 to 1.0
- Integrate with audio intensity via a repeating timer in `main.cpp`

**Architecture**:
- **PWM Control**: Hardware PWM via Pico SDK
- **Mapping**: Deadzone, gamma and peak-hold applied in `main.cpp`

```cpp
// Example usage in main.cpp
Exterminate::SimpleLED::initializePwmPin(11, 255, 4.0f);
Exterminate::SimpleLED::initializePwmPin(12, 255, 4.0f);
Exterminate::SimpleLED::setBrightnessPin(11, 0.75f);
Exterminate::SimpleLED::setBrightnessPin(12, 0.5f);
```

## Data Flow Architecture

### Input Processing Flow

```
Gamepad Input → BluePad32 → Platform Layer → Control Logic → Hardware Output
     │              │            │              │              │
     ▼              ▼            ▼              ▼              ▼
Bluetooth HID → Event Queue → Normalization → Algorithms → PWM/PIO
```

**Latency Budget**:
- Bluetooth input: ~5-10ms
- Event processing: <1ms
- Control algorithms: <1ms
- Hardware output: <1ms
- **Total system latency**: <15ms

### Control Loop Architecture

```cpp
// Real-time control loop (event-driven)
void on_controller_data(uni_controller_t* controller) {
    // 1. Input validation and normalization
    float forward = normalize_axis(controller->gamepad.axis_y);
    float turn = normalize_axis(controller->gamepad.axis_x);
    
    // 2. Apply deadzone and filtering
    forward = apply_deadzone(forward, 0.1f);
    turn = apply_deadzone(turn, 0.1f);
    
    // 3. Control algorithm execution
    motorController->setDifferentialDrive(forward, turn);
    
    // 4. Audio/visual feedback
    handle_button_events(controller->gamepad.buttons);
}
```

## Memory Architecture

### Flash Memory Layout

```
Flash (2MB total):
├── Bootloader (32KB)
├── Application Code (400KB)
├── BluePad32 Library (600KB)
├── Pico SDK (400KB)
├── Audio Data (400KB)
└── Free Space (200KB)
```

### RAM Usage (264KB total)

```
RAM Allocation:
├── Stack (32KB)
├── Heap (100KB)
├── BluePad32 Buffers (64KB)
├── Audio Buffers (32KB)
├── Application State (8KB)
└── System Reserved (28KB)
```

### Resource Management

**RAII Pattern Implementation**:
```cpp
class ResourceManager {
    // Constructor acquires resources
    ResourceManager() : gpio_initialized_(false) {
        if (initialize_gpio()) {
            gpio_initialized_ = true;
        }
    }
    
    // Destructor releases resources
    ~ResourceManager() {
        if (gpio_initialized_) {
            cleanup_gpio();
        }
    }
    
private:
    bool gpio_initialized_;
};
```

## Concurrency Model

### Event-Driven Architecture

The system uses an event-driven, single-threaded model with cooperative multitasking:

```cpp
// BTstack run loop handles all events
void btstack_run_loop_execute() {
    while (true) {
        // Process Bluetooth events
        process_bluetooth_events();
        
        // Handle timer callbacks
        process_timer_events();
        
        // Yield to other tasks
        tight_loop_contents();
    }
}
```

**Benefits**:
- No thread synchronization complexity
- Deterministic event processing
- Lower memory overhead
- Easier debugging and testing

### Real-Time Guarantees

**Critical Timing Requirements**:
- Motor control updates: <1ms response time
- Audio sample processing: 44.1kHz (22.7μs per sample)
- Gamepad input processing: <10ms end-to-end

**Implementation**:
- Hardware timers for audio sampling
- Interrupt-driven PWM for motor control
- Event prioritization for critical functions

## Error Handling Strategy

### Defensive Programming

```cpp
class SafeMotorController {
public:
    bool setSpeed(float speed) {
        // Input validation
        if (speed < -1.0f || speed > 1.0f) {
            log_error("Invalid speed value: %f", speed);
            return false;
        }
        
        // Hardware state validation
        if (!hardware_initialized_) {
            log_error("Hardware not initialized");
            return false;
        }
        
        // Safe execution
        return execute_speed_change(speed);
    }
};
```

### Graceful Degradation

**Failure Recovery**:
1. **Communication Loss**: Motors stop automatically
2. **Power Issues**: Reduce performance gracefully
3. **Hardware Faults**: Disable affected subsystems
4. **Memory Issues**: Reduce audio quality/features

## Configuration Management

### Compile-Time Configuration

**Build System**: CMake with configurable options
```cmake
# Motor control options
option(ENABLE_MOTOR_DEBUG "Enable motor debug output" OFF)
option(MOTOR_PWM_FREQUENCY "Motor PWM frequency" 10000)

# Audio system options  
option(ENABLE_AUDIO "Enable audio subsystem" ON)
option(AUDIO_SAMPLE_RATE "Audio sample rate" 44100)
```

### Runtime Configuration

**Configuration Structures**:
```cpp
struct SystemConfig {
    MotorController::Config motor_config;
    AudioController::Config audio_config;
    float control_deadzone = 0.1f;
    uint32_t safety_timeout_ms = 1000;
};
```

## Testing Strategy

### Unit Testing

**Testable Components**:
- Motor control algorithms (isolated from hardware)
- Audio file management (embedded resource access)
- Input processing and filtering
- Safety and validation logic

**Mock Hardware**:
```cpp
class MockMotorHardware : public MotorHardwareInterface {
public:
    void setPWM(uint8_t pin, float dutyCycle) override {
        recorded_pwm_values_[pin] = dutyCycle;
    }
    
    std::map<uint8_t, float> recorded_pwm_values_;
};
```

### Integration Testing

**Hardware-in-the-Loop**:
- Real gamepad input testing
- Motor response verification
- Audio output validation
- Safety system testing

## Performance Optimization

### CPU Usage Optimization

**Hot Paths**:
- Motor control: Optimized for <50μs execution time
- Audio processing: DMA-based for zero-copy operation
- Input handling: Minimal processing in event callbacks

**Profiling Points**:
```cpp
#ifdef ENABLE_PROFILING
#define PROFILE_START(name) uint32_t start_##name = time_us_32()
#define PROFILE_END(name) log_profile(#name, time_us_32() - start_##name)
#else
#define PROFILE_START(name)
#define PROFILE_END(name)
#endif
```

### Memory Usage Optimization

**Strategies**:
- Stack allocation preferred over heap
- Compile-time resource allocation
- Minimal dynamic memory usage
- Efficient data structures

This architecture provides a solid foundation for reliable, real-time control of the Exterminate Dalek while maintaining code quality and extensibility.
