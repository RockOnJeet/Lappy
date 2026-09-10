#pragma once

/**
 * @file arduino_compat.h
 * @brief Minimal Arduino-compatible timing declarations for third-party
 *        libraries (e.g. AnimatedGIF) that call millis() / delay().
 *
 * The actual implementations live in platform/<target>/platform.cpp.
 * This header is injected into the AnimatedGIF include path via CMake so
 * the unpatched upstream source compiles without modification.
 */

#ifdef __cplusplus
extern "C" {
#endif

unsigned long millis(void);
void delay(unsigned long ms);

#ifdef __cplusplus
}
#endif
