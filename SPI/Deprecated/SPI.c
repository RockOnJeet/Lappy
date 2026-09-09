// Implementation of the SPI driver (see SPI.h for details)
#include "SPI.h"
#include <stdio.h>
#include "pico/stdlib.h"

// Global variables for SPI configuration and state
spi_inst_t* SPI_PORT = spi0;
int g_dma_chan_A = -1;
int g_dma_chan_B = -1;

bool dma_ready = false;  // Flag to indicate if DMA is ready for transfers

// ============================================================================
// COMPILE-TIME MODULE DEBUG CONFIGURATION
// ============================================================================
#define MODULE_DEBUG \
  0  // Set to 1 for active console logs, 0 to silence completely

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[SPI] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif
// ============================================================================

bool check_spi_pins(uint16_t sck, uint16_t mosi) {
  if ((sck == 2 && mosi == 3))
    return true;  // SPI0
  if ((sck == 26 && mosi == 27))
    return false;  // SPI1
  DBG_PRINT(
      "Warning: Pins do not match standard HW SPI configurations. Recommended "
      "change to PIO SPI.\n");
  // TODO: Support non-standard pin configurations by implementing SW SPI
  return true;  // Default to SPI0 for non-standard pin configurations
}

static inline void setBusWidth(uint8_t width) {
  static uint8_t current_width = 0;
  if (current_width == width)
    return;  // No change needed
  current_width = width;
  spi_set_format(SPI_PORT, width, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  DBG_PRINT("Set SPI bus width to %u bits\n", width);
}

uint init(uint baudrate, uint16_t sck, uint16_t mosi, bool use_dma) {
#if SPI_MODE == 0  // HW SPI
  // Check compatible pins for HW SPI0/1
  SPI_PORT = (check_spi_pins(sck, mosi)
                  ? spi0
                  : spi1);  // Select SPI0 or SPI1 based on pin config
  DBG_PRINT("Initializing SPI with baudrate %.1fMHz (Detected SPI%u)\n",
            (float)(baudrate / (1000 * 1000)), SPI_PORT == spi0 ? 0 : 1);
  baudrate = spi_init(SPI_PORT ? spi0 : spi1, baudrate);

  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(mosi, GPIO_FUNC_SPI);

  // Stop any ongoing DMA transfers and reset the flags
  if (use_dma) {
    dma_ready = false;        // Reset DMA ready flag
    if (g_dma_chan_A >= 0) {  // Clear any existing DMA channel A
      dma_channel_abort(g_dma_chan_A);
      dma_channel_unclaim(g_dma_chan_A);
      g_dma_chan_A = -1;
    }
    DBG_PRINT("Setting up DMA channel for SPI transfers\n");
    g_dma_chan_A = dma_claim_unused_channel(true);

    // Setup DMA channel to handle the main transfer
    dma_channel_config c_A = dma_channel_get_default_config(g_dma_chan_A);
    channel_config_set_transfer_data_size(
        &c_A, DMA_SIZE_16);  //! Never send any data smaller than 16 bits to the
                             //! SPI DR register
    channel_config_set_read_increment(
        &c_A, true);  // Increment read address (from buffer)
    channel_config_set_write_increment(
        &c_A, false);  // Fixed write address (to SPI DR)
    channel_config_set_dreq(&c_A, spi_get_dreq(SPI_PORT, true));
    dma_channel_set_write_addr(g_dma_chan_A, &spi_get_hw(SPI_PORT)->dr, false);
    dma_channel_set_config(g_dma_chan_A, &c_A, false);  // DO NOT start yet
  }
  return baudrate;  // Return the actual baud rate set
#else               // PIO SPI
  // Initialize PIO SPI (implementation not shown)
  DBG_PRINT("Initializing PIO SPI with baudrate %u\n", baudrate);
  // Placeholder for PIO SPI initialization
  return baudrate;  // Return the actual baud rate set
#endif
}

void write8(uint8_t* data, size_t len) {
  DBG_PRINT("Writing %zu byte(s) to SPI 8-bit\n", len);
  setBusWidth(8);
  // gpio_put(g_csPin, 0);  // Assert CS
  spi_write_blocking(SPI_PORT, data, len);
  // gpio_put(g_csPin, 1);  // Deassert CS
  // DBG_PRINT("SPI 8-bit write complete\n");
}

void write16(uint16_t* data, size_t len) {
  DBG_PRINT("Writing %zu bytes to SPI 16-bit\n", len);
  setBusWidth(16);
  // gpio_put(g_csPin, 0);  // Assert CS
  spi_write16_blocking(SPI_PORT, data, len);
  // gpio_put(g_csPin, 1);  // Deassert CS
  // DBG_PRINT("SPI 16-bit write complete\n");
}

void dma_bus_start() {
  if (g_dma_chan_A < 0) {
    DBG_PRINT("Error: DMA channel not initialized. Call init() first.\n");
    return;
  }
  dma_ready = true;  // Set DMA ready flag
  setBusWidth(16);   // Set bus width to 16 bits for DMA transfers
  // gpio_put(g_csPin, 0);  // Assert CS to indicate ready for transfer
  DBG_PRINT("DMA bus initialized and ready for transfers\n");
}

bool is_dma_bus_idle() {
  if (!dma_ready) {
    DBG_PRINT("Warning: DMA bus not ready. Call dma_bus_init() first.\n");
    return true;  // If not ready, consider it idle
  }
  return !dma_channel_is_busy(g_dma_chan_A) && !spi_is_busy(SPI_PORT);
}

bool queue_dma_transfer(uint16_t* buffer, size_t len) {
  if (!dma_ready) {
    DBG_PRINT("Error: DMA not ready for transfer\n");
    return false;
  }
  if (len == 0) {
    DBG_PRINT("Warning: Attempted to transfer zero-length buffer\n");
    return true;  // Nothing to transfer
  }

  // Wait for any previous DMA transfer to complete
  bool has_printed = false;
  while (!is_dma_bus_idle()) {
    if (!has_printed) {
      DBG_PRINT("Waiting for DMA transfer to complete...\n");
      has_printed = true;
    }
    tight_loop_contents();  // Wait for DMA to finish
  }

  // Configure the DMA channel for the transfer
  DBG_PRINT("Started DMA transfer of %zu bytes\n", len);
  dma_channel_transfer_from_buffer_now(g_dma_chan_A, buffer, len);
  return true;
}

void dma_bus_end() {
  // Wait for the DMA transfer to complete
  bool has_printed = false;
  while (!is_dma_bus_idle()) {
    if (!has_printed) {
      DBG_PRINT("Waiting for DMA transfer to complete before ending bus...\n");
      has_printed = true;
    }
    tight_loop_contents();  // Wait for DMA to finish
  }
  // gpio_put(g_csPin, 1);  // Deassert CS to indicate end of transfer
  dma_ready = false;  // Reset DMA ready flag
  DBG_PRINT("DMA bus transfer completed and ended\n");
}
