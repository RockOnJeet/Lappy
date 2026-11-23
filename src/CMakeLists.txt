add_executable(Lappy
    Lappy.cpp
    ${PROJECT_SOURCE_DIR}/include/${PLATFORM}/lappy_stdlib.cpp
)

target_include_directories(Lappy PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# ==================================================
# Platform-specific executable configuration
# ==================================================

if(PLATFORM STREQUAL "rp2")

    pico_set_program_name(Lappy "Lappy")
    pico_set_program_version(Lappy "0.1")

    pico_enable_stdio_usb(Lappy 1)
    pico_enable_stdio_uart(Lappy 0)

    target_link_libraries(Lappy PRIVATE
        pico_stdlib
        hardware_spi
        # hardware_dma
        # hardware_pio
    )

    pico_add_extra_outputs(Lappy)

elseif(PLATFORM STREQUAL "native")

    # Host-native configuration.

endif()