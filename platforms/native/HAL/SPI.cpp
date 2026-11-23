#include "SPI.hpp"

#include <cstdio>

// ----------------- TODO -----------------
// Make this code output to a routable setup, so that it can be used in unit
// tests without polluting stdout.
// ----------------------------------------

#define MODULE_DEBUG 1

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[SPI] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

// ============================================================================
// Construction
// ============================================================================
SPI::SPI(uint16_t sck, uint16_t mosi) {
  DBG_PRINT("constructed (sck=%u, mosi=%u)\n", sck, mosi);
}

// ============================================================================
// ISPI overrides
// ============================================================================
uint32_t SPI::begin(uint32_t baudrate) {
  DBG_PRINT("begin @ %.1f MHz (simulated)\n", (float)(baudrate / 1'000'000.0));
  return baudrate;
}

void SPI::transfer(uint8_t* data, size_t len) {
  DBG_PRINT("write %zu byte(s): ", len);
  for (size_t i = 0; i < len; i++)
    printf("%02X ", data[i]);
  printf("\n");
}

void SPI::transfer(uint16_t* data, size_t len) {
  DBG_PRINT("write %zu word(s): ", len);
  for (size_t i = 0; i < len; i++)
    printf("%04X ", data[i]);
  printf("\n");
}

bool SPI::isIdle() {
  DBG_PRINT("checked state\n");
  return true;
}

void SPI::end() {
  DBG_PRINT("deinitialized\n");
}
