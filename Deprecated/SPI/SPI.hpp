#ifndef SPI_HPP
#define SPI_HPP

#ifndef SPI_MODE
#define SPI_MODE HW
#endif

#if defined(NATIVE_BUILD)
// Native (host) build: no Pico SDK hardware headers
#include <cstddef>
#include <cstdint>
typedef unsigned int uint;  // Pico SDK typedef, not standard C++
#else
#if SPI_MODE == HW
#include <hardware/spi.h>
#else
#include <hardware/pio.h>
#endif

#include <hardware/dma.h>
#include <pico/stdlib.h>
#endif

class SPI {
 public:
  /**
   * @brief Construct a new SPI object with specified SCK and MOSI pins
   * @param sck The GPIO pin number for the SCK (clock) line
   * @param mosi The GPIO pin number for the MOSI/RX (Master Out Slave In) line
   * @note The SCK and MOSI pins must be valid GPIO pins for the platform
   */
  SPI(uint16_t sck, uint16_t mosi);

  /**
   * @brief Initialize the SPI interface with the specified baud rate and DMA
   * usage
   * @param baudrate The desired baud rate for SPI communication
   * @return The actual baud rate set for SPI communication
   */
  uint begin(uint baudrate);

  /**
   * @brief Write an 8-bit value to the SPI interface
   * @param data The pointer to the array of 8-bit values to write
   * @param len The number of 8-bit values to write
   * @warning Blocking function; SPI blocks until the transfer is complete.
   */
  void transfer(uint8_t* data, size_t len);

  /**
   * @brief Write a 16-bit value to the SPI interface
   * @param data The pointer to the array of 16-bit values to write
   * @param len The number of 16-bit values (words) to write
   * @note Uses DMA if channel is valid (see addDMAchannel())
   * @warning Blocking function; SPI blocks, DMA blocks till bus idle.
   */
  void transfer(uint16_t* data, size_t len);

  /**
   * @brief Check status of the SPI interface
   * @return true if the SPI interface is idle, false otherwise
   * @note This also checks if the DMA transfer is complete if a DMA channel is
   * in use.
   */
  bool spi_idle();

  /**
   * @brief End the SPI communication and release resources
   * @note Removes the SPI instance and resets the DMA channel if used.
   */
  void end();

  /**
   * @brief Add a DMA channel for SPI transfers
   * @return The DMA channel number added, or -1 if no channel is available
   * @warning This function should be called after SPI.begin() and before any
   * DMA transfers.
   */
  int addDMAchannel();

  /**
   * @brief Check status of the DMA transfer
   * @return true if the DMA transfer is complete, false otherwise
   */
  bool dma_idle();

  /**
   * @brief End the DMA channel and release resources
   * @note This function should be called after all DMA transfers are complete.
   */
  void endDMAchannel();

 private:
#if defined(NATIVE_BUILD)
  // Native build: opaque state, no Pico SDK hardware types
  void* curr_inst = nullptr;            // Opaque handle (unused on native)
  bool current_width_is_16bit = false;  // Track current bus width state
  int dma_chan = -1;                    // DMA channel number (unused on native)
  // Helper function to set the bus width (8 or 16 bits)
  void setBusWidth(bool is16bit);
#else
  spi_inst_t* curr_inst = nullptr;  // Current SPI instance (spi0 or spi1)

  // Helper function to check if the provided SCK and MOSI pins are valid for
  spi_inst_t* check_pins(uint16_t sck, uint16_t mosi);

  bool current_width_is_16bit = false;  // Track current bus width state
  // Helper function to set the bus width (8 or 16 bits)
  void setBusWidth(bool is16bit);

  // DMA state
  int dma_chan = -1;                     // DMA channel number for SPI transfers
  dma_channel_config_t dma_chan_config;  // DMA channel configuration
#endif
};

#endif  // SPI_HPP