# Lappy — HAL-structured CMake root.
# Delegates to platform/<target>/ and src/ via add_subdirectory.

# ============================================================================
# Global C++ standards
# ============================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Build mode option
# ============================================================================
option(NATIVE_BUILD "Build for native host instead of Pico device" OFF)

# ============================================================================
# Pico SDK bootstrap (skipped for native builds)
# ============================================================================
if(NOT NATIVE_BUILD)
    # == Pico VS Code extension integration (DO NOT EDIT) ==
    if(WIN32)
        set(USERHOME $ENV{USERPROFILE})
    else()
        set(USERHOME $ENV{HOME})
    endif()
    set(sdkVersion 2.2.0)
    set(toolchainVersion 14_2_Rel1)
    set(picotoolVersion 2.2.0-a4)
    set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
    if(EXISTS ${picoVscode})
        include(${picoVscode})
    endif()
    # ========================================================

    set(PICO_BOARD pico2 CACHE STRING "Board type")
    include(pico_sdk_import.cmake)
endif()

# ============================================================================
# Project declaration
# ============================================================================
project(Lappy C CXX ASM)

# ============================================================================
# SDK init (Pico only) & platform selection
# ============================================================================
if(NOT NATIVE_BUILD)
    pico_sdk_init()
    set(TARGET_PLATFORM pico)
else()
    set(TARGET_PLATFORM native)
endif()

# ============================================================================
# Global abstraction interface library (header-only)
# ============================================================================
add_library(hal_interface INTERFACE)
target_include_directories(hal_interface INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# ============================================================================
# Delegates (platform/<target>/ and src/) to subdirectories
# ============================================================================
add_subdirectory(platform/${TARGET_PLATFORM})
add_subdirectory(src)

