// Native (host) implementation of the SPI class (see SPI.hpp for details).
// Compiled INSTEAD of SPI.cpp when NATIVE_BUILD is set in CMake.
// Same public class/method names as SPI.cpp, but simulated logic: transfers
// are logged to stdout and DMA is a no-op. Useful for host-side testing.
#ifndef NATIVE_BUILD
#error "SPI_native.cpp requires NATIVE_BUILD to be defined (set in CMake)"
#endif

#include "SPI/SPI.hpp"

#include <cstdio>

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

// ----------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR
// ----------------------------------------------------------------------------
SPI::SPI(uint16_t sck, uint16_t mosi) {
  // Native build: pins are informational only
  curr_inst = reinterpret_cast<void*>(1);  // Non-null sentinel = "constructed"
  DBG_PRINT("Native SPI constructed (sck=%u, mosi=%u)\n", sck, mosi);
}

uint SPI::begin(uint baudrate) {
  if (curr_inst == nullptr) {
    DBG_PRINT("Error: SPI not constructed. Cannot initialize.\n");
    return 0;  // Return 0 to indicate failure
  }
  DBG_PRINT("Native SPI begin @ %.1fMHz (simulated)\n",
            (float)(baudrate / (1000 * 1000)));
  return baudrate;  // Simulate success: report the requested baud rate
}

void SPI::transfer(uint8_t* data, size_t len) {
  setBusWidth(false);  // Set the bus width to 8 bits for this transfer
  DBG_PRINT("Native SPI write %zu byte(s): ", len);
  for (size_t i = 0; i < len; i++)
    printf("%02X ", data[i]);
  printf("\n");
}

void SPI::transfer(uint16_t* data, size_t len) {
  setBusWidth(true);  // Set the bus width to 16 bits for this transfer
  DBG_PRINT("Native SPI write %zu word(s): ", len);
  for (size_t i = 0; i < len; i++)
    printf("%04X ", data[i]);
  printf("\n");
}

bool SPI::spi_idle() {
  return true;  // Native SPI is always idle
}

void SPI::end() {
  curr_inst = nullptr;
  dma_chan = -1;
  DBG_PRINT("Native SPI deinitialized\n");
}

// ----------------------------------------------------------------------------
// DMA Functions (simulated no-ops on native)
// ----------------------------------------------------------------------------
int SPI::addDMAchannel() {
  if (curr_inst == nullptr) {
    DBG_PRINT("Error: SPI not initialized. Call begin() first.\n");
    return -1;  // Return -1 to indicate failure
  }
  if (dma_chan >= 0) {
    DBG_PRINT(
        "Error: DMA channel already added. Only one channel supported.\n");
    return -1;  // Return -1 to indicate failure
  }
  dma_chan = 0;  // Simulated channel number
  DBG_PRINT("Native DMA channel %d added (simulated)\n", dma_chan);
  return dma_chan;
}

bool SPI::dma_idle() {
  return true;  // Native DMA is always idle
}

void SPI::endDMAchannel() {
  dma_chan = -1;
  DBG_PRINT("Native DMA channel released\n");
}

// ----------------------------------------------------------------------------
// PRIVATE HELPERS
// ----------------------------------------------------------------------------
void SPI::setBusWidth(bool is16bit) {
  if (current_width_is_16bit == is16bit)
    return;  // No change needed
  current_width_is_16bit = is16bit;
  DBG_PRINT("Native SPI bus width set to %u bits\n", is16bit ? 16 : 8);
}