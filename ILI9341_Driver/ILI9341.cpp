// Implementation of the ILI9341 driver (see ILI9341.hpp for details)
#include "ILI9341.hpp"

#include <stdio.h>

// ============================================================================
// COMPILE-TIME DEBUG CONFIGURATION
// ============================================================================
#define MODULE_DEBUG \
  1  // Set to 1 for active console prints, 0 to silence completely

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[ILI9341] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

ILI9341::ILI9341(uint16_t dc, uint16_t rst) {
  // Define pins
  _dcPin = dc;
  _rstPin = rst;
  // Initialize pins
  gpio_init(_dcPin);
  gpio_set_dir(_dcPin, GPIO_OUT);
  gpio_put(_dcPin, 1);  // Set DC high by default (data mode)
  if (_rstPin != (uint16_t)-1) {
    gpio_init(_rstPin);
    gpio_set_dir(_rstPin, GPIO_OUT);

    // Perform a clean physical hardware reset
    gpio_put(_rstPin, 1);
    sleep_ms(5);
    gpio_put(_rstPin, 0);
    sleep_ms(20);
    gpio_put(_rstPin, 1);
    sleep_ms(150);
  }
}

ILI9341::~ILI9341() {
  // Cleanup (if necessary)
  // Note: GPIO pins are not automatically released, so we can leave them as
  // is
}

inline void ILI9341::writeCmd(uint8_t cmd) {
  // Set DC low for command mode
  gpio_put(_dcPin, 0);
  write8(&cmd, 1);  // For single vars, address must be passed
}

inline void ILI9341::writeData(uint8_t data) {
  // Set DC high for data mode
  gpio_put(_dcPin, 1);
  write8(&data, 1);  // For single vars, address must be passed
}

void ILI9341::init() {
  // Initialize the display
  DBG_PRINT("Initializing display on DC pin %d, RST pin %d\n", _dcPin,
            _rstPin == (uint16_t)-1 ? -1 : _rstPin);
  // -------------------------------------------------------------
  // Group 1: Manufacturer Extended & Protection Gateway Setup
  // -------------------------------------------------------------
  writeCmd(0xEF);
  writeData(0x03);
  writeData(0x80);
  writeData(0x02);

  writeCmd(0xCF);
  writeData(0x00);
  writeData(0xC1);
  writeData(0x30);

  writeCmd(0xED);
  writeData(0x64);
  writeData(0x03);
  writeData(0x12);
  writeData(0x81);

  writeCmd(0xE8);
  writeData(0x85);
  writeData(0x00);
  writeData(0x78);

  writeCmd(0xCB);
  writeData(0x39);
  writeData(0x2C);
  writeData(0x00);
  writeData(0x34);
  writeData(0x02);

  writeCmd(0xF7);
  writeData(0x20);

  writeCmd(0xEA);
  writeData(0x00);
  writeData(0x00);

  // -------------------------------------------------------------
  // Group 2: Core Power & VCOM Optimizations
  // -------------------------------------------------------------
  writeCmd(0xC0);   // Power Control 1 (ILI9341_PWCTR1)
  writeData(0x23);  // VRH[5:0]

  writeCmd(0xC1);   // Power Control 2 (ILI9341_PWCTR2)
  writeData(0x10);  // SAP[2:0];BT[3:0]

  writeCmd(0xC5);  // VCOM Control 1 (ILI9341_VMCTR1)
  writeData(0x30);
  writeData(0x30);

  writeCmd(0xC7);  // VCOM Control 2 (ILI9341_VMCTR2)
  writeData(0xB7);

  // -------------------------------------------------------------
  // Group 3: Memory Routing & Formatting
  // -------------------------------------------------------------
  writeCmd(0x36);   // Memory Access Control (MADCTL)
  writeData(0x08);  // Default Native Portrait layout (RGB/BGR filter layout)

  writeCmd(0x3A);   // Interface Pixel Format (PIXFMT)
  writeData(0x55);  // Sets screen data mode natively to 16-bit RGB565

  // -------------------------------------------------------------
  // Group 4: Panel Drive Timings & Refresh Speeds
  // -------------------------------------------------------------
  writeCmd(0xB1);  // Frame Rate Control (ILI9341_FRMCTR1)
  writeData(0x00);
  writeData(0x1A);  // Accelerates internal refresh to a smooth ~100Hz

  writeCmd(0xB6);  // Display Function Control (ILI9341_DFUNCTR)
  writeData(0x08);
  writeData(0x82);
  writeData(0x27);

  writeCmd(0xF2);  // 3Gamma Function Disable
  writeData(0x00);

  writeCmd(0x26);  // Gamma Curve Selected
  writeData(0x01);

  // -------------------------------------------------------------
  // Group 5: Digital Gamma Calibration Curves
  // -------------------------------------------------------------
  writeCmd(0xE0);  // Positive Gamma Correction (GMCTRP1)
  uint8_t posGamma[] = {0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0xA9,
                        0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00};
  gpio_put(_dcPin, 1);  // Put into data mode manually for array burst
  write8(posGamma, sizeof(posGamma));

  writeCmd(0xE1);  // Negative Gamma Correction (GMCTRN1)
  uint8_t negGamma[] = {0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56,
                        0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F};
  gpio_put(_dcPin, 1);  // Put into data mode manually for array burst
  write8(negGamma, sizeof(negGamma));

  // -------------------------------------------------------------
  // Group 6: Sleep Wakeup and Panel Ignition Execution
  // -------------------------------------------------------------
  writeCmd(0x11);  // Sleep Out (ILI9341_SLPOUT)
  sleep_ms(120);   // Obligatory internal charge-pump stabilizer delay

  writeCmd(0x29);  // Display On (ILI9341_DISPON)
  sleep_ms(20);    // Final physical latch stabilization window

  DBG_PRINT("Display panel initialization completely active.\n");
}

bool ILI9341::setAddressWindow(int16_t x0, int16_t y0, int16_t w, int16_t h) {
  // Sanity check: Ensure the window falls within the physical screen boundaries
  if (x0 < 0 || y0 < 0 || (x0 + w) > ILI9341_WIDTH ||
      (y0 + h) > ILI9341_HEIGHT || w <= 0 || h <= 0) {
    return false;
  }

  int16_t x1 = x0 + w - 1;
  int16_t y1 = y0 + h - 1;

  // 1. Column Address Set (0x2A): Pass X start and X end coordinates
  writeCmd(0x2A);
  gpio_put(_dcPin, 1);  // Assert Data Mode
  uint8_t colData[] = {
      (uint8_t)(x0 >> 8),
      (uint8_t)(x0 & 0xFF),                     // X-Start (High Byte, Low Byte)
      (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF)  // X-End (High Byte, Low Byte)
  };
  write8(colData, sizeof(colData));  // write8 automatically switches hardware
                                     // to 8-bit bus mode

  // 2. Page Address Set (0x2B): Pass Y start and Y end coordinates
  writeCmd(0x2B);
  gpio_put(_dcPin, 1);  // Assert Data Mode
  uint8_t pageData[] = {
      (uint8_t)(y0 >> 8),
      (uint8_t)(y0 & 0xFF),                     // Y-Start (High Byte, Low Byte)
      (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF)  // Y-End (High Byte, Low Byte)
  };
  write8(pageData, sizeof(pageData));

  // 3. Memory Write (0x2C): Tell the display RAM controller to expect pixel
  // stream
  writeCmd(0x2C);

  return true;
}

void ILI9341::fillScreen(uint16_t color) {
  DBG_PRINT("Filling entire screen with color 0x%04X\n", color);

  // Capture the precise hardware timestamp right before setup begins
  absolute_time_t startTime = get_absolute_time();

  // 1. Map full-frame coordinates using the optimized bounding box utility
  setAddressWindow(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT);

  // 2. Set DC high for data mode before starting any pixel stream
  gpio_put(_dcPin, 1);

  // 3. Select execution path based on DMA availability
  if (g_dma_channel >= 0) {
    // --- HIGH SPEED ASYNC DMA PIPELINE ---
    DBG_PRINT(
        "Using DMA channel %d for high-speed asynchronous pixel streaming\n",
        g_dma_channel);
    // Allocate Ping-Pong Double Buffers on stack frame execution context
    uint16_t bufferPing[ILI9341_WIDTH];
    uint16_t bufferPong[ILI9341_WIDTH];

    for (int i = 0; i < ILI9341_WIDTH; i++) {
      bufferPing[i] = color;
    }

    uint16_t* currentDmaBuffer = bufferPing;
    uint16_t* currentCpuBuffer = bufferPong;

    for (int line = 0; line < ILI9341_HEIGHT; line++) {
      // Stream current buffer to SPI pipeline in 16-bit mode asynchronously
      write16_dma_async(currentDmaBuffer, ILI9341_WIDTH);

      // CPU prepares the next buffer concurrently (while DMA is transferring)
      if (line < (ILI9341_HEIGHT - 1)) {
        for (int i = 0; i < ILI9341_WIDTH; i++) {
          currentCpuBuffer[i] = color;
        }
      }

      // Block until the active DMA transaction finishes cleanly
      wait16_dma();

      // Swap the pointers for the next line iteration loop
      uint16_t* temp = currentDmaBuffer;
      currentDmaBuffer = currentCpuBuffer;
      currentCpuBuffer = temp;
    }
  } else {
    // --- SYNCHRONOUS CPU FALLBACK PIPELINE ---
    DBG_PRINT(
        "DMA channel unavailable, falling back to synchronous CPU pixel "
        "streaming\n");
    // Allocate a single line buffer to limit memory allocation sizes
    uint16_t lineBuffer[ILI9341_WIDTH];
    for (int i = 0; i < ILI9341_WIDTH; i++) {
      lineBuffer[i] = color;
    }

    // Sequentially push line data chunks (write16 blocks internally until done)
    for (int line = 0; line < ILI9341_HEIGHT; line++) {
      write16(lineBuffer, ILI9341_WIDTH);
    }
  }

  // Capture the final timestamp after all data has finished transmitting
  absolute_time_t endTime = get_absolute_time();

  // Compute the microsecond diff and translate to milliseconds
  int64_t durationUs = absolute_time_diff_us(startTime, endTime);
  float durationMs = (float)durationUs / 1000.0f;

  // Output performance breakdown metrics to console
  printf("[ILI9341-PERF] fillScreen took: %.3f ms (%lld µs)\n", durationMs,
         durationUs);
}

void ILI9341::drawPixel(int16_t x, int16_t y, uint16_t color) {
  DBG_PRINT("Drawing pixel at (%d, %d) with color 0x%04X\n", x, y, color);
  // Bounding box validation check
  if (x < 0 || y < 0 || x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT) {
    return;
  }

  // 1. Set a microscopic 1x1 address window at the coordinate target
  // (setAddressWindow sends 8-bit commands and automatically flips back to
  // 8-bit bus width)
  if (!setAddressWindow(x, y, 1, 1)) {
    return;
  }

  // 2. Drive the DC line HIGH to signal that raw color payload bytes are
  // incoming
  gpio_put(_dcPin, 1);

  // 3. Write a single 16-bit color value.
  // write16 automatically shifts the RP2350 SPI peripheral to 16-bit wide mode.
  write16(&color, 1);
  DBG_PRINT("Pixel drawn at (%d, %d) with color 0x%04X\n", x, y, color);
}

void ILI9341::pushColors(uint16_t* data, int16_t len, bool first) {
  DBG_PRINT("Pushing %d colors to display (first=%d)\n", len, first);
  // If this is the start of a new stream, reset the display address frame
  // tracking. We assume the user has already bound an active target window via
  // setAddressWindow().
  if (first) {
    // Re-issue Memory Write (0x2C) to tell the display controller to reset its
    // internal RAM write pointer back to the window's starting index.
    writeCmd(0x2C);
  }

  // Ensure DC pin is high for streaming raw pixel data
  gpio_put(_dcPin, 1);

  // Check if DMA is active and available for this instance
  if (g_dma_channel >= 0) {
    DBG_PRINT("Using DMA channel %d for asynchronous pixel streaming\n",
              g_dma_channel);
    // Utilize your fast asynchronous DMA block channel wrapper.
    // This will return immediately while the RP2350 hardware moves the bytes.
    write16_dma_async(data, (size_t)len);

    // Block until this specific batch clears before leaving the function
    // context, protecting the upstream data buffer from being overwritten
    // prematurely.
    wait16_dma();
  } else {
    DBG_PRINT(
        "DMA channel unavailable, falling back to synchronous CPU pixel "
        "streaming\n");
    // Fall back to the synchronous CPU pipeline if DMA isn't configured
    write16(data, (size_t)len);
  }
}