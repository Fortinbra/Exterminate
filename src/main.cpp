#include <stdio.h>
#include "pico/stdlib.h"
#include "AudioController.h"
#include "SimpleLED.h"
#include "audio/00001.h"  // Boot sound

using namespace Exterminate;

int main() {
    stdio_init_all();
    
    // Small delay for system initialization
    sleep_ms(1000);
    
    printf("Exterminate Audio Test Starting...\n");
    
    // Initialize and test audio
    AudioController audio;
    
    if (audio.initialize()) {
        printf("Audio initialized successfully\n");
        
        // Play boot sound
        printf("Playing boot sound...\n");
        if (audio.playAudio(Audio::AudioIndex::AUDIO_00001)) {
            printf("Boot sound started successfully\n");
            // Defer LED init until audio pipeline is running to avoid timing races
#ifndef EXTERMINATE_DISABLE_LED
#ifdef PICO_DEFAULT_LED_PIN
            printf("Detected PICO_DEFAULT_LED_PIN=%d\n", PICO_DEFAULT_LED_PIN);
#else
            printf("No PICO_DEFAULT_LED_PIN defined by board.\n");
#endif
            if (Exterminate::SimpleLED::initialize()) {
                // Let audio fill first buffers
                sleep_ms(100);
                Exterminate::SimpleLED::on();
                printf("LED initialized and turned on.\n");
            } else {
                printf("LED not available on this board.\n");
            }

            // Also turn on external LED connected to GPIO 11
            const unsigned extLedPin = 11;
            if (Exterminate::SimpleLED::initializePin(extLedPin)) {
                Exterminate::SimpleLED::onPin(extLedPin);
                printf("External LED on GPIO %u initialized and turned on.\n", extLedPin);
            } else {
                printf("External LED on GPIO %u not initialized due to pin conflict.\n", extLedPin);
            }
#else
            printf("LED disabled at compile time (EXTERMINATE_DISABLE_LED).\n");
#endif
        } else {
            printf("Failed to start boot sound\n");
        }
        
        // Let the sound play for a few seconds
        sleep_ms(5000);
        
        printf("Audio test complete\n");
    } else {
        printf("Audio initialization failed!\n");
    }
    
    // Main loop - just keep the program running
    printf("Entering main loop...\n");
    while (true) {
        sleep_ms(1000);
    }
    
    return 0;
}
