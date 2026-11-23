// Source file for ILI9341 implementation within rp2 series.

#include "ILI9341.hpp"

// Tiny helper function(s)
inline void ILI9341::sendInitCmd(uint8_t cmd,
                                 const uint8_t* data,
                                 uint8_t len) {
  _gpio->set(_pins.DC, false);
  _spi->transfer8(&cmd, 1);

  if (len) {
    _gpio->set(_pins.DC, true);
    _spi->transfer8(data, len);
  }
}

ILI9341::ILI9341(const tft_spi_pins_t pins) : _pins(pins) {
  // Initialize the pins appropriately
  _gpio->init(_pins.CS, IGPIO::Mode::Output);
  _gpio->set(_pins.CS, 1);  // preload HIGH (Inactive)

  _gpio->init(_pins.DC, IGPIO::Mode::Output);
  _gpio->set(_pins.DC, 1);  // idle HIGH (data mode)

  if (_pins.RST != UINT8_MAX) {
    _gpio->init(_pins.RST, IGPIO::Mode::Output);
    _gpio->set(_pins.RST, 1);  // keep out of reset
  }
}

void ILI9341::init() {
  // _spi = spi;
  if (!_spi)
    return;  // Invalid instance provided

  // We have a valid instance; setup the default window
  _win = {0, 0, (_orient & 1) ? ILI9341_HEIGHT : ILI9341_WIDTH,
          (_orient & 1) ? ILI9341_WIDTH : ILI9341_HEIGHT};

  const uint8_t* p = initSeq;

  while (true) {
    uint8_t cmd = *p++;
    uint8_t len = *p++;

    if (cmd == 0x00 && len == 0xFF)
      break;

    uint8_t count = len & 0x7F;
    sendInitCmd(cmd, p, count);
    p += count;

    if (len & 0x80)
      sleep_ms(*p++);
  }
}

uint16_t ILI9341::getWidth() const {
  return _win.width;
}

uint16_t ILI9341::getHeight() const {
  return _win.height;
}

ILI9341::~ILI9341(void) {
  endStream();
  // Clear pin states
  _gpio->deinit(_pins.CS);
  _gpio->deinit(_pins.DC);
  if (_pins.RST != UINT8_MAX)
    _gpio->deinit(_pins.RST);
}