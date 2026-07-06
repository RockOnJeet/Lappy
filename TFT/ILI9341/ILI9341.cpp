// Implementation of the ILI9341 driver (see ILI9341.hpp for details)
#include "ILI9341.hpp"

// ============================================================================
// COMPILE-TIME DEBUG CONFIGURATION
// ============================================================================
#define MODULE_DEBUG \
  0  // Set to 1 for active console prints, 0 to silence completely

#if MODULE_DEBUG
#include <stdio.h>
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
    DBG_PRINT("Performing hardware reset on RST pin %d\n", _rstPin);
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
  // Wait for any ongoing DMA transfers to complete before sending a command
  if (_use_dma)
    while (!_spi->spi_idle())
      tight_loop_contents();

  // Set DC low for command mode
  gpio_put(_dcPin, 0);
  _spi->transfer(&cmd, 1);  // For single vars, address must be passed
}

inline void ILI9341::writeData(uint8_t data) {
  // Wait for any ongoing DMA transfers to complete before sending data
  if (_use_dma)
    while (!_spi->spi_idle())
      tight_loop_contents();

  // Set DC high for data mode
  gpio_put(_dcPin, 1);
  _spi->transfer(&data, 1);  // For single vars, address must be passed
}

void ILI9341::init(SPI* spi, bool use_dma) {
  _spi = spi;  // Store the SPI instance for later use
  _use_dma = use_dma;
  if (_use_dma)
    while (!_spi->spi_idle())
      tight_loop_contents();

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
  writeData(0x48);  // Default Native Portrait layout (RGB/BGR filter layout)

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
  _spi->transfer(posGamma, sizeof(posGamma));

  writeCmd(0xE1);  // Negative Gamma Correction (GMCTRN1)
  uint8_t negGamma[] = {0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56,
                        0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F};
  gpio_put(_dcPin, 1);  // Put into data mode manually for array burst
  _spi->transfer(negGamma, sizeof(negGamma));

  // -------------------------------------------------------------
  // Group 6: Sleep Wakeup and Panel Ignition Execution
  // -------------------------------------------------------------
  writeCmd(0x11);  // Sleep Out (ILI9341_SLPOUT)
  sleep_ms(120);   // Obligatory internal charge-pump stabilizer delay

  writeCmd(0x29);  // Display On (ILI9341_DISPON)
  sleep_ms(20);    // Final physical latch stabilization window

  DBG_PRINT("Display panel initialization completely active.\n");

  // Setup buffers to DMA (if used)
  if (_use_dma) {
    DBG_PRINT("DMA mode enabled for pixel transfers.\n");
  }
}

void ILI9341::setRotation(uint8_t rotation) {
  if (_use_dma)
    while (!_spi->spi_idle())
      tight_loop_contents();

  rotation &= 0x03;  // Ensure rotation is within 0-3 range
  writeCmd(0x36);    // Memory Access Control (MADCTL)
  switch (rotation) {
    case 0:
      writeData(0x48);  // Portrait
      _width = ILI9341_WIDTH;
      _height = ILI9341_HEIGHT;
      break;
    case 1:
      writeData(0x28);  // Landscape
      _width = ILI9341_HEIGHT;
      _height = ILI9341_WIDTH;
      break;
    case 2:
      writeData(0x88);  // Inverted Portrait
      _width = ILI9341_WIDTH;
      _height = ILI9341_HEIGHT;
      break;
    case 3:
      writeData(0xE8);  // Inverted Landscape
      _width = ILI9341_HEIGHT;
      _height = ILI9341_WIDTH;
      break;
  }
}

bool ILI9341::setAddressWindow(int32_t x0, int32_t y0, int32_t w, int32_t h) {
  if (_use_dma)
    while (!_spi->spi_idle())
      tight_loop_contents();

  // Sanity check: Ensure the window falls within the physical screen boundaries
  if (x0 < 0 || y0 < 0 || (x0 + w) > _width || (y0 + h) > _height || w <= 0 ||
      h <= 0) {
    return false;
  }

  int32_t x1 = x0 + w - 1;
  int32_t y1 = y0 + h - 1;

  // 1. Column Address Set (0x2A): Pass X start and X end coordinates
  writeCmd(0x2A);
  uint8_t colData[] = {
      (uint8_t)(x0 >> 8),
      (uint8_t)(x0 & 0xFF),                     // X-Start (High Byte, Low Byte)
      (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF)  // X-End (High Byte, Low Byte)
  };
  _spi->transfer(colData,
                 sizeof(colData));  // _spi->transfer automatically switches
  // hardware to 8-bit bus mode

  // 2. Page Address Set (0x2B): Pass Y start and Y end coordinates
  writeCmd(0x2B);
  gpio_put(_dcPin, 1);  // Assert Data Mode
  uint8_t pageData[] = {
      (uint8_t)(y0 >> 8),
      (uint8_t)(y0 & 0xFF),                     // Y-Start (High Byte, Low Byte)
      (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF)  // Y-End (High Byte, Low Byte)
  };
  _spi->transfer(pageData, sizeof(pageData));

  // 3. Memory Write (0x2C): Tell the display RAM controller to expect pixel
  // stream
  writeCmd(0x2C);

  gpio_put(_dcPin, 1);  // Assert Data Mode
  return true;
}

void ILI9341::fillScreen(uint16_t color) {
  if (_use_dma) {
    DBG_PRINT("Filling screen with color 0x%04X using DMA streaming.\n", color);

    // 1. Initialize the display window for the full screen area
    beginStream(0, 0, _width, _height);

    uint32_t total_pixels = (uint32_t)_width * (uint32_t)_height;
    uint32_t pixels_written = 0;

    // 2. Loop through the streaming pipeline exactly like an external renderer
    // would
    while (pixels_written < total_pixels) {
      size_t capacity;
      uint16_t* buf = getActiveBuffer(capacity);

      // Calculate how many pixels we can actually fill in this pass
      uint32_t to_write = total_pixels - pixels_written;
      if (to_write > capacity) {
        to_write = capacity;
      }

      // Fill the active ping-pong buffer with the solid color
      // (A simple memset-style loop or std::fill)
      for (size_t i = 0; i < to_write; i++) {
        buf[i] = color;
      }

      // 3. Submit the buffer (handles ping-ponging and DMA waits internally!)
      submitActiveBuffer(to_write);
      pixels_written += to_write;
    }

    // 4. Cleanly close out the stream session
    endStream();
  } else {
    DBG_PRINT("Filling screen with color 0x%04X using direct writes.\n", color);
    // Directly fill the screen without DMA
    setAddressWindow(0, 0, _width, _height);
    gpio_put(_dcPin, 1);  // Set DC high for data mode
    uint32_t total_pixels = (uint32_t)_width * (uint32_t)_height;
    for (uint32_t i = 0; i < total_pixels; i++) {
      _spi->transfer(&color, 1);  // Write each pixel directly
    }
  }
}

void ILI9341::beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h) {
  // Open the SPI bus for communication with the display
  // This function can be used to prepare the SPI bus for subsequent operations
  setAddressWindow(x0, y0, w, h);  // Set the address window for pixel data
  gpio_put(_dcPin, 1);             // Set DC high for data mode
  if (_use_dma) {
    _spi->addDMAchannel();  // Initialize DMA for SPI transfers
  }
  DBG_PRINT("SPI bus opened for communication with display.\n");
}

uint16_t* ILI9341::getActiveBuffer(size_t& length) {
  length =
      ILI9341_WIDTH * ILI9341_HEIGHT;  // Return the size of the active buffer
  DBG_PRINT("Returning pointer to active buffer %s\n",
            (_activeBuffer == _buffer_A)   ? "A"
            : (_activeBuffer == _buffer_B) ? "B"
                                           : "External");
  return _activeBuffer;  // Return pointer to the currently active buffer
}

void ILI9341::submitActiveBuffer(size_t length) {
  // Submit the currently active buffer to the display for rendering
  // Assumes that the address window has already been set appropriately
  if (length == 0) {
    DBG_PRINT("No data to submit; skipping buffer submission.\n");
    // _activeBuffer = (_activeBuffer == _buffer_A) ? _buffer_B : _buffer_A;
    return;
  }
  DBG_PRINT("Submitting active buffer %s for rendering.\n",
            (_activeBuffer == _buffer_A)   ? "A"
            : (_activeBuffer == _buffer_B) ? "B"
                                           : "External");
  if (_use_dma) {
    _spi->transfer(_activeBuffer,
                   length);  // Queue the active buffer for DMA transfer
  } else {
    _spi->transfer(_activeBuffer,
                   length);  // Directly write the active buffer to the display
  }
}

void ILI9341::endStream() {
  // Close the SPI bus after communication with the display
  // This function can be used to finalize the SPI bus state after operations
  if (_use_dma) {
    _spi->endDMAchannel();  // Ensure all DMA transfers are complete before
                            // closing
  }
  DBG_PRINT("SPI bus closed.\n");
}