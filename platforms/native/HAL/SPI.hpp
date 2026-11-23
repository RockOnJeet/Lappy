#pragma once

#include "HAL/ISPI.hpp"

#include <cstdint>

/**
 * @brief Native SPI implementation for the Native platform.
 * @note As native SPI is not supported, this class only logs signals.
 */
class SPI : public ISPI {
 public:
  SPI(uint16_t sck, uint16_t mosi);

  ~SPI() override = default;

  // --- ISPI overrides ---------------------------------------------------
  uint32_t begin(uint32_t baudrate) override;
  void transfer(uint8_t* data, size_t len) override;
  void transfer(uint16_t* data, size_t len) override;
  bool isIdle() override;
  void end() override;
};
