// Cross-platform entrypoint smoke test.
//
// The same source runs on both the native (host) and rp2 (Pico) platforms
// without any changes. On native it records operations to trace.json; on rp2
// it drives the real SPI/DMA hardware. The program logic and printed results
// are identical on both (the only platform-dependent value is the actual baud
// rate returned by the hardware).

#include <stdio.h>
#include "SPI.hpp"

int main(void) {
  // SPI0 with DMA enabled.
  SPI spi(spi0, true);

  spi_pins_t pins;
  pins.SCK = 2;   // Valid SPI0 SCK
  pins.MOSI = 3;  // SCK + 1
  uint baud = spi.begin(pins, 75000000);
  printf("begin: baud=%u\n", baud);

  const uint8_t cmd = 0x2A;
  int n8 = spi.transfer8(&cmd, 1);
  printf("transfer8: n=%d\n", n8);

  const uint16_t small[2] = {0x1234, 0x5678};
  int n16 = spi.transfer16(small, 2);  // below DMA threshold -> blocking
  printf("transfer16(blocking): n=%d\n", n16);

  const uint16_t big[16] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666,
                            0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
                            0xDDDD, 0xEEEE, 0xFFFF, 0x0000};
  int n16dma = spi.transfer16(big, 16);  // >= DMA threshold -> DMA
  printf("transfer16(dma): n=%d\n", n16dma);

  // Wait for the DMA transfer to finish (no-op on native, where transfers
  // complete instantly).
  while (spi.isBusy())
    ;

  bool busy = spi.isBusy();
  printf("isBusy: %d\n", busy);

  spi.end();
  printf("done\n");
  return 0;
}