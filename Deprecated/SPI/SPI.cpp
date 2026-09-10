// Implementation of the SPI class (see SPI.hpp for details)
#include "SPI.hpp"

// ============================================================================
// COMPILE-TIME MODULE DEBUG CONFIGURATION
// ============================================================================
#define MODULE_DEBUG \
  0  // Set to 1 for active console logs, 0 to silence completely

#if MODULE_DEBUG
#include <stdio.h>
#define DBG_PRINT(...) printf("[SPI] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif
// ----------------------------------------------------------------------------
// HELPER FUNCTIONS
// ----------------------------------------------------------------------------
spi_inst_t* SPI::check_pins(uint16_t sck, uint16_t mosi) {
  spi_inst_t* curr_inst = nullptr;
  if ((sck == 2 && mosi == 3)) {
    curr_inst = spi0;  // SPI0
  } else if ((sck == 26 && mosi == 27)) {
    curr_inst = spi1;  // SPI1
  } else {
    DBG_PRINT(
        "Warning: Pins do not match standard HW SPI configurations. "
        "Recommended change to PIO SPI.\n");
    return nullptr;  //* Return nullptr for non-standard pin configurations
  }
  // Set the GPIO functions for the SCK and MOSI pins
  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(mosi, GPIO_FUNC_SPI);
  return curr_inst;  // Return the selected SPI instance
}

void SPI::setBusWidth(bool is16bit) {
  if (current_width_is_16bit == is16bit)
    return;  // No change needed
  while (!spi_idle())
    tight_loop_contents();  // Wait for SPI bus to be idle before changing width
  current_width_is_16bit = is16bit;
  spi_set_format(curr_inst, is16bit ? 16 : 8, SPI_CPOL_0, SPI_CPHA_0,
                 SPI_MSB_FIRST);
  DBG_PRINT("Set SPI bus width to %u bits\n", is16bit ? 16 : 8);
}
// ----------------------------------------------------------------------------
SPI::SPI(uint16_t sck, uint16_t mosi) {
  curr_inst = check_pins(sck, mosi);  // Validate the provided SCK and MOSI pins
}

uint SPI::begin(uint baudrate) {
  if (curr_inst == nullptr) {
    DBG_PRINT(
        "Error: Invalid SCK and MOSI pin configuration. Cannot initialize "
        "SPI.\n");
    return 0;  // Return 0 to indicate failure
  }
  DBG_PRINT("Initializing SPI with baudrate %.1fMHz\n",
            (float)(baudrate / (1000 * 1000)));
  uint baud = spi_init(
      curr_inst,
      baudrate);  // Initialize the SPI interface with the specified baud rate
  return baud;    // Return the actual baud rate set
}

void SPI::transfer(uint8_t* data, size_t len) {
  setBusWidth(false);  // Set the bus width to 8 bits for this transfer
  DBG_PRINT("Writing %zu byte(s) to SPI 8-bit\n", len);
  spi_write_blocking(curr_inst, data, len);  // Perform the blocking write
}

void SPI::transfer(uint16_t* data, size_t len) {
  setBusWidth(true);  // Set the bus width to 16 bits for this transfer
  DBG_PRINT("Writing %zu word(s) to SPI 16-bit\n", len);
  if (dma_chan < 0)
    spi_write16_blocking(curr_inst, data, len);  // Perform the blocking write
  else {
    while (!dma_idle())
      tight_loop_contents();  // Wait for any previous DMA transfer to complete
    dma_channel_transfer_from_buffer_now(dma_chan, data,
                                         len);  // DMA transfer this data
  }
}

bool SPI::spi_idle() {
  if (curr_inst == nullptr) {
    DBG_PRINT("Error: SPI instance not initialized. Call begin() first.\n");
    return true;  // Return true to indicate idle since no SPI instance is set
  }
  if (dma_chan >= 0) {
    // If DMA is being used, check if both SPI and DMA are idle
    return dma_channel_is_busy(dma_chan) == false &&
           spi_is_busy(curr_inst) == false;  // Return true if both are idle
  }
  return spi_is_busy(curr_inst) == false;  // Return true if SPI is idle
}

void SPI::end() {
  while (!spi_idle())
    tight_loop_contents();  // Wait for SPI bus to be idle before deinitializing
  if (dma_chan >= 0)
    endDMAchannel();      // If a DMA channel was added, release it
  spi_deinit(curr_inst);  // Deinitialize the SPI interface
  // Reset vars
  curr_inst = nullptr;
  DBG_PRINT("SPI interface deinitialized\n");
}

// ----------------------------------------------------------------------------
// DMA Functions
// ----------------------------------------------------------------------------

int SPI::addDMAchannel() {
  if (curr_inst == nullptr) {
    DBG_PRINT("Error: SPI instance not initialized. Call begin() first.\n");
    return -1;  // Return -1 to indicate failure
  }
  if (dma_chan >= 0) {
    DBG_PRINT(
        "Error: DMA channel already added. Only one channel is supported.\n");
    return -1;  // Return -1 to indicate failure
  }
  dma_chan = dma_claim_unused_channel(false);  // Claim an unused DMA channel
  if (dma_chan < 0) {
    DBG_PRINT("Error: No available DMA channels. Cannot add DMA channel.\n");
    return -1;  // Return -1 to indicate failure
  }
  // Configure the DMA channel for SPI transfers
  dma_chan_config = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(
      &dma_chan_config,
      DMA_SIZE_16);  // Set transfer size to 16 bits for SPI DR register
  channel_config_set_read_increment(
      &dma_chan_config, true);  // Increment read address (from buffer)
  channel_config_set_write_increment(&dma_chan_config,
                                     false);  // Fixed write address (to SPI DR)
  channel_config_set_dreq(
      &dma_chan_config, spi_get_dreq(curr_inst, true));  // Set DREQ for SPI TX
  dma_channel_set_write_addr(dma_chan, &spi_get_hw(curr_inst)->dr,
                             false);  // Set write address to SPI DR register
  dma_channel_set_config(dma_chan, &dma_chan_config,
                         false);  // Do not start yet
  DBG_PRINT("DMA channel %d added for SPI transfers\n", dma_chan);
  return dma_chan;  // Return the DMA channel number added
}

bool SPI::dma_idle() {
  if (dma_chan < 0) {
    DBG_PRINT(
        "Error: DMA channel not initialized. Call addDMAchannel() first.\n");
    return true;  // Return true to indicate idle since no DMA channel is set
  }
  return dma_channel_is_busy(dma_chan) == false;  // Return true if DMA is idle
}

void SPI::endDMAchannel() {
  while (!dma_idle())
    tight_loop_contents();  // Wait for DMA transfer to complete before
                            // releasing
  if (dma_chan >= 0) {
    dma_channel_abort(dma_chan);    // Abort any ongoing DMA transfer
    dma_channel_unclaim(dma_chan);  // Release the DMA channel
    DBG_PRINT("DMA channel %d released\n", dma_chan);
    dma_chan = -1;  // Reset the DMA channel number
  }
}
