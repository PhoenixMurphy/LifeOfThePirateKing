@echo off
rem ==========================================================================
rem  raylib project - Windows build script
rem
rem    build.bat            Debug build   -> build\bin\Game.exe
rem    build.bat release    Release build
rem    build.bat clean      Delete the build folder
rem
rem  Uses MinGW/GCC (e.g. WinLibs) if gcc is on PATH, otherwise Visual Studio.
rem  See README.md for how to install either.
rem ==========================================================================
setlocal
cd /d "%~dp0"

set "CONFIG=Debug"
if /i "%~1"=="release" set "CONFIG=Release"
if /i "%~1"=="clean" (
    if exist build rmdir /s /q build
    echo Build folder removed.
    exit /b 0
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo.
    echo [ERROR] CMake was not found on PATH.
    echo         Install a compiler + CMake first - see README.md, "Windows setup".
    echo         Quickest:  winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT
    echo         then close and reopen this terminal / VS Code.
    exit /b 1
)

rem Pick a generator only on the very first configure. After that CMake
rem remembers it in build\CMakeCache.txt.
set "GEN="
if exist build\CMakeCache.txt goto configure

where gcc >nul 2>nul
if errorlevel 1 goto try_msvc

where ninja >nul 2>nul
if not errorlevel 1 (
    set GEN=-G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
    echo Using MinGW GCC with Ninja.
    goto configure
)
where mingw32-make >nul 2>nul
if not errorlevel 1 (
    set GEN=-G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
    echo Using MinGW GCC with mingw32-make.
    goto configure
)

:try_msvc
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    echo Using Visual Studio ^(CMake's default generator^).
    goto configure
)
echo.
echo [ERROR] No C++ compiler found ^(no gcc on PATH and no Visual Studio^).
echo         See README.md, "Windows setup".
exit /b 1

:configure
cmake -S . -B build %GEN% -DCMAKE_BUILD_TYPE=%CONFIG%
if errorlevel 1 goto fail

cmake --build build --config %CONFIG% --parallel
if errorlevel 1 goto fail

echo.
echo Build OK (%CONFIG%):  build\bin\Game.exe
exit /b 0

:fail
echo.
echo [ERROR] Build failed - see the messages above.
exit /b 1
