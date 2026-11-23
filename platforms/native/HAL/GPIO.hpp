#pragma once

#include "HAL/IGPIO.hpp"

#include <cstdint>

/**
 * @brief Native GPIO implementation.
 * @note As native GPIO is not supported, this class only logs signals.
 */
class GPIO : public IGPIO {
 public:
  GPIO(uint16_t pin);

  ~GPIO() override = default;

  // --- IGPIO overrides ------------------------------------------------
  void init(int pin, Mode mode = Mode::Output) override;
  void set(int pin, bool high) override;
  bool read(int pin) override;
};
