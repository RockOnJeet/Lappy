# TraceGPIO — Design Document

> **Status:** Design only. No implementation in this phase.
> This document describes the planned GPIO tracing subsystem so the trace
> format stays stable before more event producers are added.

## Architecture

GPIO tracing follows the same pattern as the existing SPI and DMA producers:

```
HAL/SPI
   │
   ▼
TraceSPI
   │
   ▼
TraceRecorder

HAL/DMA
   │
   ▼
TraceDMA
   │
   ▼
TraceRecorder
```

Future:

```
HAL/GPIO
   │
   ▼
TraceGPIO
   │
   ▼
TraceRecorder
```

`TraceGPIO` will be a thin Pico-SDK-shaped backend (like `TraceSPI` /
`TraceDMA`) that logs GPIO operations through `TraceRecorder::appendEvent(...)`.
It will not own any file state; `TraceRecorder` remains the single owner of
`trace.json`, formatting, and the monotonic `seq` numbering.

---

## 1. Goals

GPIO tracing exists so the WebUI can reconstruct, in order:

- **DC transitions** — the data/command line that tells a display controller
  whether the current SPI bytes are commands or data.
- **CS assertions** — chip-select toggles that frame each SPI transaction.
- **RESET pulses** — display reset timing (assertion and release).
- **Backlight changes** — on/off and brightness control lines.
- **General GPIO state changes** — any other digital pin writes.

Combined with the existing SPI and DMA traces, these events let the WebUI
replay the exact sequence of operations a display driver performed.

---

## 2. Planned API

The intended minimal API mirrors the Pico SDK GPIO functions, at global scope
like the other trace backends:

```cpp
void gpio_init(int pin);
void gpio_set_dir(int pin, bool out);
void gpio_put(int pin, bool value);
bool gpio_get(int pin);
void gpio_deinit(int pin);
```

Notes:

- `gpio_set_dir(pin, out)` — `out == true` selects output, `false` selects
  input.
- `gpio_put(pin, value)` — drive a pin high/low (only meaningful when the pin
  is an output).
- `gpio_get(pin)` — read the current pin level.
- No configuration structures, no pull-up/pull-down state, no interrupt
  registration in this phase.

No implementation is provided yet; this section only fixes the intended
surface so the event model below does not drift.

---

## 3. Event model

Every event is a JSON object tagged by `TraceRecorder` with a monotonic
`seq` (owned by the recorder, not by `TraceGPIO`).

### Initialization

```json
{
  "seq": 42,
  "type": "gpio_init",
  "pin": 21
}
```

### Output change

```json
{
  "seq": 43,
  "type": "gpio_write",
  "pin": 21,
  "value": 0
}
```

### Direction change

```json
{
  "seq": 44,
  "type": "gpio_dir",
  "pin": 21,
  "direction": "out"
}
```

### Read

```json
{
  "seq": 45,
  "type": "gpio_read",
  "pin": 21,
  "value": 1
}
```

### Deinitialization

```json
{
  "seq": 46,
  "type": "gpio_deinit",
  "pin": 21
}
```

Field conventions:

- `pin` — the GPIO number (0-based, matching the Pico SDK).
- `value` — `0` or `1` (JSON number).
- `direction` — `"out"` or `"in"` (JSON string).
- `seq` — assigned automatically by `TraceRecorder`; producers never supply it.

---

## 4. Replay role

GPIO is essential for display replay. A display controller (e.g. ILI9341)
multiplexes a single SPI bus between three kinds of traffic:

- **command bytes** — register writes (e.g. `0x2A` = column address set),
- **parameter bytes** — arguments for the current command,
- **pixel payloads** — raw framebuffer data.

The SPI trace alone cannot distinguish these: the bytes are identical in
format. The **DC line** is what tells the controller which kind of byte is on
the bus, and the **CS line** frames each transaction.

Without DC and CS transitions, the WebUI cannot tell:

- command bytes from parameter bytes,
- parameter bytes from pixel payloads,

even if the SPI bytes are fully known. GPIO tracing is therefore a
prerequisite for meaningful display replay, not an optional nicety.

---

## 5. Out of scope

This phase intentionally does **not** implement:

- **Timing** — no delays or duration simulation between events.
- **Edge timestamps** — no wall-clock or monotonic time attached to events
  (ordering is preserved by `seq` only).
- **Interrupt logging** — no `gpio_set_irq_enabled` or callback tracing.
- **PWM logging** — no `gpio_set_pwm` / duty-cycle events.
- **Analog behavior** — no ADC reads, no pull-up/pull-down state.

Those belong to later phases. The objective of this phase is only to fix the
GPIO event model and API surface so the trace format does not drift when the
subsystem is implemented.