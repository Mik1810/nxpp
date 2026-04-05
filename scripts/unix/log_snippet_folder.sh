#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
SNIPPET_ROOT="$ROOT_DIR/snippet"

usage() {
  cat <<'EOF'
Usage:
  scripts/unix/log_snippet_folder.sh <snippet_folder> [stdin_file]

Examples:
  scripts/unix/log_snippet_folder.sh 2sat
  scripts/unix/log_snippet_folder.sh scc
  scripts/unix/log_snippet_folder.sh 2sat /path/to/input.txt
EOF
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
  usage
  exit 1
fi

SNIPPET_NAME="$1"
USER_STDIN_FILE="${2:-}"
CASE_DIR="$SNIPPET_ROOT/$SNIPPET_NAME"
LOG_DIR="$CASE_DIR/logs"
LOG_FILE="$LOG_DIR/run_all.log"
TMP_DIR="$(mktemp -d)"

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

fail() {
  echo "ERROR: $*" >&2
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

detect_stdin_file() {
  if [[ -n "$USER_STDIN_FILE" ]]; then
    [[ -f "$USER_STDIN_FILE" ]] || fail "stdin file not found: $USER_STDIN_FILE"
    echo "$USER_STDIN_FILE"
    return
  fi

  if [[ "$SNIPPET_NAME" == "2sat" ]]; then
    local auto_input="$TMP_DIR/${SNIPPET_NAME}.stdin.txt"
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

run_cpp() {
  local source_file="$1"
  local stem
  stem="$(basename "$source_file" .cpp)"
  local bin_file="$TMP_DIR/${stem}.out"

  g++ -std=c++20 -Wall -Wextra -pedantic -O3 "$source_file" -I"$ROOT_DIR" -o "$bin_file"
  if [[ -n "$STDIN_FILE" ]]; then
    "$bin_file" < "$STDIN_FILE"
  else
    "$bin_file"
  fi
}

run_py() {
  local source_file="$1"
  if [[ -n "$STDIN_FILE" ]]; then
    "$PYTHON_BIN" -B "$source_file" < "$STDIN_FILE"
  else
    "$PYTHON_BIN" -B "$source_file"
  fi
}

[[ -d "$CASE_DIR" ]] || fail "snippet folder not found: $CASE_DIR"
PYTHON_BIN="$(resolve_python)" || fail "no Python interpreter found"
STDIN_FILE="$(detect_stdin_file)"

mapfile -t PY_FILES < <(find "$CASE_DIR" -maxdepth 1 -type f -name '*.py' | sort)
mapfile -t CPP_FILES < <(find "$CASE_DIR" -maxdepth 1 -type f -name '*.cpp' | sort)

[[ ${#PY_FILES[@]} -gt 0 || ${#CPP_FILES[@]} -gt 0 ]] || fail "no runnable snippet files found in $CASE_DIR"

mkdir -p "$LOG_DIR"

{
  for source_file in "${CPP_FILES[@]}"; do
    run_cpp "$source_file"
    echo
  done

  for source_file in "${PY_FILES[@]}"; do
    run_py "$source_file"
    echo
  done
} 2>&1 | tee "$LOG_FILE"
