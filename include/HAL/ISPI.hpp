#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @file ISPI.hpp
 * @brief Pure abstract interface for a SPI bus.
 * Implementations of this interface must be provided for each supported
 * platform.
 */
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
   * @brief Transfer 8-bit data over the SPI bus.
   * @param data Pointer to data buffer.
   * @param len  Number of bytes to transfer.
   */
  virtual void transfer(uint8_t* data, size_t len) = 0;

  /**
   * @brief Transfer 16-bit data over the SPI bus.
   * @param data Pointer to data buffer.
   * @param len  Number of 16-bit words to transfer.
   */
  virtual void transfer(uint16_t* data, size_t len) = 0;

  /**
   * @brief Check whether the SPI bus is idle.
   * @return true if idle, false if a transfer is in progress.
   */
  virtual bool isIdle() = 0;

  /**
   * @brief Deinitialize the SPI interface and release resources.
   */
  virtual void end() = 0;
};
