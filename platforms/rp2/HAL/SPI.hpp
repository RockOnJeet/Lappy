#pragma once

#include "HAL/ISPI.hpp"

#include <hardware/dma.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>

/**
 * @brief Hardware SPI implementation for RP2 platform.
 * @note As hardware pins are function-specific, check datasheet for valid pins
 * per instance.
 */
class SPI : public ISPI {
 public:
  /**
   * @brief Setup SPI instance
   * @param spi The pre-defined spi bus to be used for this instance.
   * @param use_dma Initialize DMA and point to SPI for faster transfers.
   */
  SPI(spi_inst_t* spi, bool use_dma = false);

  ~SPI() override;

  /**
   * @brief Initialize the SPI interface at the requested baud rate.
   * @param pins The pins driven by the active instance. (pins must support
   * `func_spi`)
   * @param baudrate Desired clock speed in Hz.
   * @return Actual baud rate achieved by the hardware.
   * @warning Only checks first pin to be valid, and assumes others.
   */
  uint begin(const spi_pins_t pins, const uint baudrate) override;

  /**
   * @brief Transfer 8-bit data over the SPI bus.
   * @param data Pointer to data buffer.
   * @param len  Number of bytes to transfer.
   * @param dest Pointer to the destination buffer.
   * @return Number of bytes written/read.
   * @warning This method ALWAYS blocks, recommended to batch commands
   * and use 16-bit version.
   */
  int transfer8(const uint8_t* data,
                size_t len,
                uint8_t* dest = nullptr) override;

  /**
   * @brief Transfer 16-bit data over the SPI bus.
   * @param data Pointer to data buffer.
   * @param len  Number of 16-bit words to transfer.
   * @param dest Pointer to the destination buffer.
   * @return Number of bytes written/read.
   * @note This method may use DMA if it was initialized before.
   */
  int transfer16(const uint16_t* data,
                 size_t len,
                 uint16_t* dest = nullptr) override;

  /**
   * @brief Check whether the SPI bus is busy (data still being sent in
   * background).
   * @return true if busy, false if all transfers ended.
   */
  bool isBusy() const override;

  /**
   * @brief Deinitialize the SPI interface and release resources.
   */
  void end() override;

 private:
  // SPI
  spi_inst_t* _spi = nullptr;        // Holds current instance (spi0/1)
  bool _is8b = false;                // Default to faster 16-bit width
  void setWidth(bool is8b = false);  // Helper fn to change bus width

  // DMA
  int _dma_chan = -1;  // Holds active channel number (-1 = disabled)
  dma_channel_config_t _dma_config;  // Holds the active channel config
  uint8_t DMA_THRESHOLD = 8;         // (arbitrary value for now)
};
