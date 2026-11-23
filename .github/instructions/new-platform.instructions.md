---
description: "Use when adding a new embedded platform (ESP32, STM32, etc.) or implementing HAL logic for an existing one. Covers matching the shared interface contract and SDK type shapes, the platform-add recipe, and validation."
name: "New Embedded Platform"
applyTo: "platforms/**"
---
# Adding / Implementing an Embedded Platform

## The contract is fixed; the platform adapts to it

`include/HAL/` (e.g. `ISPI.hpp`) is the shared contract every platform
implements for the same logic/operations. It is pure virtual only — no
concrete types, no platform headers, no `#ifdef PLATFORM`.

- **Implement the interface; never change it.** If the interface can't
  express what the new hardware needs, that is a design discussion, not a
  silent edit.
- The platform-specific implementation adapts to the interface, not the
  other way around.

## Matching the contract and types (primary concern)

The goal is that the same `src/` entrypoint code compiles and runs on every
platform unchanged. The new platform's public surface must match the contract
exactly:

- **Signatures** — implement every pure virtual method with the exact
  parameter and return types from the interface (`uint`, `size_t`, `int`,
  `bool`, `spi_pins_t`, ...). No reordering, no renames, no extra required
  params.
- **Type shapes** — mirror the SDK's types so shared code compiles. Instance
  handles are opaque pointers (never enums or values); compare by pointer
  identity, not value. `uint` is `unsigned int`.
- **Semantics** — match the documented behavior: `begin()` returns the actual
  baud, `transfer8`/`transfer16` return the count, `isBusy()` reflects
  in-flight transfers, `end()` releases resources.
- **SDK headers stay local** — `<hardware/spi.h>`, `<pico/stdlib.h>`, etc.
  live ONLY in `platforms/<target>/HAL/`, never in `include/HAL/` or `src/`.

## Recipe for adding a new MCU

1. Add the target to `SUPPORTED_PLATFORMS` in the root `CMakeLists.txt`.
2. Create `platforms/<target>/`:
   - `HAL/` — concrete implementations (e.g. `SPI.hpp`/`SPI.cpp`) inheriting
     the shared interfaces. SDK headers live ONLY here.
   - A `CMakeLists.txt` extension (NOT standalone) that builds
     `lappy_interfaces` from the platform sources.
3. Add a CMake preset that sets `PLATFORM=<target>`.
4. Do NOT flatten the cascading CMake design — it is intentional core
   functionality. The per-platform extension pattern is what keeps `src/`
   platform-agnostic.

## When the base contract changes (all platforms together)

Native type-matching is NOT a per-platform concern. It matters only when the
shared contract itself changes — e.g. a virtual class is updated to support
something new or made more generic, and every platform must implement the new
surface:

- Update **all** platforms in the same change: `native`, `rp2`, and any
  others.
- `native` is the sim/log reference and must type-match the embedded
  platforms so the same `src/` code still compiles everywhere.
- Keep the trace format stable: new event producers go through
  `TraceRecorder` and follow the field conventions in `TraceGPIO.md`.

## Validation

- Build **and** smoke-test the new platform AND `native` after changes.
- On native, verify `trace.json` is valid JSON with sequential `seq`.
- Confirm the new platform's runnable target actually contains the HAL code
  (e.g. check symbols in the firmware — a most-vexing-parse `SPI spi();` can
  silently drop it).

## Pitfalls

- **Most vexing parse:** `SPI spi();` declares a function, not an object.
  Always instantiate as `SPI spi(spi0, true);`.
- **Dangling CMake references:** deleting a source file without removing its
  `add_executable`/`add_test` entry breaks the `all` build.
- **`spi_pins_t` has a default member (`MISO = UINT8_MAX`)** — construct it
  field-by-field, not with aggregate braces (C++11-safe).
- **DMA threshold:** `transfer16` uses DMA only when `len >= DMA_THRESHOLD`
  (8) and a channel was claimed; otherwise it blocks. Poll `isBusy()` after a
  DMA transfer.