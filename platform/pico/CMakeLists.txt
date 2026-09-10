# platform/pico/CMakeLists.txt
# Pico platform static library — implements hal interfaces for RP2350 / RP2040.
# NOTE: named lappy_pico (not pico_platform) to avoid collision with the
# Pico SDK's own pico_platform interface target.

add_library(lappy_pico STATIC
    PicoSPI.cpp
    PicoGPIOPin.cpp
    platform.cpp
)

target_include_directories(lappy_pico
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lappy_pico
    PUBLIC  hal_interface
            pico_stdlib
            hardware_spi
            hardware_gpio
            hardware_dma
)
