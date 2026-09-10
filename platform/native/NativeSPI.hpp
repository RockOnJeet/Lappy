#pragma once

/**
 * @file NativeSPI.hpp
 * @brief Native (host) SPI stub — no hardware, logs to stdout.
 *
 * Implements ISPI for host-side testing without a Pico SDK.
 * Transfers are printed to stdout; DMA is a no-op.
 */

#include "hal/ISPI.hpp"

#include <cstdint>

class NativeSPI : public ISPI {
 public:
  NativeSPI(uint16_t sck, uint16_t mosi);

  ~NativeSPI() override = default;

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
  bool constructed_ = false;
  bool width16_ = false;
  int dmaChan_ = -1;

  void setBusWidth(bool is16bit);
};
