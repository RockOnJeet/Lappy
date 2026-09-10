#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "hal/IGPIOPin.hpp"
#include "hal/ISPI.hpp"
#include "hal/platform.hpp"

#include "AnimatedGIF.h"
#include "drivers/ILI9341/ILI9341.hpp"
#include "meow.h"

// ============================================================================
// Platform-specific includes (composition root ONLY)
// ============================================================================
#if !NATIVE_BUILD
#include "platform/pico/PicoGPIOPin.hpp"
#include "platform/pico/PicoSPI.hpp"
#else
#include "platform/native/NativeGPIOPin.hpp"
#include "platform/native/NativeSPI.hpp"
#endif

// ============================================================================
// Pin configuration
// ============================================================================
static constexpr uint32_t PIN_SCK = 2;
static constexpr uint32_t PIN_MOSI = 3;
static constexpr uint32_t PIN_CS = 1;
static constexpr uint32_t PIN_DC = 20;
static constexpr uint32_t BAUD_RATE_HZ = 75'000'000;

// ============================================================================
// Platform type aliases (composition root — the ONLY platform-aware code)
// ============================================================================
#if !NATIVE_BUILD
using PlatformSPI = PicoSPI;
using PlatformGPIOPin = PicoGPIOPin;
#else
using PlatformSPI = NativeSPI;
using PlatformGPIOPin = NativeGPIOPin;
#endif

// ============================================================================
// Hardware objects — constructor injection
// ============================================================================
PlatformSPI spi(PIN_SCK, PIN_MOSI);
PlatformGPIOPin csPin(PIN_CS);
PlatformGPIOPin dcPin(PIN_DC);

// Display driver depends only on interfaces
ILI9341 lcd(spi, dcPin);
AnimatedGIF gif;

// ============================================================================
// GIF helpers
// ============================================================================
#define GIF_NAME meow

static uint8_t* pTurboBuffer = nullptr;
static uint16_t* pFrameBuffer = nullptr;
static int iXOff;
static int iYOff;

void GIFDraw(GIFDRAW* pDraw) {
  if (pDraw->y == 0) {
    lcd.setAddressWindow(iXOff + pDraw->iX, iYOff + pDraw->iY, pDraw->iWidth,
                         pDraw->iHeight);
  }
  size_t length;
  memcpy(lcd.getActiveBuffer(length), pDraw->pPixels,
         pDraw->iWidth * sizeof(uint16_t));
  lcd.submitActiveBuffer(pDraw->iWidth);
}

// ============================================================================
// Entry point
// ============================================================================
int main() {
#if !NATIVE_BUILD
  stdio_init_all();
#endif
  hal::delay_ms(3000);

  // Initialize SPI bus
  uint baud = spi.begin(BAUD_RATE_HZ);

  // Initialize CS pin (directly driven, not passed to ILI9341)
  csPin.init();
  csPin.set(false);  // CS low (active)

  // Initialize display with DMA enabled
  lcd.init(true);

  // Clear screen and show SPI clock info
  lcd.clearScreen();
  hal::delay_ms(250);
  printf("[MAIN] SPI Clock running at %u MHz\n", baud / 1000000);

  // Open GIF stream
  lcd.beginStream(0, 0, lcd.getWidth(), lcd.getHeight());
  gif.begin(GIF_PALETTE_RGB565_LE);
  printf("[MAIN] Starting GIF playback...\n");

  if (!gif.open((uint8_t*)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
    printf("[MAIN] Error opening GIF file: %d\n", gif.getLastError());
    while (true)
      hal::cpu_yield();
  }

  const int canvasW = gif.getCanvasWidth();
  const int canvasH = gif.getCanvasHeight();
  printf("[MAIN] Canvas size = %d x %d\n", canvasW, canvasH);

  GIFINFO gi;
  if (gif.getInfo(&gi)) {
    printf("  frames: %d  duration: %d ms  delay: %d–%d ms\n", gi.iFrameCount,
           gi.iDuration, gi.iMinDelay, gi.iMaxDelay);
  }

  // Allocate GIF decode buffers
  pTurboBuffer = (uint8_t*)malloc(TURBO_BUFFER_SIZE + (canvasW * canvasH));
  pFrameBuffer = (uint16_t*)malloc(canvasW * canvasH * sizeof(uint16_t));
  if (!pTurboBuffer || !pFrameBuffer) {
    printf("[MAIN] Failed to allocate GIF buffers.\n");
    gif.close();
    free(pTurboBuffer);
    free(pFrameBuffer);
    while (true)
      hal::cpu_yield();
  }

  gif.setDrawType(GIF_DRAW_COOKED);
  gif.setFrameBuf(pFrameBuffer);
  gif.setTurboBuf(pTurboBuffer);

  // Center the GIF on screen
  lcd.clearScreen();
  lcd.setRotation(0);

  iXOff = (lcd.getWidth() - canvasW) / 2;
  if (iXOff < 0)
    iXOff = 0;
  iYOff = (lcd.getHeight() - canvasH) / 2;
  if (iYOff < 0)
    iYOff = 0;

  // Play loop
  for (;;) {
    while (gif.playFrame(true, nullptr))
      hal::cpu_yield();
    gif.reset();
  }

  gif.close();
  free(pTurboBuffer);
  free(pFrameBuffer);
  lcd.endStream();

  printf("[MAIN] Loop terminated.\n");
  while (true)
    hal::cpu_yield();
}
