// stdlib common to all platforms.
// Each platform must implement it's logic in ${PLATFORM}/stdlib.cpp

#pragma once

#include <stdint.h>

/**
 * @brief Initial setup for enabling stdio outputs to preset interfaces.
 */
void stdio_init(void);

/**
 * @brief Blocking delay for specified value.
 * @param ms Milliseconds to busy wait.
 */
void delay(uint32_t ms);
