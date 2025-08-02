# PowerShell Audio Conversion Script
# Converts audio files (MP3, WAV, etc.) to PCM C++ header files for embedded use

param(
    [string]$InputDir = "misc",
    [string]$OutputDir = "include\audio",
    [string]$Pattern = "*.mp3",
    [int]$SampleRate = 22050,
    [int]$Channels = 1,
    [int]$BitDepth = 16
)

Write-Host "Exterminate Audio Conversion Script (PCM Format)" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green
Write-Host ""

# Ensure Python is available
try {
    python --version | Out-Null
} catch {
    Write-Host "Error: Python is not installed or not in PATH" -ForegroundColor Red
    Write-Host "Please install Python 3.6+ and try again" -ForegroundColor Red
    exit 1
}

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$ConverterScript = Join-Path $ScriptDir "audio_to_pcm_header.py"

# Set paths relative to project root
$InputPath = Join-Path $ProjectRoot $InputDir
$OutputPath = Join-Path $ProjectRoot $OutputDir

Write-Host "Input directory: $InputPath"
Write-Host "Output directory: $OutputPath"
Write-Host "Target format: ${SampleRate}Hz, ${Channels} channel(s), ${BitDepth}-bit PCM" -ForegroundColor Cyan

# Check if input directory exists
if (-not (Test-Path $InputPath)) {
    Write-Host "Error: Input directory '$InputPath' does not exist" -ForegroundColor Red
    exit 1
}

# Check for audio files
$AudioFiles = Get-ChildItem -Path $InputPath -Filter $Pattern
if ($AudioFiles.Count -eq 0) {
    Write-Host "No audio files found matching '$Pattern' in '$InputPath'" -ForegroundColor Yellow
    exit 0
}

Write-Host "Found $($AudioFiles.Count) audio files" -ForegroundColor Green

# Create output directory if it doesn't exist
if (-not (Test-Path $OutputPath)) {
    New-Item -ItemType Directory -Path $OutputPath -Force | Out-Null
    Write-Host "Created output directory: $OutputPath"
}

# Check for required Python dependencies
Write-Host "Checking Python dependencies..." -ForegroundColor Yellow
try {
    python -c "import librosa, soundfile, numpy" 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Host ""
        Write-Host "Missing required Python dependencies!" -ForegroundColor Red
        Write-Host "Please install them with:" -ForegroundColor Yellow
        Write-Host "  pip install librosa soundfile numpy" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "These libraries are needed to convert audio files to PCM format." -ForegroundColor White
        exit 1
    }
} catch {
    Write-Host "Error checking dependencies: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# Run the Python converter
Write-Host "Converting audio files to PCM C++ headers..." -ForegroundColor Yellow
Set-Location $ProjectRoot
python $ConverterScript $InputPath $OutputPath --pattern $Pattern --sample-rate $SampleRate --channels $Channels --bit-depth $BitDepth

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "Audio conversion completed successfully!" -ForegroundColor Green
    Write-Host "Generated headers are in: $OutputPath" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Audio Format Details:" -ForegroundColor Cyan
    Write-Host "- Sample Rate: ${SampleRate}Hz (optimized for Pico W)" -ForegroundColor White
    Write-Host "- Channels: ${Channels} (mono saves memory)" -ForegroundColor White  
    Write-Host "- Bit Depth: ${BitDepth}-bit (good quality/size balance)" -ForegroundColor White
    Write-Host "- Format: Uncompressed PCM (no decoding needed)" -ForegroundColor White
    Write-Host ""
    Write-Host "To use in your code:" -ForegroundColor Yellow
    Write-Host '#include "audio/audio_index.h"' -ForegroundColor Gray
    Write-Host 'using namespace Exterminate::Audio;' -ForegroundColor Gray
    Write-Host 'auto audioFile = getAudioFile(AudioIndex::AUDIO_00001);' -ForegroundColor Gray
    Write-Host 'const int16_t* pcmData = audioFile->data;' -ForegroundColor Gray
    Write-Host 'size_t sampleCount = audioFile->sample_count;' -ForegroundColor Gray
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Cyan
    Write-Host "1. Build your project to include the new audio data" -ForegroundColor White
    Write-Host "2. Flash the updated firmware to your Pico W" -ForegroundColor White
    Write-Host "3. Test audio playback with your gamepad" -ForegroundColor White
} else {
    Write-Host ""
    Write-Host "Error: Audio conversion failed" -ForegroundColor Red
    exit 1
}
