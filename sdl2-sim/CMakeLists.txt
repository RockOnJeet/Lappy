cmake_minimum_required(VERSION 3.16)

project(sdl2-sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG QUIET)

if(TARGET SDL2::SDL2)
    set(SDL2_TARGET SDL2::SDL2)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    set(SDL2_TARGET PkgConfig::SDL2)
endif()

add_executable(sdl2-sim main.cpp)
target_link_libraries(sdl2-sim PRIVATE ${SDL2_TARGET})
