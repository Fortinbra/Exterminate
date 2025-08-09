# Audio System Documentation

This document explains the complete audio system in the Exterminate project, including the PIO-based I2S implementation, embedded audio conversion, and LED visualization integration.

> **Copyright Notice**: This project is for educational purposes only. Users are responsible for ensuring they have appropriate rights to any audio content used. The "Dalek" name and associated audio are the intellectual property of the BBC.

## Overview

The audio system consists of three main components:

1. **PIO-Based I2S Output**: Custom PIO state machines for reliable audio streaming
2. **Embedded Audio Data**: MP3 files converted to PCM data and compiled into firmware  
3. **LED Visualization**: Real-time audio-reactive LED effects synchronized with audio playback

## PIO-Based I2S Implementation

### Architecture

The I2S system uses the RP2040's Programmable I/O (PIO) blocks to generate precise audio timing:

- **PIO State Machine 0**: Generates I2S bit clock (BCLK) and word select (LRCLK)
- **PIO State Machine 1**: Outputs audio data synchronized with clocks
- **DMA Integration**: Double-buffered DMA for continuous audio streaming
- **Sample Rate**: 22.05kHz for optimized performance and memory usage

### Hardware Configuration

```text
GPIO Pin    I2S Signal    Function
--------    ----------    --------
GPIO 6      BCLK          Bit Clock (22.05kHz × 32)
GPIO 8      LRCLK         Left/Right Word Select (22.05kHz)
GPIO 9      DIN           Audio Data Output (16-bit PCM)
```

**MAX98357A I2S Amplifier:**
- **Power**: 5V from Pico W VSYS pin (up to 3W output)
- **Speaker**: 4-8Ω impedance, 3W power handling recommended
- **Gain**: Fixed at 9dB when GAIN pin tied to GND
- **Efficiency**: High efficiency Class D amplifier design

### PIO Assembly Programs

The system includes custom PIO assembly in `src/i2s.pio`:

- **Clock Generation**: Precise timing for I2S protocol compliance
- **Data Output**: Synchronized 16-bit PCM data transmission
- **FIFO Management**: Automatic handling of audio data flow

## Embedded Audio Conversion

### Converting MP3 Files

#### Prerequisites

- Python 3.6 or later
- MP3 files in the `misc/` directory

#### Conversion Process

1. **Place MP3 files** in the `misc/` directory
2. **Run the conversion script**:

   ```bash
   # Windows PowerShell
   .\tools\convert_audio.ps1
   
   # Windows Command Prompt
   tools\convert_audio.bat
   
   # Manual Python execution
   python tools\mp3_to_header.py misc include\audio
   ```

3. **Generated files** will be created in `include/audio/`:
   - Individual headers: `00001.h`, `00002.h`, etc.
   - Index file: `audio_index.h`

#### File Organization

- **Source MP3s**: `misc/*.mp3` (excluded from git)
- **Generated Headers**: `include/audio/*.h` (included in git)
- **Tools**: `tools/mp3_to_header.py`, `tools/convert_audio.ps1`, `tools/convert_audio.bat`

## Audio Playback with LED Integration

### Include Headers

```cpp
#include "AudioController.h"
#include "SimpleLED.h"
#include "audio/audio_index.h"
```

### Basic Usage

```cpp
// Initialize audio
AudioController audioController;

// Configure I2S with PIO for MAX98357A
AudioController::Config audioConfig = {
    .bclkPin = 6,              // I2S bit clock
    .lrclkPin = 8,             // I2S word select
    .dinPin = 9,               // I2S data output
    .sampleRate = 22050,       // 22.05kHz sample rate
    .bitsPerSample = 16        // 16-bit PCM audio
};

// Initialize audio and LED PWM pins
audioController.begin(audioConfig);

// Use SimpleLED to set up PWM on two external LEDs (GPIO 11 and 12)
Exterminate::SimpleLED::initializePwmPin(11, /*wrap*/ 255, /*clkdiv*/ 4.0f);
Exterminate::SimpleLED::initializePwmPin(12, /*wrap*/ 255, /*clkdiv*/ 4.0f);

// Play audio; a repeating timer in main.cpp maps audio intensity to LED brightness
audioController.playAudio(Audio::AudioIndex::AUDIO_00001);
```

### Initialize Audio Controller

```cpp
using namespace Exterminate;

// Configure audio controller
AudioController::Config audioConfig = {
    .bclkPin = 18,        // I2S bit clock
    .lrclkPin = 19,       // I2S left/right clock
    .dinPin = 20,         // I2S data input
    .sampleRate = 44100,  // 44.1kHz sample rate
    .bitsPerSample = 16   // 16-bit samples
};

// Create and initialize controller
AudioController audioController(audioConfig);
if (!audioController.initialize()) {
    // Handle initialization error
}
```

### Play Audio Files

```cpp
using namespace Exterminate::Audio;

// Play specific audio file
audioController.playAudio(AudioIndex::AUDIO_00001);  // "Exterminate!"

// Check if playing
if (audioController.isPlaying()) {
    // Audio is currently playing
}

// Stop playback
audioController.stopAudio();
```

### Volume Control

```cpp
// Set volume (0.0 = mute, 1.0 = maximum)
audioController.setVolume(0.8f);

// Get current volume
float currentVolume = audioController.getVolume();
```

### Audio File Information

```cpp
// Get total number of audio files
size_t fileCount = AudioController::getAudioFileCount();

// Get information about a specific file
const AudioFile* fileInfo = AudioController::getAudioFileInfo(AudioIndex::AUDIO_00001);
if (fileInfo) {
    printf("File: %s, Size: %zu bytes\\n", fileInfo->name, fileInfo->size);
}
```

## Integration with Gamepad Control

### Example Integration

```cpp
// In exterminate_platform_on_controller_data()
void exterminate_platform_on_controller_data(bp_controller_t* ctl, bp_gamepad_t* gp) {
    // Audio trigger on right trigger
    if (gp->misc_buttons & BUTTON_TRIGGER_R) {
        static bool triggerPressed = false;
        if (!triggerPressed) {
            // Play "Exterminate!" sound
            audioController.playAudio(Audio::AudioIndex::AUDIO_00001);
            triggerPressed = true;
        }
    } else {
        triggerPressed = false;
    }
    
    // Additional sound effects on face buttons
    if (gp->buttons & BUTTON_A) {
        audioController.playAudio(Audio::AudioIndex::AUDIO_00002);
    }
    if (gp->buttons & BUTTON_B) {
        audioController.playAudio(Audio::AudioIndex::AUDIO_00003);
    }
}
```

## Audio File Mapping

The following audio files are currently available:

| Index | File | Description |
|-------|------|-------------|
| `AudioIndex::AUDIO_00001` | 00001.mp3 | "Exterminate!" |
| `AudioIndex::AUDIO_00002` | 00002.mp3 | Additional sound effect |
| `AudioIndex::AUDIO_00003` | 00003.mp3 | Additional sound effect |
| ... | ... | ... |
| `AudioIndex::AUDIO_00024` | 00024.mp3 | Additional sound effect |

## Memory Usage

Each MP3 file is stored as a byte array in flash memory:

- **Total Size**: Approximately 2.2 MB for all 24 files
- **Memory Type**: Flash (program memory)
- **Access Time**: Instant (no file system overhead)

## Technical Notes

### MP3 Decoding

The current implementation stores raw MP3 data. For playback, you'll need:

1. **MP3 Decoder**: Software library to decode MP3 to PCM
2. **I2S Driver**: Hardware interface for audio output
3. **Buffering**: Audio streaming and buffer management

### Recommended Libraries

- **MP3 Decoder**: libmad, minimp3, or helix-mp3
- **I2S**: Pico SDK hardware_i2s or PIO-based implementation
- **Audio Processing**: Custom audio pipeline for your specific needs

### Performance Considerations

- **Flash Usage**: Each file uses flash memory proportional to its size
- **RAM Usage**: Only currently playing audio needs RAM buffering
- **CPU Usage**: MP3 decoding requires CPU cycles during playback

## Troubleshooting

### Common Issues

1. **Python not found**: Install Python 3.6+ and ensure it's in PATH
2. **Permission errors**: Run as administrator or check file permissions
3. **Large file sizes**: Consider compressing MP3s or using lower bitrates
4. **Compilation errors**: Ensure audio headers are properly included

### Build Integration

The conversion process should be run whenever MP3 files are added or changed:

1. Add new MP3 files to `misc/`
2. Run conversion script
3. Commit generated headers to git
4. Build project

### Memory Optimization

For large audio files, consider:

- **Lower bitrates**: 64-128 kbps instead of higher quality
- **Shorter clips**: Trim unnecessary silence or length
- **Compression**: Use more aggressive MP3 compression
- **Selective inclusion**: Only include frequently used sounds
