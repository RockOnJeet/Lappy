#include "PicoGPIOPin.hpp"

#include <hardware/gpio.h>
#include <pico/stdlib.h>

#include <cstdio>

#define MODULE_DEBUG 0

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[PicoGPIO] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

void PicoGPIOPin::init(Mode mode) {
  gpio_init(pin_);
  gpio_set_dir(pin_, mode == Mode::Output ? GPIO_OUT : GPIO_IN);
  DBG_PRINT("pin %u → %s\n", pin_, mode == Mode::Output ? "OUTPUT" : "INPUT");
}

void PicoGPIOPin::set(bool high) {
  gpio_put(pin_, high);
}

bool PicoGPIOPin::read() {
  return gpio_get(pin_);
}
