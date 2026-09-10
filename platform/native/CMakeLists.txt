# platform/native/CMakeLists.txt
# Native (host) platform static library — simulates hal interfaces.

add_library(lappy_native STATIC
    NativeSPI.cpp
    NativeGPIOPin.cpp
    platform.cpp
)

target_include_directories(lappy_native
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lappy_native
    PUBLIC  hal_interface
)
