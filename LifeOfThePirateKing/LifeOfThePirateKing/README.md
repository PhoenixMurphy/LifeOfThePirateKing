# Empty raylib project (CMake)

C++17 + raylib 6.2. raylib's source is included in `external/raylib` and is built
automatically, so nothing needs downloading. `src/main.cpp` just opens a blank window.

## Windows setup (one time)

Pick **one**:

- **WinLibs GCC (recommended).** In PowerShell run
  `winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT`
  then close and reopen the terminal / VS Code. Gives you GCC, GDB, CMake and Ninja.
- **Visual Studio** (Community is free) with the **"Desktop development with C++"** workload.

For VS Code, install the **C/C++** extension (`ms-vscode.cpptools`).

## Build and run

| | Windows | Linux |
|---|---|---|
| Build and run | double-click `run.bat` | `./run.sh` |
| Build only | `build.bat` / `build.bat release` | `./build.sh` / `./build.sh release` |
| Clean | `build.bat clean` | `./build.sh clean` |

Output: `build/bin/Game.exe`, with `assets/` copied next to it.

**VS Code:** Ctrl+Shift+B builds. F5 builds and debugs; choose "Debug - GCC / GDB"
(WinLibs) or "Debug - Visual Studio (MSVC)" in the Run and Debug panel.

## Notes

- Add `.cpp` files anywhere under `src/`. They're picked up on the next build.
- Put textures/sounds in `assets/` and load them as `"assets/whatever.png"`.
- To rename the executable, change `Game` in `CMakeLists.txt`, the scripts and `.vscode/`.
- Linux needs: `sudo apt install build-essential cmake libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`
