#include "NativeSPI.hpp"

#include <cstdio>

#define MODULE_DEBUG 0

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[NativeSPI] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

// ============================================================================
// Construction
// ============================================================================
NativeSPI::NativeSPI(uint16_t sck, uint16_t mosi) {
  constructed_ = true;
  DBG_PRINT("constructed (sck=%u, mosi=%u)\n", sck, mosi);
}

// ============================================================================
// ISPI overrides
// ============================================================================
uint32_t NativeSPI::begin(uint32_t baudrate) {
  if (!constructed_) {
    DBG_PRINT("Error: not constructed.\n");
    return 0;
  }
  DBG_PRINT("begin @ %.1f MHz (simulated)\n", (float)(baudrate / 1'000'000.0));
  return baudrate;
}

void NativeSPI::transfer(uint8_t* data, size_t len) {
  setBusWidth(false);
  DBG_PRINT("write %zu byte(s): ", len);
  for (size_t i = 0; i < len; i++)
    printf("%02X ", data[i]);
  printf("\n");
}

void NativeSPI::transfer(uint16_t* data, size_t len) {
  setBusWidth(true);
  DBG_PRINT("write %zu word(s): ", len);
  for (size_t i = 0; i < len; i++)
    printf("%04X ", data[i]);
  printf("\n");
}

bool NativeSPI::isIdle() {
  return true;
}

void NativeSPI::end() {
  constructed_ = false;
  dmaChan_ = -1;
  DBG_PRINT("deinitialized\n");
}

// ============================================================================
// DMA — simulated no-ops
// ============================================================================
int NativeSPI::addDMAchannel() {
  if (!constructed_) {
    DBG_PRINT("Error: not initialized.\n");
    return -1;
  }
  if (dmaChan_ >= 0) {
    DBG_PRINT("Error: DMA channel already active.\n");
    return -1;
  }
  dmaChan_ = 0;
  DBG_PRINT("DMA channel %d (simulated)\n", dmaChan_);
  return dmaChan_;
}

bool NativeSPI::dmaIdle() {
  return true;
}

void NativeSPI::endDMAchannel() {
  dmaChan_ = -1;
  DBG_PRINT("DMA channel released\n");
}

// ============================================================================
// Private helpers
// ============================================================================
void NativeSPI::setBusWidth(bool is16bit) {
  if (width16_ == is16bit)
    return;
  width16_ = is16bit;
  DBG_PRINT("bus width → %u bits\n", is16bit ? 16 : 8);
}
