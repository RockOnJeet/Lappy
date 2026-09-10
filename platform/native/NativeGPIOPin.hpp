#pragma once

/**
 * @file NativeGPIOPin.hpp
 * @brief Native (host) GPIO pin stub — no hardware.
 *
 * Implements IGPIOPin with debug-logging no-ops for host-side testing.
 */

#include "hal/IGPIOPin.hpp"

#include <cstdint>

class NativeGPIOPin : public IGPIOPin {
 public:
  explicit NativeGPIOPin(uint16_t pin) : pin_(pin) {}

  ~NativeGPIOPin() override = default;

  // --- IGPIOPin overrides ------------------------------------------------
  void init(Mode mode = Mode::Output) override;
  void set(bool high) override;
  bool read() override;

 private:
  uint16_t pin_;
  bool level_ = false;
};
