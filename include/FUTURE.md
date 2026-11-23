# Lappy Architecture Notes — Future HAL Design

> This document records an alternative HAL architecture considered during the initial CMake/HAL setup. It is intentionally not implemented yet, but serves as a reference if the project grows enough to justify revisiting these decisions.

## Current architecture (implemented)

Current build flow:

```
Root CMakeLists
    │
    ├── PLATFORM=native
    │
    ├── platforms/native
    │      └── lappy_interfaces
    │
    └── src
           ├── lappy_core
           └── example_demo
```

Current principles:

* Root CMake selects the active platform (`PLATFORM` cache variable).

* Only one platform implementation exists in a build.

* `src` links against `lappy_interfaces` and does not know which platform produced it.

* Platform implementations currently expose concrete classes (`SPI`, `GPIO`, etc.).

This keeps the project simple and minimizes CMake complexity.

## Alternative architecture (future consideration)

The main issue discovered with the current design is that different platforms naturally require different private members.

Example:

Native SPI:

C++

```
class SPI {
private:
    Logger logger;
};
```

RP2350 SPI:

C++

```
class SPI {
private:
    spi_inst_t* spi;
    uint sck;
    uint mosi;
};
```

Keeping only one shared `SPI.hpp` becomes difficult without introducing opaque storage (`void*`, PImpl, etc.).

### Considered solution

Instead of replacing headers per platform, separate portable APIs from platform extensions.

Proposed layout:

```
include/
└── HAL/
    SPI.hpp
    GPIO.hpp
    Display.hpp

platforms/
├── native/
│   └── HAL/
│       SPI.cpp
│       GPIO.cpp
└── rp2/
    └── HAL/
        SPI.cpp
        GPIO.cpp
        SPIExt.hpp
        SPIExt.cpp
```

The idea:

* `include/HAL/*.hpp` defines the portable API.

* Each platform implements that API.

* Platform-only functionality lives in extension headers/libraries.

Example:

Portable:

C++

```
SPI spi;
spi.transfer(...);
```

RP2350-only:

C++

```
#include "HAL/SPIExt.hpp"

enableDMA(spi);
```

This explicitly advertises:

> "This code is intentionally platform-specific."

instead of silently extending `SPI`.

## Why this was not adopted yet

### 1. The current architecture is faster to develop with

Today the project has:

* one native implementation

* one future RP2 implementation

* no need for runtime polymorphism

Using concrete classes avoids:

* factories

* dependency injection

* virtual methods

* additional boilerplate

### 2. Build-time swapping already solves the primary problem

CMake already guarantees that only one platform implementation is compiled.

For example:

```
PLATFORM=native
```

automatically loads:

```
platforms/native
```

and `src` remains unchanged.

### 3. Platform-specific extensions deserve explicit opt-in

One idea considered was allowing:

C++

```
spi.enableDMA();
```

only on RP2.

This was rejected because shared code could accidentally become platform-dependent.

Extension headers make platform-specific code explicit.

## Why not keep `ISPI` forever?

Originally, the design used:

C++

```
class ISPI {
public:
    virtual void transfer() = 0;
};
```

with:

C++

```
class SPI : public ISPI;
```

Reasons this became less attractive:

* Only one SPI implementation exists per build.

* Runtime polymorphism isn't currently needed.

* Every implementation would still need matching constructors and public members.

* Most of the code existed only to preserve inheritance.

The conclusion was that inheritance should only be introduced if multiple implementations must coexist in the same build.

## Future migration triggers

Revisit this architecture if any of the following become true:

* More than two supported platforms.

* Shared libraries begin exposing HAL types publicly.

* Tests require extensive mocking instead of native logging.

* Platform-specific functionality (DMA, PIO, etc.) becomes common.

* The same build needs multiple SPI implementations simultaneously.

Until then, the simpler concrete-class approach remains the preferred implementation.

## Industry references considered

During this design discussion, several established approaches were compared.

### Zephyr Device Driver Model

Zephyr keeps one canonical public API while selecting platform-specific implementations underneath. It also explicitly supports device-specific API extensions, which closely matches the proposed `SPIExt.hpp` idea.

* Device Driver Model: [https://docs.zephyrproject.org/latest/kernel/drivers/index.html](https://docs.zephyrproject.org/latest/kernel/drivers/index.html) 

* Zephyr Driver API Reference: [https://docs.zephyrproject.org/latest/doxygen/html/](https://docs.zephyrproject.org/latest/doxygen/html/) 

### Zephyr's Device-Specific Extensions

One particularly relevant section describes how drivers expose generic subsystem APIs while allowing additional device-specific APIs in separate headers.

* [https://docs.zephyrproject.org/latest/kernel/drivers/index.html#device-specific-api-extensions](https://docs.zephyrproject.org/latest/kernel/drivers/index.html#device-specific-api-extensions) 

### Zephyr API Structures

Zephyr's documentation also discusses subsystem APIs, driver implementations, and API inheritance for specialized drivers.

* [https://docs.zephyrproject.org/latest/kernel/drivers/index.html#subsystems-and-api-structures](https://docs.zephyrproject.org/latest/kernel/drivers/index.html#subsystems-and-api-structures) 

### PImpl (Pointer-to-Implementation)

Although not adopted, the PImpl pattern was considered because it solves differing private members while preserving a single public header.

* C++ Core Guidelines – PImpl discussion: [https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i27-for-stable-library-abi-consider-the-pimpl-idiom](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i27-for-stable-library-abi-consider-the-pimpl-idiom) 

## Final takeaway

For Lappy's current stage, optimizing for clarity and development speed outweighs optimizing for maximum abstraction.

The current build-selected implementation model keeps the codebase small, while this document preserves a migration path if the project eventually grows into a larger multi-platform HAL.

## One hindsight note

After researching Zephyr further, the strongest validation wasn't actually its function-pointer driver model—that's specific to C—but its separation of concerns:

* one stable public API,

* one build-selected implementation,

* separate headers for platform-specific extensions.

That last idea (`SPIExt.hpp`, `GPIOExt.hpp`) was the most valuable takeaway, because it lets portable code remain clean while making platform-specific code an explicit choice by the developer.
