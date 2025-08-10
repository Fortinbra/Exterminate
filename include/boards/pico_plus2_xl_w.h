#pragma once

// Custom board: Pico Plus 2 XL W (RP2350B) with CYW43 pins like Pico W
// Based on pimoroni_pico_plus2_rp2350 but enabling WiFi/Bluetooth pin defaults.

// Identify RP2350 variant
#define PICO_RP2350 1
#define PICO_RP2350B 1

// UART defaults
#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif
#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 0
#endif
#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 1
#endif

// I2C defaults
#ifndef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C 0
#endif
#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 4
#endif
#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 5
#endif

// SPI defaults
#ifndef PICO_DEFAULT_SPI
#define PICO_DEFAULT_SPI 0
#endif
#ifndef PICO_DEFAULT_SPI_SCK_PIN
#define PICO_DEFAULT_SPI_SCK_PIN 18
#endif
#ifndef PICO_DEFAULT_SPI_TX_PIN
#define PICO_DEFAULT_SPI_TX_PIN 19
#endif
#ifndef PICO_DEFAULT_SPI_RX_PIN
#define PICO_DEFAULT_SPI_RX_PIN 16
#endif
#ifndef PICO_DEFAULT_SPI_CSN_PIN
#define PICO_DEFAULT_SPI_CSN_PIN 17
#endif

// LED (onboard) - if present; Pico Plus 2 typically uses a PWM LED on GPIO25
#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25
#endif

// CYW43 (Pico W style) pins/defines
#define CYW43_WL_GPIO_COUNT      3
#define CYW43_WL_GPIO0           23
#define CYW43_WL_GPIO1           24
#define CYW43_WL_GPIO2           25

#define CYW43_PIN_WL_HOST_WAKE   24
#define CYW43_PIN_WL_REG_ON      23
#define CYW43_PIN_WL_SDIO_1BIT   0

// SDIO pins follow Pico W defaults
#define CYW43_PIN_SDIO_CLK       23
#define CYW43_PIN_SDIO_CMD       24
#define CYW43_PIN_SDIO_D0        25
#define CYW43_PIN_SDIO_D1        26
#define CYW43_PIN_SDIO_D2        27
#define CYW43_PIN_SDIO_D3        28

// Bluetooth supported flag (through CYW43)
#define PICO_CYW43_SUPPORTED 1
#define PICO_WIFI_SUPPORTED 1
#define PICO_BLUETOOTH_SUPPORTED 1

// Allow USB stdio by default
#ifndef PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS
#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 0
#endif
