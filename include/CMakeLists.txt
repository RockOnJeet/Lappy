# -----------------------------------------------------------------------
# CMakeLists.txt extension for generic interfaces (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Create an interface library for generic interfaces
add_library(generic_interfaces INTERFACE)

# Include the current directory for header files
target_include_directories(generic_interfaces
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)