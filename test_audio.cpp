// Quick test to verify audio system compilation
#include "include/AudioController.h"
#include "include/audio/audio_index.h"
#include <iostream>

int main() {
    using namespace Exterminate::Audio;
    
    std::cout << "Audio System Test" << std::endl;
    std::cout << "Available audio files: " << AUDIO_FILE_COUNT << std::endl;
    
    // Test that we can access audio file information
    for (size_t i = 0; i < AUDIO_FILE_COUNT; ++i) {
        const auto& file = AUDIO_FILES[i];
        std::cout << "File " << i << ": " << file.name 
                  << " (" << file.size << " bytes)" << std::endl;
    }
    
    // Test AudioController configuration
    AudioController::Config config;
    config.clock_pin = 18;
    config.data_pin = 19;
    config.select_pin = 20;
    config.sample_rate = 44100;
    
    std::cout << "AudioController config initialized" << std::endl;
    std::cout << "Test completed successfully!" << std::endl;
    
    return 0;
}
