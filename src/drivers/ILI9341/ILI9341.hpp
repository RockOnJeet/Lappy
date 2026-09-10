#pragma once

/**
 * @file ILI9341.hpp
 * @brief ILI9341 display driver — depends only on HAL interfaces.
 *
 * Accepts ISPI and IGPIOPin via constructor injection.
 * Zero platform-specific includes.
 */

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "hal/IDisplay.hpp"
#include "hal/IGPIOPin.hpp"
#include "hal/ISPI.hpp"

#include "ILI9341_macros.h"

class ILI9341 : public IDisplay {
 public:
  /**
   * @brief Construct the display driver.
   * @param spi Reference to an SPI bus (interface).
   * @param dc  Reference to the D/C (Data/Command) GPIO pin.
   * @param rst Pointer to the optional hardware-reset GPIO pin.
   *            Pass nullptr if RST is not connected.
   */
  ILI9341(ISPI& spi, IGPIOPin& dc, IGPIOPin* rst = nullptr);

  ~ILI9341() override = default;

  /**
   * @brief Initialize the display hardware (must call after SPI is begun).
   * @param use_dma Enable DMA pixel transfers.
   */
  void init(bool use_dma = false);

  // --- IDisplay overrides ------------------------------------------------
  void setRotation(uint8_t rotation) override;
  uint16_t getWidth() const override;
  uint16_t getHeight() const override;
  bool setAddressWindow(int32_t x0, int32_t y0, int32_t w, int32_t h) override;
  void fillScreen(uint16_t color) override;
  void beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h) override;
  uint16_t* getActiveBuffer(size_t& length) override;
  void submitActiveBuffer(size_t length) override;
  void endStream() override;

  /** @brief Convenience — fill the screen with black. */
  void clearScreen();

 private:
  ISPI& spi_;
  IGPIOPin& dc_;
  IGPIOPin* rst_;  // Optional; may be nullptr

  uint16_t width_ = ILI9341_WIDTH;
  uint16_t height_ = ILI9341_HEIGHT;
  bool dma_ = false;

  // Internal pixel buffer (ping-pong)
  static constexpr size_t kBufPixels = ILI9341_WIDTH * ILI9341_HEIGHT;
  uint16_t buffer_[kBufPixels];
  uint16_t* activeBuffer_ = buffer_;

  // Low-level helpers
  void writeCmd(uint8_t cmd);
  void writeData(uint8_t data);
  void waitIdle();
};
