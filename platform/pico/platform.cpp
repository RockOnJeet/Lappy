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
