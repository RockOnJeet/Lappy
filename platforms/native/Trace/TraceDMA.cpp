// Source file for TraceDMA tracing backend within native platform.
// Phase 3B: minimal DMA lifecycle/transfer tracing through TraceRecorder.

#include "TraceDMA.hpp"
#include "TraceRecorder.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace {

// ---------------------------------------------------------------------------
// Minimal bookkeeping
// ---------------------------------------------------------------------------

// One flag per channel; nothing else is tracked (no config, DREQ, addresses).
struct dma_channel_state {
  bool claimed = false;
};

// Fixed channel pool. Allocation is deterministic (0, 1, 2, ...).
constexpr int kDmaChannelCount = 12;  // Matches RP2040's 12 DMA channels.
dma_channel_state g_channels[kDmaChannelCount];

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Build a pretty-printed JSON object from a list of "key":value fields.
std::string buildObject(const std::vector<std::string>& fields) {
  std::string out = "{\n";
  for (size_t i = 0; i < fields.size(); ++i) {
    out += "      " + fields[i];
    if (i + 1 < fields.size())
      out += ",";
    out += "\n";
  }
  out += "    }";
  return out;
}

}  // namespace

// ---------------------------------------------------------------------------
// Channel lifecycle
// ---------------------------------------------------------------------------

int dma_claim_unused_channel(bool required) {
  for (int ch = 0; ch < kDmaChannelCount; ++ch) {
    if (!g_channels[ch].claimed) {
      g_channels[ch].claimed = true;
      TraceRecorder::appendEvent(buildObject({
          "\"type\":\"dma_claim\"",
          "\"channel\":" + std::to_string(ch),
          "\"required\":" + std::string(required ? "true" : "false"),
      }));
      return ch;
    }
  }
  // No free channel; log the failed claim and return -1.
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"dma_claim\"",
      "\"channel\":-1",
      "\"required\":" + std::string(required ? "true" : "false"),
  }));
  return -1;
}

void dma_channel_unclaim(int channel) {
  if (channel < 0 || channel >= kDmaChannelCount)
    return;
  g_channels[channel].claimed = false;
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"dma_unclaim\"",
      "\"channel\":" + std::to_string(channel),
  }));
}

void dma_channel_abort(int channel) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"dma_abort\"",
      "\"channel\":" + std::to_string(channel),
  }));
}

void dma_channel_cleanup(int channel) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"dma_cleanup\"",
      "\"channel\":" + std::to_string(channel),
  }));
}

bool dma_channel_is_busy(int channel) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"dma_busy\"",
      "\"channel\":" + std::to_string(channel),
      "\"busy\": false",
  }));
  return false;
}

// ---------------------------------------------------------------------------
// Transfer
// ---------------------------------------------------------------------------

void dma_channel_transfer_from_buffer_now(int channel,
                                          const void* buffer,
                                          size_t count) {
  (void)buffer;  // No payload copying.
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"dma_transfer\"",
      "\"channel\":" + std::to_string(channel),
      "\"count\":" + std::to_string(count),
  }));
}