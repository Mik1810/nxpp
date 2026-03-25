#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOG_DIR="$ROOT_DIR/logs"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
LOG_FILE="$LOG_DIR/snippet_test_${TIMESTAMP}.log"
PYTHON_BIN="$ROOT_DIR/.venv/bin/python"

mkdir -p "$LOG_DIR"

log() {
  echo "$@" | tee -a "$LOG_FILE"
}

time_command() {
  local label="$1"
  shift

  local timing_file
  timing_file="$(mktemp)"
  /usr/bin/time -f "%e" -o "$timing_file" "$@"
  local seconds
  seconds="$(cat "$timing_file")"
  rm -f "$timing_file"
  log "TIME $label: ${seconds}s"
}

compile_cpp() {
  local source_file="$1"
  local output_file="$2"
  shift 2
  time_command "compile $(basename "$source_file")" g++ -std=c++20 -Wall -pedantic -O3 "$source_file" -o "$output_file" "$@"
}

run_and_capture() {
  local label="$1"
  local output_file="$2"
  local stdin_file="$3"
  shift 3

  local timing_file
  timing_file="$(mktemp)"

  if [[ -n "$stdin_file" ]]; then
    /usr/bin/time -f "%e" -o "$timing_file" "$@" < "$stdin_file" > "$output_file"
  else
    /usr/bin/time -f "%e" -o "$timing_file" "$@" > "$output_file"
  fi

  local seconds
  seconds="$(cat "$timing_file")"
  rm -f "$timing_file"
  log "TIME $label: ${seconds}s"
}

compare_outputs() {
  local name="$1"
  local left_label="$2"
  local left_file="$3"
  local right_label="$4"
  local right_file="$5"

  if diff -u "$left_file" "$right_file" > /tmp/nxpp_snippet_diff.txt; then
    log "OK $name: $left_label matches $right_label"
  else
    log "FAIL $name: $left_label does not match $right_label"
    sed -n '1,40p' /tmp/nxpp_snippet_diff.txt | tee -a "$LOG_FILE"
    return 1
  fi
}

run_case() {
  local folder="$1"
  local stdin_file="$2"

  local case_dir="$ROOT_DIR/snippet/$folder"
  local cpp_file="$case_dir/$folder.cpp"
  local py_file="$case_dir/$folder.py"
  local nxpp_file="$case_dir/${folder}_nxpp.cpp"

  local tmp_dir
  tmp_dir="$(mktemp -d)"
  local cpp_bin="$tmp_dir/${folder}_cpp.out"
  local nxpp_bin="$tmp_dir/${folder}_nxpp.out"
  local cpp_out="$tmp_dir/${folder}_cpp.txt"
  local py_out="$tmp_dir/${folder}_py.txt"
  local nxpp_out="$tmp_dir/${folder}_nxpp.txt"

  log ""
  log "CASE $folder"
  compile_cpp "$cpp_file" "$cpp_bin"
  compile_cpp "$nxpp_file" "$nxpp_bin" -I"$ROOT_DIR"
  run_and_capture "$folder cpp" "$cpp_out" "$stdin_file" "$cpp_bin"
  run_and_capture "$folder py" "$py_out" "$stdin_file" "$PYTHON_BIN" -B "$py_file"
  run_and_capture "$folder nxpp" "$nxpp_out" "$stdin_file" "$nxpp_bin"
  compare_outputs "$folder" "cpp" "$cpp_out" "py" "$py_out"
  compare_outputs "$folder" "cpp" "$cpp_out" "nxpp" "$nxpp_out"

  rm -rf "$tmp_dir"
}

main() {
  if [[ ! -x "$PYTHON_BIN" ]]; then
    log "ERROR: Python virtualenv interpreter not found at $PYTHON_BIN"
    exit 1
  fi

  local two_sat_input
  two_sat_input="$(mktemp)"
  cat > "$two_sat_input" <<'EOF'
2 2
1 2
-1 2
EOF

  : > "$LOG_FILE"
  log "NXPP snippet test run"
  log "Log file: $LOG_FILE"
  log "Folders: 2sat bellman_ford bfs cc"

  run_case "2sat" "$two_sat_input"
  run_case "bellman_ford" ""
  run_case "bfs" ""
  run_case "cc" ""

  rm -f "$two_sat_input"
  log ""
  log "DONE"
}

main "$@"
