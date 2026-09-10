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
