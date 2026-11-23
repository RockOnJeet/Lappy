// Source file for SPI implementation within native as stubs.

#include "SPI.hpp"

// Tiny helper function(s)
void SPI::setWidth(bool is8b) {
  if (_is8b == is8b || _spi == nullptr)
    return;
  set_bus_width(_spi, is8b);
  _is8b = is8b;  // Update new size
}

SPI::SPI(spi_inst_t* spi, bool use_dma) : _spi(spi) {
  if (!use_dma)
    return;
  // If we're here, DMA setup is requested
  _dma_chan = dma_claim_unused_channel(false);
  if (_dma_chan < 0)
    return;  // Couldn't find any empty channels
}

uint SPI::begin(const spi_pins_t pins, const uint baudrate) {
  spi_init(_spi, baudrate);
  return baudrate;
}

int SPI::transfer8(const uint8_t* data, size_t len, uint8_t* dest) {
  if (_spi == nullptr)
    return 0;
  setWidth(true);
  if (dest)  // User expects data back; only blocking SPI can do this
    spi_write_read_blocking(_spi, data, dest, len);
  else
    spi_write_blocking(_spi, data, len);
  setWidth(false);
  return len;
}

int SPI::transfer16(const uint16_t* data, size_t len, uint16_t* dest) {
  if (_spi == nullptr)
    return 0;
  if (dest) {  // User expects data back; only blocking SPI can do this
    spi_write16_read16_blocking(_spi, data, dest, len);
    return len;
  }
  if (_dma_chan == -1 ||
      len < DMA_THRESHOLD) {  // DMA is disabled OR data below threshold
    spi_write16_blocking(_spi, data, len);
    return len;
  } else {
    dma_channel_transfer_from_buffer_now(_dma_chan, data, len);
    return len;
  }
}

bool SPI::isBusy() const {
  // Check if DMA (if used) and SPI are idle.
  return ((_dma_chan >= 0 && dma_channel_is_busy(_dma_chan)) ||
          spi_is_busy(_spi));
}

void SPI::end(void) {
  // Performs cleanup of SPI and DMA (if configured)
  if (_dma_chan >= 0) {  // Valid DMA channel exists
    dma_channel_abort(_dma_chan);
    dma_channel_cleanup(_dma_chan);
    dma_channel_unclaim(_dma_chan);
    _dma_chan = -1;  // Reset interface
  }
  if (_spi) {  // Valid SPI channel exists
    spi_deinit(_spi);
    _spi = nullptr;  // Reset interface
  }
}

SPI::~SPI() {
  end();
}
