# Audio System README

## Overview

The Exterminate Dalek project uses a preprocessed audio system for GitHub Actions compatibility. Audio files are converted from MP3 to PCM format during development and the generated header files are committed to the repository.

## Audio File Management

### Current Audio Library

The project contains **24 audio files** (00001.mp3 through 00024.mp3) converted to PCM format:

- **Format**: 22.05kHz, mono, 16-bit PCM
- **Total Duration**: ~2.5 minutes of audio
- **Memory Usage**: ~6MB of embedded audio data
- **Files**: `include/audio/00001.h` through `include/audio/00024.h`
- **Index**: `include/audio/audio_index.h` (master audio file registry)

### Adding or Updating Audio Files

**For Development (Local Changes):**

1. **Install Dependencies** (one-time setup):
   ```bash
   pip install librosa soundfile numpy==2.1.0
   ```

2. **Add MP3 Files**:
   - Place MP3 files in `misc/` directory
   - Use numbered naming: `00001.mp3`, `00002.mp3`, etc.

3. **Convert to PCM Headers**:
   ```bash
   # PowerShell
   .\tools\convert_audio.ps1
   
   # Command Prompt
   tools\convert_audio.bat
   
   # Python directly
   python tools/audio_to_pcm_header.py misc include/audio --pattern "*.mp3"
   ```

4. **Commit Generated Headers**:
   ```bash
   git add include/audio/*.h
   git commit -m "Update audio files"
   ```

**For GitHub Actions (CI/CD):**

- ✅ **No audio dependencies needed** - Header files are pre-generated
- ✅ **Fast builds** - No MP3 processing during CI
- ✅ **Reliable** - No dependency on external audio libraries

### Audio File Structure

**Generated Header Format:**
```cpp
// Example: include/audio/00001.h
namespace Exterminate::Audio {
    extern const int16_t AUDIO_00001_DATA[];      // PCM sample data
    extern const size_t AUDIO_00001_SAMPLE_COUNT; // Number of samples
    extern const size_t AUDIO_00001_BYTE_SIZE;    // Total bytes
    extern const uint32_t AUDIO_00001_SAMPLE_RATE; // 22050 Hz
    extern const uint8_t AUDIO_00001_CHANNELS;    // 1 (mono)
    extern const uint8_t AUDIO_00001_BIT_DEPTH;   // 16 bits
}
```

**Master Index (`audio_index.h`):**
```cpp
// Audio file registry with helper functions
enum class AudioIndex : size_t {
    AUDIO_00001 = 0,
    AUDIO_00002 = 1,
    // ... all 24 files
    COUNT = 24
};

const AudioFile* getAudioFile(AudioIndex index);
const AudioFile* getAudioFile(const char* name);
```

### Usage in Code

**Playing Audio Files:**
```cpp
#include "audio/audio_index.h"
using namespace Exterminate::Audio;

// Method 1: By index
auto audioFile = getAudioFile(AudioIndex::AUDIO_00001);
audioController.playPCMAudioData(
    audioFile->data,
    audioFile->sample_count,
    audioFile->sample_rate,
    audioFile->channels
);

// Method 2: By name
auto audioFile = getAudioFile("00001.mp3");
if (audioFile) {
    audioController.playAudio(AudioIndex::AUDIO_00001);
}
```

**Checking Available Audio:**
```cpp
// Get total number of audio files
size_t totalFiles = static_cast<size_t>(AudioIndex::COUNT); // 24

// List all available files
for (size_t i = 0; i < AUDIO_FILE_COUNT; ++i) {
    printf("Audio file %zu: %s (%.2f seconds)\n", 
           i, 
           AUDIO_FILES[i].name,
           (float)AUDIO_FILES[i].sample_count / AUDIO_FILES[i].sample_rate);
}
```

## Memory Usage

**Total Audio Memory**: ~6MB (embedded in flash)

**Per-File Breakdown:**
- **Shortest**: 00001.mp3 (1.4s, 61KB)
- **Longest**: 00022.mp3 (23.0s, 1MB)
- **Average**: ~250KB per file

**Memory Optimization:**
- Files are stored in flash memory (not RAM)
- PCM data is accessed directly (no copying)
- Mono format saves 50% vs stereo
- 22.05kHz sample rate balances quality/size

## Technical Details

**PCM Format Advantages:**
- ✅ **No CPU overhead** - Direct I2S output
- ✅ **Predictable timing** - No decoding delays
- ✅ **High quality** - No compression artifacts
- ✅ **Deterministic** - Perfect for real-time systems

**Build Integration:**
- Audio headers are automatically included in CMake build
- No additional build steps required
- Compatible with all Pico SDK versions
- Works with VS Code and command-line builds

**GitHub Actions Compatibility:**
- Pre-generated headers eliminate Python dependencies
- Fast CI builds (no audio processing)
- Deterministic builds across all platforms
- No external library requirements

## File Locations

```
Exterminate/
├── misc/                    # MP3 source files (not in git)
│   ├── 00001.mp3           # Original audio files
│   └── ...                 # (excluded by .gitignore)
├── include/audio/          # Generated PCM headers (in git)
│   ├── 00001.h            # PCM data for each file
│   ├── ...                # All 24 audio files
│   └── audio_index.h      # Master registry
└── tools/                  # Conversion tools
    ├── audio_to_pcm_header.py  # Main conversion script
    ├── convert_audio.ps1   # PowerShell wrapper
    └── convert_audio.bat   # Batch wrapper
```

## Troubleshooting

**Build Errors:**
- Ensure all `.h` files are committed to git
- Check that `audio_index.h` includes all files
- Verify PCM format consistency across files

**Audio Quality Issues:**
- Original MP3 quality affects PCM output
- Consider higher sample rates for critical audio
- Check for clipping in conversion process

**Memory Issues:**
- Monitor total flash usage with large audio sets
- Consider shorter clips or lower sample rates
- Use compression-friendly original formats

**Development Workflow:**
- Always test converted audio before committing
- Keep MP3 sources backed up separately
- Document audio file purposes in code comments

## Future Enhancements

**Potential Improvements:**
- Dynamic loading for larger audio libraries
- Compression-optimized PCM formats
- Multi-sample rate support
- Background audio streaming
- Audio effects processing
