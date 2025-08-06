#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "AudioController.h"
#include "audio/audio_index.h"

// Debug function to test I2S pins manually
void test_i2s_pins_manually() {
    printf("=== Manual I2S Pin Test ===\n");
    
    const uint DATA_PIN = 6;  // I2S Data
    const uint BCK_PIN = 8;   // Bit Clock  
    const uint LRC_PIN = 9;   // Left/Right Clock
    
    // Initialize pins as outputs for testing
    gpio_init(DATA_PIN);
    gpio_init(BCK_PIN);
    gpio_init(LRC_PIN);
    gpio_set_dir(DATA_PIN, GPIO_OUT);
    gpio_set_dir(BCK_PIN, GPIO_OUT);
    gpio_set_dir(LRC_PIN, GPIO_OUT);
    
    printf("Testing pin control - you should see signals on scope/logic analyzer:\n");
    printf("DATA_PIN=%d, BCK_PIN=%d, LRC_PIN=%d\n", DATA_PIN, BCK_PIN, LRC_PIN);
    
    // Generate test signals for 2 seconds
    for (int cycle = 0; cycle < 1000; cycle++) {
        // Toggle bit clock at ~1kHz (much slower than real I2S for easier debugging)
        gpio_put(BCK_PIN, 1);
        sleep_us(500);
        gpio_put(BCK_PIN, 0);
        sleep_us(500);
        
        // Toggle LRC every 32 bit clocks (simulate word select)
        if (cycle % 32 == 0) {
            gpio_put(LRC_PIN, !gpio_get(LRC_PIN));
        }
        
        // Toggle data pin occasionally
        if (cycle % 16 == 0) {
            gpio_put(DATA_PIN, !gpio_get(DATA_PIN));
        }
    }
    
    // Clean up - set pins back to low
    gpio_put(DATA_PIN, 0);
    gpio_put(BCK_PIN, 0);
    gpio_put(LRC_PIN, 0);
    
    printf("Manual pin test completed\n");
}

int main()
{
    stdio_init_all();
    
    printf("Exterminate: Audio Test - Starting up...\n");

    // Initialize GPIO for onboard LED (Pico Plus 2 uses GPIO 25)
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    // Turn on LED to indicate we're starting up
    gpio_put(LED_PIN, 1);
    
    printf("Exterminate: LED initialized, setting up Audio Controller...\n");

    // Create audio controller with configuration for DACs without system clock
    auto audioConfig = Exterminate::AudioController::Config::getDefaultConfig();
    audioConfig.enableSystemClock = false;  // Disable MCLK for MAX98357A-style DACs
    
    // Print configuration details for debugging
    printf("Audio Config: dataOut=%d, clockBase=%d (BCK=%d, LRC=%d), sampleRate=%d, systemClock=%s\n", 
           audioConfig.dataOutPin, audioConfig.clockPinBase, audioConfig.clockPinBase, 
           audioConfig.clockPinBase + 1, audioConfig.sampleRate, 
           audioConfig.enableSystemClock ? "enabled" : "disabled");
    
    Exterminate::AudioController audioController(audioConfig);
    
    printf("Exterminate: Audio Controller created, initializing I2S...\n");
    
    // Initialize audio system
    if (!audioController.initialize()) {
        printf("ERROR: Failed to initialize audio controller!\n");
        // Flash LED to indicate error
        while (true) {
            gpio_put(LED_PIN, 1);
            sleep_ms(200);
            gpio_put(LED_PIN, 0);
            sleep_ms(200);
        }
    }
    
    printf("Exterminate: Audio Controller initialized successfully!\n");
    
    // Additional I2S system validation
    printf("=== I2S System Validation ===\n");
    
    // Check if pins are configured correctly after initialization
    printf("Checking GPIO function assignments after I2S init...\n");
    for (int pin = 6; pin <= 10; pin++) {
        uint32_t gpio_ctrl = gpio_get_function(pin);
        printf("GPIO %d function: %d %s\n", pin, gpio_ctrl,
               gpio_ctrl == GPIO_FUNC_PIO0 ? "(PIO0)" :
               gpio_ctrl == GPIO_FUNC_PIO1 ? "(PIO1)" :
               gpio_ctrl == GPIO_FUNC_SIO ? "(SIO)" : "(OTHER)");
    }
    
    // Validate audio data
    printf("Exterminate: Validating audio data...\n");
    
    // Check if we can access the audio file info
    const auto* audioFile = Exterminate::Audio::getAudioFile(Exterminate::Audio::AudioIndex::AUDIO_00001);
    if (audioFile) {
        printf("Audio file found: %s, samples=%zu, rate=%d Hz, channels=%d, bits=%d\n", 
               audioFile->name, audioFile->sample_count, audioFile->sample_rate, 
               audioFile->channels, audioFile->bit_depth);
        
        // Check if data pointer is valid and has some data
        if (audioFile->data && audioFile->sample_count > 0) {
            printf("First few samples: %d, %d, %d, %d\n", 
                   audioFile->data[0], audioFile->data[1], 
                   audioFile->data[2], audioFile->data[3]);
        } else {
            printf("ERROR: Audio data pointer is invalid!\n");
        }
    } else {
        printf("ERROR: Could not find audio file!\n");
    }
    
    // Wait a moment for system to stabilize
    sleep_ms(1000);
    
    printf("Exterminate: Playing first audio clip...\n");
    
    // Play the first audio clip
    if (!audioController.playAudio(Exterminate::Audio::AudioIndex::AUDIO_00001)) {
        printf("ERROR: Failed to start audio playback!\n");
        // Flash LED rapidly to indicate playback error
        while (true) {
            gpio_put(LED_PIN, 1);
            sleep_ms(100);
            gpio_put(LED_PIN, 0);
            sleep_ms(100);
        }
    }
    
    printf("Exterminate: Audio playback started successfully!\n");
    printf("Expected to hear audio on GPIO 6 (DIN), with clocks on GPIO 8 (BCK) and GPIO 9 (LRC)\n");
    printf("Audio should loop automatically every ~1.4 seconds\n");
    
    // Add detailed I2S debugging
    printf("=== I2S Hardware Debug ===\n");
    printf("Checking PIO state...\n");
    
    printf("PIO0 status: enabled\n");
    
    // Check PIO state machine status
    printf("PIO0 state machine status:\n");
    for (int sm = 0; sm < 4; sm++) {
        uint32_t sm_ctrl = pio0->sm[sm].clkdiv;
        bool sm_enabled = (pio0->ctrl & (1 << sm)) != 0;
        printf("  SM%d: %s (ctrl=0x%08x)\n", sm, sm_enabled ? "ENABLED" : "disabled", sm_ctrl);
    }
    
    // Check if any DMA channels are active
    printf("DMA channel status:\n");
    for (int ch = 0; ch < 12; ch++) {
        bool ch_busy = dma_channel_is_busy(ch);
        if (ch_busy) {
            printf("  DMA%d: ACTIVE\n", ch);
        }
    }
    
    // Add a breakpoint-friendly delay and status check
    for (int debug_cycle = 0; debug_cycle < 10; debug_cycle++) {
        printf("Debug cycle %d: Audio playing = %s\n", 
               debug_cycle, audioController.isPlaying() ? "YES" : "NO");
        
        // This is a good place for a breakpoint - step through audio state
        sleep_ms(500);  // 500ms intervals for debugging
        
        if (debug_cycle == 2) {
            printf("*** BREAKPOINT: Good place to examine I2S registers and PIO state ***\n");
            // You can set a breakpoint here to examine:
            // - PIO state machine status
            // - DMA channel status  
            // - GPIO pin states
            // - Audio buffer contents
        }
    }
    
    // Add a quick test to verify GPIO functionality
    printf("Testing GPIO functionality...\n");
    for (int i = 0; i < 5; i++) {
        printf("GPIO test %d/5\n", i + 1);
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
    }
    printf("GPIO test complete\n");
    
    // Test I2S pins manually before starting audio
    test_i2s_pins_manually();
    
    // MAX98357A specific debugging
    printf("=== MAX98357A Specific Tests ===\n");
    printf("Checking common MAX98357A issues...\n");
    
    // Test 1: Check if SD pin needs to be controlled
    printf("MAX98357A SD pin configuration:\n");
    printf("- Connect SD to GND for left channel\n");
    printf("- Connect SD to 3.3V for right channel  \n");
    printf("- Leave SD floating for (L+R)/2 mix\n");
    printf("- Current setup should have SD connected to GND or 3.3V\n");
    
    // Test 2: Power and timing check
    printf("MAX98357A power/timing requirements:\n");
    printf("- VDD should be 2.7V to 5.5V (check your 3.3V rail)\n");
    printf("- No MCLK required (our config: enableSystemClock=false ✓)\n");
    printf("- BCK frequency should be 32*Fs = 32*44100 = ~1.41MHz\n");
    printf("- LRC frequency should be Fs = 44100Hz\n");
    
    // Test 3: Signal integrity check
    printf("Recommended: Check with oscilloscope/logic analyzer:\n");
    printf("- GPIO 8 (BCK): should show ~1.41MHz square wave\n");
    printf("- GPIO 9 (LRC): should show 44.1kHz square wave\n");
    printf("- GPIO 6 (DIN): should show data synchronized with BCK\n");
    printf("- All signals should be 3.3V logic levels\n");
    
    // Test 4: SD pin control (if available)
    printf("=== SD Pin Control Test ===\n");
    const uint SD_PIN = 7;  // Common choice for SD control pin
    printf("Testing SD pin control on GPIO %d...\n", SD_PIN);
    
    // Initialize SD pin for testing
    gpio_init(SD_PIN);
    gpio_set_dir(SD_PIN, GPIO_OUT);
    
    // Test SD = LOW (left channel)
    printf("Setting SD pin LOW (left channel mode)...\n");
    gpio_put(SD_PIN, 0);
    sleep_ms(500);
    
    printf("IMPORTANT: If your SD pin is still floating, manually connect it now:\n");
    printf("- Connect SD to GND for left channel (recommended for mono)\n");
    printf("- Connect SD to 3.3V for right channel\n");
    printf("- Do NOT leave SD floating - this causes shutdown!\n");
    printf("Press RESET to continue after fixing SD connection...\n");
    sleep_ms(3000);
    
    printf("Testing with simple tone pattern...\n");
    
    // Stop current audio
    audioController.stopAudio();
    sleep_ms(500);
    
    // Create a reasonable-sized pattern for testing (1 second worth - avoid large heap allocation)
    printf("Generating test pattern (1 second at 44.1kHz)...\n");
    const int pattern_size = 44100;  // 1 second worth of samples (~176KB for 32-bit)
    static int32_t test_pattern[44100];  // Static allocation to avoid heap issues - now 32-bit
    
    for (int i = 0; i < pattern_size; i++) {
        // Generate a more aggressive square wave pattern - 440Hz tone (A note)
        // Use maximum amplitude for easier debugging - 32-bit samples
        int cycle_length = 44100 / 440;  // Samples per cycle for 440Hz
        test_pattern[i] = (i % cycle_length < cycle_length/2) ? 0x7FFF0000 : 0x80000000;  // Max 32-bit amplitude
    }
    
    printf("Playing 1-second test tone (440Hz A note) - you should hear a clear tone...\n");
    printf("Buffer details: pattern_size=%d samples, duration=%.2f seconds\n", 
           pattern_size, (float)pattern_size / 44100.0f);
    printf("Sample values: first=%d, mid=%d, last=%d (max amplitude)\n", 
           test_pattern[0], test_pattern[pattern_size/2], test_pattern[pattern_size-1]);
    
    // Try multiple test patterns if first one fails
    bool testPassed = false;
    
    printf("TEST 1: High-frequency 440Hz tone\n");
    if (audioController.playPCMAudioData(test_pattern, pattern_size)) {
        testPassed = true;
        printf("440Hz test tone started - monitoring for 2 seconds...\n");
        
        // IMMEDIATE: Check if I2S started right after playback begins
        sleep_ms(50);  // Give it a moment to start
        printf("*** IMMEDIATE I2S CHECK (50ms after start) ***\n");
        bool immediate_bck = false, immediate_lrc = false, immediate_din = false;
        uint32_t gpio_states[5];
        for (int j = 0; j < 5; j++) {
            gpio_states[j] = gpio_get_all();
            sleep_us(200);  // Very fast sampling
        }
        for (int j = 1; j < 5; j++) {
            if ((gpio_states[j] & (1 << 8)) != (gpio_states[j-1] & (1 << 8))) immediate_bck = true;
            if ((gpio_states[j] & (1 << 9)) != (gpio_states[j-1] & (1 << 9))) immediate_lrc = true;
            if ((gpio_states[j] & (1 << 6)) != (gpio_states[j-1] & (1 << 6))) immediate_din = true;
        }
        printf("Immediate I2S activity: BCK=%s, LRC=%s, DIN=%s\n",
               immediate_bck ? "ACTIVE" : "STATIC",
               immediate_lrc ? "ACTIVE" : "STATIC", 
               immediate_din ? "ACTIVE" : "STATIC");
        
        // Monitor for 2 seconds (longer than the audio)
        for (int i = 0; i < 20; i++) {
            bool isPlaying = audioController.isPlaying();
            printf("Test tone: %d/20 (%.1fs) - Playing: %s\n", 
                   i+1, (i+1)*0.1f, isPlaying ? "YES" : "NO");
            
            // CRITICAL: Test if I2S pins are actually changing during playback
            if (i == 5 && isPlaying) {  // Test at 0.5 seconds
                printf("*** CHECKING I2S PIN ACTIVITY DURING PLAYBACK ***\n");
                
                // First, check what PIO state machines are active right now
                printf("Real-time PIO status during playback:\n");
                for (int sm = 0; sm < 4; sm++) {
                    bool sm_enabled = (pio0->ctrl & (1 << sm)) != 0;
                    if (sm_enabled) {
                        printf("  PIO0_SM%d: ENABLED, PC=0x%02x\n", sm, pio0->sm[sm].addr);
                    }
                }
                
                // Check DMA activity during playback
                printf("Real-time DMA status during playback:\n");
                for (int ch = 0; ch < 12; ch++) {
                    bool ch_busy = dma_channel_is_busy(ch);
                    if (ch_busy) {
                        printf("  DMA%d: ACTIVE\n", ch);
                    }
                }
                
                // Read GPIO states multiple times to see if they're changing
                uint32_t gpio_states[10];
                for (int j = 0; j < 10; j++) {
                    gpio_states[j] = gpio_get_all();
                    sleep_us(100);  // Reduced to 100us for higher resolution
                }
                
                // Check if BCK (pin 8), LRC (pin 9), or DIN (pin 6) are changing
                bool bck_changing = false, lrc_changing = false, din_changing = false;
                for (int j = 1; j < 10; j++) {
                    if ((gpio_states[j] & (1 << 8)) != (gpio_states[j-1] & (1 << 8))) bck_changing = true;
                    if ((gpio_states[j] & (1 << 9)) != (gpio_states[j-1] & (1 << 9))) lrc_changing = true;
                    if ((gpio_states[j] & (1 << 6)) != (gpio_states[j-1] & (1 << 6))) din_changing = true;
                }
                
                printf("I2S Pin Activity: BCK=%s, LRC=%s, DIN=%s\n",
                       bck_changing ? "ACTIVE" : "STATIC",
                       lrc_changing ? "ACTIVE" : "STATIC", 
                       din_changing ? "ACTIVE" : "STATIC");
                
                // Also check the pin states (HIGH/LOW)
                uint32_t current_gpio = gpio_get_all();
                printf("Current pin states: BCK(8)=%s, LRC(9)=%s, DIN(6)=%s\n",
                       (current_gpio & (1 << 8)) ? "HIGH" : "LOW",
                       (current_gpio & (1 << 9)) ? "HIGH" : "LOW", 
                       (current_gpio & (1 << 6)) ? "HIGH" : "LOW");
                
                if (!bck_changing && !lrc_changing && !din_changing) {
                    printf("*** CRITICAL: NO I2S ACTIVITY DETECTED - I2S/DMA NOT WORKING ***\n");
                    printf("*** This suggests the I2S system is not actually generating signals ***\n");
                } else {
                    printf("*** I2S signals detected - hardware working, check MAX98357A ***\n");
                }
            }
            
            // If it stops before expected, note it
            if (!isPlaying && i < 10) {  // Should play for ~1 second (10 iterations)
                printf("*** AUDIO STOPPED at %.1f seconds (expected ~1.0s) ***\n", (i+1)*0.1f);
                printf("This suggests: %s\n", (i+1)*0.1f < 0.5f ? 
                       "PROBLEM: Audio stopping too early - check I2S/DMA" :
                       "OK: Audio completed normally - check MAX98357A SD pin!");
                break;
            }
            sleep_ms(100);
        }
    }
    
    // Generate a lower frequency tone (100Hz) - easier to hear
    printf("\nTEST 2: Low-frequency 100Hz tone (easier to detect)\n");
    audioController.stopAudio();
    sleep_ms(500);
    
    for (int i = 0; i < pattern_size; i++) {
        int cycle_length = 44100 / 100;  // 100Hz tone - much lower frequency
        test_pattern[i] = (i % cycle_length < cycle_length/2) ? 0x7FFF0000 : 0x80000000;  // Max 32-bit amplitude
    }
    
    if (audioController.playPCMAudioData(test_pattern, pattern_size)) {
        printf("100Hz test tone started - should be a deep buzz...\n");
        for (int i = 0; i < 10; i++) {
            bool isPlaying = audioController.isPlaying();
            printf("100Hz tone: %d/10 (%.1fs) - Playing: %s\n", 
                   i+1, (i+1)*0.1f, isPlaying ? "YES" : "NO");
            if (!isPlaying && i < 10) break;
            sleep_ms(100);
        }
        testPassed = true;
    }
    
    if (!testPassed) {
        printf("Failed to start any test tones\n");
    }
    
    printf("Test tones completed - did you hear anything?\n");
    printf("=== Resuming original audio file ===\n");
    
    // Try playing the original audio again
    if (audioController.playAudio(Exterminate::Audio::AudioIndex::AUDIO_00001)) {
        printf("Original audio restarted\n");
    } else {
        printf("Failed to restart original audio\n");
    }
    
    // Keep the program running and monitor playback
    uint32_t lastStatus = 0;
    while (true) {
        // Check if audio is still playing
        bool isCurrentlyPlaying = audioController.isPlaying();
        
        if (isCurrentlyPlaying) {
            // Slow blink to indicate playing
            gpio_put(LED_PIN, (lastStatus++ / 1000) % 2);
        } else {
            // Audio finished, restart it
            printf("Exterminate: Audio finished, restarting...\n");
            sleep_ms(1000);  // Wait 1 second before restarting
            audioController.playAudio(Exterminate::Audio::AudioIndex::AUDIO_00001);
        }
        
        sleep_ms(10);  // Small delay to prevent tight loop
    }

    return 0;
}
