// Source file for SPI implementation within rp2 series.

#include "SPI.hpp"

// Tiny pin check masks
#define SPI0_SCK_MASK ((1u << 2) | (1u << 6))
#define SPI1_SCK_MASK ((1u << 10) | (1u << 14))

// Tiny helper function(s)
void SPI::setWidth(bool is8b) {
  if (_is8b == is8b || _spi == nullptr)
    return;

  // Wait until DMA (if used) and SPI are idle.
  while (isBusy())
    tight_loop_contents();

  spi_hw_t* hw = spi_get_hw(_spi);               // Get current instance
  hw_clear_bits(&hw->cr1, SPI_SSPCR1_SSE_BITS);  // Disable SSP
  hw->cr0 =
      (hw->cr0 & ~0xFu) |
      (is8b ? 0x7u : 0xFu);  // Change DSS only (8-bit = 0x7, 16-bit = 0xF)
  hw_set_bits(&hw->cr1, SPI_SSPCR1_SSE_BITS);  // Re-enable SSP

  _is8b = is8b;  // Update new size
}

SPI::SPI(spi_inst_t* spi, bool use_dma) : _spi(spi) {
  if (!use_dma)
    return;
  // If we're here, DMA setup is requested
  _dma_chan = dma_claim_unused_channel(false);
  if (_dma_chan < 0)
    return;  // Couldn't find any empty channels

  // Get default configuration for DMA and modify for our use
  _dma_config = dma_channel_get_default_config(_dma_chan);
  channel_config_set_transfer_data_size(&_dma_config, DMA_SIZE_16);
  channel_config_set_read_increment(&_dma_config,
                                    true);  // Read src incrementally
  channel_config_set_write_increment(&_dma_config,
                                     false);  // Write dest from fixed address
  channel_config_set_dreq(
      &_dma_config,
      spi_get_dreq(_spi, true));  // DMA waits when SPI buffers are full

  // Setup the dest buffer address
  dma_channel_set_write_addr(_dma_chan, &spi_get_hw(_spi)->dr, false);
  dma_channel_set_config(_dma_chan, &_dma_config,
                         false);  // Set channel to use above config
}

uint SPI::begin(const spi_pins_t pins, const uint baudrate) {
  /* Logic: The valid pin sets are in a pattern -
  When divided by 16, they leave remainders of 2/6 for spi0;
  10/14 for spi1.
  We perform the same operation for current pin, if remainder matches = valid
  */
  const uint8_t s = pins.SCK;
  const bool is_valid =
      s <= 47 &&
      ((_spi == spi0) ? (SPI0_SCK_MASK & (1u << (s & 0x0F)))
                      : (SPI1_SCK_MASK & (1u << (s & 0x0F)))) &&
      pins.MOSI == s + 1 && (pins.MISO == UINT8_MAX || pins.MISO == s - 2);
  // Initialize the pins appropriately
  gpio_set_function(pins.SCK, GPIO_FUNC_SPI);
  gpio_set_function(pins.MOSI, GPIO_FUNC_SPI);
  if (pins.MISO != UINT8_MAX)
    gpio_set_function(pins.MISO, GPIO_FUNC_SPI);
  return (is_valid ? spi_init(_spi, baudrate) : 0);
}

int SPI::transfer8(const uint8_t* data, size_t len, uint8_t* dest) {
  if (_spi == nullptr)
    return 0;
  setWidth(true);
  int result =
      ((dest == nullptr) ? spi_write_blocking(_spi, data, len)
                         : spi_write_read_blocking(_spi, data, dest, len));
  setWidth(false);
  return result;
}

int SPI::transfer16(const uint16_t* data, size_t len, uint16_t* dest) {
  if (_spi == nullptr)
    return 0;
  if (dest)  // User expects data back; only blocking SPI can do this
    return spi_write16_read16_blocking(_spi, data, dest, len);
  if (_dma_chan == -1 ||
      len < DMA_THRESHOLD) {  // DMA is disabled OR data below threshold
    return spi_write16_blocking(_spi, data, len);
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
