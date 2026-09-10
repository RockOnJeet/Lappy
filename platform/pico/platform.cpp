#include "hal/platform.hpp"

#include <pico/stdlib.h>

namespace hal {

void delay_ms(uint32_t ms) {
  sleep_ms(ms);
}

void cpu_yield() {
  tight_loop_contents();
}

}  // namespace hal

// ---------------------------------------------------------------------------
// Arduino-compatible timing primitives consumed by third-party libraries
// (e.g. AnimatedGIF) that expect millis() / delay().
// ---------------------------------------------------------------------------
extern "C" {

unsigned long millis(void) {
  return to_ms_since_boot(get_absolute_time());
}

void delay(unsigned long ms) {
  sleep_ms((uint32_t)ms);
}

}  // extern "C"
