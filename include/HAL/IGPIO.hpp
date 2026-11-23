#pragma once

#include <cstdint>

/**
 * @file IGPIO.hpp
 * @brief Pure abstract interface for digital GPIO pin control.
 *
 * Implementations of this interface should be provided for each supported
 * platform.
 */
class IGPIO {
 public:
  virtual ~IGPIO() = default;

  /**
   * @brief Pin mode enumeration.
   */
  enum class Mode : uint8_t { Output = 0, Input = 1 };

  /**
   * @brief Input pull enumeration.
   */
  enum class Pull : uint8_t { NONE = 0, PULLDOWN = 1, PULLUP = 2 };

  /**
   * @brief Initialize the GPIO pin.
   * @param pin The pin affected by this operation.
   * @param mode Pin direction - Output (Default) or Input.
   * @note Input mode sets "None" as default pull, Output mode sets "Low" as
   * default logic.
   */
  virtual void init(int pin, Mode mode = Mode::Output) = 0;

  /**
   * @brief Configure the internal pull resistor for a pin.
   * @param pin The pin affected by this operation.
   * @param pull Pin pull reference - None (Default), Pulldown, Pullup
   * @note Setting an output pin with a pull is saved implicitly, and is applied
   * when mode changes to "Input".
   */
  virtual void set_pull(int pin, Pull pull = Pull::NONE) = 0;

  /**
   * @brief Set the mode of a pin at runtime.
   * @param pin The pin affected by this operation.
   * @param mode Pin direction - Output (Default) or Input.
   * @note The previous values (if any) for both Input and Output sets are
   * preserved.
   */
  virtual void set_mode(int pin, Mode mode) = 0;

  /**
   * @brief Drive the pin high or low.
   * @param pin The pin affected by this operation.
   * @param high true = logic high, false = logic low.
   * @note Setting an input pin with a logic out is saved implicitly, and is
   * applied when mode changes to "Output".
   */
  virtual void set(int pin, bool high) = 0;

  /**
   * @brief Read the current logic level of the pin.
   * @param pin The pin affected by this operation.
   * @return true if high, false if low.
   */
  virtual bool read(int pin) = 0;

  /**
   * @brief De-initialize the GPIO pin.
   * @param pin The pin affected by this operation.
   * @warning The exact electrical state post command is platform dependent, and
   * the pin is no longer managed by this interface.
   */
  virtual void deinit(int pin) = 0;
};
