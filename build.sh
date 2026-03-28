#!/bin/zsh

PROJECT_DIR="$HOME/Desktop/pico-console"
BUILD_DIR="$PROJECT_DIR/build"

if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    mkdir -p "$BUILD_DIR"
    cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DPICO_BOARD=pico2_w
fi

cmake --build "$BUILD_DIR" -j$(nproc)

picotool load "$BUILD_DIR/pico-console.elf" -fx
