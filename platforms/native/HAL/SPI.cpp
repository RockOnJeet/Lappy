// Source file for SPI implementation within native as stubs.

#include "SPI.hpp"

// Tiny helper function(s)
void SPI::setWidth(bool is8b) {
  if (_is8b == is8b || _spi == nullptr)
    return;
  // TODO: Log the size change
  _is8b = is8b;  // Update new size
}

SPI::SPI(spi_inst_t* spi, bool use_dma) : _spi(spi) {
  if (!use_dma)
    return;
  // If we're here, DMA setup is requested
  // TODO: Log DMA channel init request
  _dma_chan = 0;  // Simulate fake channel 0 creation
}

uint SPI::begin(const spi_pins_t pins, const uint baudrate) {
  // TODO: Log provided pins and requested baudrate
  return baudrate;
}

int SPI::transfer8(const uint8_t* data, size_t len, uint8_t* dest) {
  if (_spi == nullptr)
    return 0;
  setWidth(true);
  if (dest)  // User expects data back; only blocking SPI can do this
    ;        // TODO: Log read+write operation
  else
    ;  // TODO: Log write only op
  setWidth(false);
  return len;
}

int SPI::transfer16(const uint16_t* data, size_t len, uint16_t* dest) {
  if (_spi == nullptr)
    return 0;
  if (dest) {  // User expects data back; only blocking SPI can do this
    // TODO: Log read+write operation
    return len;
  }
  if (_dma_chan == -1 ||
      len < DMA_THRESHOLD) {  // DMA is disabled OR data below threshold
                              // TODO: Log write only op
    return len;
  } else {
    // TODO: Log DMA call
    return len;
  }
}

bool SPI::isBusy() const {
  // Check if DMA (if used) and SPI are idle.
  // TODO: Log status check
  return false;
}

SPI::~SPI() {
  // Performs cleanup of SPI and DMA (if configured)
  if (_dma_chan >= 0) {  // Valid DMA channel exists
    // TODO: Log channel cleanup request
    _dma_chan = -1;  // Reset interface
  }
  if (_spi) {  // Valid SPI channel exists
    // TODO: Log spi cleanup request
    _spi = nullptr;  // Reset interface
  }
}
