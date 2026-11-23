# -----------------------------------------------------------------------
# CMakeLists.txt extension for the native platform (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Combine all source files into a single library for this platform
add_library(lappy_interfaces STATIC
    HAL/SPI.cpp

    # Internal tracing implementations
    Trace/TraceRecorder.cpp
    Trace/TraceSPI.cpp
    Trace/TraceDMA.cpp
)

# Expose generic_interfaces to this lib
target_link_libraries(lappy_interfaces
    PUBLIC
        generic_interfaces
)

# Include the current directory for header files
target_include_directories(lappy_interfaces
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/HAL
        ${CMAKE_CURRENT_SOURCE_DIR}/Trace
)
