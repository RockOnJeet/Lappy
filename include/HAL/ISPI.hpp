#pragma once

#include <cstddef>
#include <cstdint>

// Special def as it's somehow not in lib
typedef unsigned int uint;

/**
 * @brief Struct for setting up 2/3-pin SPI.
 * @param SCK System Bus Clock
 * @param MOSI Master Out Slave In (Required)
 * @param MISO Master In Slave Out (Optional)
 * @note CS Pin is ignored as it is not common for all devices.
 */
struct spi_pins_t {
  uint8_t SCK;
  uint8_t MOSI;
  uint8_t MISO = UINT8_MAX;  // Disabled by default
};

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
  virtual uint begin(const spi_pins_t pins, const uint baudrate) = 0;

  /**
   * @brief Transfer 8-bit data over the SPI bus.
   * @param data Pointer to data buffer.
   * @param len  Number of bytes to transfer.
   * @param dest Pointer to the destination buffer.
   * @return Number of bytes written/read.
   * @warning This method must ALWAYS block, and recommend to batch commands
   * to use the 16-bit version.
   */
  virtual int transfer8(const uint8_t* data,
                        size_t len,
                        uint8_t* dest = nullptr) = 0;

  /**
   * @brief Transfer 16-bit data over the SPI bus.
   * @param data Pointer to data buffer.
   * @param len  Number of 16-bit words to transfer.
   * @param dest Pointer to the destination buffer.
   * @return Number of bytes written/read.
   * @note Should have a setup that speeds up batched mass transfers.
   */
  virtual int transfer16(const uint16_t* data,
                         size_t len,
                         uint16_t* dest = nullptr) = 0;

  /**
   * @brief Check whether the SPI bus is busy (data still being sent in
   * background).
   * @return true if busy, false if all transfers ended.
   */
  virtual bool isBusy() const = 0;

  /**
   * @brief Deinitialize the SPI interface and release resources.
   */
  virtual void end() = 0;
};
