#include "hal/platform.hpp"

#include <chrono>
#include <thread>

namespace hal {

void delay_ms(uint32_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void cpu_yield() {
  // No-op on native host.
}

}  // namespace hal

// ---------------------------------------------------------------------------
// Arduino-compatible timing primitives consumed by third-party libraries
// (e.g. AnimatedGIF) that expect millis() / delay().
// ---------------------------------------------------------------------------
extern "C" {

#include <time.h>

unsigned long millis(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (unsigned long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

void delay(unsigned long ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

}  // extern "C"
