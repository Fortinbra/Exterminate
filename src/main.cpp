#include <stdio.h>
#include "pico/stdlib.h"

#include <btstack_run_loop.h>
#include <pico/cyw43_arch.h>
#include <uni.h>

#include "sdkconfig.h"
#include "exterminate_platform.h"

// Sanity check
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

int main()
{
    stdio_init_all();
    
    printf("Exterminate: Starting up...\n");

    // Initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    // Turn on LED to indicate we're starting up
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    printf("Exterminate: CYW43 initialized, setting up BluePad32...\n");

    // Must be called before uni_init()
    uni_platform_set_custom(get_exterminate_platform());

    // Initialize BluePad32
    uni_init(0, NULL);
    
    printf("Exterminate: BluePad32 initialized, ready for gamepad connections!\n");

    // Does not return - BTstack run loop takes over
    btstack_run_loop_execute();

    return 0;
}
