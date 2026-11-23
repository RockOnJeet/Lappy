#include "GPIO.hpp"

#include <cstdio>

// ----------------- TODO -----------------
// Make this code output to a routable setup, so that it can be used in unit
// tests without polluting stdout.
// ----------------------------------------

#define MODULE_DEBUG 1

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[GPIO] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

void GPIO::init(int pin, Mode mode) {
  DBG_PRINT("pin %u → %s (stub)\n", pin,
            mode == Mode::Output   ? "OUTPUT"
            : mode == Mode::Input  ? "INPUT"
            : mode == Mode::Pullup ? "PULLUP"
                                   : "PULLDOWN");
}

void GPIO::set(int pin, bool high) {
  DBG_PRINT("pin %u → %s\n", pin, high ? "HIGH" : "LOW");
}

bool GPIO::read(int pin) {
  bool level = false;  // stub
  DBG_PRINT("pin %u read → %s\n", pin, level ? "HIGH" : "LOW");
  return false;  // stub
}
