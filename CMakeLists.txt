cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico-console C CXX ASM)

pico_sdk_init()

add_executable(pico-console
    src/main.c
    src/display/display.c
    src/display/renderer.c
    src/drivers/input.c
)

target_link_libraries(pico-console
    pico_stdlib
    hardware_spi
    hardware_dma
    hardware_gpio
    pico_time
    pico_divider)

pico_enable_stdio_usb(pico-console 1)
pico_enable_stdio_uart(pico-console 0)

pico_add_extra_outputs(pico-console)
