#pragma once

// #include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

using uint = unsigned int;

// Opaque SPI instance type, mirroring the Pico SDK's `spi_inst_t`.
struct spi_inst_t;

// Predefined instances, mirroring the Pico SDK's `spi0`/`spi1` macros.
extern spi_inst_t* const spi0;
extern spi_inst_t* const spi1;

/**
 * @brief Initialize the SPI instance at the requested baud rate.
 * @return The requested baudrate (stub).
 */
uint spi_init(spi_inst_t* inst, uint baudrate);

/**
 * @brief Sets bus width to 8/16 bits.
 */
void set_bus_width(spi_inst_t* inst, bool is8b);

/**
 * @brief Blocking 8-bit write.
 * @return Number of bytes written (len).
 */
int spi_write_blocking(spi_inst_t* inst, const uint8_t* data, size_t len);

/**
 * @brief Blocking 16-bit write.
 * @return Number of words written (len).
 */
int spi_write16_blocking(spi_inst_t* inst, const uint16_t* data, size_t len);

/**
 * @brief Blocking 8-bit write + read.
 * @return Number of bytes transferred (len).
 * @note `rx` is zero-filled for now.
 */
int spi_write_read_blocking(spi_inst_t* inst,
                            const uint8_t* tx,
                            uint8_t* rx,
                            size_t len);

/**
 * @brief Blocking 16-bit write + read.
 * @return Number of words transferred (len).
 * @note `rx` is zero-filled for now.
 */
int spi_write16_read16_blocking(spi_inst_t* inst,
                                const uint16_t* tx,
                                uint16_t* rx,
                                size_t len);

/**
 * @brief Check whether the SPI bus is busy.
 * @return Always false (stub).
 */
bool spi_is_busy(spi_inst_t* inst);

/**
 * @brief Deinitialize the SPI instance and finalize the trace.
 */
void spi_deinit(spi_inst_t* inst);