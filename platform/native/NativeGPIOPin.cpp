#include "NativeGPIOPin.hpp"

#include <cstdio>

#define MODULE_DEBUG 0

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[NativeGPIO] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

void NativeGPIOPin::init(Mode mode) {
  DBG_PRINT("pin %u → %s (stub)\n", pin_,
            mode == Mode::Output ? "OUTPUT" : "INPUT");
}

void NativeGPIOPin::set(bool high) {
  level_ = high;
  DBG_PRINT("pin %u → %s\n", pin_, high ? "HIGH" : "LOW");
}

bool NativeGPIOPin::read() {
  DBG_PRINT("pin %u read → %s\n", pin_, level_ ? "HIGH" : "LOW");
  return level_;
}
