// Stub pico/stdlib.h for NATIVE (host) builds.
// Provides no-op implementations of the Pico SDK APIs used by this project so
// the code compiles and runs on the host. Only used when NATIVE_BUILD=ON in
// CMake (see CMakeLists.txt). The real header comes from the Pico SDK on
// device.
#ifndef _PICO_STDLIB_H
#define _PICO_STDLIB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int uint;  // Pico SDK typedef, not standard C++

// GPIO constants (subset used by the project)
#define GPIO_OUT 1
#define GPIO_IN  0

// MAX/MIN macros (provided by pico/stdlib.h on device)
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// --- no-op stubs -----------------------------------------------------------
static inline void stdio_init_all(void) {}
static inline void tight_loop_contents(void) {}
static inline void gpio_init(uint gpio) {
  (void)gpio;
}
static inline void gpio_set_dir(uint gpio, bool out) {
  (void)gpio;
  (void)out;
}
static inline void gpio_put(uint gpio, bool value) {
  (void)gpio;
  (void)value;
}

// --- time stubs (pico/time.h subset) ---------------------------------------
typedef struct {
  uint64_t _private_us_since_boot;
} absolute_time_t;

static inline absolute_time_t get_absolute_time(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  absolute_time_t t;
  t._private_us_since_boot =
      (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
  return t;
}

static inline int64_t absolute_time_diff_us(absolute_time_t from,
                                            absolute_time_t to) {
  return (int64_t)(to._private_us_since_boot - from._private_us_since_boot);
}

static inline void sleep_ms(uint32_t ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (long)(ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

#ifdef __cplusplus
}
#endif

#endif  // _PICO_STDLIB_H