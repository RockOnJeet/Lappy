# -----------------------------------------------------------------------
# CMakeLists.txt extension for the rp2 platform (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Combine all source files into a single library for this platform
add_library(lappy_interfaces STATIC
    # HAL interfaces
    # HAL/GPIO.cpp
    SPI/SPI.cpp

    # Peripheral interfaces
    ../../peripherals/Displays/ILI9341/ILI9341.cpp
)

# Expose generic_interfaces to this lib
target_link_libraries(lappy_interfaces
    PUBLIC
        generic_interfaces
        pico_stdlib     # std defs support
        hardware_spi    # HW SPI support
        hardware_dma     # DMA support
        # hardware_pio    # PIO support
)

# Generate PIO headers
# pico_generate_pio_header(Lappy
#     ${CMAKE_CURRENT_LIST_DIR}/main.pio
# )

# Include the current directory for header files
target_include_directories(lappy_interfaces
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Fn to setup extras (must be called POST add_exec())
function(lappy_configure_rp2 TARGET)
    pico_set_program_name(${TARGET} "${TARGET}")
    pico_set_program_version(${TARGET} "0.1")
    pico_enable_stdio_uart(${TARGET} 0)     # STDIO over UART pins
    pico_enable_stdio_usb(${TARGET} 1)      # STDIO over USB
    pico_add_extra_outputs(${TARGET})

    target_link_libraries(${TARGET}
        PRIVATE
        lappy_interfaces
    )
endfunction()
