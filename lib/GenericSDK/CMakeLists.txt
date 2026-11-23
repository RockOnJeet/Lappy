# -----------------------------------------------------------------------
# GenericSDK - Native platform SDK
# -----------------------------------------------------------------------

# Shared internal GPIO state used by simulated peripherals.
add_library(sim_gpio_state STATIC
    src/sim/gpio/gpio_state.c
)


# Native standard library
add_library(generic_stdlib STATIC
    src/native/stdlib.c
)

target_include_directories(generic_stdlib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(generic_stdlib
    PRIVATE
        sim_gpio_state
)


# Simulated SPI peripheral
add_library(sim_spi STATIC
    src/sim/spi/spi.c
)

target_include_directories(sim_spi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sim_spi
    PRIVATE
        sim_gpio_state
)
