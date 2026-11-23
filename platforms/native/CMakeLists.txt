# -----------------------------------------------------------------------
# CMakeLists.txt extension for the native platform (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Initialise the Sim Platform
native_sdk_init()

# Combine all source files into a single library for this platform
add_library(lappy_interfaces STATIC
    # HAL interfaces
    # HAL/GPIO.cpp
    HAL/SPI.cpp

    # Peripheral interfaces
    TFT/ILI9341/ILI9341.cpp
)

# Expose generic_interfaces to this lib
target_link_libraries(lappy_interfaces
    PUBLIC
        generic_interfaces
        generic_stdlib     # std defs support
        sim_spi          # Simulated SPI support
        # sim_dma          # Simulated DMA support
        # sim_pio         # Simulated PIO support
)

# Include the current directory for header files
target_include_directories(lappy_interfaces
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/HAL
)
