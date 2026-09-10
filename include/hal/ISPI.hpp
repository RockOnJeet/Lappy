#pragma once

/**
 * @file ISPI.hpp
 * @brief Pure abstract interface for SPI bus communication.
 *
 * Defines the hardware-independent contract for SPI peripherals.
 * Concrete implementations live in platform/<target>/ directories.
 *
 * Zero hardware-specific types, #ifdef directives, or private state.
 */

#include <cstddef>
#include <cstdint>

class ISPI {
 public:
  virtual ~ISPI() = default;

  /**
   * @brief Initialize the SPI interface at the requested baud rate.
   * @param baudrate Desired clock speed in Hz.
   * @return Actual baud rate achieved by the hardware.
   */
  virtual uint32_t begin(uint32_t baudrate) = 0;

  /**
   * @brief Transfer 8-bit data over the SPI bus (blocking).
   * @param data Pointer to data buffer.
   * @param len  Number of bytes to transfer.
   */
  virtual void transfer(uint8_t* data, size_t len) = 0;

  /**
   * @brief Transfer 16-bit data over the SPI bus (blocking / DMA).
   * @param data Pointer to data buffer.
   * @param len  Number of 16-bit words to transfer.
   */
  virtual void transfer(uint16_t* data, size_t len) = 0;

  /**
   * @brief Check whether the SPI bus (and any DMA) is idle.
   * @return true if idle, false if a transfer is in progress.
   */
  virtual bool isIdle() = 0;

  /**
   * @brief Deinitialize the SPI interface and release resources.
   */
  virtual void end() = 0;

  /**
   * @brief Allocate a DMA channel for SPI transfers.
   * @return Channel number on success, -1 on failure.
   */
  virtual int addDMAchannel() = 0;

  /**
   * @brief Check whether the DMA transfer is complete.
   * @return true if idle / no DMA, false if a DMA transfer is in progress.
   */
  virtual bool dmaIdle() = 0;

  /**
   * @brief Release the DMA channel.
   */
  virtual void endDMAchannel() = 0;
};
