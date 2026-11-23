#!/usr/bin/env python3
"""Scaffold a new Lappy platform target.

Usage:
    python3 scaffold.py <platform-name>

Creates:
    platforms/<name>/HAL/            (empty, for the HAL implementation)
    platforms/<name>/CMakeLists.txt  (lappy_interfaces extension template)

Also updates (idempotently):
    CMakeLists.txt        -> adds <name> to SUPPORTED_PLATFORMS
    CMakePresets.json     -> adds a configure+build preset for <name>
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]  # repo root


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    name = sys.argv[1].strip().lower()
    if not re.fullmatch(r"[a-z0-9_]+", name):
        print(f"Invalid platform name: {name!r} "
              "(use lowercase letters, digits, underscore)")
        return 2

    # 1. Create platforms/<name>/HAL/ and the CMakeLists.txt extension.
    hal_dir = ROOT / "platforms" / name / "HAL"
    hal_dir.mkdir(parents=True, exist_ok=True)
    cmake = ROOT / "platforms" / name / "CMakeLists.txt"
    if not cmake.exists():
        cmake.write_text(
            f"""# -----------------------------------------------------------------------
# CMakeLists.txt extension for the {name} platform (NOT a standalone CMakeLists.txt)
# -----------------------------------------------------------------------

# Combine all source files into a single library for this platform
add_library(lappy_interfaces STATIC
    # HAL/SPI.cpp
)

# Expose generic_interfaces to this lib
target_link_libraries(lappy_interfaces
    PUBLIC
        generic_interfaces
)

# Include the current directory for header files
target_include_directories(lappy_interfaces
    PUBLIC
        ${{CMAKE_CURRENT_SOURCE_DIR}}/HAL
)
"""
        )
        print(f"created {cmake.relative_to(ROOT)}")

    # 2. Add to SUPPORTED_PLATFORMS in the root CMakeLists.txt.
    root_cmake = ROOT / "CMakeLists.txt"
    text = root_cmake.read_text()
    if name not in text:
        text = text.replace(
            "set(SUPPORTED_PLATFORMS\n",
            "set(SUPPORTED_PLATFORMS\n" + name + "\n",
            1,
        )
        root_cmake.write_text(text)
        print(f"added {name} to SUPPORTED_PLATFORMS in CMakeLists.txt")
    else:
        print(f"{name} already in SUPPORTED_PLATFORMS")

    # 3. Add a configure + build preset in CMakePresets.json.
    presets_path = ROOT / "CMakePresets.json"
    presets = json.loads(presets_path.read_text())
    names = {p["name"] for p in presets["configurePresets"]}
    if name not in names:
        presets["configurePresets"].append(
            {
                "name": name,
                "displayName": name,
                "generator": "Ninja",
                "binaryDir": "${sourceDir}/build/" + name,
                "cacheVariables": {"PLATFORM": name},
            }
        )
        presets["buildPresets"].append({"name": name, "configurePreset": name})
        presets_path.write_text(json.dumps(presets, indent=2) + "\n")
        print(f"added {name} preset to CMakePresets.json")
    else:
        print(f"{name} preset already exists")

    print(
        f"\nNext: implement the HAL in platforms/{name}/HAL/ against the "
        "contract in include/HAL/. See "
        ".github/instructions/new-platform.instructions.md"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
