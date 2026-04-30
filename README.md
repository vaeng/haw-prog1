# MyGame Setup Guide (Windows, Ubuntu, macOS)

This guide is for students starting from zero:
- no VS Code
- no compiler
- no CMake
- no build tools

It gives two setup tracks per OS:
1. Minimal: simplest path to build and run.
2. Advanced: the full modern workflow (Ninja + CMake presets + Clang + clang-tidy + sccache + VS Code integration).

The project uses:
- C++20
- CMake 3.25+
- SFML 3

## 1) Quick Choose

If you are new, start with Minimal for your OS.

If you already know C++ tooling (or want the fastest workflow), use Advanced for your OS.

## 2) Common Build Commands

From the repository root:

- Configure:
  - cmake --preset <configure-preset>
- Build:
  - cmake --build --preset <build-preset>
- Run game:
  - Windows: .\\build\\debug\\game.exe
  - Linux/macOS: ./build/debug/game

## 3) Windows

### 3A) Windows Minimal (Console Only)

This is the easiest reliable setup from scratch.

Install tools (PowerShell as Administrator):

```powershell
winget install --id Git.Git -e
winget install --id Kitware.CMake -e
winget install --id Microsoft.VisualStudio.2022.BuildTools -e
```

In Visual Studio Installer, include workload:
- Desktop development with C++

Install vcpkg and SFML 3 (PowerShell):

```powershell
git clone https://github.com/microsoft/vcpkg.git $env:USERPROFILE\vcpkg
$env:VCPKG_ROOT = "$env:USERPROFILE\vcpkg"
& "$env:VCPKG_ROOT\bootstrap-vcpkg.bat"
& "$env:VCPKG_ROOT\vcpkg.exe" install sfml:x64-windows
```

Build using classic CMake command (no presets needed):

```powershell
cmake -S . -B build/minimal-windows -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build/minimal-windows --config Debug
.\build\minimal-windows\Debug\game.exe
```

### 3B) Windows Minimal (With VS Code)

Install VS Code:

```powershell
winget install --id Microsoft.VisualStudioCode -e
```

Open folder in VS Code and install extensions:
- CMake Tools
- Clangd

Then run from terminal in VS Code:

```powershell
cmake -S . -B build/minimal-windows -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build/minimal-windows --config Debug
```

### 3C) Windows Advanced (Console)

Install all advanced tools:

```powershell
winget install --id Git.Git -e
winget install --id Kitware.CMake -e
winget install --id LLVM.LLVM -e
winget install --id Ninja-build.Ninja -e
winget install --id Mozilla.sccache -e
winget install --id Microsoft.VisualStudio.2022.BuildTools -e
```

Install vcpkg + SFML 3 if not done already:

```powershell
git clone https://github.com/microsoft/vcpkg.git $env:USERPROFILE\vcpkg
$env:VCPKG_ROOT = "$env:USERPROFILE\vcpkg"
& "$env:VCPKG_ROOT\bootstrap-vcpkg.bat"
& "$env:VCPKG_ROOT\vcpkg.exe" install sfml:x64-windows
```

Use advanced cached preset:

```powershell
cmake --preset windows-dev
cmake --build --preset windows-dev
.\build\debug\game.exe
```

### 3D) Windows Advanced (With VS Code)

Install VS Code and extensions:
- CMake Tools
- Clangd
- clang-format (optional)
- CodeLLDB (optional, debugger)

Open this repository. The user-level VS Code settings can select:
- cmake.configurePreset = windows-local-dev
- cmake.buildPreset = windows-local-dev
- cmake.testPreset = windows-local-dev

Then use CMake Tools buttons:
- Configure
- Build
- Run

## 4) Ubuntu Linux

The commands below use Ubuntu/Debian style package names.

### 4A) Ubuntu Minimal (Console Only)

Install tools:

```bash
sudo apt update
sudo apt install -y git build-essential cmake
```

Install vcpkg and SFML 3:

```bash
git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg
export VCPKG_ROOT="$HOME/vcpkg"
$VCPKG_ROOT/bootstrap-vcpkg.sh
$VCPKG_ROOT/vcpkg install sfml:x64-linux
```

Build with plain CMake:

```bash
cmake -S . -B build/minimal-linux -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build/minimal-linux
./build/minimal-linux/game
```

### 4B) Ubuntu Minimal (With VS Code)

Install VS Code (Snap variant):

```bash
sudo snap install code --classic
```

Install extensions in VS Code:
- CMake Tools
- Clangd

Build from VS Code terminal using the same minimal commands above.

### 4C) Ubuntu Advanced (Console)

Install advanced tools:

```bash
sudo apt update
sudo apt install -y git cmake ninja-build clang clang-tidy lldb
```

Install sccache:

```bash
cargo install sccache
```

Note: cargo comes from Rust toolchain. Install first if needed:

```bash
curl https://sh.rustup.rs -sSf | sh
source "$HOME/.cargo/env"
cargo install sccache
```

Install vcpkg + SFML 3:

```bash
git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg
export VCPKG_ROOT="$HOME/vcpkg"
$VCPKG_ROOT/bootstrap-vcpkg.sh
$VCPKG_ROOT/vcpkg install sfml:x64-linux
```

Use advanced cached preset:

```bash
cmake --preset linux-dev
cmake --build --preset linux-dev
./build/debug/game
```

### 4D) Ubuntu Advanced (With VS Code)

Install VS Code and extensions:
- CMake Tools
- Clangd
- CodeLLDB (optional)

In VS Code, select these presets from CMake Tools UI:
- Configure preset: linux-dev
- Build preset: linux-dev

Then Configure and Build from the status bar.

## 5) macOS

### 5A) macOS Minimal (Console Only)

Install Xcode Command Line Tools:

```bash
xcode-select --install
```

Install Homebrew (if missing):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Install tools:

```bash
brew install git cmake
```

Install vcpkg + SFML 3:

```bash
git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg
export VCPKG_ROOT="$HOME/vcpkg"
$VCPKG_ROOT/bootstrap-vcpkg.sh
$VCPKG_ROOT/vcpkg install sfml:arm64-osx
```

Build with plain CMake:

```bash
cmake -S . -B build/minimal-macos -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build/minimal-macos
./build/minimal-macos/game
```

### 5B) macOS Minimal (With VS Code)

Install VS Code:

```bash
brew install --cask visual-studio-code
```

Install extensions:
- CMake Tools
- Clangd

Build from VS Code terminal using the same minimal commands above.

### 5C) macOS Advanced (Console)

Install advanced tools:

```bash
xcode-select --install
brew install git cmake ninja llvm
brew install --cask visual-studio-code
```

Install clang-tidy and sccache:

```bash
brew install llvm
brew install sccache
```

Install vcpkg + SFML 3:

```bash
git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg
export VCPKG_ROOT="$HOME/vcpkg"
$VCPKG_ROOT/bootstrap-vcpkg.sh
$VCPKG_ROOT/vcpkg install sfml:arm64-osx
```

Use advanced cached preset:

```bash
cmake --preset macos-dev
cmake --build --preset macos-dev
./build/debug/game
```

### 5D) macOS Advanced (With VS Code)

Install extensions:
- CMake Tools
- Clangd
- CodeLLDB (optional)

In CMake Tools select:
- Configure preset: macos-dev
- Build preset: macos-dev

Then Configure and Build.

## 6) No CMake Fallback (g++ Only)

You can compile without CMake/Ninja, but this is not the recommended team workflow once external libraries are involved.

Why:
- CMake handles dependency detection and linker setup for you.
- With plain g++, each machine must provide manual include and linker flags.

Use this fallback only for emergencies or quick experiments.

### 6A) Linux/macOS (recommended fallback path)

If SFML is installed and pkg-config metadata is available:

```bash
g++ -std=c++20 src/main.cpp -o game $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system sfml-audio)
./game
```

If pkg-config cannot find SFML, install SFML development files correctly first.

### 6B) Windows (possible, but more fragile)

Windows g++ fallback depends on how SFML was installed and where libs are located. A typical MinGW-style command looks like:

```powershell
g++ -std=c++20 src/main.cpp -o game.exe -IC:/path/to/sfml/include -LC:/path/to/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
.\game.exe
```

This works only if library naming/ABI/runtime match your compiler setup.

Team recommendation:
- Use CMake + presets as the standard path.
- Keep g++-only commands as a last-resort fallback.

## 7) Why Ninja With CMake?

Short version:
- CMake decides what to build.
- Ninja executes the build graph very quickly.

Main benefits of Ninja in this project:
- Faster incremental builds:
  - Ninja has very low scheduling overhead.
  - Small changes rebuild quickly.
- Better default behavior for automation:
  - Deterministic and script-friendly output.
  - Works well with CMake presets and CI.
- Cross-platform consistency:
  - Same generator and commands on Windows/Linux/macOS.
  - Fewer "works on my machine" differences.
- Good fit for clang + sccache:
  - Ninja dispatches compile steps efficiently.
  - sccache can reuse cached objects quickly.

Do you need Ninja?
- No, CMake can generate other build systems.
- But for this template, Ninja is a strong default for speed and consistency.

## 8) Notes About Caching

- sccache works on Windows, Linux, and macOS.
- If sccache is missing, use non-sccache presets (same names without -sccache).
- ccache is also common on Linux/macOS, but this project currently standardizes on sccache presets.

## 9) Troubleshooting

### Error: Could not find toolchain file /scripts/buildsystems/vcpkg.cmake

Reason:
- VCPKG_ROOT is not set.

Fix:
- Windows PowerShell:
  - $env:VCPKG_ROOT = "$env:USERPROFILE\vcpkg"
- Linux/macOS:
  - export VCPKG_ROOT="$HOME/vcpkg"

Then configure again.

### Error: CMAKE_MAKE_PROGRAM is not set (Ninja)

Reason:
- Ninja is not installed, but preset expects Ninja.

Fix:
- Install Ninja, or use the Minimal commands without presets.

### Error: sfml not found

Reason:
- SFML dependency not installed in the selected toolchain.

Fix:
- Install via vcpkg for your OS triplet:
  - x64-windows
  - x64-linux
  - arm64-osx

