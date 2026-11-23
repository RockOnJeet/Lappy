#pragma once

#include <cstddef>

/**
 * @file TraceDMA.hpp
 * @brief Minimal Pico-SDK-shaped DMA tracing backend for the native platform.
 *
 * Records DMA channel lifecycle and transfer events through the shared
 * TraceRecorder. Preserves execution order only; no timing or hardware
 * emulation is performed.
 */

// ---------------------------------------------------------------------------
// Channel lifecycle
// ---------------------------------------------------------------------------

/**
 * @brief Claim the next free DMA channel.
 * @param required  If true the real SDK would panic when unavailable.
 * @return Channel index (0-based), or -1 if none available.
 *
 * Channels are allocated deterministically (0, 1, 2, ...) and reused after
 * being unclaimed.
 */
int dma_claim_unused_channel(bool required);

/**
 * @brief Release a previously claimed channel.
 */
void dma_channel_unclaim(int channel);

/**
 * @brief Abort an in-flight DMA transfer (logged, not simulated).
 */
void dma_channel_abort(int channel);

/**
 * @brief Clean up a channel after use (logged, not simulated).
 */
void dma_channel_cleanup(int channel);

/**
 * @brief Check whether a channel's transfer is still in progress.
 * @return Always false (successful transfer model).
 */
bool dma_channel_is_busy(int channel);

// ---------------------------------------------------------------------------
// Transfer
// ---------------------------------------------------------------------------

/**
 * @brief Begin a DMA transfer from a buffer.
 * @param channel  The channel to transfer on.
 * @param buffer   Source buffer (not copied).
 * @param count    Number of items to transfer.
 *
 * Logged only; no payload is copied and the transfer is modelled as completing
 * immediately.
 */
void dma_channel_transfer_from_buffer_now(int channel,
                                          const void* buffer,
                                          size_t count);
