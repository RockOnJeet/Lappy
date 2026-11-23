#pragma once

#include <cstddef>
#include <cstdint>

#include "HAL/ISPI.hpp"

/**
 * @brief Struct for setting up display pins.
 * @param CS Chip Select - Used to switch active device
 * @param DC Data/Command (Required) - Informs display the type of data sent.
 * @param RST Reset (Optional) - Used to toggle HW reset
 * @note Only SPI mode supported; will union 8/16bit data later.
 */
struct tft_spi_pins_t {
  uint8_t CS;
  uint8_t DC;
  uint8_t RST = UINT8_MAX;  // Disabled by default
};

/**
 * @brief Struct for defining any window size in pixels.
 * @param x The starting x coordinate.
 * @param y The starting y coordinate.
 * @param width The width of the window from the x coordinate.
 * @param height The height of the window from the y coordinate.
 */
struct window_size_t {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
};

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
   * @brief Setup the display to be ready to receive data.
   * @param spi SPI bus to which the display is connected to.
   */
  virtual void init(ISPI* spi) = 0;

  /**
   * @brief Set display rotation.
   * @param rotation 0–3 mapping to 0°, 90°, 180°, 270°.
   */
  virtual void setRotation(uint8_t rotation) = 0;

  /**
   * @brief Get the active draw area width.
   * @return Number of drawable pixel columns.
   * @note This fn accounts for rotation and address windows.
   */
  virtual uint16_t getWidth() const = 0;

  /**
   * @brief Get the active draw area height.
   * @return Number of drawable pixel rows.
   * @note This fn accounts for rotation and address windows.
   */
  virtual uint16_t getHeight() const = 0;

  /**
   * @brief Define the rectangular region for subsequent pixel writes.
   * @warning Invalid areas (out-of-bounds) are rejected.
   * @return Whether the area is valid.
   */
  virtual bool setAddressWindow(window_size_t size) = 0;

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
