#!/usr/bin/env bash
# Build (Debug) and run.
set -e
cd "$(dirname "$0")"
./build.sh
./build/bin/Game
