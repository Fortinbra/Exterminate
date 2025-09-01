#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "pico/stdlib.h"
#include "GamepadController.h"
#include "AudioController.h"
#include "SimpleLED.h"
#include "MotorController.h"
#include "audio/00001.h"  // Boot sound
#include "MosfetDriver.h"

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
using namespace Exterminate::SimpleLED;

int main() {
    stdio_init_all();
    
    // Small delay for system initialization
    sleep_ms(1000);
    
    printf("===========================================\n");
    printf("Exterminate Dalek - Full System Starting\n");
    printf("===========================================\n");
    
    // Initialize LED status controller for blue eye stalk LED
    LEDStatusController eyeLED;
    // Relocated to a higher GPIO (bottom edge exposed) per hardware mounting requirement.
    // Moved out of the 35-43 range to avoid conflicts with external wiring.
    // Use a high GPIO in the 44-47 range by default.
    const unsigned int BLUE_LED_PIN = 44; // Blue LED for eye stalk status (previously 36)
    
    if (eyeLED.initialize(BLUE_LED_PIN)) {
        printf("Blue eye LED initialized on GPIO %u\n", BLUE_LED_PIN);
    } else {
        printf("WARNING: Failed to initialize blue eye LED on GPIO %u\n", BLUE_LED_PIN);
        printf("Continuing without LED status indication...\n");
    }
    
    // Initialize gamepad controller first
    GamepadController& gamepadController = GamepadController::getInstance();
    
    // Set the LED controller for automatic status updates
    if (eyeLED.isInitialized()) {
        gamepadController.setLEDController(&eyeLED);
    }
    
    if (!gamepadController.initialize()) {
        printf("ERROR: Failed to initialize gamepad controller!\n");
        printf("Make sure you're using a Pico W board with Bluetooth support.\n");
        return -1;
    }

    printf("GamepadController initialized successfully.\n");
    
    // Initialize and test audio system
    AudioController audio;
    if (audio.initialize()) {
        printf("Audio initialized successfully\n");
        
        // Set the audio controller for gamepad button controls after audio init
        gamepadController.setAudioController(&audio);
        
        // Play boot sound
        printf("Playing boot sound...\n");
        if (audio.playAudio(Audio::AudioIndex::AUDIO_00001)) {
            printf("Boot sound started successfully\n");
            
            // Two external LEDs driven by audio intensity via PWM (skip onboard LED)
            // Moved to higher GPIO range (35-47) to avoid mechanical blockage and wiring congestion.
            // Move external audio LEDs out of 35-43 into a lower header-friendly range (13-18)
            const unsigned extLedPins[] = {14, 15}; // Red audio LEDs (moved from 37,38) -> now using 14,15
            bool pwmOk[2] = {false, false};
            for (int i = 0; i < 2; ++i) {
                pwmOk[i] = Exterminate::SimpleLED::initializePwmPin(extLedPins[i], /*wrap*/255, /*clkdiv*/4.0f);
                printf("External LED on GPIO %u %s\n", extLedPins[i], pwmOk[i] ? "initialized with PWM." : "failed PWM init!");
            }

            bool redLedsWorking = (pwmOk[0] || pwmOk[1]);

            if (redLedsWorking) {
                // Periodically update LED brightness from audio intensity
                static repeating_timer_t ledTimer;
                struct LedTimerCtx { Exterminate::AudioController* audio; unsigned pins[2]; int count; float displayLevel; };
                static LedTimerCtx ctx{ &audio, {extLedPins[0], extLedPins[1]}, 2, 0.0f };
                add_repeating_timer_ms(20, [](repeating_timer_t* rt) -> bool {
                    auto* c = static_cast<LedTimerCtx*>(rt->user_data);
                    float intensity = 0.0f;
                    if (c && c->audio) {
                        // Apply natural decay to audio intensity for LED effects
                        c->audio->decayAudioIntensity();
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
                printf("Red LEDs configured to react to audio intensity\n");
            } else {
                printf("No external LEDs initialized. Check pins/wiring.\n");
            }
            
            // Store LED status for later reporting
            static bool s_redLedsWorking = redLedsWorking;
        } else {
            printf("Failed to start boot sound\n");
        }
    } else {
        printf("Audio initialization failed!\n");
    }
    
    // Configure the motor controller for the DRV8833
    static MotorController::Config motorConfig{
        .leftMotorPin1 = 6,  // AIN1
        .leftMotorPin2 = 7,  // AIN2
        .rightMotorPin1 = 27, // BIN1
        .rightMotorPin2 = 26, // BIN2
        .pwmFrequency = 20000 // 20 kHz
    };

    static MotorController motorController(motorConfig);

    if (motorController.initialize()) {
        printf("Motor controller initialized successfully.\n");
    } else {
        printf("Failed to initialize motor controller.\n");
        return -1;
    }

    // Set the MotorController for tank-style control using existing gamepadController
    gamepadController.setMotorController(&motorController);

    // Instantiate and register MOSFET driver (use a free GPIO pin)
    // Move MOSFET control out of 35-43 to the high GPIO region (44-47)
    const uint8_t MOSFET_CONTROL_PIN = 45; // MOSFET gate control (moved from 43)
    static Exterminate::MosfetDriver mosfetDriver(MOSFET_CONTROL_PIN);
    mosfetDriver.initialize();
    gamepadController.setMosfetDriver(&mosfetDriver);

    // Start the gamepad event loop
    gamepadController.startEventLoop();
    
    printf("\n");
    printf("===========================================\n");
    printf("System Status:\n");
    printf("- Blue Eye LED: %s\n", eyeLED.isInitialized() ? "Active (breathing = pairing mode)" : "Disabled");
    printf("- Red Audio LEDs: %s\n", audio.isInitialized() ? "Active (react to audio)" : "Disabled");
    printf("- Audio System: %s\n", audio.isInitialized() ? "Ready" : "Failed");
    printf("- Motor Control: %s\n", motorController.isInitialized() ? "Ready" : "Failed");
    printf("- Gamepad Controller: Ready for connections\n");
    printf("\n");
    printf("LED Status Indicators:\n");
    printf("- Blue LED Breathing: Pairing mode (ready for connections)\n");
    printf("- Blue LED Solid: Controller paired and ready\n");
    printf("- Blue LED Fast blink: Error state\n");
    printf("- Blue LED Slow blink: Initializing or connecting\n");
    printf("- Red LEDs: Brightness follows audio intensity\n");
    printf("\n");
    printf("Instructions:\n");
    printf("1. Put your gamepad into pairing mode\n");
    printf("2. All gamepad inputs will be logged to this UART console\n");
    printf("3. Audio Controls:\n");
    printf("   - A Button: Trigger sound bite\n");
    printf("   - Red LEDs will react to audio playback\n");
    printf("4. Use Ctrl+C to stop the program if needed\n");
    printf("\n");
    printf("Starting BluePad32 event loop...\n");
    printf("LED updates and system operation handled automatically.\n");
    printf("===========================================\n");
    
    // Start the gamepad event loop (this blocks and doesn't return)
    // All LED updates, audio, and motor control are handled via callbacks
    gamepadController.startEventLoop();
    
    // This line should never be reached
    printf("Event loop ended unexpectedly!\n");
    return 0;
}
