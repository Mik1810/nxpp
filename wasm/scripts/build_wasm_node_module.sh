#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
BUILD_DIR="$ROOT_DIR/wasm/build"
mkdir -p "$BUILD_DIR"

EMXX=${EMXX:-em++}
EMXXFLAGS=${EMXXFLAGS:-"-std=c++20 -Wall -Wextra -pedantic -O1 -fexceptions -sDISABLE_EXCEPTION_CATCHING=0 -sENVIRONMENT=node -sALLOW_MEMORY_GROWTH=1 -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXIT_RUNTIME=1"}
BOOST_INCLUDE=${BOOST_INCLUDE:-/usr/include}

SOURCE_ROOT="$ROOT_DIR/wasm/src"
OUTPUT_MODULE="$BUILD_DIR/nxpp_node.mjs"

if ! command -v "$EMXX" >/dev/null 2>&1; then
    echo "[WASM-NODE] em++ not found. Install Emscripten or set EMXX to a valid compiler." >&2
    exit 1
fi

echo "[WASM-NODE] Building Node-compatible wasm module with $EMXX"
mapfile -t SOURCE_FILES < <(find "$SOURCE_ROOT" -type f -name '*.cpp' | sort)
if [ "${#SOURCE_FILES[@]}" -eq 0 ]; then
    echo "[WASM-NODE] no source files found under $SOURCE_ROOT" >&2
    exit 1
fi

$EMXX $EMXXFLAGS "${SOURCE_FILES[@]}" -I"$ROOT_DIR" -I"$ROOT_DIR/wasm/include" -idirafter "$BOOST_INCLUDE" --bind -o "$OUTPUT_MODULE"

echo "[WASM-NODE] module built at $OUTPUT_MODULE"
