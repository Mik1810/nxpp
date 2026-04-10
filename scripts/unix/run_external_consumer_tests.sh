#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
BUILD_ROOT="$ROOT_DIR/.tmp/external-consumers"
PREFIX_DIR="$BUILD_ROOT/prefix"

GENERATOR="Ninja"
if ! command -v ninja >/dev/null 2>&1; then
    GENERATOR="Unix Makefiles"
fi

echo "[external-consumers] root: $ROOT_DIR"
echo "[external-consumers] build root: $BUILD_ROOT"
echo "[external-consumers] cmake generator: $GENERATOR"

rm -rf "$BUILD_ROOT"
mkdir -p "$BUILD_ROOT"

run_vendored_cmake_mode() {
    local mode_dir="$BUILD_ROOT/vendored"
    local out_file="$mode_dir/output.txt"

    echo
    echo "Mode: vendored-cmake-add_subdirectory"
    cmake -S "$ROOT_DIR/tests/external_consumers/vendored_cmake" \
          -B "$mode_dir" \
          -G "$GENERATOR" \
          -DCMAKE_BUILD_TYPE=Release \
          -DNXPP_ROOT="$ROOT_DIR"
    cmake --build "$mode_dir"
    "$mode_dir/nxpp_external_vendored" | tee "$out_file"
    grep -qx "vendored-ok" "$out_file"
}

run_installed_package_mode() {
    local install_build_dir="$BUILD_ROOT/nxpp-install"
    local consumer_build_dir="$BUILD_ROOT/installed"
    local out_file="$consumer_build_dir/output.txt"

    echo
    echo "Mode: installed-cmake-find_package"
    cmake -S "$ROOT_DIR" \
          -B "$install_build_dir" \
          -G "$GENERATOR" \
          -DCMAKE_BUILD_TYPE=Release
    cmake --build "$install_build_dir"
    cmake --install "$install_build_dir" --prefix "$PREFIX_DIR"

    cmake -S "$ROOT_DIR/tests/external_consumers/installed_cmake" \
          -B "$consumer_build_dir" \
          -G "$GENERATOR" \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_PREFIX_PATH="$PREFIX_DIR"
    cmake --build "$consumer_build_dir"
    "$consumer_build_dir/nxpp_external_installed" | tee "$out_file"
    grep -qx "installed-ok" "$out_file"
}

run_single_header_mode() {
    local out_bin="$BUILD_ROOT/nxpp_external_single_header"
    local out_file="$BUILD_ROOT/single_header_output.txt"
    local cxx="${CXX:-g++}"

    echo
    echo "Mode: standalone-single-header-consumer"
    bash "$ROOT_DIR/scripts/unix/build_single_header.sh"
    "$cxx" -std=c++20 -Wall -Wextra -pedantic -O0 \
        "$ROOT_DIR/tests/external_consumers/single_header/main.cpp" \
        -I"$ROOT_DIR/dist" \
        -o "$out_bin"
    "$out_bin" | tee "$out_file"
    grep -qx "single-header-ok" "$out_file"
}

run_optional_conan_mode() {
    if [[ "${NXPP_EXTERNAL_CONAN:-0}" != "1" ]]; then
        return 0
    fi

    echo
    echo "Mode: optional-local-conan-consumer"
    if ! command -v conan >/dev/null 2>&1; then
        echo "[external-consumers] conan not found, skipping optional Conan mode"
        return 0
    fi

    conan create "$ROOT_DIR/packaging/conan-center-index/recipes/nxpp/all" \
        --version=1.0.19 \
        --profile:build=default \
        --profile:host=default \
        -s:h compiler.cppstd=20
}

run_vendored_cmake_mode
run_installed_package_mode
run_single_header_mode
run_optional_conan_mode

echo
echo "[external-consumers] all required integration modes passed"
