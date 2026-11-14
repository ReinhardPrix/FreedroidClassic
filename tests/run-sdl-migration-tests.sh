#!/bin/sh
set -eu

cd "$(dirname "$0")/.."
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM

${CC:-cc} -I. -Isrc -DSDL_GetTicks=Test_GetTicks \
  $(pkg-config --cflags sdl3 sdl3-image sdl3-mixer) \
  -Wall -Wextra -g -Og -fsanitize=address,undefined -fno-omit-frame-pointer \
  -ffunction-sections -fdata-sections \
  tests/sdl_migration.c src/input.c \
  -Wl,--gc-sections $(pkg-config --libs sdl3) -o "$build/sdl-migration-test"
"$build/sdl-migration-test"
