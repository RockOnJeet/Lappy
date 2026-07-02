#include <stdio.h>
#include "AnimatedGIF/src/AnimatedGIF.h"
// #include "AnimatedGIF/test_images/thisisfine_240x179.h"
#include "ILI9341_Driver/ILI9341.hpp"
#include "SPI/SPI.h"
#include "meow.h"
#include "pico/stdlib.h"

#define BAUD_RATE_MHZ 75
#define PIN_SCK       2
#define PIN_MOSI      3
#define PIN_CS        1
#define PIN_DC        20

AnimatedGIF gif;
ILI9341 lcd(PIN_DC);

#define GIF_NAME meow

static uint8_t* pTurboBuffer = nullptr;
static uint16_t* pFrameBuffer = nullptr;

static int iXOff;
static int iYOff;

void GIFDraw(GIFDRAW* pDraw) {
  if (pDraw->y == 0) {
    lcd.beginStream(iXOff + pDraw->iX, iYOff + pDraw->iY, pDraw->iWidth,
                    pDraw->iHeight);
  }

  lcd.setExternalBuffer(reinterpret_cast<uint16_t*>(pDraw->pPixels),
                        pDraw->iWidth);

  lcd.submitActiveBuffer(pDraw->iWidth);

  // lcd.clearExternalBuffer();

  if (pDraw->y == (pDraw->iHeight - 1))
    lcd.endStream();
}

int main() {
  stdio_init_all();
  sleep_ms(3000);

  uint baud =
      init(BAUD_RATE_MHZ * 1000 * 1000, PIN_SCK, PIN_MOSI, PIN_CS, true);
  lcd.init(true);
  // lcd.setRotation(1);  // Set to landscape orientation

  // Clear the screen background completely to black before drawing the centered
  // GIF
  lcd.clearScreen();  // Clear the screen to black before drawing the GIF
  sleep_ms(250);
  printf("[MAIN] SPI Clock running at %u MHz\n", baud / 1000000);

  gif.begin(GIF_PALETTE_RGB565_LE);
  printf("[MAIN] Starting GIF playback...\n");

  if (!gif.open((uint8_t*)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
    printf("[MAIN] Error opening GIF file: %d\n", gif.getLastError());
    while (true) {
      tight_loop_contents();
    }
  }
  // GIF opened successfully, retrieve GIF info
  const int canvasW = gif.getCanvasWidth();
  const int canvasH = gif.getCanvasHeight();
  printf("[MAIN] Successfully opened GIF; Canvas size = %d x %d\n", canvasW,
         canvasH);
  GIFINFO gi;
  if (gif.getInfo(&gi)) {
    printf("frame count: %d\n", gi.iFrameCount);
    printf("duration: %d ms\n", gi.iDuration);
    printf("max delay: %d ms\n", gi.iMaxDelay);
    printf("min delay: %d ms\n", gi.iMinDelay);
  }

  pTurboBuffer = (uint8_t*)malloc(TURBO_BUFFER_SIZE + (canvasW * canvasH));
  pFrameBuffer = (uint16_t*)malloc(canvasW * canvasH * sizeof(uint16_t));

  if (!pTurboBuffer || !pFrameBuffer) {
    printf("[MAIN] Failed to allocate GIF buffers.\n");
    gif.close();

    free(pTurboBuffer);
    free(pFrameBuffer);

    while (true) {
      tight_loop_contents();
    }
  }

  gif.setDrawType(GIF_DRAW_COOKED);
  gif.setFrameBuf(pFrameBuffer);
  gif.setTurboBuf(pTurboBuffer);

  uint8_t rotation = 0;
  lcd.clearScreen();  // Clear the screen to black before drawing the GIF
  lcd.setRotation(rotation);
  rotation--;

  iXOff = (lcd.getWidth() - canvasW) / 2;
  iXOff = MAX(iXOff, 0);

  iYOff = (lcd.getHeight() - canvasH) / 2;
  iYOff = MAX(iYOff, 0);
  for (;;) {
    while (gif.playFrame(true, nullptr)) {
      tight_loop_contents();
    }
    // GIF playback finished, reset to the beginning for looping
    gif.reset();
  }

  printf("[MAIN] Loop terminated. Entering infinite idle loop.\n");
  while (true) {
    tight_loop_contents();
  }
}