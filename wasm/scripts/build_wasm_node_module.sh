#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
BUILD_DIR="$ROOT_DIR/wasm/build"
GENERATED_DIR="$ROOT_DIR/wasm/generated"
mkdir -p "$BUILD_DIR"

EMXX=${EMXX:-em++}
EMXXFLAGS=${EMXXFLAGS:-"-std=c++20 -Wall -Wextra -pedantic -O1 -fexceptions -sDISABLE_EXCEPTION_CATCHING=0 -sEXPORTED_RUNTIME_METHODS=getExceptionMessage,decrementExceptionRefcount -sENVIRONMENT=node -sALLOW_MEMORY_GROWTH=1 -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXIT_RUNTIME=1"}
BOOST_INCLUDE=${BOOST_INCLUDE:-/usr/include}
NXPP_WASM_EMIT_TSD=${NXPP_WASM_EMIT_TSD:-0}

SOURCE_ROOT="$ROOT_DIR/wasm/src"
OUTPUT_MODULE="$BUILD_DIR/nxpp_node.mjs"
RAW_DECLARATION="$GENERATED_DIR/nxpp_node.raw.d.ts"

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

TSD_FLAGS=()
if [ "$NXPP_WASM_EMIT_TSD" = "1" ]; then
    mkdir -p "$GENERATED_DIR"
    TSC_BIN_DIR="$ROOT_DIR/wasm/node_modules/.bin"
    if [ ! -x "$TSC_BIN_DIR/tsc" ] && ! command -v tsc >/dev/null 2>&1; then
        echo "[WASM-NODE] tsc not found. Run 'npm --prefix wasm ci' before emitting declarations." >&2
        exit 1
    fi
    export PATH="$TSC_BIN_DIR:$PATH"
    TSD_FLAGS=(--emit-tsd "$RAW_DECLARATION")
fi

$EMXX $EMXXFLAGS "${SOURCE_FILES[@]}" -I"$ROOT_DIR" -I"$ROOT_DIR/wasm/include" -idirafter "$BOOST_INCLUDE" --bind "${TSD_FLAGS[@]}" -o "$OUTPUT_MODULE"

echo "[WASM-NODE] module built at $OUTPUT_MODULE"
if [ "$NXPP_WASM_EMIT_TSD" = "1" ]; then
    echo "[WASM-NODE] raw TypeScript declaration emitted at $RAW_DECLARATION"
fi
