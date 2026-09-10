#include "PicoSPI.hpp"

#include <hardware/dma.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <cstdio>

// ============================================================================
// Compile-time module debug
// ============================================================================
#define MODULE_DEBUG 0

#if MODULE_DEBUG
#define DBG_PRINT(...) printf("[PicoSPI] " __VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#endif

// ============================================================================
// Pin → SPI instance mapping
// ============================================================================
void* PicoSPI::resolveInstance(uint16_t sck, uint16_t mosi) {
  spi_inst_t* inst = nullptr;

  if (sck == 2 && mosi == 3) {
    inst = spi0;
  } else if (sck == 26 && mosi == 27) {
    inst = spi1;
  } else {
    DBG_PRINT("Warning: non-standard SPI pins; HW SPI may not work.\n");
    return nullptr;
  }

  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(mosi, GPIO_FUNC_SPI);
  return inst;
}

// ============================================================================
// Construction
// ============================================================================
PicoSPI::PicoSPI(uint16_t sck, uint16_t mosi) {
  inst_ = resolveInstance(sck, mosi);
}

// ============================================================================
// ISPI overrides
// ============================================================================
uint32_t PicoSPI::begin(uint32_t baudrate) {
  if (!inst_) {
    DBG_PRINT("Error: invalid pins, cannot init SPI.\n");
    return 0;
  }
  DBG_PRINT("init @ %.1f MHz\n", (float)(baudrate / 1'000'000.0));
  return spi_init(static_cast<spi_inst_t*>(inst_), baudrate);
}

void PicoSPI::transfer(uint8_t* data, size_t len) {
  setBusWidth(false);
  DBG_PRINT("write %zu byte(s)\n", len);
  spi_write_blocking(static_cast<spi_inst_t*>(inst_), data, len);
}

void PicoSPI::transfer(uint16_t* data, size_t len) {
  setBusWidth(true);
  DBG_PRINT("write %zu word(s)\n", len);
  if (dmaChan_ < 0) {
    spi_write16_blocking(static_cast<spi_inst_t*>(inst_), data, len);
  } else {
    while (!dmaIdle())
      tight_loop_contents();
    dma_channel_transfer_from_buffer_now(dmaChan_, data, len);
  }
}

bool PicoSPI::isIdle() {
  if (!inst_)
    return true;
  if (dmaChan_ >= 0)
    return !dma_channel_is_busy(dmaChan_) &&
           !spi_is_busy(static_cast<spi_inst_t*>(inst_));
  return !spi_is_busy(static_cast<spi_inst_t*>(inst_));
}

void PicoSPI::end() {
  while (!isIdle())
    tight_loop_contents();
  if (dmaChan_ >= 0)
    endDMAchannel();
  spi_deinit(static_cast<spi_inst_t*>(inst_));
  inst_ = nullptr;
  DBG_PRINT("deinitialized\n");
}

// ============================================================================
// DMA
// ============================================================================
int PicoSPI::addDMAchannel() {
  if (!inst_) {
    DBG_PRINT("Error: SPI not initialized.\n");
    return -1;
  }
  if (dmaChan_ >= 0) {
    DBG_PRINT("Error: DMA channel already active.\n");
    return -1;
  }

  dmaChan_ = dma_claim_unused_channel(false);
  if (dmaChan_ < 0) {
    DBG_PRINT("Error: no free DMA channels.\n");
    return -1;
  }

  dma_channel_config cfg = dma_channel_get_default_config(dmaChan_);
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg, true);
  channel_config_set_write_increment(&cfg, false);
  channel_config_set_dreq(&cfg,
                          spi_get_dreq(static_cast<spi_inst_t*>(inst_), true));
  dma_channel_set_write_addr(
      dmaChan_, &spi_get_hw(static_cast<spi_inst_t*>(inst_))->dr, false);
  dma_channel_set_config(dmaChan_, &cfg, false);

  DBG_PRINT("DMA channel %d claimed.\n", dmaChan_);
  return dmaChan_;
}

bool PicoSPI::dmaIdle() {
  if (dmaChan_ < 0)
    return true;
  return !dma_channel_is_busy(dmaChan_);
}

void PicoSPI::endDMAchannel() {
  while (!dmaIdle())
    tight_loop_contents();
  if (dmaChan_ >= 0) {
    dma_channel_abort(dmaChan_);
    dma_channel_unclaim(dmaChan_);
    DBG_PRINT("DMA channel %d released.\n", dmaChan_);
    dmaChan_ = -1;
  }
}

// ============================================================================
// Private helpers
// ============================================================================
void PicoSPI::setBusWidth(bool is16bit) {
  if (width16_ == is16bit)
    return;
  while (!isIdle())
    tight_loop_contents();
  width16_ = is16bit;
  spi_set_format(static_cast<spi_inst_t*>(inst_), is16bit ? 16 : 8, SPI_CPOL_0,
                 SPI_CPHA_0, SPI_MSB_FIRST);
  DBG_PRINT("bus width → %u bits\n", is16bit ? 16 : 8);
}
