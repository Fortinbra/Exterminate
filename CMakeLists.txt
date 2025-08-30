# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# BluePad32 configuration
set(BLUEPAD32_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/lib/bluepad32)
set(BTSTACK_ROOT ${PICO_SDK_PATH}/lib/btstack)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Use the official Pimoroni Pico Plus 2 W RP2350 board definition
set(PICO_BOARD pimoroni_pico_plus2_w_rp2350 CACHE STRING "Board type" FORCE)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(Exterminate C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Exterminate 
    src/main.cpp 
    src/AudioController.cpp
    src/AudioData.cpp
    src/AudioIndex.cpp
    src/SimpleLED.cpp
    src/MosfetDriver.cpp
    src/MotorController.cpp
    src/GamepadController.cpp
)
pico_set_program_name(Exterminate "Exterminate")
pico_set_program_version(Exterminate "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Exterminate 1)
pico_enable_stdio_usb(Exterminate 0)

# Add the standard library to the build
target_link_libraries(Exterminate
        pico_stdlib
        pico_cyw43_arch_none
        pico_btstack_classic
        pico_btstack_cyw43
        hardware_pwm
        pico_audio_i2s
        pico_multicore
        bluepad32)

# Needed for btstack_config.h / sdkconfig.h
# so that libbluepad32 can include them - MUST be before add_subdirectory
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# Need for BTstack headers
include_directories(${BTSTACK_ROOT}/src)

# Needed for btstack_config.h / sdkconfig.h
# so that libbluepad32 can include them
include_directories(src)

# Add BluePad32 library 
add_subdirectory(${BLUEPAD32_ROOT}/src/components/bluepad32 libbluepad32)

# Add the standard include files to the build
target_include_directories(Exterminate PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/audio
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${BLUEPAD32_ROOT}/src/components/bluepad32/include
)

pico_add_extra_outputs(Exterminate)

