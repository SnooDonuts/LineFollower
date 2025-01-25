# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# We want to build for the Pico W
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Import the Pico SDK (must come before 'project()')
include(pico_sdk_import.cmake)

project(linefollower C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add our main executable target (named "linefollower" to match below)
add_executable(linefollower
    linefollower.c
    sensors.c
    motors.c
)

# Give the program a name and version (these lines apply to the 'linefollower' target)
pico_set_program_name(linefollower "linefollower")
pico_set_program_version(linefollower "0.1")

# Enable/disable UART/USB standard IO
pico_enable_stdio_uart(linefollower 0)
pico_enable_stdio_usb(linefollower 0)

# Link the target with necessary libraries
# - pico_stdlib: standard SDK library (printf, time, etc.)
# - pico_cyw43_arch_none: for Pico W Wi-Fi/LED control (no networking)
# - hardware_adc, hardware_pwm, hardware_gpio for sensor and motor code
target_link_libraries(linefollower
    pico_stdlib
    pico_cyw43_arch_none
    hardware_adc
    hardware_pwm
    hardware_gpio
)

# Include the current directory so that headers like "linefollower_sensors.h"
# and "linefollower_motors.h" can be found automatically.
target_include_directories(linefollower PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Generate extra files like .uf2, .bin, .hex for flashing
pico_add_extra_outputs(linefollower)
