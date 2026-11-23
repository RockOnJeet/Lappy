// Source file for GPIO implementation within rp2 series.

#include "GPIO.hpp"

GPIO() = default;
~GPIO() override = default;

void GPIO::init(int pin, Mode mode) {
  gpio_init(pin);
  gpio_disable_pulls(pin);
  gpio_set_dir(pin, mode == Mode::Output ? GPIO_OUT : GPIO_IN);
}

void GPIO::set_pull(int pin, Pull pull) {
  gpio_set_pulls(pin, pull == Pull::PULLUP, pull == Pull::PULLDOWN);
}

void GPIO::set_mode(int pin, Mode mode) {
  gpio_set_dir(pin, (mode == Mode::Output ? GPIO_OUT : GPIO_IN));
}

void GPIO::set(int pin, bool high) {
  gpio_put(pin, high);
}

bool GPIO::read(int pin) {
  return gpio_get(pin);
}

void GPIO::deinit(int pin) {
  gpio_deinit(pin);
}
