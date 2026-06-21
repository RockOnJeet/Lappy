// Implementation of the SPI driver (see SPI.h for details)
#include "SPI.h"
#include <stdio.h>
#include "pico/stdlib.h"

// Global variables for SPI configuration and state
spi_inst_t* SPI_PORT = spi0;
uint16_t g_csPin;
int g_dma_channel = -1;

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

bool check_spi_pins(uint16_t sck, uint16_t mosi, uint16_t chip_sel) {
  if ((sck == 2 && mosi == 3 && chip_sel == 1))
    return true;  // SPI0
  if ((sck == 26 && mosi == 27 && chip_sel == 29))
    return false;  // SPI1
  DBG_PRINT(
      "Warning: Pins do not match standard HW SPI configurations. Recommended "
      "change to PIO SPI.\n");
  // TODO: Support non-standard pin configurations by implementing SW SPI
  return true;  // Default to SPI0 for non-standard pin configurations
}

static inline void setBusWidth(uint8_t width) {
  // Placeholder for setting bus width (8 or 16 bits)
  // Actual implementation would depend on the SPI hardware and driver
  spi_set_format(SPI_PORT, width, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  DBG_PRINT("Set SPI bus width to %u bits\n", width);
}

uint init(uint baudrate,
          uint16_t sck,
          uint16_t mosi,
          uint16_t chip_sel,
          bool use_dma) {
  // Save the pin for CS
  g_csPin = chip_sel;
#if SPI_MODE == 0  // HW SPI
  // Check compatible pins for HW SPI0/1
  SPI_PORT = (check_spi_pins(sck, mosi, chip_sel)
                  ? spi0
                  : spi1);  // Select SPI0 or SPI1 based on pin config
  DBG_PRINT("Initializing SPI with baudrate %.1fMHz (Detected SPI%u)\n",
            (float)(baudrate / (1000 * 1000)), SPI_PORT == spi0 ? 0 : 1);
  baudrate = spi_init(SPI_PORT ? spi0 : spi1, baudrate);

  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(mosi, GPIO_FUNC_SPI);

  gpio_init(g_csPin);
  gpio_set_dir(g_csPin, GPIO_OUT);
  gpio_put(g_csPin, 1);

  if (use_dma) {
    // Setup DMA channel to push data to the SPI data register
    DBG_PRINT("Setting up DMA channel for SPI transfers\n");
    g_dma_channel = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(g_dma_channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_16);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);
    channel_config_set_dreq(&config, spi_get_dreq(SPI_PORT, true));
    dma_channel_set_write_addr(g_dma_channel, &spi_get_hw(SPI_PORT)->dr, false);
    dma_channel_set_config(g_dma_channel, &config, false);
    DBG_PRINT("DMA channel %u initialized for SPI transfers\n", g_dma_channel);
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
  gpio_put(g_csPin, 0);  // Assert CS
  spi_write_blocking(SPI_PORT, data, len);
  gpio_put(g_csPin, 1);  // Deassert CS
  DBG_PRINT("SPI 8-bit write complete\n");
}

void write16(uint16_t* data, size_t len) {
  DBG_PRINT("Writing %zu bytes to SPI 16-bit\n", len);
  setBusWidth(16);
  gpio_put(g_csPin, 0);  // Assert CS
  spi_write16_blocking(SPI_PORT, data, len);
  gpio_put(g_csPin, 1);  // Deassert CS
  DBG_PRINT("SPI 16-bit write complete\n");
}

void write16_dma_async(uint16_t* data, size_t len) {
  DBG_PRINT("Starting asynchronous DMA transfer of %zu bytes to SPI 16-bit\n",
            len);
  if (g_dma_channel < 0) {
    DBG_PRINT(
        "Error: DMA channel not initialized. Call init() with use_dma "
        "set to true.\n");
    return;
  }
  setBusWidth(16);
  gpio_put(g_csPin, 0);  // Assert CS
  dma_channel_transfer_from_buffer_now(g_dma_channel, data, len);
}

void wait16_dma() {
  if (g_dma_channel < 0) {
    DBG_PRINT(
        "Error: DMA channel not initialized. Call init() with use_dma "
        "set to true.\n");
    return;
  }
  DBG_PRINT("Waiting for DMA transfer to complete\n");
  dma_channel_wait_for_finish_blocking(g_dma_channel);
  // Wait for SPI as well
  while (spi_is_busy(SPI_PORT))
    tight_loop_contents();
  gpio_put(g_csPin, 1);  // Deassert CS
  DBG_PRINT("DMA transfer complete\n");
}

void write16_dma(uint16_t* data, size_t len) {
  write16_dma_async(data, len);
  wait16_dma();
}