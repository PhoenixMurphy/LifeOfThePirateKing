#!/usr/bin/env bash
# raylib project - Linux build script
#   ./build.sh            Debug build   -> build/bin/Game
#   ./build.sh release    Release build
#   ./build.sh clean      Delete the build folder
set -e
cd "$(dirname "$0")"

CONFIG=Debug
case "${1,,}" in
    release) CONFIG=Release ;;
    clean)   rm -rf build; echo "Build folder removed."; exit 0 ;;
esac

GEN=()
if [ ! -f build/CMakeCache.txt ] && command -v ninja >/dev/null 2>&1; then
    GEN=(-G Ninja)
fi

cmake -S . -B build "${GEN[@]}" -DCMAKE_BUILD_TYPE="$CONFIG"
cmake --build build --config "$CONFIG" --parallel

echo
echo "Build OK ($CONFIG):  build/bin/Game"
