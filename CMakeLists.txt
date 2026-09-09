# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build mode: OFF = Pico device (default), ON = native host (no Pico SDK hardware)
option(NATIVE_BUILD "Build for native host instead of Pico device" OFF)

if(NATIVE_BUILD)
    # ============================================================================
    # NATIVE BUILD: host executable, no Pico SDK, no hardware libraries
    # ============================================================================
    project(Lappy C CXX)

    add_executable(Lappy Lappy.cpp
        TFT/ILI9341/ILI9341.cpp
        SPI/Sim/SPI.cpp
        AnimatedGIF/src/AnimatedGIF.cpp
        )

    target_compile_definitions(Lappy PRIVATE NATIVE_BUILD=1)

    target_include_directories(Lappy PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/stubs   # pico/stdlib.h stub for native
    )
else()
    # ============================================================================
    # PICO BUILD: Raspberry Pi Pico SDK (RP2350)
    # ============================================================================
    # == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
    if(WIN32)
        set(USERHOME $ENV{USERPROFILE})
    else()
        set(USERHOME $ENV{HOME})
    endif()
    set(sdkVersion 2.2.0)
    set(toolchainVersion 14_2_Rel1)
    set(picotoolVersion 2.2.0-a4)
    set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
    if (EXISTS ${picoVscode})
        include(${picoVscode})
    endif()
    # ====================================================================================
    set(PICO_BOARD pico2 CACHE STRING "Board type")

    # Pull in Raspberry Pi Pico SDK (must be before project)
    include(pico_sdk_import.cmake)

    project(Lappy C CXX ASM)

    # Initialise the Raspberry Pi Pico SDK
    pico_sdk_init()

    add_executable(Lappy Lappy.cpp
        TFT/ILI9341/ILI9341.cpp
        SPI/SPI.cpp
        AnimatedGIF/src/AnimatedGIF.cpp
        # asset_registry.S
        )

    pico_set_program_name(Lappy "Lappy")
    pico_set_program_version(Lappy "0.1")

    # Generate PIO header
    # pico_generate_pio_header(Lappy ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

    # Modify the below lines to enable/disable output over UART/USB
    pico_enable_stdio_uart(Lappy 0)
    pico_enable_stdio_usb(Lappy 1)

    # Add the standard library to the build
    target_link_libraries(Lappy
            pico_stdlib)

    # Add the standard include files to the build
    target_include_directories(Lappy PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
    )

    # Add any user requested libraries
    target_link_libraries(Lappy 
            hardware_spi
            hardware_dma
            hardware_pio
            )

    pico_add_extra_outputs(Lappy)
endif()

