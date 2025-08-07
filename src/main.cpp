#include <stdio.h>
#include "pico/stdlib.h"
#include "AudioController.h"
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
