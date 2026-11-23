#pragma once

#include <string>

/**
 * @file TraceRecorder.hpp
 * @brief Shared JSON trace backend for the native platform.
 *
 * Owns `trace.json` and builds the JSON document incrementally. Every trace
 * module (TraceSPI, future TraceDMA, future TraceGPIO) appends events through
 * this recorder so the file format stays consistent.
 *
 * The file is created lazily on the first event and overwritten each run.
 */
namespace TraceRecorder {

/**
 * @brief Append one JSON object to the trace.
 * @param jsonObject A complete JSON object (e.g. `{ "type":"spi8", ... }`).
 *
 * Creates `trace.json` on first use and flushes after every event so a crash
 * still leaves nearly complete JSON.
 */
void appendEvent(const std::string& jsonObject);

/**
 * @brief Write the footer and close the file.
 *
 * Safe to call multiple times. Also invoked automatically at shutdown if the
 * caller forgets to close.
 */
void close();

}  // namespace TraceRecorder