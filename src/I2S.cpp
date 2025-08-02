/*
 * I2S.cpp - I2S Audio Interface Implementation
 * 
 * This file is part of the Exterminate project.
 * 
 * Implements modern C++ I2S audio interface using PIO state machines
 * following SOLID principles and RAII patterns.
 * 
 * Adapted from malacalypse/rp2040_i2s_example (GPL v3)
 * Original Copyright (c) 2022 Daniel Collins
 */

#include "I2S.h"
#include "i2s.pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <cmath>
#include <cstring>

namespace Exterminate {

// Global instance pointer for interrupt handler
static I2SController* g_i2sInstance = nullptr;

// External C interrupt handler that calls C++ method
extern "C" void i2s_dma_irq_handler() {
    if (g_i2sInstance) {
        g_i2sInstance->dmaInterruptHandler();
    }
}

// Helper function to calculate PIO clock dividers
static float calculatePIODivider(float targetFreq, uint16_t* divider, uint8_t* fraction) {
    float systemClock = static_cast<float>(clock_get_hz(clk_sys));
    float ratio = systemClock / targetFreq;
    
    float wholePart;
    float fractionalPart = std::modf(ratio, &wholePart);
    
    *divider = static_cast<uint16_t>(wholePart);
    *fraction = static_cast<uint8_t>(fractionalPart * 256.0f);
    
    // Return actual frequency achieved
    return systemClock / (static_cast<float>(*divider) + static_cast<float>(*fraction) / 256.0f);
}

//-----------------------------------------------------------------------------
// I2SClocks Implementation
//-----------------------------------------------------------------------------

bool I2SClocks::calculateClocks(const I2SConfig& config) {
    // Calculate system clock requirements
    float desiredSystemClock = static_cast<float>(config.sampleRate) * 
                               static_cast<float>(config.systemClockMult) * 
                               i2s_sck_program_pio_mult;
    
    float achievedSystemClock = calculatePIODivider(desiredSystemClock, 
                                                   &systemClockDivider, 
                                                   &systemClockFraction);
    
    // Calculate actual sample rate based on achieved system clock
    actualSampleRate = achievedSystemClock / 
                      (static_cast<float>(config.systemClockMult) * i2s_sck_program_pio_mult);
    
    // Calculate bit clock based on actual sample rate
    float bitClockFreq = actualSampleRate * static_cast<float>(config.bitDepth) * 2.0f;
    systemClockHz = actualSampleRate * static_cast<float>(config.systemClockMult);
    
    calculatePIODivider(bitClockFreq * i2s_out_master_program_pio_mult, 
                       &bitClockDivider, 
                       &bitClockFraction);
    
    bitClockHz = bitClockFreq;
    
    return true;
}

bool I2SClocks::validateClockSync() const {
    if (!systemClockHz || !bitClockHz) {
        return false;
    }
    
    float ratio = systemClockHz / bitClockHz;
    float wholePart;
    float fractionalPart = std::modf(ratio, &wholePart);
    
    // Allow small tolerance for floating point precision
    const float tolerance = 0.001f;
    return std::abs(fractionalPart) < tolerance || std::abs(fractionalPart - 1.0f) < tolerance;
}

//-----------------------------------------------------------------------------
// I2SDMA Implementation
//-----------------------------------------------------------------------------

I2SDMA::~I2SDMA() {
    stop();
    releaseDMAChannels();
}

bool I2SDMA::initialize(PIO pio, uint outputStateMachine, uint inputStateMachine,
                       I2SBuffers& buffers, std::function<void()> handler) {
    dmaHandler = std::move(handler);
    
    try {
        claimDMAChannels();
        configureDMAChannels(pio, outputStateMachine, inputStateMachine, buffers);
        return true;
    } catch (...) {
        releaseDMAChannels();
        return false;
    }
}

void I2SDMA::claimDMAChannels() {
    inputControlChannel = dma_claim_unused_channel(true);
    inputDataChannel = dma_claim_unused_channel(true);
    outputControlChannel = dma_claim_unused_channel(true);
    outputDataChannel = dma_claim_unused_channel(true);
}

void I2SDMA::releaseDMAChannels() {
    if (inputControlChannel != (uint)-1) dma_channel_unclaim(inputControlChannel);
    if (inputDataChannel != (uint)-1) dma_channel_unclaim(inputDataChannel);
    if (outputControlChannel != (uint)-1) dma_channel_unclaim(outputControlChannel);
    if (outputDataChannel != (uint)-1) dma_channel_unclaim(outputDataChannel);
}

void I2SDMA::configureDMAChannels(PIO pio, uint outputSM, uint inputSM, I2SBuffers& buffers) {
    // Configure output control channel
    dma_channel_config config = dma_channel_get_default_config(outputControlChannel);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);
    channel_config_set_ring(&config, false, 3);  // 8-byte ring
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    
    dma_channel_configure(outputControlChannel, &config, 
                         &dma_hw->ch[outputDataChannel].al3_read_addr_trig,
                         buffers.outputControlBlocks, 1, false);
    
    // Configure output data channel
    config = dma_channel_get_default_config(outputDataChannel);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);
    channel_config_set_chain_to(&config, outputControlChannel);
    channel_config_set_dreq(&config, pio_get_dreq(pio, outputSM, true));
    
    dma_channel_configure(outputDataChannel, &config,
                         &pio->txf[outputSM],
                         nullptr,  // Set by control channel
                         buffers.getStereoBufferSize(),
                         false);
    
    // Configure input control channel
    config = dma_channel_get_default_config(inputControlChannel);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);
    channel_config_set_ring(&config, false, 3);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    
    dma_channel_configure(inputControlChannel, &config,
                         &dma_hw->ch[inputDataChannel].al2_write_addr_trig,
                         buffers.inputControlBlocks, 1, false);
    
    // Configure input data channel
    config = dma_channel_get_default_config(inputDataChannel);
    channel_config_set_read_increment(&config, false);
    channel_config_set_write_increment(&config, true);
    channel_config_set_chain_to(&config, inputControlChannel);
    channel_config_set_dreq(&config, pio_get_dreq(pio, inputSM, false));
    
    dma_channel_configure(inputDataChannel, &config,
                         nullptr,  // Set by control channel
                         &pio->rxf[inputSM],
                         buffers.getStereoBufferSize(),
                         false);
    
    // Enable interrupt on input data channel completion
    dma_channel_set_irq0_enabled(inputDataChannel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, i2s_dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void I2SDMA::start() {
    dma_channel_start(outputControlChannel);
    dma_channel_start(inputControlChannel);
}

void I2SDMA::stop() {
    dma_channel_abort(outputDataChannel);
    dma_channel_abort(inputDataChannel);
    dma_channel_abort(outputControlChannel);
    dma_channel_abort(inputControlChannel);
}

//-----------------------------------------------------------------------------
// I2SController Implementation
//-----------------------------------------------------------------------------

I2SController::I2SController(const I2SConfig& config, 
                           std::unique_ptr<IAudioProcessor> processor)
    : config(config)
    , audioProcessor(std::move(processor))
    , pio(pio0)  // Use PIO0 by default
    , systemClockSM(0)
    , outputSM(0)
    , inputSM(0)
    , stateMachineMask(0) {
    
    // Set global instance for interrupt handler
    g_i2sInstance = this;
    
    // Use simple processor if none provided
    if (!audioProcessor) {
        audioProcessor = std::make_unique<SimpleAudioProcessor>();
    }
}

I2SController::~I2SController() {
    cleanupResources();
    g_i2sInstance = nullptr;
}

bool I2SController::initialize() {
    // Calculate clocks
    if (!clocks.calculateClocks(config)) {
        return false;
    }
    
    // Validate clock synchronization if system clock is enabled
    if (config.enableSystemClock && !clocks.validateClockSync()) {
        return false;
    }
    
    // Initialize PIO and state machines
    if (!initializePIO() || !initializeStateMachines()) {
        return false;
    }
    
    // Initialize DMA
    auto dmaHandler = [this]() { dmaInterruptHandler(); };
    if (!dmaController.initialize(pio, outputSM, inputSM, buffers, dmaHandler)) {
        return false;
    }
    
    return true;
}

bool I2SController::initializePIO() {
    // For now, use PIO0. In a more complex system, we might need to check availability
    pio = pio0;
    return true;
}

bool I2SController::initializeStateMachines() {
    uint offset = 0;
    
    // Initialize system clock state machine if enabled
    if (config.enableSystemClock) {
        systemClockSM = pio_claim_unused_sm(pio, true);
        stateMachineMask |= (1u << systemClockSM);
        
        offset = pio_add_program(pio, &i2s_sck_program);
        i2s_sck_program_init(pio, systemClockSM, offset, config.systemClockPin);
        pio_sm_set_clkdiv_int_frac(pio, systemClockSM, 
                                  clocks.getSystemClockDivider(),
                                  clocks.getSystemClockFraction());
    }
    
    // Initialize input state machine
    inputSM = pio_claim_unused_sm(pio, true);
    stateMachineMask |= (1u << inputSM);
    
    offset = pio_add_program(pio, &i2s_in_slave_program);
    i2s_in_slave_program_init(pio, inputSM, offset, config.dataInPin);
    pio_sm_set_clkdiv_int_frac(pio, inputSM,
                              clocks.getBitClockDivider(),
                              clocks.getBitClockFraction());
    
    // Initialize output state machine  
    outputSM = pio_claim_unused_sm(pio, true);
    stateMachineMask |= (1u << outputSM);
    
    offset = pio_add_program(pio, &i2s_out_master_program);
    i2s_out_master_program_init(pio, outputSM, offset, config.bitDepth, 
                               config.dataOutPin, config.clockPinBase);
    pio_sm_set_clkdiv_int_frac(pio, outputSM,
                              clocks.getBitClockDivider(),
                              clocks.getBitClockFraction());
    
    return true;
}

void I2SController::start() {
    dmaController.start();
    pio_enable_sm_mask_in_sync(pio, stateMachineMask);
}

void I2SController::stop() {
    pio_set_sm_mask_enabled(pio, stateMachineMask, false);
    dmaController.stop();
}

void I2SController::setAudioProcessor(std::unique_ptr<IAudioProcessor> processor) {
    audioProcessor = std::move(processor);
    if (!audioProcessor) {
        audioProcessor = std::make_unique<SimpleAudioProcessor>();
    }
}

void I2SController::dmaInterruptHandler() {
    if (!audioProcessor) {
        return;
    }
    
    // Simple double-buffered output processing
    // Get current DMA read address to determine which buffer half to process
    uint32_t currentChannel = dmaController.outputDataChannel;
    volatile uint32_t currentAddr = dma_hw->ch[currentChannel].read_addr;
    
    // Determine which half of the buffer DMA is currently reading from
    uint32_t bufferStart = reinterpret_cast<uint32_t>(buffers.outputBuffer);
    uint32_t bufferHalf = buffers.getStereoBufferSize() * sizeof(int32_t);
    
    int32_t* fillBuffer;
    if (currentAddr < bufferStart + bufferHalf) {
        // DMA is reading first half, we can fill second half
        fillBuffer = &buffers.outputBuffer[buffers.getStereoBufferSize()];
    } else {
        // DMA is reading second half, we can fill first half
        fillBuffer = buffers.outputBuffer;
    }
    
    // Process audio into the buffer that's not being read by DMA
    // For output-only mode, pass nullptr for input
    audioProcessor->processAudio(nullptr, fillBuffer, buffers.getFrameCount());
    
    // Clear the interrupt
    dma_hw->ints0 = 1u << currentChannel;
}

void I2SController::cleanupResources() {
    stop();
    
    // Release state machines
    if (stateMachineMask) {
        for (uint sm = 0; sm < 4; ++sm) {
            if (stateMachineMask & (1u << sm)) {
                pio_sm_unclaim(pio, sm);
            }
        }
        stateMachineMask = 0;
    }
}

} // namespace Exterminate
