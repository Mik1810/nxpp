#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
BUILD_DIR="$ROOT_DIR/wasm/build/benchmarks"
mkdir -p "$BUILD_DIR"

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:-"-std=c++20 -O2 -DNDEBUG"}
NATIVE_BIN="$BUILD_DIR/native_overhead_bench"

ITERATIONS=${NXPP_WASM_BENCH_ITERATIONS:-5}
NODES=${NXPP_WASM_BENCH_NODES:-300}
EDGES=${NXPP_WASM_BENCH_EDGES:-900}
FLOYD_NODES=${NXPP_WASM_BENCH_FLOYD_NODES:-35}
ATTR_OPS=${NXPP_WASM_BENCH_ATTR_OPS:-1000}
MULTIGRAPH_EDGES=${NXPP_WASM_BENCH_MULTIGRAPH_EDGES:-600}

ARGS=(
    --iterations "$ITERATIONS"
    --nodes "$NODES"
    --edges "$EDGES"
    --floyd-nodes "$FLOYD_NODES"
    --attr-ops "$ATTR_OPS"
    --multigraph-edges "$MULTIGRAPH_EDGES"
)

echo "[BENCH] Building Node-compatible wasm module" >&2
bash "$ROOT_DIR/wasm/scripts/build_wasm_node_module.sh" >&2

echo "[BENCH] Building native benchmark with $CXX" >&2
"$CXX" $CXXFLAGS "$ROOT_DIR/wasm/benchmarks/native_overhead_bench.cpp" -I"$ROOT_DIR" -o "$NATIVE_BIN"

echo "[BENCH] Running native benchmark" >&2
"$NATIVE_BIN" "${ARGS[@]}"

echo "[BENCH] Running Node wasm benchmark" >&2
node "$ROOT_DIR/wasm/benchmarks/node_overhead_bench.mjs" "${ARGS[@]}" | sed '1d'
