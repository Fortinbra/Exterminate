# Embedded Audio System Usage

> **Copyright Notice**: This project is for educational purposes only. Users are responsible for ensuring they have appropriate rights to any audio content used. The system provides tools for audio conversion but does not include copyrighted content.

## Overview

The project includes a complete embedded audio system that converts audio files into PCM C++ headers for direct I2S playback. Audio data is preprocessed and committed to the repository for GitHub Actions compatibility.

## Audio System Features

✅ **Pre-processed PCM Audio** - No real-time decoding required  
✅ **GitHub Actions Ready** - No Python dependencies for builds  
✅ **24 Audio Files** - Complete Dalek sound library included  
✅ **Optimized Format** - 22.05kHz mono 16-bit for memory efficiency  
✅ **Direct I2S Output** - Zero CPU overhead during playback  

## Quick Start

### 1. Using Existing Audio (Recommended)

The repository includes 24 pre-converted audio files ready for use:

```cpp
#include "AudioController.h"
#include "audio/audio_index.h"

// Initialize audio controller
AudioController::Config config;
config.bclkPin = 18;        // I2S Bit Clock
config.lrclkPin = 19;       // I2S Left/Right Clock  
config.dinPin = 20;         // I2S Data Input
config.sampleRate = 22050;  // Sample rate
config.bitsPerSample = 16;  // 16-bit PCM

AudioController audioController(config);
audioController.initialize();

// Play audio file by index
audioController.playAudio(Audio::AudioIndex::AUDIO_00001);

// Or get file info first
auto audioFile = Audio::getAudioFile(Audio::AudioIndex::AUDIO_00001);
if (audioFile) {
    printf("Playing: %s (%.2f seconds)\n", 
           audioFile->name,
           (float)audioFile->sample_count / audioFile->sample_rate);
    
    audioController.playPCMAudioData(
        audioFile->data,
        audioFile->sample_count,
        audioFile->sample_rate,
        audioFile->channels
    );
}
```

### 2. Adding New Audio Files (Development Only)

**Prerequisites** (one-time setup):
```bash
pip install librosa soundfile numpy==2.1.0
```

**Conversion Process**:
1. Place MP3 files in `misc/` directory with numbered names (`00025.mp3`, etc.)
2. Run conversion script:
   ```powershell
   # PowerShell
   .\tools\convert_audio.ps1
   
   # Command Prompt  
   tools\convert_audio.bat
   
   # Python directly
   python tools/audio_to_pcm_header.py misc include/audio --pattern "*.mp3"
   ```
3. Commit the generated `.h` files to git (MP3 files are excluded by `.gitignore`)
4. Rebuild project

AudioController audio(config);

// Play a specific audio file
audio.playAudio(AudioIndex::AUDIO_00001);

// Set volume (0.0 to 1.0)
audio.setVolume(0.8f);
```

## System Architecture

### Generated Files

For each MP3 file `XXXXX.mp3`, the system generates:
- `include/audio/XXXXX.h` - Contains the audio data as a byte array
- `include/audio/audio_index.h` - Registry of all available audio files

### Audio File Registry

The `audio_index.h` provides:
- `AUDIO_FILES[]` - Array of AudioFile structs with name, data pointer, and size
- `AUDIO_FILE_COUNT` - Total number of available files
- `AudioIndex` enum - Strongly typed indices for each audio file

### Hardware Configuration

The audio system uses PIO (Programmable I/O) to implement I2S output:
- **GPIO 18**: I2S Clock (SCK)
- **GPIO 19**: I2S Data (SD) 
- **GPIO 20**: I2S Word Select (WS/LRCK)

Connect these to an I2S DAC or audio codec for output.

## File Size Considerations

- Audio files are embedded directly in flash memory
- Large MP3 files will increase the program size
- The RP2040 has 2MB of flash memory total
- Current 24 audio files use approximately 2.2MB total

## Git Configuration

The system is configured to:
- ✅ Include generated `.h` files in version control
- ❌ Exclude source `.mp3` files from version control (via `.gitignore`)

This means the generated headers are committed, but the source MP3 files are not tracked.

## Build Integration

The audio system is automatically included in the build:
- Headers are in the include path
- AudioController.cpp is compiled
- No additional CMake configuration needed

## Troubleshooting

### Compilation Errors
If you get "missing terminating character" errors:
1. Delete all files in `include/audio/`
2. Re-run `python tools\mp3_to_header.py misc include\audio`
3. Clean and rebuild: `ninja clean && ninja`

### Memory Issues
If you run out of flash memory:
1. Remove unused MP3 files from `misc/`
2. Use shorter/compressed audio clips
3. Consider using lower bitrate MP3s

## Example: Dalek Voice System

```cpp
// Play the iconic "Exterminate!" sound
if (gamepad.buttonPressed(BUTTON_A)) {
    audio.playAudio(AudioIndex::AUDIO_00001); // Exterminate
}

// Play movement sounds
if (gamepad.leftStick.x != 0 || gamepad.leftStick.y != 0) {
    audio.playAudio(AudioIndex::AUDIO_00005); // Movement sound
}
```

This embedded audio system provides a robust foundation for the animatronic Dalek's sound effects, allowing for rich audio feedback without external dependencies.
