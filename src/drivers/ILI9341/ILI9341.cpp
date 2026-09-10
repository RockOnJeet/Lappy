// Implementation of the ILI9341 driver (see ILI9341.hpp for details)
#include "ILI9341.hpp"

#include "hal/platform.hpp"

// ============================================================================
// COMPILE-TIME DEBUG CONFIGURATION
// ============================================================================
#define MODULE_DEBUG \
  0  // Set to 1 for active console prints, 0 to silence completely

#if MODULE_DEBUG
#include <cstdio>
#define DBG_PRINT(...) printf("[ILI9341] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

// ============================================================================
// Construction
// ============================================================================
ILI9341::ILI9341(ISPI& spi, IGPIOPin& dc, IGPIOPin* rst)
    : spi_(spi), dc_(dc), rst_(rst) {
  // Initialize D/C pin as output, default high (data mode)
  dc_.init(IGPIOPin::Mode::Output);
  dc_.set(true);

  // Perform hardware reset if RST is connected
  if (rst_) {
    rst_->init(IGPIOPin::Mode::Output);
    DBG_PRINT("Performing hardware reset.\n");
    rst_->set(true);
    hal::delay_ms(5);
    rst_->set(false);
    hal::delay_ms(20);
    rst_->set(true);
    hal::delay_ms(150);
  }
}

// ============================================================================
// Private helpers
// ============================================================================
void ILI9341::waitIdle() {
  if (dma_)
    while (!spi_.isIdle())
      hal::cpu_yield();
}

inline void ILI9341::writeCmd(uint8_t cmd) {
  waitIdle();
  dc_.set(false);  // D/C low → command
  spi_.transfer(&cmd, 1);
}

inline void ILI9341::writeData(uint8_t data) {
  waitIdle();
  dc_.set(true);  // D/C high → data
  spi_.transfer(&data, 1);
}

// ============================================================================
// IDisplay — init
// ============================================================================
void ILI9341::init(bool use_dma) {
  dma_ = use_dma;
  if (dma_)
    waitIdle();

  DBG_PRINT("Initializing display (DMA=%s).\n", dma_ ? "on" : "off");

  // -----------------------------------------------------------
  // Group 1: Manufacturer Extended & Protection Gateway Setup
  // -----------------------------------------------------------
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

  // -----------------------------------------------------------
  // Group 2: Core Power & VCOM Optimizations
  // -----------------------------------------------------------
  writeCmd(0xC0);  // Power Control 1
  writeData(0x23);

  writeCmd(0xC1);  // Power Control 2
  writeData(0x10);

  writeCmd(0xC5);  // VCOM Control 1
  writeData(0x30);
  writeData(0x30);

  writeCmd(0xC7);  // VCOM Control 2
  writeData(0xB7);

  // -----------------------------------------------------------
  // Group 3: Memory Routing & Formatting
  // -----------------------------------------------------------
  writeCmd(0x36);   // Memory Access Control
  writeData(0x48);  // Portrait

  writeCmd(0x3A);   // Interface Pixel Format
  writeData(0x55);  // RGB565

  // -----------------------------------------------------------
  // Group 4: Panel Drive Timings & Refresh Speeds
  // -----------------------------------------------------------
  writeCmd(0xB1);  // Frame Rate Control
  writeData(0x00);
  writeData(0x1A);

  writeCmd(0xB6);  // Display Function Control
  writeData(0x08);
  writeData(0x82);
  writeData(0x27);

  writeCmd(0xF2);  // 3-Gamma Function Disable
  writeData(0x00);

  writeCmd(0x26);  // Gamma Curve Selected
  writeData(0x01);

  // -----------------------------------------------------------
  // Group 5: Digital Gamma Calibration Curves
  // -----------------------------------------------------------
  writeCmd(0xE0);  // Positive Gamma Correction
  uint8_t posGamma[] = {0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0xA9,
                        0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00};
  dc_.set(true);
  spi_.transfer(posGamma, sizeof(posGamma));

  writeCmd(0xE1);  // Negative Gamma Correction
  uint8_t negGamma[] = {0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56,
                        0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F};
  dc_.set(true);
  spi_.transfer(negGamma, sizeof(negGamma));

  // -----------------------------------------------------------
  // Group 6: Sleep Wakeup and Panel Ignition Execution
  // -----------------------------------------------------------
  writeCmd(0x11);  // Sleep Out
  hal::delay_ms(120);

  writeCmd(0x29);  // Display On
  hal::delay_ms(20);

  DBG_PRINT("Display panel initialization complete.\n");
}

// ============================================================================
// IDisplay — rotation
// ============================================================================
void ILI9341::setRotation(uint8_t rotation) {
  waitIdle();
  rotation &= 0x03;
  writeCmd(0x36);
  switch (rotation) {
    case 0:
      writeData(0x48);
      width_ = ILI9341_WIDTH;
      height_ = ILI9341_HEIGHT;
      break;
    case 1:
      writeData(0x28);
      width_ = ILI9341_HEIGHT;
      height_ = ILI9341_WIDTH;
      break;
    case 2:
      writeData(0x88);
      width_ = ILI9341_WIDTH;
      height_ = ILI9341_HEIGHT;
      break;
    case 3:
      writeData(0xE8);
      width_ = ILI9341_HEIGHT;
      height_ = ILI9341_WIDTH;
      break;
  }
}

uint16_t ILI9341::getWidth() const {
  return width_;
}
uint16_t ILI9341::getHeight() const {
  return height_;
}

// ============================================================================
// IDisplay — address window
// ============================================================================
bool ILI9341::setAddressWindow(int32_t x0, int32_t y0, int32_t w, int32_t h) {
  waitIdle();

  if (x0 < 0 || y0 < 0 || (x0 + w) > width_ || (y0 + h) > height_ || w <= 0 ||
      h <= 0) {
    return false;
  }

  int32_t x1 = x0 + w - 1;
  int32_t y1 = y0 + h - 1;

  // Column Address Set (0x2A)
  writeCmd(0x2A);
  uint8_t colData[] = {
      static_cast<uint8_t>(x0 >> 8), static_cast<uint8_t>(x0 & 0xFF),
      static_cast<uint8_t>(x1 >> 8), static_cast<uint8_t>(x1 & 0xFF)};
  spi_.transfer(colData, sizeof(colData));

  // Page Address Set (0x2B)
  writeCmd(0x2B);
  dc_.set(true);
  uint8_t pageData[] = {
      static_cast<uint8_t>(y0 >> 8), static_cast<uint8_t>(y0 & 0xFF),
      static_cast<uint8_t>(y1 >> 8), static_cast<uint8_t>(y1 & 0xFF)};
  spi_.transfer(pageData, sizeof(pageData));

  // Memory Write (0x2C)
  writeCmd(0x2C);
  dc_.set(true);
  return true;
}

// ============================================================================
// IDisplay — fill
// ============================================================================
void ILI9341::fillScreen(uint16_t color) {
  if (dma_) {
    beginStream(0, 0, width_, height_);
    uint32_t total = static_cast<uint32_t>(width_) * height_;
    uint32_t written = 0;
    while (written < total) {
      size_t cap;
      uint16_t* buf = getActiveBuffer(cap);
      uint32_t toWrite = total - written;
      if (toWrite > cap)
        toWrite = cap;
      for (size_t i = 0; i < toWrite; i++)
        buf[i] = color;
      submitActiveBuffer(toWrite);
      written += toWrite;
    }
    endStream();
  } else {
    setAddressWindow(0, 0, width_, height_);
    dc_.set(true);
    uint32_t total = static_cast<uint32_t>(width_) * height_;
    for (uint32_t i = 0; i < total; i++) {
      spi_.transfer(&color, 1);
    }
  }
}

void ILI9341::clearScreen() {
  fillScreen(0);
}

// ============================================================================
// IDisplay — streaming
// ============================================================================
void ILI9341::beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h) {
  setAddressWindow(x0, y0, w, h);
  dc_.set(true);
  if (dma_)
    spi_.addDMAchannel();
  DBG_PRINT("Stream opened (%dx%d @ %d,%d).\n", w, h, x0, y0);
}

uint16_t* ILI9341::getActiveBuffer(size_t& length) {
  length = kBufPixels;
  return activeBuffer_;
}

void ILI9341::submitActiveBuffer(size_t length) {
  if (length == 0) {
    DBG_PRINT("Skipping empty buffer.\n");
    return;
  }
  spi_.transfer(activeBuffer_, length);
}

void ILI9341::endStream() {
  if (dma_)
    spi_.endDMAchannel();
  DBG_PRINT("Stream closed.\n");
}
