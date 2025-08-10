# Custom board cmake for Pico Plus 2 XL W (RP2350B) with CYW43
set(PICO_PLATFORM rp2350)
set(PICO_CHIP rp2350)
set(PICO_TARGET rp2350)

# Enable cyw43 build (WiFi/BT)
set(PICO_CYW43_SUPPORTED 1)
set(PICO_WIFI_SUPPORTED 1)
set(PICO_BLUETOOTH_SUPPORTED 1)

# Use default boot2 and standard linker scripts
