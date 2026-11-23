// Source file for TraceRecorder: shared JSON trace backend for the native
// platform. Owns trace.json and builds the JSON document incrementally.

#include "TraceRecorder.hpp"

#include <cstddef>
#include <fstream>
#include <string>

namespace TraceRecorder {
namespace {

// ---------------------------------------------------------------------------
// Recorder: owns trace.json and builds the JSON document incrementally.
// ---------------------------------------------------------------------------
class Recorder {
 public:
  static Recorder& instance() {
    static Recorder recorder;
    return recorder;
  }

  // Append one event object. The first event follows the header directly;
  // later events are comma-separated. Every event is tagged with a monotonic
  // sequence number owned by this recorder.
  void appendEvent(const std::string& jsonObject) {
    ensureOpen();
    if (!_file.is_open())
      return;
    // Insert "seq":<N>, as the first field, right after the opening brace.
    std::string tagged = jsonObject;
    const size_t brace = tagged.find('{');
    if (brace != std::string::npos)
      tagged.insert(brace + 1,
                    "\n      \"seq\":" + std::to_string(_next_seq) + ",");
    ++_next_seq;
    _file << (_first ? "" : ",") << "\n    " << tagged;
    _first = false;
    _file.flush();
  }

  // Write the footer and close the file. Idempotent.
  void close() {
    if (!_file.is_open())
      return;
    _file << "\n  ]\n}\n";
    _file.close();
  }

 private:
  // Open (or reopen) trace.json, truncating any previous run.
  void ensureOpen() {
    if (_file.is_open())
      return;
    _file.open("trace.json", std::ios::out | std::ios::trunc);
    if (_file.is_open()) {
      _file << "{\n  \"version\":1,\n  \"events\":[";
      _first = true;
      _next_seq = 0;  // New trace file => sequence restarts at 0.
    }
  }

  ~Recorder() { close(); }

  std::ofstream _file;
  bool _first = true;
  size_t _next_seq = 0;  // Monotonic sequence for the next event.
};

}  // namespace

void appendEvent(const std::string& jsonObject) {
  Recorder::instance().appendEvent(jsonObject);
}

void close() {
  Recorder::instance().close();
}

}  // namespace TraceRecorder