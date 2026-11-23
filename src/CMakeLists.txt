# -----------------------------------------------------------------------
# CMakeLists.txt extension for entrypoints (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Common source files for all entrypoints
# add_library(lappy_core STATIC
#     core/app.cpp
# )
# Include the current directory for header files
# target_include_directories(lappy_core
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/core
# )

# Example entrypoints
add_executable(Lappy
    Lappy/example_01.cpp
)

target_link_libraries(Lappy
    PRIVATE
        # lappy_core
        lappy_interfaces
)

# RP2 specific setup
if(PLATFORM STREQUAL "rp2")
lappy_configure_rp2(Lappy)
endif()

