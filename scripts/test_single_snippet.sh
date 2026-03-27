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
RUN_DIR="$LOG_ROOT/${SNIPPET_NAME}_${TIMESTAMP}"
LOG_FILE="$RUN_DIR/run.log"
TMP_DIR="$(mktemp -d)"

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
  local left_name="$1"
  local left_file="$2"
  local right_name="$3"
  local right_file="$4"
  local diff_file="$5"

  if diff -u "$left_file" "$right_file" > "$diff_file"; then
    log "OK compare: $left_name matches $right_name"
  else
    log "FAIL compare: $left_name differs from $right_name"
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

sanitize_name() {
  echo "$1" | tr '/ .' '___'
}

[[ -d "$CASE_DIR" ]] || fail "snippet folder not found: $CASE_DIR"
mkdir -p "$RUN_DIR"

PYTHON_BIN="$(resolve_python)" || fail "no Python interpreter found"
STDIN_FILE="$(detect_stdin_file)"

mapfile -t CPP_FILES < <(find "$CASE_DIR" -maxdepth 1 -type f -name '*.cpp' | sort)
mapfile -t PY_FILES < <(find "$CASE_DIR" -maxdepth 1 -type f -name '*.py' | sort)

IMPLEMENTATION_COUNT=$(( ${#CPP_FILES[@]} + ${#PY_FILES[@]} ))
[[ $IMPLEMENTATION_COUNT -ge 2 ]] || fail "need at least two runnable implementations in $CASE_DIR"

declare -a LABELS=()
declare -a OUTPUTS=()

: > "$LOG_FILE"
log "NXPP snippet implementation test"
log "Snippet: $SNIPPET_NAME"
log "Case dir: $CASE_DIR"
log "Log dir: $RUN_DIR"
log "Python: $PYTHON_BIN"
if [[ -n "$STDIN_FILE" ]]; then
  log "stdin: $STDIN_FILE"
else
  log "stdin: <none>"
fi

for source_file in "${CPP_FILES[@]}"; do
  local_name="$(basename "$source_file")"
  stem="${local_name%.cpp}"
  bin_file="$TMP_DIR/${stem}.out"
  compile_err="$RUN_DIR/${local_name}.compile.err"
  stdout_file="$RUN_DIR/${local_name}.out"
  stderr_file="$RUN_DIR/${local_name}.err"

  log ""
  log "Compiling $local_name"
  log "CMD: g++ -std=c++20 -Wall -Wextra -pedantic -O3 $source_file -I$ROOT_DIR -o $bin_file"
  compile_and_capture "compile $local_name" "$compile_err" \
    g++ -std=c++20 -Wall -Wextra -pedantic -O3 "$source_file" -I"$ROOT_DIR" -o "$bin_file"
  log "compile stderr: $compile_err"

  log ""
  log "Running $local_name"
  run_and_capture "$local_name" "$stdout_file" "$stderr_file" "$STDIN_FILE" "$bin_file"
  log "stdout: $stdout_file"
  log "stderr: $stderr_file"

  LABELS+=("$local_name")
  OUTPUTS+=("$stdout_file")
done

for source_file in "${PY_FILES[@]}"; do
  local_name="$(basename "$source_file")"
  stdout_file="$RUN_DIR/${local_name}.out"
  stderr_file="$RUN_DIR/${local_name}.err"

  log ""
  log "Running $local_name"
  run_and_capture "$local_name" "$stdout_file" "$stderr_file" "$STDIN_FILE" "$PYTHON_BIN" -B "$source_file"
  log "stdout: $stdout_file"
  log "stderr: $stderr_file"

  LABELS+=("$local_name")
  OUTPUTS+=("$stdout_file")
done

log ""
log "Comparing outputs"

all_ok=true
for ((i = 0; i < ${#LABELS[@]}; ++i)); do
  for ((j = i + 1; j < ${#LABELS[@]}; ++j)); do
    left_label="${LABELS[i]}"
    right_label="${LABELS[j]}"
    left_output="${OUTPUTS[i]}"
    right_output="${OUTPUTS[j]}"
    diff_file="$RUN_DIR/diff_$(sanitize_name "$left_label")_vs_$(sanitize_name "$right_label").diff"
    compare_pair "$left_label" "$left_output" "$right_label" "$right_output" "$diff_file" || all_ok=false
  done
done

log ""
log "Artifacts saved in: $RUN_DIR"

if [[ "$all_ok" == true ]]; then
  log "RESULT: PASS"
else
  log "RESULT: FAIL"
  exit 1
fi
