#ifndef SPI_H
#define SPI_H

#if defined(__cplusplus)
extern "C" {
#endif

#define SPI_MODE 0  // 0 for HW SPI, 1 for PIO SPI

#if SPI_MODE == 0
#include <hardware/spi.h>
extern spi_inst_t* SPI_PORT;  // Global variable to store the SPI instance
#else
#include <hardware/pio.h>
#endif
#include <hardware/dma.h>

// Global variables for SPI configuration and state
extern uint16_t g_csPin;  // Global variable to store the chip select pin

// DMA global state
extern int g_dma_channel;  // Global variable to store the DMA channel used for
                           // SPI transfers

/**
 * @brief Initialize the SPI interface with the specified parameters
 * @param baudrate The desired baud rate for SPI communication
 * @param sck The GPIO pin number for the SCK (clock) line
 * @param mosi The GPIO pin number for the MOSI/RX (Master Out Slave In)
 * @param chip_sel The GPIO pin number for the chip select line
 * @param use_dma Whether to use DMA for transfers
 * @return The actual baud rate set for SPI communication
 */
uint init(uint baudrate,
          uint16_t sck,
          uint16_t mosi,
          uint16_t chip_sel,
          bool use_dma);

/**
 * @brief Write an 8-bit value to the SPI interface
 * @param data The pointer to the array of 8-bit values to write
 * @param len The number of 8-bit values to write
 * @note Blocking function; Bus gets dynamically adjusted to 8bits.
 */
void write8(uint8_t* data, size_t len);

/**
 * @brief Write a 16-bit value to the SPI interface
 * @param data The pointer to the array of 16-bit values to write
 * @param len The number of 16-bit values to write
 * @note Blocking function; Bus gets dynamically adjusted to 16bits.
 */
void write16(uint16_t* data, size_t len);

/**
 * @brief Write a 16-bit value to the SPI interface using DMA asynchronously
 * @param data The pointer to the array of 16-bit values to write
 * @param len The number of 16-bit values to write
 * @note Non-blocking function; Bus gets dynamically adjusted to 16bits.
 */
void write16_dma_async(uint16_t* data, size_t len);

/**
 * @brief Wait for any previous DMA transfers to complete
 * @note This function should be called after write16_dma_async to ensure that
 * the DMA transfer has finished before proceeding with further operations.
 */
void wait16_dma();

/**
 * @brief Write a 16-bit value to the SPI interface using DMA
 * @param data The pointer to the array of 16-bit values to write
 * @param len The number of 16-bit values to write
 * @note Blocking function; Bus gets dynamically adjusted to 16bits.
 */
void write16_dma(uint16_t* data, size_t len);

#if defined(__cplusplus)
}
#endif

#endif  // SPI_H
