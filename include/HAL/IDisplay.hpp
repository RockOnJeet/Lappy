#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @file IDisplay.hpp
 * @brief Pure abstract interface for a color display.
 * Implementations of this interface should be provided for each supported
 * platform.
 */
class IDisplay {
 public:
  virtual ~IDisplay() = default;

  /**
   * @brief Set display rotation.
   * @param rotation 0–3 mapping to 0°, 90°, 180°, 270°.
   */
  virtual void setRotation(uint8_t rotation) = 0;

  /** @brief Get current width in pixels (accounts for rotation and address
   * window). */
  virtual uint16_t getWidth() const = 0;

  /** @brief Get current height in pixels (accounts for rotation and address
   * window). */
  virtual uint16_t getHeight() const = 0;

  /**
   * @brief Define the rectangular region for subsequent pixel writes.
   * @note Allows overdraw, i.e. out-of-bounds coordinates are clipped.
   */
  virtual void setAddressWindow(int32_t x0,
                                int32_t y0,
                                int32_t w,
                                int32_t h) = 0;

  /**
   * @brief Fill the entire display with a single RGB565 color.
   * @param color 16-bit RGB565 pixel value.
   * @warning Slow, as it updates the entire display.
   */
  virtual void fillScreen(uint16_t color) = 0;
  // Convenience function to clear the screen to black.
  void clearScreen() { fillScreen(0x0000); }

  /**
   * @brief Open a streaming session to a rectangular region.
   * @note  After this call, submitActiveBuffer() may be called repeatedly
   *        to push pixel rows into the defined region.
   */
  virtual void beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h) = 0;

  /**
   * @brief Get the driver's internal pixel buffer for writing.
   * @param length Capacity of the buffer in pixels, as a reference parameter.
   * @return Pointer to the writable buffer.
   */
  virtual uint16_t* getActiveBuffer(size_t& length) = 0;

  /**
   * @brief Push the active buffer (or a portion of it) to the display.
   * @param index Starting pixel index in the active buffer to send.
   * @param length Number of pixels to send from the current buffer.
   */
  virtual void submitActiveBuffer(size_t index, size_t length) = 0;

  /**
   * @brief Close the streaming session and flush any pending transfers.
   */
  virtual void endStream() = 0;
};
