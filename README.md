
# Lappy - Your personal connected assistant

This project entails an attempt to build a **small-screen assistant OS**, capable of integrating with various APIs and running on low-resource hardware.

## Specs

- Runs on RP2x devices
- ~~Controls ILI9341 TFT LCD~~CD~~
- Supports SPI transactions with DMA
- ~~Capable of locally storing and running ~3MB worth of GIFs~~
- Fast enough to play DOOM, probably (should test that next)
- Doesn't support wireless currently, hence communicates via USB
- ~~Can show GIFs, system status, custom frontends if needed~~

## Architecture

The codebase follows an industry-standard, cross-platform **Hardware Abstraction Layer (HAL)** design using C++ target-based CMake and dependency inversion.

```
.
├── include     # Keeps all virtual classes
│   └── HAL
├── lib         # Keeps all third-party libraries
├── platforms   # Keeps all platform-specific implementations
│   ├── native
│   │   └── HAL
│   └── rp2
│       └── HAL
├── src         # Keeps common codes that actually defines the program
│   └── Lappy
└── test        # Keeps all tests for software testing
    └── test_demo
```

### Key design rules

- **Interfaces** (`include/HAL/`) contain only pure virtual methods — zero hardware types, `#ifdef`s, or private state.
- **Implementations** (`platforms/<target>/`) inherit from the interfaces and keep all SDK headers (`<hardware/spi.h>`, `<pico/stdlib.h>`) confined to their own directory.
- ~~**Dependency injection** — the `ILI9341` driver receives `ISPI&` and `IGPIOPin&` via its constructor, never concrete platform types.~~
- **CMake hierarchy** — `generic_interface` (INTERFACE) provides the abstraction headers; each platform builds a static library (`lappy_pico` / `lappy_native` mapped to `lappy_interfaces`); `src/` links only to the active platform library.

## Current Implementations

- ~~**V0 (current):** Full display pipeline done; shows looping RGB colors for now.~~
- ~~**V1:** GIF processor done. GIFs found on-device will play one after another in loop.~~
- ~~**V2:** USB support done. A custom Python node does preprocessing of GIFs to auto-send to the device for playback.~~
- ~~**V3:** Custom frontend support - Use web designing tools to render the pages of your choice, define static v/s dynamic parts for display rendering through Python.~~
- ~~V4: (To be continued)~~

## How to install/run this

### Prerequisites

- [Raspberry Pi Pico VS Code extension](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) (for device builds)
- CMake ≥ 3.13 and a C++17 compiler (for native builds)

### Build for the Pico device (RP2350)

Use the `rp2` configuration preset to build for the RP2040 device.

```bash
cd Lappy
cmake --preset rp2
cmake --build build/rp2
```

Flash `build/rp2/src/Lappy.uf2` to the device in BOOTSEL mode, or use the **"upload"** button in the VS Code extension.

### Build for native host (no hardware)

Use the `native` configuration preset to build for the host.

```bash
cd Lappy
cmake --preset native
cmake --build build/native
```

Run the host executable at `build/native/src/Lappy` for hardware-free testing.

### Enjoy!