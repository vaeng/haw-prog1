# MyGame Setup Guide

This project requires:

- C++23 compiler
- CMake 3.25+
- Ninja build tool
- vcpkg (dependency manager for Windows)
- SFML 3 library

---

## Installation & Building

Below are the recommended steps per platform. All presets follow the pattern `<os>-<type>` (for example `linux-debug`).

### Windows

Install required build tools (PowerShell as Administrator):

```powershell
winget install --id Git.Git -e
winget install --id Kitware.CMake -e
winget install --id LLVM.LLVM -e
winget install --id Ninja-build.Ninja -e
winget install --id Microsoft.VisualStudio.2022.BuildTools -e
```

In Visual Studio Installer, enable the "Desktop development with C++" workload.

Install vcpkg and SFML 3.0 (PowerShell):

```powershell
git clone https://github.com/microsoft/vcpkg.git $env:USERPROFILE\vcpkg
& "$env:USERPROFILE\vcpkg\bootstrap-vcpkg.bat"
& "$env:USERPROFILE\vcpkg\vcpkg.exe" install "sfml:x64-windows@3.0"
```

Important: Windows machines should use a personal `CMakeUserPresets.json` that points to your local vcpkg toolchain. The repository includes `CMakeUserPresets.json.template` — copy this file, update the `CMAKE_TOOLCHAIN_FILE` path (or replace `YourUsername`), and do NOT commit your personal file.

```powershell
copy CMakeUserPresets.json.template CMakeUserPresets.json
# edit CMakeUserPresets.json and set CMAKE_TOOLCHAIN_FILE to C:/Users/<you>/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Configure and build (after creating your `CMakeUserPresets.json`):

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
.\build\windows-debug\my_game.exe
```

Notes:

- The repository `CMakePresets.json` intentionally does not contain machine-specific Windows toolchain paths to avoid duplicate preset names. Keep `CMakeUserPresets.json` local and gitignored.
- The presets expect Ninja as the generator; ensure `Ninja` is installed and on `PATH`.

---

### Linux (Ubuntu/Debian)

Install build tools:

```bash
sudo apt update
sudo apt install -y git cmake ninja-build clang
```

Install SFML 3.0:

```bash
sudo apt install -y libsfml-dev
# or build SFML 3.0 from source: https://github.com/SFML/SFML/releases/tag/3.0
```

Configure and build:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/linux-debug/my_game
```

---

### macOS

If Homebrew is not installed, install it first:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Then install build tools:

```bash
brew install git cmake ninja llvm
```

Install SFML 3.0 (or build from source if Homebrew doesn't provide the required version):

```bash
brew install sfml
# or: follow upstream SFML 3.0 build instructions
```

Configure and build:

```bash
cmake --preset macos-debug
cmake --build --preset macos-debug
./build/macos-debug/my_game
```

---

## VS Code Setup (Optional)

If you're using VS Code:

1. Install extensions:
   - **CMake Tools** (Microsoft)
   - **Clangd** (LLVM)

2. To avoid duplicate IntelliSense diagnostics, set the Microsoft C/C++ extension IntelliSense engine to `Disabled` (optional):

```
Open Settings -> search "IntelliSense" -> set C_Cpp.intelliSenseEngine = Disabled
```

---

## CMake Presets

CMake presets are configuration shortcuts that set up the build environment automatically. They are stored in `CMakePresets.json` (version-controlled) and per-user `CMakeUserPresets.json` (local overrides).

**Available presets:**

- `linux-debug` / `linux-release` – Linux
- `macos-debug` / `macos-release` – macOS

Windows presets live in a per-user `CMakeUserPresets.json` (created from `CMakeUserPresets.json.template`) so each developer can point to their own vcpkg installation. Do not add machine-specific Windows presets to the repository `CMakePresets.json` — CMake will report duplicate preset errors if presets with identical names exist across files.

**Usage:**

```bash
cmake --preset <preset-name>            # Configure (creates/updates build/<preset>)
cmake --build --preset <preset-name>    # Build
```

How they work:

- Configure once: run `cmake --preset <name>` once to set up the build directory. Re-run only when dependencies/CMake files change.
- Build repeatedly: run `cmake --build --preset <name>` for fast incremental builds.

---

## Windows: CMakeUserPresets.json with vcpkg

Windows users need to create a personal `CMakeUserPresets.json` file (this file should be ignored by Git):

1. Copy `CMakeUserPresets.json.template` to `CMakeUserPresets.json` in the repository root.
2. Edit `CMakeUserPresets.json` and set the `CMAKE_TOOLCHAIN_FILE` path to your local vcpkg installation, for example:

```json
"CMAKE_TOOLCHAIN_FILE": "C:/Users/christian/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

3. Then configure and build using `windows-debug` / `windows-release` presets.

---

## Code Quality: clang-tidy and clang-format

**clang-tidy** checks code for issues. **clang-format** automatically reformats code.

Both are available as CMake targets:

```bash
cmake --build --preset <preset> --target format      # Format code
cmake --build --preset <preset> --target clang-tidy  # Check for issues
```

Note: `clang-tidy` can be slow; run it in CI or before submitting changes.

