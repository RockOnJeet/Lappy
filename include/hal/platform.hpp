#pragma once

/**
 * @file platform.hpp
 * @brief Platform-service declarations (delay, yield).
 *
 * Each platform provides its own implementation in
 * platform/<target>/platform.cpp. This header lets driver code call
 * hal::delay_ms() / hal::cpu_yield() without ever including target-specific SDK
 * headers.
 */

#include <cstdint>

namespace hal {

/**
 * @brief Blocking delay for the given number of milliseconds.
 * @param ms Milliseconds to sleep.
 */
void delay_ms(uint32_t ms);

/**
 * @brief Yield the CPU in a spin-wait loop.
 *
 * On bare-metal targets this typically calls tight_loop_contents() or WFI;
 * on native (host) builds it may be a no-op or std::this_thread::yield().
 */
void cpu_yield();

}  // namespace hal
