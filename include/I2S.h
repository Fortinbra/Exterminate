/*
 * I2S.h - I2S Audio Interface Header
 * 
 * This file is part of the Exterminate project.
 * 
 * Implements modern C++ I2S audio interface using PIO state machines
 * following SOLID principles and RAII patterns.
 * 
 * Adapted from malacalypse/rp2040_i2s_example (GPL v3)
 * Original Copyright (c) 2022 Daniel Collins
 */

#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include "hardware/pio.h"
#include "hardware/dma.h"

namespace Exterminate {

// Forward declarations
class I2SController;

/**
 * I2S Configuration Structure
 * Encapsulates all parameters needed for I2S operation
 */
struct I2SConfig {
    uint32_t sampleRate;        // Sample rate in Hz (e.g., 22050, 44100, 48000)
    uint32_t systemClockMult;   // System clock multiplier (typically 256 or 384)
    uint8_t bitDepth;           // Bit depth (16, 24, or 32)
    uint8_t systemClockPin;     // System clock output pin (optional)
    uint8_t dataOutPin;         // Data output pin (to DAC)
    uint8_t dataInPin;          // Data input pin (from ADC) - optional
    uint8_t clockPinBase;       // Base pin for BCK and LRCK (consecutive pins)
    bool enableSystemClock;     // Whether to generate system clock
    
    // Default configuration for 22.05kHz PCM audio (matching our audio files)
    static const I2SConfig& getDefaultConfig() {
        static const I2SConfig config = {
            .sampleRate = 22050,        // Match our PCM audio files
            .systemClockMult = 256,     // Standard multiplier
            .bitDepth = 16,             // 16-bit audio data
            .systemClockPin = 10,       // GPIO 10 for system clock
            .dataOutPin = 6,            // GPIO 6 for audio output
            .dataInPin = 7,             // GPIO 7 for audio input (unused)
            .clockPinBase = 8,          // GPIO 8,9 for BCK/LRCK
            .enableSystemClock = true   // Generate system clock
        };
        return config;
    }
};

/**
 * I2S Buffer Management
 * Double-buffered audio data with proper alignment
 */
class I2SBuffers {
private:
    static constexpr size_t AUDIO_BUFFER_FRAMES = 64;  // Frames per buffer
    static constexpr size_t STEREO_BUFFER_SIZE = AUDIO_BUFFER_FRAMES * 2;  // L+R samples
    
public:
    // Audio buffers must be 8-byte aligned for DMA
    alignas(8) int32_t inputBuffer[STEREO_BUFFER_SIZE * 2];   // Double buffered
    alignas(8) int32_t outputBuffer[STEREO_BUFFER_SIZE * 2];  // Double buffered
    
    // Control block pointers for DMA chaining
    int32_t* inputControlBlocks[2];
    int32_t* outputControlBlocks[2];
    
    I2SBuffers() {
        // Initialize control block pointers
        inputControlBlocks[0] = inputBuffer;
        inputControlBlocks[1] = &inputBuffer[STEREO_BUFFER_SIZE];
        outputControlBlocks[0] = outputBuffer;
        outputControlBlocks[1] = &outputBuffer[STEREO_BUFFER_SIZE];
    }
    
    size_t getFrameCount() const { return AUDIO_BUFFER_FRAMES; }
    size_t getStereoBufferSize() const { return STEREO_BUFFER_SIZE; }
};

/**
 * I2S Clock Configuration
 * Manages precise clock relationships for I2S operation
 */
class I2SClocks {
private:
    float actualSampleRate;
    float systemClockHz;
    float bitClockHz;
    
    uint16_t systemClockDivider;
    uint8_t systemClockFraction;
    uint16_t bitClockDivider;
    uint8_t bitClockFraction;
    
public:
    /**
     * Calculate optimal clock dividers for given configuration
     */
    bool calculateClocks(const I2SConfig& config);
    
    /**
     * Validate that system and bit clocks are properly synchronized
     */
    bool validateClockSync() const;
    
    // Getters for calculated values
    float getActualSampleRate() const { return actualSampleRate; }
    uint16_t getSystemClockDivider() const { return systemClockDivider; }
    uint8_t getSystemClockFraction() const { return systemClockFraction; }
    uint16_t getBitClockDivider() const { return bitClockDivider; }
    uint8_t getBitClockFraction() const { return bitClockFraction; }
};

/**
 * I2S DMA Controller
 * Manages DMA channels and double-buffering for audio data
 */
class I2SDMA {
public:
    uint inputControlChannel;
    uint inputDataChannel;
    uint outputControlChannel;
    uint outputDataChannel;
    
    std::function<void()> dmaHandler;
    
    I2SDMA() = default;
    ~I2SDMA();
    
    // Non-copyable, movable
    I2SDMA(const I2SDMA&) = delete;
    I2SDMA& operator=(const I2SDMA&) = delete;
    I2SDMA(I2SDMA&&) = default;
    I2SDMA& operator=(I2SDMA&&) = default;
    
    /**
     * Initialize DMA channels for double-buffered I2S operation
     */
    bool initialize(PIO pio, uint outputStateMachine, uint inputStateMachine,
                   I2SBuffers& buffers, std::function<void()> handler);
    
    /**
     * Start DMA transfers
     */
    void start();
    
    /**
     * Stop DMA transfers
     */
    void stop();
    
    /**
     * Get output data channel number
     */
    uint getOutputDataChannel() const { return outputDataChannel; }
    
    /**
     * Get input data channel number
     */
    uint getInputDataChannel() const { return inputDataChannel; }
    
private:
    void configureDMAChannels(PIO pio, uint outputSM, uint inputSM, I2SBuffers& buffers);
    void claimDMAChannels();
    void releaseDMAChannels();
};

/**
 * I2S Audio Processing Interface
 * Abstract interface for audio processing implementations
 * Follows Interface Segregation Principle
 */
class IAudioProcessor {
public:
    virtual ~IAudioProcessor() = default;
    
    /**
     * Process audio samples
     * @param input Input audio buffer (interleaved L/R)
     * @param output Output audio buffer (interleaved L/R)
     * @param frameCount Number of stereo frames to process
     */
    virtual void processAudio(const int32_t* input, int32_t* output, size_t frameCount) = 0;
};

/**
 * Main I2S Controller Class
 * Manages complete I2S audio interface using RAII and dependency injection
 */
class I2SController {
private:
    I2SConfig config;
    I2SClocks clocks;
    I2SBuffers buffers;
    I2SDMA dmaController;
    
    PIO pio;
    uint systemClockSM;
    uint outputSM;
    uint inputSM;
    uint stateMachineMask;
    
    std::unique_ptr<IAudioProcessor> audioProcessor;
    
public:
    /**
     * Constructor with dependency injection
     * @param config I2S configuration
     * @param processor Audio processing implementation
     */
    explicit I2SController(const I2SConfig& config, 
                          std::unique_ptr<IAudioProcessor> processor = nullptr);
    
    /**
     * Destructor - RAII cleanup of PIO and DMA resources
     */
    ~I2SController();
    
    // Non-copyable, movable
    I2SController(const I2SController&) = delete;
    I2SController& operator=(const I2SController&) = delete;
    I2SController(I2SController&&) = default;
    I2SController& operator=(I2SController&&) = default;
    
    /**
     * Initialize I2S hardware and start audio processing
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * Start I2S audio processing
     */
    void start();
    
    /**
     * Stop I2S audio processing
     */
    void stop();
    
    /**
     * Set audio processor (dependency injection)
     */
    void setAudioProcessor(std::unique_ptr<IAudioProcessor> processor);
    
    /**
     * Get current configuration
     */
    const I2SConfig& getConfig() const { return config; }
    
    /**
     * Get actual achieved sample rate
     */
    float getActualSampleRate() const { return clocks.getActualSampleRate(); }
    
    /**
     * DMA interrupt handling (needs to be accessible by global handler)
     */
    void dmaInterruptHandler();
    
private:
    bool initializePIO();
    bool initializeStateMachines();
    void cleanupResources();
};

/**
 * Simple Audio Processor Implementation
 * Demonstrates basic audio processing (copy input to output)
 */
class SimpleAudioProcessor : public IAudioProcessor {
public:
    void processAudio(const int32_t* input, int32_t* output, size_t frameCount) override {
        // Simple copy - can be replaced with more complex processing
        for (size_t i = 0; i < frameCount * 2; ++i) {
            output[i] = input[i];
        }
    }
};

} // namespace Exterminate
