#pragma once

/**
 * @file IGPIOPin.hpp
 * @brief Pure abstract interface for a digital GPIO output pin.
 *
 * Provides hardware-independent pin control for DC, RST, CS, and other
 * digital output signals used by peripheral drivers.
 */

#include <cstdint>

class IGPIOPin {
 public:
  virtual ~IGPIOPin() = default;

  /**
   * @brief Pin mode (direction) enumeration.
   */
  enum class Mode : uint8_t {
    Output = 0,
    Input = 1,
  };

  /**
   * @brief Initialize the GPIO pin and set its direction.
   * @param mode Pin direction — Output or Input.
   */
  virtual void init(Mode mode = Mode::Output) = 0;

  /**
   * @brief Drive the pin high or low.
   * @param high true = logic high, false = logic low.
   */
  virtual void set(bool high) = 0;

  /**
   * @brief Read the current logic level of the pin.
   * @return true if high, false if low.
   */
  virtual bool read() = 0;
};
