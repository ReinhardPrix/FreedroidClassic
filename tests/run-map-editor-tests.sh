#!/bin/sh
set -eu

cd "$(dirname "$0")/.."
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM

${CC:-cc} -DHAVE_CONFIG_H -I. -Isrc \
  $(pkg-config --cflags sdl3 sdl3-image sdl3-mixer) \
  -Wall -Wextra -g -Og -fsanitize=address,undefined -fno-omit-frame-pointer \
  -ffunction-sections -fdata-sections \
  tests/map_editor.c src/map.c src/level_editor.c src/enemy.c src/fd_memory.c \
  -Wl,--gc-sections -lm -o "$build/map-editor-test"
"$build/map-editor-test"
