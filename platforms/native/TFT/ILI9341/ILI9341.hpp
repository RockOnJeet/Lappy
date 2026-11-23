#ifndef ILI9341_HPP
#define ILI9341_HPP

#include "ILI9341_macros.h"
#include "Peripherals/IDisplay.hpp"
#include "pico/stdlib.h"

class ILI9341 : public IDisplay {
 public:
  /**
   * @brief Setup ILI9341 instance.
   * @param pins The pins driven by the active instance. (can be any free GPIO)
   */
  ILI9341(const tft_spi_pins_t pins);

  ~ILI9341() override;

  /**
   * @brief Setup the display to be ready to receive data.
   * @param spi SPI bus to which the display is connected to.
   */
  virtual void init(ISPI* spi = nullptr) override;

  /**
   * @brief Set display rotation.
   * @param rotation 0–3 mapping to 0°, 90°, 180°, 270°.
   */
  void setRotation(uint8_t rotation) override;

  /**
   * @brief Get the active draw area width.
   * @return Number of drawable pixel columns.
   * @note This fn accounts for rotation and address windows.
   */
  uint16_t getWidth() const override;

  /**
   * @brief Get the active draw area height.
   * @return Number of drawable pixel rows.
   * @note This fn accounts for rotation and address windows.
   */
  uint16_t getHeight() const override;

  /**
   * @brief Define the rectangular region for subsequent pixel writes.
   * @warning Invalid areas (out-of-bounds) are rejected.
   * @return Whether the area is valid.
   */
  bool setAddressWindow(window_size_t size) override;

  /**
   * @brief Fill the entire display with a single RGB565 color.
   * @param color 16-bit RGB565 pixel value.
   * @warning Slow, as it updates the entire display.
   */
  void fillScreen(uint16_t color) override;
  // Convenience function to clear the screen to black.
  void clearScreen() { fillScreen(0x0000); }

  /**
   * @brief Open a streaming session to a rectangular region.
   * @note  After this call, submitActiveBuffer() may be called repeatedly
   *        to push pixel rows into the defined region.
   */
  void beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h) override;

  /**
   * @brief Get the driver's internal pixel buffer for writing.
   * @param length Capacity of the buffer in pixels, as a reference parameter.
   * @return Pointer to the writable buffer.
   */
  uint16_t* getActiveBuffer(size_t& length) override;

  /**
   * @brief Push the active buffer (or a portion of it) to the display.
   * @param index Starting pixel index in the active buffer to send.
   * @param length Number of pixels to send from the current buffer.
   */
  void submitActiveBuffer(size_t index, size_t length) override;

  /**
   * @brief Close the streaming session and flush any pending transfers.
   */
  void endStream() override;

 private:
  tft_spi_pins_t _pins;  // Holds connected pins
  ISPI* _spi = nullptr;  // Holds current SPI instance
  uint8_t _orient = 0;   // Current orientation (portrait default)
  window_size_t _win;    // Active window size

  // Helper function(s)
  inline void sendInitCmd(uint8_t cmd, const uint8_t* data, uint8_t len);
};

#endif  // ILI9341_HPP