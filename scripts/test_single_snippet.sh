#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SNIPPET_ROOT="$ROOT_DIR/snippet"
LOG_ROOT="$ROOT_DIR/logs/snippet"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"

usage() {
  cat <<'EOF'
Usage:
  scripts/test_single_snippet.sh <snippet_folder> [stdin_file]

Examples:
  scripts/test_single_snippet.sh bfs
  scripts/test_single_snippet.sh 2sat
  scripts/test_single_snippet.sh 2sat /path/to/input.txt
EOF
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
  usage
  exit 1
fi

SNIPPET_NAME="$1"
USER_STDIN_FILE="${2:-}"
CASE_DIR="$SNIPPET_ROOT/$SNIPPET_NAME"

resolve_case_basename() {
  local case_dir="$1"
  local preferred="$2"

  if [[ -f "$case_dir/$preferred.cpp" && -f "$case_dir/$preferred.py" && -f "$case_dir/${preferred}_nxpp.cpp" ]]; then
    echo "$preferred"
    return
  fi

  local candidate
  for cpp_path in "$case_dir"/*.cpp; do
    [[ -e "$cpp_path" ]] || continue
    candidate="$(basename "$cpp_path" .cpp)"
    [[ "$candidate" == *_nxpp ]] && continue
    if [[ -f "$case_dir/$candidate.py" && -f "$case_dir/${candidate}_nxpp.cpp" ]]; then
      echo "$candidate"
      return
    fi
  done

  return 1
}

CASE_BASENAME="$(resolve_case_basename "$CASE_DIR" "$SNIPPET_NAME")" || {
  echo "ERROR: could not resolve snippet basename inside $CASE_DIR" >&2
  exit 1
}

CPP_FILE="$CASE_DIR/$CASE_BASENAME.cpp"
PY_FILE="$CASE_DIR/$CASE_BASENAME.py"
NXPP_FILE="$CASE_DIR/${CASE_BASENAME}_nxpp.cpp"

RUN_DIR="$LOG_ROOT/${SNIPPET_NAME}_${TIMESTAMP}"
TMP_DIR="$(mktemp -d)"
LOG_FILE="$RUN_DIR/run.log"

CPP_BIN="$TMP_DIR/${CASE_BASENAME}_cpp.out"
NXPP_BIN="$TMP_DIR/${CASE_BASENAME}_nxpp.out"
CPP_OUT="$RUN_DIR/${CASE_BASENAME}.cpp.out"
PY_OUT="$RUN_DIR/${CASE_BASENAME}.py.out"
NXPP_OUT="$RUN_DIR/${CASE_BASENAME}_nxpp.cpp.out"
DIFF_CPP_PY="$RUN_DIR/diff_cpp_vs_py.diff"
DIFF_CPP_NXPP="$RUN_DIR/diff_cpp_vs_nxpp.diff"
DIFF_PY_NXPP="$RUN_DIR/diff_py_vs_nxpp.diff"

mkdir -p "$RUN_DIR"

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

log() {
  echo "$@" | tee -a "$LOG_FILE"
}

fail() {
  log "ERROR: $*"
  exit 1
}

resolve_python() {
  if [[ -x "$ROOT_DIR/.venv/bin/python" ]]; then
    echo "$ROOT_DIR/.venv/bin/python"
    return
  fi
  if command -v python3 >/dev/null 2>&1; then
    command -v python3
    return
  fi
  if command -v python >/dev/null 2>&1; then
    command -v python
    return
  fi
  return 1
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

compile_and_capture() {
  local label="$1"
  local stderr_file="$2"
  shift 2

  local timing_file
  timing_file="$(mktemp)"

  /usr/bin/time -f "%e" -o "$timing_file" "$@" 2> "$stderr_file"

  local seconds
  seconds="$(cat "$timing_file")"
  rm -f "$timing_file"
  log "TIME $label: ${seconds}s"
}

run_and_capture() {
  local label="$1"
  local stdout_file="$2"
  local stderr_file="$3"
  local stdin_file="$4"
  shift 4

  local timing_file
  timing_file="$(mktemp)"

  if [[ -n "$stdin_file" ]]; then
    /usr/bin/time -f "%e" -o "$timing_file" "$@" < "$stdin_file" > "$stdout_file" 2> "$stderr_file"
  else
    /usr/bin/time -f "%e" -o "$timing_file" "$@" > "$stdout_file" 2> "$stderr_file"
  fi

  local seconds
  seconds="$(cat "$timing_file")"
  rm -f "$timing_file"
  log "TIME $label: ${seconds}s"
}

compare_pair() {
  local label="$1"
  local left_name="$2"
  local left_file="$3"
  local right_name="$4"
  local right_file="$5"
  local diff_file="$6"

  if diff -u "$left_file" "$right_file" > "$diff_file"; then
    log "OK $label: $left_name matches $right_name"
  else
    log "FAIL $label: $left_name differs from $right_name"
    log "Saved diff: $diff_file"
    sed -n '1,80p' "$diff_file" | tee -a "$LOG_FILE"
    return 1
  fi
}

detect_stdin_file() {
  if [[ -n "$USER_STDIN_FILE" ]]; then
    [[ -f "$USER_STDIN_FILE" ]] || fail "stdin file not found: $USER_STDIN_FILE"
    echo "$USER_STDIN_FILE"
    return
  fi

  if [[ "$SNIPPET_NAME" == "2sat" ]]; then
    local auto_input="$RUN_DIR/${SNIPPET_NAME}.stdin.txt"
    cat > "$auto_input" <<'EOF'
2 2
1 2
-1 2
EOF
    echo "$auto_input"
    return
  fi

  echo ""
}

[[ -d "$CASE_DIR" ]] || fail "snippet folder not found: $CASE_DIR"
[[ -f "$CPP_FILE" ]] || fail "missing source file: $CPP_FILE"
[[ -f "$PY_FILE" ]] || fail "missing source file: $PY_FILE"
[[ -f "$NXPP_FILE" ]] || fail "missing source file: $NXPP_FILE"

PYTHON_BIN="$(resolve_python)" || fail "no Python interpreter found"
STDIN_FILE="$(detect_stdin_file)"

: > "$LOG_FILE"
log "NXPP snippet triplet test"
log "Snippet: $SNIPPET_NAME"
log "Basename: $CASE_BASENAME"
log "Case dir: $CASE_DIR"
log "Log dir: $RUN_DIR"
log "Python: $PYTHON_BIN"
if [[ -n "$STDIN_FILE" ]]; then
  log "stdin: $STDIN_FILE"
else
  log "stdin: <none>"
fi

log ""
log "Compiling C++ reference"
log "CMD: g++ -std=c++20 -Wall -Wextra -pedantic -O3 $CPP_FILE -o $CPP_BIN"
compile_and_capture "compile ${CASE_BASENAME}.cpp" "$RUN_DIR/${CASE_BASENAME}.compile.err" \
  g++ -std=c++20 -Wall -Wextra -pedantic -O3 "$CPP_FILE" -o "$CPP_BIN"
log "compile stderr: $RUN_DIR/${CASE_BASENAME}.compile.err"

log ""
log "Compiling nxpp variant"
log "CMD: g++ -std=c++20 -Wall -Wextra -pedantic -O3 $NXPP_FILE -I$ROOT_DIR -o $NXPP_BIN"
compile_and_capture "compile ${CASE_BASENAME}_nxpp.cpp" "$RUN_DIR/${CASE_BASENAME}_nxpp.compile.err" \
  g++ -std=c++20 -Wall -Wextra -pedantic -O3 "$NXPP_FILE" -I"$ROOT_DIR" -o "$NXPP_BIN"
log "compile stderr: $RUN_DIR/${CASE_BASENAME}_nxpp.compile.err"

log ""
log "Running reference C++"
run_and_capture "${CASE_BASENAME}.cpp" \
  "$CPP_OUT" "$RUN_DIR/${CASE_BASENAME}.cpp.err" "$STDIN_FILE" "$CPP_BIN"
log "stdout: $CPP_OUT"
log "stderr: $RUN_DIR/${CASE_BASENAME}.cpp.err"

log ""
log "Running Python reference"
run_and_capture "${CASE_BASENAME}.py" \
  "$PY_OUT" "$RUN_DIR/${CASE_BASENAME}.py.err" "$STDIN_FILE" "$PYTHON_BIN" -B "$PY_FILE"
log "stdout: $PY_OUT"
log "stderr: $RUN_DIR/${CASE_BASENAME}.py.err"

log ""
log "Running nxpp C++"
run_and_capture "${CASE_BASENAME}_nxpp.cpp" \
  "$NXPP_OUT" "$RUN_DIR/${CASE_BASENAME}_nxpp.cpp.err" "$STDIN_FILE" "$NXPP_BIN"
log "stdout: $NXPP_OUT"
log "stderr: $RUN_DIR/${CASE_BASENAME}_nxpp.cpp.err"

log ""
log "Comparing outputs"

all_ok=true
compare_pair "$SNIPPET_NAME" "cpp" "$CPP_OUT" "py" "$PY_OUT" "$DIFF_CPP_PY" || all_ok=false
compare_pair "$SNIPPET_NAME" "cpp" "$CPP_OUT" "nxpp" "$NXPP_OUT" "$DIFF_CPP_NXPP" || all_ok=false
compare_pair "$SNIPPET_NAME" "py" "$PY_OUT" "nxpp" "$NXPP_OUT" "$DIFF_PY_NXPP" || all_ok=false

log ""
log "Artifacts saved in: $RUN_DIR"

if [[ "$all_ok" == true ]]; then
  log "RESULT: PASS"
else
  log "RESULT: FAIL"
  exit 1
fi
