# src/CMakeLists.txt
# Application executable — links ONLY to the platform interface library.

# --- AnimatedGIF (third-party, static) -------------------------------------
add_library(animatedgif STATIC
    ${CMAKE_SOURCE_DIR}/AnimatedGIF/src/AnimatedGIF.cpp
)
target_include_directories(animatedgif PUBLIC
    ${CMAKE_SOURCE_DIR}/AnimatedGIF/src
)

# Upstream AnimatedGIF expects Arduino millis() / delay().
# Our platform.cpp files provide them; this header declares them.
target_include_directories(animatedgif PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Pico stdlib is still needed for other SDK calls (GPIO, stdio, etc.).
if(NATIVE_BUILD)
    # Upstream AnimatedGIF guards millis()/delay() behind !__LINUX__ &&
    # !__MACH__.  Define __LINUX__ so those Arduino-only calls are compiled out
    # on native host builds.
    target_compile_definitions(animatedgif PUBLIC __LINUX__)
else()
    target_link_libraries(animatedgif PUBLIC pico_stdlib)
endif()

# --- Application sources ---------------------------------------------------
add_executable(Lappy
    Lappy.cpp
    drivers/ILI9341/ILI9341.cpp
)

target_link_libraries(Lappy
    PRIVATE
        lappy_${TARGET_PLATFORM}   # resolves to lappy_pico or lappy_native
        animatedgif
)

# Project root — lets Lappy.cpp resolve drivers/ILI9341/... and
# platform/<target>/... include paths.
target_include_directories(Lappy PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# ============================================================================
# Target-specific extras
# ============================================================================
if(NOT NATIVE_BUILD)
    # --- Pico SDK extras ---
    pico_set_program_name(Lappy "Lappy")
    pico_set_program_version(Lappy "0.1")
    pico_enable_stdio_uart(Lappy 0)
    pico_enable_stdio_usb(Lappy 1)
    pico_add_extra_outputs(Lappy)
else()
    # --- Native (host) build ---
    target_compile_definitions(Lappy PRIVATE NATIVE_BUILD=1)
endif()
