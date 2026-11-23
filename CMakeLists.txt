cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define SUPPORTED_PLATFORMS list
set(SUPPORTED_PLATFORMS
native
rp2
# Add other supported platforms here
)
# Default platform setup
set(PLATFORM "" CACHE STRING "Build target")
if(NOT PLATFORM)
    message(FATAL_ERROR "PLATFORM must be set by a CMake preset.")
endif()
set_property(CACHE PLATFORM PROPERTY STRINGS ${SUPPORTED_PLATFORMS})

# Check if the selected target is in the SUPPORTED_PLATFORMS list
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" PLATFORM_INDEX)
if(PLATFORM_INDEX EQUAL -1)
message(FATAL_ERROR "Unsupported platform: ${PLATFORM}. Supported platforms are: ${SUPPORTED_PLATFORMS}")
endif()

# RP2 specific setup
if(PLATFORM STREQUAL "rp2")
    # Extension compatibility
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
    # Board definition
    set(PICO_BOARD pico2 CACHE STRING "Board type")
    # Pull in Raspberry Pi Pico SDK (must be before project)
    include(${CMAKE_SOURCE_DIR}/HAL/rp2/pico_sdk_import.cmake)
endif()
if(PLATFORM STREQUAL "native")
    # Pull in Native SDK configuration (must be before project)
    include(${CMAKE_SOURCE_DIR}/HAL/native/native_sdk_import.cmake)
endif()

# Define the project
project(Lappy LANGUAGES C CXX)

# Make the native SDK targets available before configuring the native HAL.
if(PLATFORM STREQUAL "native")
    add_subdirectory(lib/GenericSDK)
endif()

# Delegate to the target-specific CMakeLists.txt file based on the selected target
add_subdirectory(include)
add_subdirectory(HAL/${PLATFORM})
add_subdirectory(src)
add_subdirectory(test)
