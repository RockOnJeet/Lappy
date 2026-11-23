# -----------------------------------------------------------------------
# CMakeLists.txt extension for the native platform (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Combine all source files into a single library for this platform
add_library(lappy_core STATIC
    # HAL interfaces
    # HAL/GPIO.cpp
    SPI/SPI.cpp
)

# Expose generic_interfaces and SDK dependencies required by public headers.
target_link_libraries(lappy_core
    PUBLIC
        generic_interfaces  # from include/CMakeLists.txt
        # Platform specific dependencies
        generic_stdlib
        sim_spi
        # sim_dma
        # sim_pio
)

# Include the current directory for header files
target_include_directories(lappy_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/SPI
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/GenericSDK/src
)
