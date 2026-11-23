// Source file for TraceSPI tracing backend within native platform.
// Phase 1: tracing infrastructure only. Records SPI operations to trace.json.

#include "TraceSPI.hpp"
#include "TraceRecorder.hpp"

// Dummy instance addresses (never dereferenced; used only for identity).
// Values mirror the RP2040 SPI peripheral base addresses.
spi_inst_t* const spi0 = reinterpret_cast<spi_inst_t*>(uintptr_t{0x40034000u});
spi_inst_t* const spi1 = reinterpret_cast<spi_inst_t*>(uintptr_t{0x40038000u});

namespace {

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Encode a byte buffer as uppercase hex, e.g. {0x2A, 0x00} -> "2A00".
std::string bytesToHex(const uint8_t* data, size_t len) {
  static const char kDigits[] = "0123456789ABCDEF";
  std::string out;
  out.reserve(len * 2);
  for (size_t i = 0; i < len; ++i) {
    out.push_back(kDigits[data[i] >> 4]);
    out.push_back(kDigits[data[i] & 0x0F]);
  }
  return out;
}

// Encode a 16-bit word buffer as uppercase hex, e.g. {0x1234} -> "1234".
std::string wordsToHex(const uint16_t* data, size_t len) {
  static const char kDigits[] = "0123456789ABCDEF";
  std::string out;
  out.reserve(len * 4);
  for (size_t i = 0; i < len; ++i) {
    const uint16_t w = data[i];
    out.push_back(kDigits[(w >> 12) & 0x0F]);
    out.push_back(kDigits[(w >> 8) & 0x0F]);
    out.push_back(kDigits[(w >> 4) & 0x0F]);
    out.push_back(kDigits[w & 0x0F]);
  }
  return out;
}

// Map an instance to a display name by comparing against the predefined
// instances (mirrors the Pico SDK's pointer-based instance identity).
std::string instanceName(spi_inst_t* inst) {
  if (inst == spi0)
    return "spi0";
  if (inst == spi1)
    return "spi1";
  return "spi?";
}

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
// Public API
// ---------------------------------------------------------------------------

uint spi_init(spi_inst_t* inst, uint baudrate) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi_init\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"baud\":" + std::to_string(baudrate),
  }));
  return baudrate;
}

void set_bus_width(spi_inst_t* inst, bool is8b) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"bus_width\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"bit_width\":" + std::to_string(is8b ? 8 : 16),
  }));
}

int spi_write_blocking(spi_inst_t* inst, const uint8_t* data, size_t len) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi8\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"data\":\"" + bytesToHex(data, len) + "\"",
  }));
  return static_cast<int>(len);
}

int spi_write16_blocking(spi_inst_t* inst, const uint16_t* data, size_t len) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi16\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"count\":" + std::to_string(len),
      "\"data\":\"" + wordsToHex(data, len) + "\"",
  }));
  return static_cast<int>(len);
}

int spi_write_read_blocking(spi_inst_t* inst,
                            const uint8_t* tx,
                            uint8_t* rx,
                            size_t len) {
  if (rx != nullptr)
    std::memset(rx, 0, len);  // Read buffers are zero-filled for now.
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi8_rw\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"tx\":\"" + bytesToHex(tx, len) + "\"",
      "\"len\":" + std::to_string(len),
  }));
  return static_cast<int>(len);
}

int spi_write16_read16_blocking(spi_inst_t* inst,
                                const uint16_t* tx,
                                uint16_t* rx,
                                size_t len) {
  if (rx != nullptr)
    std::memset(rx, 0, len * sizeof(uint16_t));  // Zero-filled for now.
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi16_rw\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"tx\":\"" + wordsToHex(tx, len) + "\"",
      "\"len\":" + std::to_string(len),
  }));
  return static_cast<int>(len);
}

bool spi_is_busy(spi_inst_t* inst) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi_busy\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
      "\"set\": false",
  }));
  return false;
}

void spi_deinit(spi_inst_t* inst) {
  TraceRecorder::appendEvent(buildObject({
      "\"type\":\"spi_deinit\"",
      "\"instance\":\"" + instanceName(inst) + "\"",
  }));
  TraceRecorder::close();
}