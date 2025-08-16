#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "pico/stdlib.h"
#include "GamepadController.h"
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
    const unsigned int BLUE_LED_PIN = 15; // Blue LED for eye stalk status
    
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
        
        // Play boot sound
        printf("Playing boot sound...\n");
        if (audio.playAudio(Audio::AudioIndex::AUDIO_00001)) {
            printf("Boot sound started successfully\n");
            
            // Two external LEDs driven by audio intensity via PWM (skip onboard LED)
            // Changed from GPIO 11,12 to GPIO 18,19 to avoid PWM conflicts with motors
            // GPIO 11 shares PWM channel 5B with GPIO 27 (motor pin), causing interference
            const unsigned extLedPins[] = {18, 19};
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
    } else {
        printf("Audio initialization failed!\n");
    }
    
    // Initialize DRV8833 motor controller
    using Exterminate::MotorController;
    
    // If your DRV8833 breakout has nSLEEP, you can optionally define DRV8833_SLEEP_PIN
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
        /*leftMotorPin1 (AIN1)*/ 7,  // Hardware wiring: GPIO7 -> AIN1
        /*leftMotorPin2 (AIN2)*/ 6,  // Hardware wiring: GPIO6 -> AIN2
        /*rightMotorPin1 (BIN1)*/ 26, // Hardware wiring: GPIO26 -> BIN1
        /*rightMotorPin2 (BIN2)*/ 27, // Hardware wiring: GPIO27 -> BIN2
        /*pwmFrequency*/ 20000
    };
    static MotorController motors(mc);
    if (motors.initialize()) {
        printf("Motor controller initialized successfully.\n");
        
        // Quick motor test - brief forward motion to verify motors work
        printf("Testing motors briefly...\n");
        motors.setDifferentialDrive(0.3f, 0.0f);  // 30% forward
        sleep_ms(500);  // Run for 500ms
        motors.stopAllMotors();  // Stop
        printf("Motor test completed.\n");
        
        // Connect motor controller to gamepad for tank steering
        gamepadController.setMotorController(&motors);
        printf("Tank steering enabled - use left analog stick to drive!\n");
    } else {
        printf("Motor controller initialization failed.\n");
    }
    
    printf("\n");
    printf("===========================================\n");
    printf("System Status:\n");
    printf("- Blue Eye LED: %s\n", eyeLED.isInitialized() ? "Active (breathing = pairing mode)" : "Disabled");
    printf("- Audio System: %s\n", audio.isInitialized() ? "Ready" : "Failed");
    printf("- Motor Control: %s\n", motors.isInitialized() ? "Ready" : "Failed");
    printf("- Gamepad Controller: Ready for connections\n");
    printf("\n");
    printf("LED Status Indicators:\n");
    printf("- Breathing: Pairing mode (ready for connections)\n");
    printf("- Solid: Controller paired and ready\n");
    printf("- Fast blink: Error state\n");
    printf("- Slow blink: Initializing or connecting\n");
    printf("\n");
    printf("Instructions:\n");
    printf("1. Put your gamepad into pairing mode\n");
    printf("2. All gamepad inputs will be logged to this UART console\n");
    printf("3. Tank Steering Controls:\n");
    printf("   - Left Analog Stick Y-axis: Forward/Reverse throttle\n");
    printf("   - Left Analog Stick X-axis: Left/Right steering\n");
    printf("4. Use other gamepad controls to operate audio\n");
    printf("5. Use Ctrl+C to stop the program if needed\n");
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
