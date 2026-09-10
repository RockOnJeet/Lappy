
# Lappy - Your personal connected assistant

This project entails an attempt to build a **small-screen assistant OS**, capable of integrating with various APIs and running on low-resource hardware.

## Specs

- Runs on RP2x devices
- Controls ILI9341 TFT LCD
- Supports SPI transactions with DMA
- Capable of locally storing and running ~3MB worth of GIFs
- Fast enough to play DOOM, probably (should test that next)
- Doesn't support wireless currently, hence communicates via USB
- Can show GIFs, system status, custom frontends if needed

## Architecture

The codebase follows an industry-standard, cross-platform **Hardware Abstraction Layer (HAL)** design using C++ target-based CMake and dependency inversion.

```
include/hal/          Pure abstract interfaces (ISPI, IGPIOPin, IDisplay, platform)
platform/pico/        RP2350/RP2040 concrete implementations (PicoSPI, PicoGPIOPin)
platform/native/      Host-side stubs for testing without hardware
src/                  Application logic + ILI9341 driver (depends only on interfaces)
AnimatedGIF/          Third-party GIF decoder
```

### Key design rules

- **Interfaces** (`include/hal/`) contain only pure virtual methods — zero hardware types, `#ifdef`s, or private state.
- **Implementations** (`platform/<target>/`) inherit from the interfaces and keep all SDK headers (`<hardware/spi.h>`, `<pico/stdlib.h>`) confined to their own directory.
- **Dependency injection** — the `ILI9341` driver receives `ISPI&` and `IGPIOPin&` via its constructor, never concrete platform types.
- **CMake hierarchy** — `hal_interface` (INTERFACE) provides the abstraction headers; each platform builds a static library (`lappy_pico` / `lappy_native`); `src/` links only to the active platform library.

## Current Implementations

- **V0 (current):** Full display pipeline done; shows looping RGB colors for now.
- **V1:** GIF processor done. GIFs found on-device will play one after another in loop.
- **V2:** USB support done. A custom Python node does preprocessing of GIFs to auto-send to the device for playback.
- **V3:** Custom frontend support - Use web designing tools to render the pages of your choice, define static v/s dynamic parts for display rendering through Python.
- **V4:** (To be continued)

## How to install/run this

### Prerequisites

- [Raspberry Pi Pico VS Code extension](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) (for device builds)
- CMake ≥ 3.13 and a C++17 compiler (for native builds)

### Build for the Pico device (RP2350)

```bash
git clone https://github.com/RockOnJeet/Lappy.git
cd Lappy
cmake -S . -B build/pico
cmake --build build/pico
```

Flash `build/pico/src/Lappy.uf2` to the device in BOOTSEL mode, or use the **"upload"** button in the VS Code extension.

### Build for native host (no hardware)

```bash
cmake -S . -B build/native -DNATIVE_BUILD=ON
cmake --build build/native
```

Run the host executable at `build/native/src/Lappy` for hardware-free testing.

### Enjoy!