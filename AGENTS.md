# AGENTS.md — Lappy

Guidance for AI coding agents working in this repository. Read this before
making changes. See [README.md](README.md) for the project overview and
[include/FUTURE.md](include/FUTURE.md) for the (unimplemented) alternative HAL
architecture.

## What this project is

Lappy is a cross-platform embedded display-driver project. The **same C++
source** in `src/` must compile and run on every platform:

- **`native`** — host build. **Logging/simulation only**: hardware calls are
  stubbed by a tracing backend that records every operation to `trace.json`
  (no real hardware). It exists to develop and verify logic without a device.
- **`rp2`** — Raspberry Pi Pico 2 (RP2350) via the Pico SDK. Drives real
  SPI/DMA hardware and produces a runnable firmware target (`Lappy.uf2`).

Every **embedded** platform produces its own runnable target from the same
entrypoint; `native` is the sim/log reference. More MCUs (ESP32, STM32, ...)
are planned — each gets its own `platforms/<target>/` implementation behind
the same shared interface.

The golden rule: **entrypoint code in `src/` must be identical for all
platforms** and produce the same logical result. When a discrepancy prevents
this, **modify `native` to type-match the embedded platform** — never the
reverse.

## Build & run

Two CMake presets (Ninja). `PLATFORM` is set by the preset and required.

```bash
# Native (host)
cmake --preset native
cmake --build build/native
./build/native/src/Lappy            # prints results, writes trace.json (in cwd)

# rp2 (Pico 2 / RP2350)
cmake --preset rp2
cmake --build build/rp2             # produces build/rp2/src/Lappy.uf2
```

Tests use Unity (fetched via FetchContent) and run through CTest:

```bash
ctest --test-dir build/native       # or build/rp2
```

## Architecture & where things go

```
include/HAL/        # Pure abstract interfaces (ISPI.hpp). NO hardware types,
                    # #ifdefs, or private state. Shared by all platforms.
platforms/<target>/HAL/   # Concrete implementations (SPI.hpp/.cpp). SDK
                    # headers (<hardware/spi.h>, <pico/stdlib.h>) live ONLY here.
platforms/native/Trace/   # Native tracing backend (see below).
src/Lappy/          # Entrypoints (example_01.cpp). Links lappy_interfaces only.
test/               # Unity tests (test_demo).
```

CMake hierarchy: `generic_interfaces` (INTERFACE) → `lappy_interfaces` (static
lib, one per platform) → `src/` and `test/` executables. Each platform's
`CMakeLists.txt` is an extension included from the root, not standalone.

**The cascading CMake design is intentional core functionality — maintain it.**
The root `CMakeLists.txt` selects the platform via the `PLATFORM` cache
variable (set by preset), validates it against `SUPPORTED_PLATFORMS`, and
`add_subdirectory()`s `include/`, `platforms/<PLATFORM>/`, `src/`, and `test/`.
Adding a new MCU means: add it to `SUPPORTED_PLATFORMS`, create
`platforms/<target>/` (HAL impl + a CMakeLists extension), and add a preset.
Do not "simplify" this into a single flat build — the per-platform extension
pattern is what keeps `src/` platform-agnostic.

### Native tracing backend (`platforms/native/Trace/`)

- `TraceRecorder` — **single owner** of `trace.json`. Builds JSON
  incrementally, flushes after every event, and injects a monotonic `seq`
  (starts at 0, resets per run). Producers never supply `seq`.
- `TraceSPI` / `TraceDMA` — thin Pico-SDK-shaped shims at global scope that
  call `TraceRecorder::appendEvent(...)`. They own no file state.
- `TraceGPIO.md` — design doc for the planned GPIO producer; the event model
  there is the reference for adding new producers.

## Conventions & rules

- **Interfaces are the middle ground.** `include/HAL/` is the shared contract
  that every platform implements for the same logic/operations. It is pure
  virtual only — no concrete types, no platform headers, no
  `#ifdef PLATFORM`. When adding a new MCU, the interface is what stays fixed;
  the platform-specific implementation adapts to it.
- **Type-match the embedded platform in native.** `spi_inst_t` is an opaque
  `struct` (never dereferenced); `spi0`/`spi1` are `spi_inst_t* const`
  pointers. Compare instances by pointer identity, not value. `uint` is
  `unsigned int`.
- **Keep trace format stable.** New event producers must go through
  `TraceRecorder` and follow the field conventions in `TraceGPIO.md`.
- **Scope discipline.** Work is done in phases with explicit scope. Do not
  modify existing HAL behavior or files outside the stated scope of a phase.
- **Validate both platforms.** After changes, build **and** smoke-test both
  presets; verify `trace.json` is valid JSON with sequential `seq`.

## Pitfalls (learned the hard way)

- **Most vexing parse:** `SPI spi();` declares a function, not an object —
  the SPI code silently never links into the firmware. Always instantiate as
  `SPI spi(spi0, true);`.
- **Dangling CMake references:** deleting a source file (e.g. a test) without
  removing its `add_executable`/`add_test` entry breaks the `all` build.
- **`spi_pins_t` has a default member (`MISO = UINT8_MAX`)** — construct it
  field-by-field, not with aggregate braces, to stay C++11-safe.
- **DMA threshold:** `transfer16` uses DMA only when `len >= DMA_THRESHOLD`
  (8) and a channel was claimed; otherwise it blocks. `isBusy()` must be
  polled after a DMA transfer.
- **rp2 `begin()` validates pins** (SCK/MOSI/MISO relationship) and returns
  the hardware's actual baud; native returns the requested baud. This is the
  one intentional platform difference.