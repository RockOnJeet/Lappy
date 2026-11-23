#pragma once

/**
 * @file GPIO.hpp
 * @brief Hardware GPIO implementation for Native platform.
 *
 * Implements the IGPIO interface using the Native simulated GPIO.
 */

#include "HAL/IGPIO.hpp"

// Platform specific base includes
#include <native/stdlib.h>

/**
 * @brief Hardware GPIO implementation for RP2 platform.
 * @note As hardware pins are function-specific, check datasheet for valid pins
 * per instance.
 */
class GPIO : public IGPIO {
 public:
  /**
   * @brief Setup GPIO instance
   */
  GPIO();

  ~GPIO() override;

  /**
   * @brief Initialize the GPIO pin.
   * @param pin The pin affected by this operation.
   * @param mode Pin direction - Output (Default) or Input.
   * @note Input mode sets "None" as default pull, Output mode sets "Low" as
   * default logic.
   */
  void init(int pin, Mode mode = Mode::Output) override;

  /**
   * @brief Configure the internal pull resistor for a pin.
   * @param pin The pin affected by this operation.
   * @param pull Pin pull reference - None (Default), Pulldown, Pullup
   * @note Setting an output pin with a pull is saved implicitly, and is applied
   * when mode changes to "Input".
   */
  void set_pull(int pin, Pull pull = Pull::NONE) override;

  /**
   * @brief Set the mode of a pin at runtime.
   * @param pin The pin affected by this operation.
   * @param mode Pin direction - Output (Default) or Input.
   * @note The previous values (if any) for both Input and Output sets are
   * preserved.
   */
  void set_mode(int pin, Mode mode) override;

  /**
   * @brief Drive the pin high or low.
   * @param pin The pin affected by this operation.
   * @param high true = logic high, false = logic low.
   * @note Setting an input pin with a logic out is saved implicitly, and is
   * applied when mode changes to "Output".
   */
  void set(int pin, bool high) override;

  /**
   * @brief Read the current logic level of the pin.
   * @param pin The pin affected by this operation.
   * @return true if high, false if low.
   */
  bool read(int pin) override;

  /**
   * @brief De-initialize the GPIO pin.
   * @param pin The pin affected by this operation.
   * @warning The exact electrical state post command is platform dependent, and
   * the pin is no longer managed by this interface.
   */
  void deinit(int pin) override;
};
