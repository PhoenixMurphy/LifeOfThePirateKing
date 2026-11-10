@echo off
rem Build (Debug) and run. Double-click this file or run it from a terminal.
setlocal
cd /d "%~dp0"
call build.bat
if errorlevel 1 (
    pause
    exit /b 1
)
build\bin\Game.exe
