# Nokia N9 Harmattan App

Qt 4.7/4.8 + Qt Quick 1.1 application for the Nokia N9 (MeeGo 1.2 Harmattan).

---

## Build commands

```bash
# Configure + run conan automatically:
./configure <n9|simulator> <build_dir> [cmake flags...]

# Examples:
./configure simulator build-simulator
./configure n9 build-n9
./configure n9 build-n9-debug -DCMAKE_BUILD_TYPE=Debug

# Then build:
cmake --build <build_dir>
```

`./configure` runs Conan automatically — do not run it manually.

---

## IMPORTANT: QML change workflow — YOU MUST follow this every time

After **any** edit to a `.qml` file:

1. `./configure simulator build-simulator && cmake --build build-simulator`
2. Run the simulator binary and verify the UI visually — no runtime errors, no broken layouts.
3. Do **not** mark the task done until the simulator passes.

---

## Platform constraints

| Property | Value |
|---|---|
| Qt version | **4.7 / 4.8 only** — no Qt5/Qt6 APIs |
| QML version | **Qt Quick 1.1 only** |
| Target kernel | Linux **2.6.32** |
| CPU | ARMv7 |
| C++ standard | **C++23** (GCC 14.1 toolchain supports it) |
| Screen | 854 × 480 px, 3.9 in, portrait-primary |

Never use APIs introduced after Qt 4.8 or Qt Quick 1.1. When in doubt, check the N9 Developer Library.

---

## N9 UX — IMPORTANT: strict compliance required

**All QML interfaces MUST conform to the N9 UX Guidelines.**
Before implementing any UI element, consult the guidelines first.

- N9 UX / HIG: https://n9.dy.fi/meego/UX/
- Harmattan Developer Library: https://n9.dy.fi/meego/html/guide/html/Developer_Library_MeeGo_1.2_Harmattan_Developer_Library.html
- Available Qt components: https://n9.dy.fi/meego/html/qt-components-extras/qt-components-extras.html?tab=1

Key HIG rules to keep in mind:
- Design for one-handed thumb use, portrait-primary layout.
- Use platform components from the imports path below — do not invent custom controls where a Harmattan component exists.
- Swipe navigation, page stacks, and toolbar patterns must match the platform conventions.

---

## Paths

| Resource | Path |
|---|---|
| Cross-compiler (GCC 14.1) | `/opt/harmattan-gcc/` |
| QtSDK root | `/home/iarchep/QtSDK/C` |
| N9 sysroot | `/home/iarchep/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim/` |
| QML imports (N9 + simulator) | `/home/iarchep/QtSDK/Simulator/Qt/gcc/imports/` |
| Harmattan UI assets (themes, images) | `/home/iarchep/QtSDK/Simulator/Qt/gcc/harmattanthemes/blanco/meegotouch/` |

> The QML imports path covers both build targets, except `simulatorHarmattan`-specific components.
> `libpng12` for the simulator is fetched by CMake FetchContent — do not add it via Conan.

---

## Code conventions

- No business logic in QML — keep it strictly in C++.
- One QML component per file; filename matches component name.
- Qt naming: `camelCase` methods, `PascalCase` classes, `m_` prefix for members.
- No host-glibc symbols in cross-compiled code — link against the sysroot.