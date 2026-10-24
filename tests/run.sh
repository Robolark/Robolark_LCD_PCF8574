#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
binary=$(mktemp)
trap 'rm -f "$binary"' EXIT
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -Itests/fakes -Isrc src/Robolark_LCD_PCF8574.cpp tests/test.cpp -o "$binary"
"$binary"
