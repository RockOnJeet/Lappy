#pragma once

/**
 * @file PicoSPI.hpp
 * @brief Pico RP2350 / RP2040 concrete SPI implementation.
 *
 * Implements ISPI using the Raspberry Pi Pico SDK (hardware/spi.h).
 * All Pico SDK headers are confined to this file and its .cpp.
 */

#include "hal/ISPI.hpp"

class PicoSPI : public ISPI {
 public:
  /**
   * @brief Construct and auto-detect the SPI instance from pin numbers.
   * @param sck  GPIO pin for SPI clock.
   * @param mosi GPIO pin for SPI MOSI.
   */
  PicoSPI(uint16_t sck, uint16_t mosi);

  ~PicoSPI() override = default;

  // --- ISPI overrides ---------------------------------------------------
  uint32_t begin(uint32_t baudrate) override;
  void transfer(uint8_t* data, size_t len) override;
  void transfer(uint16_t* data, size_t len) override;
  bool isIdle() override;
  void end() override;
  int addDMAchannel() override;
  bool dmaIdle() override;
  void endDMAchannel() override;

 private:
  void* inst_ = nullptr;  // spi_inst_t* (opaque — SDK type kept in .cpp)
  bool width16_ = false;
  int dmaChan_ = -1;

  void* resolveInstance(uint16_t sck, uint16_t mosi);
  void setBusWidth(bool is16bit);
};
