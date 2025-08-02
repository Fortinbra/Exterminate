# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pimoroni_pico_plus2_w_rp2350 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Exterminate C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# BluePad32 configuration
set(BLUEPAD32_ROOT ${CMAKE_CURRENT_LIST_DIR}/lib/bluepad32)

# Add executable. Default name is the project name, version 0.1

add_executable(Exterminate 
    src/main.cpp 
    src/exterminate_platform.cpp 
    src/MotorController.cpp 
    src/AudioController.cpp
    src/I2S.cpp
    src/LEDController.cpp
)

pico_set_program_name(Exterminate "Exterminate")
pico_set_program_version(Exterminate "0.1")

# Generate PIO headers
pico_generate_pio_header(Exterminate ${CMAKE_CURRENT_LIST_DIR}/blink.pio)
pico_generate_pio_header(Exterminate ${CMAKE_CURRENT_LIST_DIR}/src/i2s.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Exterminate 0)
pico_enable_stdio_usb(Exterminate 1)

# Add the standard library to the build
target_link_libraries(Exterminate
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Exterminate PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/audio
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${BLUEPAD32_ROOT}/src/components/bluepad32/include
)

# Needed when using BTstack from our branch
include_directories(${PICO_SDK_PATH}/lib/btstack/3rd-party/bluedroid/encoder/include)
include_directories(${PICO_SDK_PATH}/lib/btstack/3rd-party/bluedroid/decoder/include)
include_directories(${PICO_SDK_PATH}/lib/btstack/src)

# Add any user requested libraries
target_link_libraries(Exterminate 
        hardware_pio
        hardware_pwm
        hardware_dma
        pico_cyw43_arch_none
        pico_btstack_classic
        pico_btstack_cyw43
        bluepad32
        )

# Needed for btstack_config.h / sdkconfig.h
# so that libbluepad32 can include them
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# Add compile definitions to help with BluePad32 compatibility
add_compile_definitions(NDEBUG)

# Add BluePad32 subdirectory
add_subdirectory(${BLUEPAD32_ROOT}/src/components/bluepad32 libbluepad32)

# Workaround for missing assert.h includes in BluePad32 - apply only to C files
target_compile_options(bluepad32 PRIVATE $<$<COMPILE_LANGUAGE:C>:-include assert.h>)

# Make sure BluePad32 can find our config files
target_include_directories(bluepad32 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

pico_add_extra_outputs(Exterminate)

