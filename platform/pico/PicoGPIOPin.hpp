#pragma once

/**
 * @file PicoGPIOPin.hpp
 * @brief Pico RP2350 / RP2040 concrete GPIO pin implementation.
 *
 * Implements IGPIOPin using the Pico SDK (hardware/gpio.h).
 */

#include "hal/IGPIOPin.hpp"

#include <cstdint>

class PicoGPIOPin : public IGPIOPin {
 public:
  explicit PicoGPIOPin(uint16_t pin) : pin_(pin) {}

  ~PicoGPIOPin() override = default;

  // --- IGPIOPin overrides ------------------------------------------------
  void init(Mode mode = Mode::Output) override;
  void set(bool high) override;
  bool read() override;

 private:
  uint16_t pin_;
};
