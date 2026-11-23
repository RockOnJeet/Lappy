---
name: add-platform
description: 'Scaffold and implement a new embedded platform (ESP32, STM32, etc.) for Lappy. Use when adding a new MCU target, creating platforms/<target>/ HAL implementations, or wiring a new platform into the cascading CMake build.'
argument-hint: '<platform-name>'
---

# Add a New Embedded Platform

## When to Use
- Adding a new MCU target (ESP32, STM32, ...) to the project
- Scaffolding `platforms/<target>/` with a HAL implementation
- Wiring a new platform into the cascading CMake build

## Prerequisites
- Read `AGENTS.md` for the project overview and golden rule.
- Follow `.github/instructions/new-platform.instructions.md` (auto-attached
  when editing `platforms/**`) for the contract-matching rules.

## Procedure

### 1. Scaffold the platform
Run the scaffold script with the platform name:

```bash
python3 .github/skills/add-platform/scripts/scaffold.py <platform-name>
```

This creates `platforms/<platform-name>/HAL/` and a CMakeLists extension
template, adds the platform to `SUPPORTED_PLATFORMS`, and adds a CMake preset.

### 2. Implement the HAL against the contract
- Read `include/HAL/ISPI.hpp` (and any other interfaces).
- Implement every pure virtual method with exact signatures and types.
- Keep SDK headers confined to `platforms/<platform-name>/HAL/`.
- Match the SDK's type shapes so `src/` code compiles unchanged.

### 3. Validate
- Build the new platform preset AND `native`.
- Smoke-test both; on native verify `trace.json` is valid JSON with
  sequential `seq`.
- Confirm the firmware contains the HAL symbols.

## Resources
- [scaffold.py](./scripts/scaffold.py) — platform scaffolding script