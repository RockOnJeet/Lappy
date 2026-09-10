#pragma once

/**
 * @file IDisplay.hpp
 * @brief Pure abstract interface for a color display.
 *
 * Defines the hardware-independent rendering contract used by application
 * code. Concrete implementations (e.g., ILI9341) live in src/drivers/.
 * The SPI and GPIO dependencies are injected at construction time via
 * ISPI and IGPIOPin interfaces.
 */

#include <cstddef>
#include <cstdint>

class IDisplay {
 public:
  virtual ~IDisplay() = default;

  /**
   * @brief Set display rotation.
   * @param rotation 0–3 mapping to 0°, 90°, 180°, 270°.
   */
  virtual void setRotation(uint8_t rotation) = 0;

  /** @brief Get current width in pixels (accounts for rotation). */
  virtual uint16_t getWidth() const = 0;

  /** @brief Get current height in pixels (accounts for rotation). */
  virtual uint16_t getHeight() const = 0;

  /**
   * @brief Define the rectangular region for subsequent pixel writes.
   * @return true if the window is within bounds, false otherwise.
   */
  virtual bool setAddressWindow(int32_t x0,
                                int32_t y0,
                                int32_t w,
                                int32_t h) = 0;

  /**
   * @brief Fill the entire display with a single RGB565 color.
   * @param color 16-bit RGB565 pixel value.
   */
  virtual void fillScreen(uint16_t color) = 0;

  /**
   * @brief Open a streaming session to a rectangular region.
   * @note  After this call, submitActiveBuffer() may be called repeatedly
   *        to push pixel rows into the defined region.
   */
  virtual void beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h) = 0;

  /**
   * @brief Get the driver's internal pixel buffer for writing.
   * @param length [out] Capacity of the buffer in pixels.
   * @return Pointer to the writable buffer.
   */
  virtual uint16_t* getActiveBuffer(size_t& length) = 0;

  /**
   * @brief Push the active buffer (or a portion of it) to the display.
   * @param length Number of pixels to send from the current buffer.
   */
  virtual void submitActiveBuffer(size_t length) = 0;

  /**
   * @brief Close the streaming session and flush any pending transfers.
   */
  virtual void endStream() = 0;
};
