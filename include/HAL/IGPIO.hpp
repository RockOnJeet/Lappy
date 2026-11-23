#pragma once

#include <cstdint>

/**
 * @file IGPIO.hpp
 * @brief Pure abstract interface for a digital GPIO output pin.
 *
 * Provides hardware-independent pin control.
 * Implementations of this interface should be provided for each supported
 * platform.
 * @note This class should be made so that it keeps track of all defined pins
 * internally, and groups them by registers. This allows for efficient
 * read/writes.
 */
class IGPIO {
 public:
  virtual ~IGPIO() = default;

  /**
   * @brief Pin mode (direction) enumeration.
   */
  enum class Mode : uint8_t {
    Output = 0,
    Input = 1,
    Pullup = 2,
    Pulldown = 3,
  };

  /**
   * @brief Initialize the GPIO pin and set its direction.
   * @param mode Pin direction — Output or Input.
   */
  virtual void init(int pin, Mode mode = Mode::Output) = 0;

  /**
   * @brief Drive the pin high or low.
   * @param high true = logic high, false = logic low.
   */
  virtual void set(int pin, bool high) = 0;

  /**
   * @brief Read the current logic level of the pin.
   * @return true if high, false if low.
   */
  virtual bool read(int pin) = 0;
};
