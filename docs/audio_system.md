# Audio System Documentation

This document explains the audio system in the Exterminate project, including the Pico Extras I2S implementation, embedded PCM audio conversion, and LED visualization integration.

> **Copyright Notice**: This project is for educational purposes only. Users are responsible for ensuring they have appropriate rights to any audio content used. The "Dalek" name and associated audio are the intellectual property of the BBC.

## Overview

The audio system consists of three main components:

1. I2S output via Pico Extras library (reliable, battle‑tested)
2. Embedded PCM audio data converted from MP3/WAV and compiled into firmware
3. LED visualization: real-time audio‑reactive effects synchronized with playback

## I2S Output (Pico Extras)

### Architecture

The audio pipeline leverages the Pico Extras `audio_i2s` driver, which manages I2S clocks, DMA, and buffer queues:

- **Pico Extras `audio_i2s`**: Provides BCLK/LRCLK and streams PCM via DMA
- **Producer buffer pool**: Triple‑buffered for smooth playback
- **Default sample rate**: 44.1 kHz (matches converted PCM files)
- **Resource Discovery Pattern**: Finds available DMA channels and PIO state machines without conflicts

### Resource Management

**Critical Implementation Detail**: The system uses a resource discovery pattern to avoid DMA channel conflicts with the pico-extras I2S library:

```cpp
// Find available resources without permanently claiming them
int dma_channel = dma_claim_unused_channel(false);  // false = don't claim permanently
uint pio_sm = pio_claim_unused_sm(pio, false);      // false = don't claim permanently

// Immediately release for the I2S library to claim internally
dma_channel_unclaim(dma_channel);
pio_sm_unclaim(pio, pio_sm);

// Let pico-extras library manage resources internally
audio_i2s_config_t config = {
    .data_pin = discovered_data_pin,
    .clock_pin_base = discovered_clock_base,
    .dma_channel = dma_channel,
    .pio_sm = pio_sm
};
audio_i2s_setup(&format, &config);
```

**Why This Approach?**: The pico-extras I2S library expects to manage its own DMA channels and PIO state machines internally. Pre-claiming these resources causes runtime panics like "DMA channel X is already claimed".

### Hardware Configuration

```text
GPIO Pin    I2S Signal    Function
--------    ----------    --------
GPIO 32     BCLK          Bit Clock base (clockPinBase)
GPIO 33     LRCLK         Word Select (clockPinBase + 1)
GPIO 34     DOUT          Audio Data Output (16‑bit PCM)
```

**MAX98357A I2S Amplifier:**

- **Power**: 5V from Pico W VSYS pin (up to 3W output)
- **Speaker**: 4-8Ω impedance, 3W power handling recommended
- **Gain**: Fixed at 9dB when GAIN pin tied to GND
- **Efficiency**: High efficiency Class D amplifier design

**Library Integration**: The pico-extras library automatically configures PIO and DMA resources. No custom PIO assembly is required.

## Embedded Audio Conversion (to PCM)

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

3. Generated files will be created in `include/audio/`:
    - Individual headers: `00001.h`, `00002.h`, etc.
    - Index file: `audio_index.h`

Note: Ensure your conversion sample rate matches the runtime I2S sample rate. The code defaults to 44,100 Hz. If you prefer 22,050 Hz, either pass `-SampleRate 22050` to the script and update `AudioController::Config.sampleRate`, or convert at 44,100 Hz to match the default.

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
// Initialize audio with Pico Extras I2S (defaults: dataPin=34, clockPinBase=32, sampleRate=44100)
AudioController::Config cfg = AudioController::Config::getDefault();
// Optional: customize pins or sample rate
// cfg.dataPin = 34;           // I2S DOUT
// cfg.clockPinBase = 32;      // BCLK at 32, LRCLK at 33
// cfg.sampleRate = 44100;     // Must match converted PCM files

AudioController audioController(cfg);
audioController.initialize();

// Use SimpleLED to set up PWM on two external LEDs (GPIO 11 and 12)
Exterminate::SimpleLED::initializePwmPin(11, /*wrap*/ 255, /*clkdiv*/ 4.0f);
Exterminate::SimpleLED::initializePwmPin(12, /*wrap*/ 255, /*clkdiv*/ 4.0f);

// Play audio; a repeating timer in main.cpp maps audio intensity to LED brightness
audioController.playAudio(Exterminate::Audio::AudioIndex::AUDIO_00001);
```

### Initialize Audio Controller

```cpp
// Create and initialize controller
AudioController audioController(AudioController::Config::getDefault());
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
// Get total number of audio files (from audio_index.h)
size_t fileCount = Exterminate::Audio::AUDIO_FILE_COUNT;

// Get information about a specific file
const Exterminate::Audio::AudioFile* fileInfo = Exterminate::Audio::getAudioFile(Exterminate::Audio::AudioIndex::AUDIO_00001);
if (fileInfo) {
    printf("File: %s, Samples: %zu, Rate: %u Hz\n",
           fileInfo->name, fileInfo->sample_count, fileInfo->sample_rate);
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

Each audio file is stored as PCM samples in flash memory (int16_t arrays):

- Memory type: Flash (program memory)
- Access time: Instant (no filesystem)
- Size scales with duration × sample rate × channels × 2 bytes

## Technical Notes

### Decoding

The current implementation stores uncompressed PCM data. No runtime decoding is required; data is streamed directly to I2S.

### Recommended Libraries

- I2S: Pico Extras `audio_i2s`
- Optional: add DSP or effects as needed

### Performance Considerations

- Flash usage: proportional to PCM size
- RAM usage: small I2S buffers (configurable)
- CPU usage: minimal (DMA‑driven I2S)

## Troubleshooting

### Common Issues

1. **Python not found**: Install Python 3.6+ and ensure it's in PATH
2. **Permission errors**: Run as administrator or check file permissions
3. **Large file sizes**: Consider compressing MP3s or using lower bitrates
4. **Compilation errors**: Ensure audio headers are properly included

### Build Integration

Run the conversion whenever audio files are added or changed:

1. Add new source files to `misc/`
2. Run conversion script (ensure sample rate matches runtime)
3. Commit generated headers to git
4. Build project

### Memory Optimization

For large audio files, consider:

- **Lower bitrates**: 64-128 kbps instead of higher quality
- **Shorter clips**: Trim unnecessary silence or length
- **Compression**: Use more aggressive MP3 compression
- **Selective inclusion**: Only include frequently used sounds
