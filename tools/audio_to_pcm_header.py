#!/usr/bin/env python3
"""
Audio to PCM Header Converter

Converts audio files (MP3, WAV, etc.) to PCM format C++ header files for direct I2S playback.

USAGE FOR GITHUB ACTIONS:
This script is used to preprocess all audio files during development.
The generated .h files are committed to the repository so that GitHub Actions
can build without needing audio processing dependencies (librosa, soundfile).

To update audio files:
1. Place new MP3 files in misc/ directory
2. Run: python tools/audio_to_pcm_header.py misc include/audio --pattern "*.mp3"
3. Commit the generated .h files to git
4. The MP3 files are excluded by .gitignore
"""

import os
import sys
import argparse
from pathlib import Path
import numpy as np

# Check for required dependencies
try:
    import librosa
    import soundfile as sf
    AUDIO_LIBS_AVAILABLE = True
except ImportError:
    AUDIO_LIBS_AVAILABLE = False
    print("Warning: librosa and/or soundfile not installed.")
    print("Install with: pip install librosa soundfile")
    print("Falling back to basic file reading (for pre-converted PCM files only)")

def sanitize_variable_name(filename):
    """Convert filename to a valid C++ variable name."""
    # Remove extension and convert to valid identifier
    name = Path(filename).stem
    # Replace invalid characters with underscores
    sanitized = ''.join(c if c.isalnum() else '_' for c in name)
    # Ensure it doesn't start with a number
    if sanitized[0].isdigit():
        sanitized = 'audio_' + sanitized
    return sanitized.upper()

def audio_to_pcm(audio_path, sample_rate=44100, channels=1, bit_depth=16):
    """
    Convert audio file to PCM format suitable for I2S playback.
    
    Args:
        audio_path: Path to input audio file
        sample_rate: Target sample rate (Hz)
        channels: Number of channels (1=mono, 2=stereo)
        bit_depth: Bits per sample (16 or 32)
    
    Returns:
        numpy array of PCM samples, or None if conversion failed
    """
    if not AUDIO_LIBS_AVAILABLE:
        print(f"Error: Cannot convert {audio_path} - audio libraries not installed")
        return None
    
    try:
        # Load audio file using librosa
        print(f"Loading {audio_path}...")
        y, sr = librosa.load(audio_path, sr=sample_rate, mono=(channels == 1))
        
        # Convert to target format
        if bit_depth == 16:
            # Convert to 16-bit signed integers
            pcm_data = (y * 32767).astype(np.int16)
            dtype_name = "int16_t"
        elif bit_depth == 32:
            # Convert to 32-bit signed integers  
            pcm_data = (y * 2147483647).astype(np.int32)
            dtype_name = "int32_t"
        else:
            raise ValueError(f"Unsupported bit depth: {bit_depth}")
        
        # Handle stereo conversion if needed
        if channels == 2 and len(pcm_data.shape) == 1:
            # Convert mono to stereo by duplicating channel
            pcm_data = np.stack([pcm_data, pcm_data], axis=1)
        elif channels == 1 and len(pcm_data.shape) == 2:
            # Convert stereo to mono by averaging channels
            pcm_data = np.mean(pcm_data, axis=1).astype(pcm_data.dtype)
        
        print(f"Converted to {sample_rate}Hz, {channels} channel(s), {bit_depth}-bit PCM")
        print(f"Duration: {len(pcm_data) / sample_rate:.2f} seconds")
        print(f"Data size: {len(pcm_data) * (bit_depth // 8):,} bytes")
        
        return pcm_data, dtype_name
        
    except Exception as e:
        print(f"Error converting {audio_path}: {e}")
        return None

def pcm_to_header(audio_path, output_dir, sample_rate=44100, channels=1, bit_depth=16):
    """Convert an audio file to a C++ header file with PCM data."""
    audio_file = Path(audio_path)
    if not audio_file.exists():
        print(f"Error: {audio_path} does not exist")
        return False
    
    # Convert audio to PCM
    result = audio_to_pcm(audio_path, sample_rate, channels, bit_depth)
    if result is None:
        return False
    
    pcm_data, dtype_name = result
    
    # Generate variable name
    var_name = sanitize_variable_name(audio_file.name)
    
    # Create header filename
    header_name = f"{audio_file.stem}.h"
    header_path = Path(output_dir) / header_name
    
    # Ensure output directory exists
    header_path.parent.mkdir(parents=True, exist_ok=True)
    
    # Calculate audio metadata
    duration_ms = int((len(pcm_data) / sample_rate) * 1000)
    bytes_per_sample = bit_depth // 8
    total_bytes = len(pcm_data) * bytes_per_sample
    
    # Generate C++ header content
    header_content = f"""#pragma once

// Auto-generated from {audio_file.name}
// DO NOT EDIT - Generated by tools/audio_to_pcm_header.py

#include <cstdint>
#include <cstddef>

namespace Exterminate {{
namespace Audio {{

// PCM audio data for {audio_file.name}
// Format: {sample_rate}Hz, {channels} channel(s), {bit_depth}-bit PCM
// Duration: {duration_ms}ms ({len(pcm_data):,} samples)
extern const {dtype_name} {var_name}_DATA[];
extern const size_t {var_name}_SAMPLE_COUNT;
extern const size_t {var_name}_BYTE_SIZE;
extern const uint32_t {var_name}_SAMPLE_RATE;
extern const uint8_t {var_name}_CHANNELS;
extern const uint8_t {var_name}_BIT_DEPTH;

}} // namespace Audio
}} // namespace Exterminate

// Implementation
namespace Exterminate {{
namespace Audio {{

const {dtype_name} {var_name}_DATA[] = {{
"""
    
    # Add PCM data (8 samples per line for readability)
    flat_data = pcm_data.flatten() if len(pcm_data.shape) > 1 else pcm_data
    for i in range(0, len(flat_data), 8):
        chunk = flat_data[i:i+8]
        sample_values = ', '.join(f'{int(sample)}' for sample in chunk)
        header_content += f"    {sample_values}"
        if i + 8 < len(flat_data):
            header_content += ","
        header_content += "\n"
    
    header_content += f"""}};

const size_t {var_name}_SAMPLE_COUNT = {len(flat_data)};
const size_t {var_name}_BYTE_SIZE = {total_bytes};
const uint32_t {var_name}_SAMPLE_RATE = {sample_rate};
const uint8_t {var_name}_CHANNELS = {channels};
const uint8_t {var_name}_BIT_DEPTH = {bit_depth};

}} // namespace Audio
}} // namespace Exterminate
"""
    
    # Write header file
    with open(header_path, 'w') as f:
        f.write(header_content)
    
    print(f"Generated: {header_path} ({total_bytes:,} bytes)")
    return True

def generate_audio_index(audio_files, output_dir, sample_rate=44100, channels=1, bit_depth=16):
    """Generate an index header that includes all audio files."""
    index_path = Path(output_dir) / "audio_index.h"
    
    dtype_name = "int16_t" if bit_depth == 16 else "int32_t"
    
    content = f"""#pragma once

// Auto-generated PCM audio index
// DO NOT EDIT - Generated by tools/audio_to_pcm_header.py

#include <cstdint>
#include <cstddef>
#include <cstring>

"""
    
    # Include all individual headers
    for audio_file in audio_files:
        header_name = f"{Path(audio_file).stem}.h"
        content += f'#include "{header_name}"\n'
    
    content += f"""
namespace Exterminate {{
namespace Audio {{

// Audio format constants
constexpr uint32_t AUDIO_SAMPLE_RATE = {sample_rate};
constexpr uint8_t AUDIO_CHANNELS = {channels};
constexpr uint8_t AUDIO_BIT_DEPTH = {bit_depth};

// PCM audio file registry
struct AudioFile {{
    const char* name;
    const {dtype_name}* data;
    size_t sample_count;
    size_t byte_size;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bit_depth;
}};

// Available audio files
extern const AudioFile AUDIO_FILES[];
extern const size_t AUDIO_FILE_COUNT;

// Audio file indices for easy access
enum class AudioIndex : size_t {{
"""
    
    # Add enum values
    for i, audio_file in enumerate(audio_files):
        var_name = sanitize_variable_name(Path(audio_file).name)
        content += f"    {var_name} = {i},\n"
    
    content += f"""    COUNT = {len(audio_files)}
}};

// Helper functions
const AudioFile* getAudioFile(AudioIndex index);
const AudioFile* getAudioFile(const char* name);

}} // namespace Audio
}} // namespace Exterminate

// Implementation
namespace Exterminate {{
namespace Audio {{

const AudioFile AUDIO_FILES[] = {{
"""
    
    # Add audio file entries
    for audio_file in audio_files:
        var_name = sanitize_variable_name(Path(audio_file).name)
        filename = Path(audio_file).name
        content += f"""    {{"{filename}", {var_name}_DATA, {var_name}_SAMPLE_COUNT, {var_name}_BYTE_SIZE, 
     {var_name}_SAMPLE_RATE, {var_name}_CHANNELS, {var_name}_BIT_DEPTH}},
"""
    
    content += f"""}};

const size_t AUDIO_FILE_COUNT = {len(audio_files)};

const AudioFile* getAudioFile(AudioIndex index) {{
    if (static_cast<size_t>(index) >= AUDIO_FILE_COUNT) {{
        return nullptr;
    }}
    return &AUDIO_FILES[static_cast<size_t>(index)];
}}

const AudioFile* getAudioFile(const char* name) {{
    for (size_t i = 0; i < AUDIO_FILE_COUNT; ++i) {{
        if (strcmp(AUDIO_FILES[i].name, name) == 0) {{
            return &AUDIO_FILES[i];
        }}
    }}
    return nullptr;
}}

}} // namespace Audio
}} // namespace Exterminate
"""
    
    with open(index_path, 'w') as f:
        f.write(content)
    
    print(f"Generated: {index_path}")

def main():
    parser = argparse.ArgumentParser(description='Convert audio files to PCM C++ headers')
    parser.add_argument('input_dir', help='Directory containing audio files')
    parser.add_argument('output_dir', help='Directory for generated header files')
    parser.add_argument('--pattern', default='*.mp3', help='File pattern to match (default: *.mp3)')
    parser.add_argument('--sample-rate', type=int, default=44100, help='Target sample rate (default: 44100)')
    parser.add_argument('--channels', type=int, choices=[1, 2], default=1, help='Number of channels (default: 1=mono)')
    parser.add_argument('--bit-depth', type=int, choices=[16, 32], default=16, help='Bits per sample (default: 16)')
    
    args = parser.parse_args()
    
    input_path = Path(args.input_dir)
    output_path = Path(args.output_dir)
    
    if not input_path.exists():
        print(f"Error: Input directory {input_path} does not exist")
        return 1
    
    # Find all audio files
    audio_files = []
    for pattern in ['*.mp3', '*.wav', '*.flac', '*.ogg']:
        audio_files.extend(input_path.glob(pattern))
    
    # Filter by user pattern if different from default
    if args.pattern != '*.mp3':
        audio_files = list(input_path.glob(args.pattern))
    
    audio_files.sort()  # Sort for consistent ordering
    
    if not audio_files:
        print(f"No audio files found in {input_path}")
        return 1
    
    print(f"Found {len(audio_files)} audio files")
    print(f"Target format: {args.sample_rate}Hz, {args.channels} channel(s), {args.bit_depth}-bit PCM")
    
    # Convert each audio file to header
    success_count = 0
    for audio_file in audio_files:
        if pcm_to_header(audio_file, output_path, args.sample_rate, args.channels, args.bit_depth):
            success_count += 1
    
    if success_count > 0:
        # Generate index file
        generate_audio_index(audio_files, output_path, args.sample_rate, args.channels, args.bit_depth)
        print(f"Successfully converted {success_count}/{len(audio_files)} files")
        print(f"\nTo use in your code:")
        print(f"#include \"audio/audio_index.h\"")
        print(f"auto audioFile = Exterminate::Audio::getAudioFile(Exterminate::Audio::AudioIndex::AUDIO_00001);")
    
    return 0 if success_count == len(audio_files) else 1

if __name__ == '__main__':
    sys.exit(main())
