@echo off
REM Windows Batch Audio Conversion Script
REM Converts audio files (MP3, WAV, etc.) to PCM C++ header files for embedded use

setlocal

REM Default parameters
set INPUT_DIR=misc
set OUTPUT_DIR=include\audio
set PATTERN=*.mp3
set SAMPLE_RATE=22050
set CHANNELS=1
set BIT_DEPTH=16

echo Exterminate Audio Conversion Script (PCM Format)
echo ================================================
echo.

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo Error: Python is not installed or not in PATH
    echo Please install Python 3.6+ and try again
    pause
    exit /b 1
)

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
set CONVERTER_SCRIPT=%SCRIPT_DIR%audio_to_pcm_header.py

REM Set paths relative to project root
set INPUT_PATH=%PROJECT_ROOT%\%INPUT_DIR%
set OUTPUT_PATH=%PROJECT_ROOT%\%OUTPUT_DIR%

echo Input directory: %INPUT_PATH%
echo Output directory: %OUTPUT_PATH%
echo Target format: %SAMPLE_RATE%Hz, %CHANNELS% channel(s), %BIT_DEPTH%-bit PCM
echo.

REM Check if input directory exists
if not exist "%INPUT_PATH%" (
    echo Error: Input directory '%INPUT_PATH%' does not exist
    pause
    exit /b 1
)

REM Check for audio files
dir "%INPUT_PATH%\%PATTERN%" >nul 2>&1
if errorlevel 1 (
    echo No audio files found matching '%PATTERN%' in '%INPUT_PATH%'
    pause
    exit /b 0
)

echo Found audio files matching pattern
echo.

REM Create output directory if it doesn't exist
if not exist "%OUTPUT_PATH%" (
    mkdir "%OUTPUT_PATH%"
    echo Created output directory: %OUTPUT_PATH%
)

REM Check for required Python dependencies
echo Checking Python dependencies...
python -c "import librosa, soundfile, numpy" 2>nul
if errorlevel 1 (
    echo.
    echo Missing required Python dependencies!
    echo Please install them with:
    echo   pip install librosa soundfile numpy
    echo.
    echo These libraries are needed to convert audio files to PCM format.
    pause
    exit /b 1
)

REM Run the Python converter
echo Converting audio files to PCM C++ headers...
cd /d "%PROJECT_ROOT%"
python "%CONVERTER_SCRIPT%" "%INPUT_PATH%" "%OUTPUT_PATH%" --pattern "%PATTERN%" --sample-rate %SAMPLE_RATE% --channels %CHANNELS% --bit-depth %BIT_DEPTH%

if errorlevel 1 (
    echo.
    echo Error: Audio conversion failed
    pause
    exit /b 1
)

echo.
echo Audio conversion completed successfully!
echo Generated headers are in: %OUTPUT_PATH%
echo.
echo Audio Format Details:
echo - Sample Rate: %SAMPLE_RATE%Hz (optimized for Pico W)
echo - Channels: %CHANNELS% (mono saves memory)
echo - Bit Depth: %BIT_DEPTH%-bit (good quality/size balance)
echo - Format: Uncompressed PCM (no decoding needed)
echo.
echo To use in your code:
echo #include "audio/audio_index.h"
echo using namespace Exterminate::Audio;
echo auto audioFile = getAudioFile(AudioIndex::AUDIO_00001);
echo const int16_t* pcmData = audioFile-^>data;
echo size_t sampleCount = audioFile-^>sample_count;
echo.
echo Next steps:
echo 1. Build your project to include the new audio data
echo 2. Flash the updated firmware to your Pico W
echo 3. Test audio playback with your gamepad

pause
