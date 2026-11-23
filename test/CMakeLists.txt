# -----------------------------------------------------------------------
# CMakeLists.txt extension for tests (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

include(CTest)  # Include CTest module for testing support

# Setup and load dependencies for unit testing
include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(unity)

# Test entrypoints
add_executable(test_demo test_demo/test_demo.cpp)
target_link_libraries(test_demo PRIVATE unity)
add_test(NAME TestDEMO COMMAND test_demo)

add_executable(test_spi test_spi/test_spi.cpp)
target_link_libraries(test_spi PRIVATE unity lappy_interfaces)
add_test(NAME TestSPI COMMAND test_spi)

# RP2 specific setup
if(PLATFORM STREQUAL "rp2")
lappy_configure_rp2(test_demo)
lappy_configure_rp2(test_spi)
endif()