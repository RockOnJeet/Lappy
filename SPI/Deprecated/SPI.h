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

// DMA global state
extern bool dma_ready;

/**
 * @brief Initialize the SPI interface with the specified parameters
 * @param baudrate The desired baud rate for SPI communication
 * @param sck The GPIO pin number for the SCK (clock) line
 * @param mosi The GPIO pin number for the MOSI/RX (Master Out Slave In)
 * @param use_dma Whether to use DMA for transfers
 * @return The actual baud rate set for SPI communication
 */
uint init(uint baudrate, uint16_t sck, uint16_t mosi, bool use_dma);

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

// DMA functions

/**
 * @brief Setup a DMA bus transfer with the specified buffer and length
 */
void dma_bus_start();

/**
 * @brief Check if the DMA transfer is complete
 * @return true if the transfer is complete, false otherwise
 * @note This function is non-blocking; it checks the status of the transfer.
 */
bool is_dma_bus_idle();

/**
 * @brief Start/queue a DMA transfer for the specified buffer and length
 * @param buffer The pointer to the buffer containing data to transfer
 * @param length The number of bytes to transfer
 * @return true if the transfer was successfully started, false otherwise
 * @note This function is blocking; it waits for any ongoing transfer to
 * complete before starting a new one.
 */
bool queue_dma_transfer(uint16_t* buffer, size_t length);

/**
 * @brief End the current DMA bus transfer
 * @note This function is blocking; it waits for the transfer to complete before
 * returning.
 */
void dma_bus_end();

#if defined(__cplusplus)
}
#endif

#endif  // SPI_H
