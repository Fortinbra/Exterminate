#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "pico/stdlib.h"
#include "AudioController.h"
#include "SimpleLED.h"
#include "MotorController.h"
#include "audio/00001.h"  // Boot sound

// Guard optional CYW43 include so builds succeed even if headers aren't present
#if defined(__has_include)
#  if __has_include("pico/cyw43_arch.h")
#    include "pico/cyw43_arch.h"
#    define EX_HAS_CYW43 1
#  else
#    define EX_HAS_CYW43 0
#  endif
#else
#  define EX_HAS_CYW43 0
#endif

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
            // Two external LEDs driven by audio intensity via PWM (skip onboard LED)
            const unsigned extLedPins[] = {11, 12};
            bool pwmOk[2] = {false, false};
            for (int i = 0; i < 2; ++i) {
                pwmOk[i] = Exterminate::SimpleLED::initializePwmPin(extLedPins[i], /*wrap*/255, /*clkdiv*/4.0f);
                printf("External LED on GPIO %u %s\n", extLedPins[i], pwmOk[i] ? "initialized with PWM." : "failed PWM init!");
            }

            if (pwmOk[0] || pwmOk[1]) {
                // Periodically update LED brightness from audio intensity
                static repeating_timer_t ledTimer;
                struct LedTimerCtx { Exterminate::AudioController* audio; unsigned pins[2]; int count; float displayLevel; };
                static LedTimerCtx ctx{ &audio, {extLedPins[0], extLedPins[1]}, 2, 0.0f };
                add_repeating_timer_ms(20, [](repeating_timer_t* rt) -> bool {
                    auto* c = static_cast<LedTimerCtx*>(rt->user_data);
                    float intensity = 0.0f;
                    if (c && c->audio) {
                        intensity = c->audio->getAudioIntensity();
                    }
                    // Increase contrast: deadzone + gamma + peak hold
                    const float deadzone = 0.20f;
                    float adj = (intensity - deadzone) * (1.0f / (1.0f - deadzone));
                    if (adj < 0.0f) adj = 0.0f;
                    if (adj > 1.0f) adj = 1.0f;
                    const float gamma = 2.5f;
                    float b = adj <= 0.0f ? 0.0f : static_cast<float>(std::pow(adj, gamma));
                    c->displayLevel = std::max(b, c->displayLevel * 0.90f);
                    for (int i = 0; i < c->count; ++i) {
                        Exterminate::SimpleLED::setBrightnessPin(c->pins[i], c->displayLevel);
                    }
                    return true; // keep repeating
                }, &ctx, &ledTimer);
            } else {
                printf("No external LEDs initialized. Check pins/wiring.\n");
            }
        } else {
            printf("Failed to start boot sound\n");
        }
        
    // Let the sound play for a few seconds
    sleep_ms(5000);
        
        printf("Audio test complete\n");
    } else {
        printf("Audio initialization failed!\n");
    }
    
    // Minimal cyw43 init to validate board setup (no background thread)
#if EX_HAS_CYW43
    if (cyw43_arch_init()) {
        printf("cyw43 init failed (expected if no module)") ;
    } else {
        printf("cyw43 init ok\n");
        cyw43_arch_deinit();
    }
#else
    printf("CYW43 headers not available at build time; skipping init.\n");
#endif

    // Initialize DRV8833 motor controller and spin motors at startup
    using Exterminate::MotorController;
    
    // If your DRV8833 breakout has nSLEEP, you can optionally define DRV8833_SLEEP_PIN
    // at compile time to drive it HIGH here. Otherwise, tie nSLEEP to VM on hardware.
    #ifdef DRV8833_SLEEP_PIN
    {
        const uint SLP = DRV8833_SLEEP_PIN;
        gpio_init(SLP);
        gpio_set_dir(SLP, GPIO_OUT);
        gpio_put(SLP, 1);
        printf("DRV8833 nSLEEP on GPIO %u set HIGH.\n", SLP);
    }
    #endif

    static MotorController::Config mc{
        /*leftMotorPin1 (AIN1)*/ 3, // swapped to match wiring: GPIO3 -> AIN1
        /*leftMotorPin2 (AIN2)*/ 2, // swapped to match wiring: GPIO2 -> AIN2
        /*rightMotorPin1*/ 4,
        /*rightMotorPin2*/ 5,
        /*pwmFrequency*/ 20000
    };
    static MotorController motors(mc);
    if (motors.initialize()) {
        printf("Motor controller initialized. Spinning both motors full forward.\n");
        motors.setMotorSpeed(MotorController::Motor::LEFT, 1.0f);
        motors.setMotorSpeed(MotorController::Motor::RIGHT, 1.0f);
    } else {
        printf("Motor controller initialization failed.\n");
    }
    
    // Main loop - just keep the program running
    printf("Entering main loop...\n");
    while (true) {
        sleep_ms(1000);
    }
    
    return 0;
}
