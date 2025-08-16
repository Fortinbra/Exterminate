#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "pico/stdlib.h"
#include "GamepadController.h"
#include "AudioController.h"
#include "SimpleLED.h"
#include "MotorController.h"

using namespace Exterminate;
using namespace Exterminate::SimpleLED;

int main() {
    stdio_init_all();
    
    // Small delay for system initialization
    sleep_ms(1000);
    
    printf("===========================================\n");
    printf("Exterminate Dalek - BluePad32 Test Starting\n");
    printf("===========================================\n");
    
    // Initialize LED status controller for blue eye stalk LED
    // Using GPIO 15 as blue LED (you may need to adjust this based on your hardware)
    LEDStatusController eyeLED;
    const unsigned int BLUE_LED_PIN = 15; // Adjust this pin as needed for your blue LED
    
    if (eyeLED.initialize(BLUE_LED_PIN)) {
        printf("Blue eye LED initialized on GPIO %u\n", BLUE_LED_PIN);
    } else {
        printf("WARNING: Failed to initialize blue eye LED on GPIO %u\n", BLUE_LED_PIN);
        printf("Continuing without LED status indication...\n");
    }
    
    // Initialize gamepad controller
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
    printf("Ready to accept Bluetooth gamepad connections!\n");
    printf("\n");
    printf("LED Status:\n");
    printf("- Breathing: Pairing mode (ready for connections)\n");
    printf("- Solid: Controller paired and ready\n");
    printf("- Fast blink: Error state\n");
    printf("- Slow blink: Initializing or connecting\n");
    printf("\n");
    printf("Instructions:\n");
    printf("1. Put your gamepad into pairing mode\n");
    printf("2. All gamepad inputs will be logged to this UART console\n");
    printf("3. Use Ctrl+C to stop the program if needed\n");
    printf("\n");
    printf("Starting BluePad32 event loop...\n");
    printf("LED updates will be handled automatically via timer.\n");
    printf("===========================================\n");
    
    // Start the event loop (this blocks and doesn't return)
    // LED updates are now handled automatically via the timer callback
    gamepadController.startEventLoop();
    
    // This line should never be reached
    printf("Event loop ended unexpectedly!\n");
    return 0;
}
