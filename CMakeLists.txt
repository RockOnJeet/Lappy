# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

# ==================================================
# Platform selection
# ==================================================

set(SUPPORTED_PLATFORMS
    native
    rp2
    # Add more platform names here.
)

set(PLATFORM "" CACHE STRING "Build target")

if(NOT PLATFORM)
    message(FATAL_ERROR "PLATFORM must be set by a CMake preset.")
endif()

set_property(CACHE PLATFORM PROPERTY STRINGS ${SUPPORTED_PLATFORMS})

list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)

if(PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR
        "Unsupported platform: ${PLATFORM}. "
        "Supported platforms: ${SUPPORTED_PLATFORMS}"
    )
endif()


# ==================================================
# Pre-project platform setup
# ==================================================

if(PLATFORM STREQUAL "rp2")

    set(PICO_BOARD pico2 CACHE STRING "Board type")

    # Import Pico SDK (must happen before project())
    include(pico_sdk_import.cmake)

elseif(PLATFORM STREQUAL "native")

    message(STATUS "Configuring native build")

endif()


# ==================================================
# Common project configuration
# ==================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Lappy C CXX ASM)


# ==================================================
# Post-project platform setup
# ==================================================

if(PLATFORM STREQUAL "rp2")

    pico_sdk_init()

elseif(PLATFORM STREQUAL "native")

    # Native dependencies can be initialized here later.

endif()


# ==================================================
# Source directory
# ==================================================

add_subdirectory(src)
