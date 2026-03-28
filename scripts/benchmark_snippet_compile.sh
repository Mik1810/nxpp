#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SNIPPET_ROOT="$ROOT_DIR/snippet"

usage() {
  cat <<'EOF'
Usage:
  scripts/benchmark_snippet_compile.sh [options]
  scripts/benchmark_snippet_compile.sh <snippet_folder|all> [iterations]

Examples:
  scripts/benchmark_snippet_compile.sh --snippet mcmf_cc
  scripts/benchmark_snippet_compile.sh --snippet mcmf_cc --iterations 5
  scripts/benchmark_snippet_compile.sh --snippet mcmf_cc --opt-level O0
  scripts/benchmark_snippet_compile.sh --all --iterations 3 --csv benchmark/compilation_snippet.csv
  scripts/benchmark_snippet_compile.sh mcmf_cc 5
  scripts/benchmark_snippet_compile.sh all 3

Options:
  --snippet <name>       Benchmark a single snippet folder
  --all                  Benchmark every snippet folder
  --iterations <n>       Number of compile runs per snippet (default: 3)
  --opt-level <level>    Set optimization level: O0, O1, O2, O3, Og, Os, Oz
  --cxxflags "<flags>"   Override full compile flags used after `g++ -std=c++20`
  --csv <path>           Write per-snippet summary rows to CSV
  -h, --help             Show this help

What it does:
  - compiles the C++ snippet implementations in snippet/<snippet_folder>/
  - repeats compilation N times
  - reports per-run times
  - reports mean and median compile times
  - reports nxpp overhead percentage relative to the baseline C++ implementation
  - can benchmark one snippet folder or every snippet folder via `all`

Expected naming:
  - baseline C++: <name>.cpp
  - nxpp C++:     <name>_nxpp.cpp
EOF
}

CXXFLAGS_OVERRIDE=""
DEFAULT_WARN_FLAGS="-Wall -Wextra -pedantic"
OPT_LEVEL="O3"
CSV_PATH=""
POSITIONAL=()
TARGET=""
ITERATIONS="3"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --snippet)
      [[ $# -ge 2 ]] || { echo "ERROR: --snippet requires a value" >&2; exit 1; }
      TARGET="$2"
      shift 2
      ;;
    --all)
      TARGET="all"
      shift
      ;;
    --iterations)
      [[ $# -ge 2 ]] || { echo "ERROR: --iterations requires a value" >&2; exit 1; }
      ITERATIONS="$2"
      shift 2
      ;;
    --opt-level)
      [[ $# -ge 2 ]] || { echo "ERROR: --opt-level requires a value" >&2; exit 1; }
      OPT_LEVEL="$2"
      shift 2
      ;;
    --cxxflags)
      [[ $# -ge 2 ]] || { echo "ERROR: --cxxflags requires a value" >&2; exit 1; }
      CXXFLAGS_OVERRIDE="$2"
      shift 2
      ;;
    --csv)
      [[ $# -ge 2 ]] || { echo "ERROR: --csv requires a path" >&2; exit 1; }
      CSV_PATH="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      POSITIONAL+=("$1")
      shift
      ;;
  esac
done

set -- "${POSITIONAL[@]}"

if [[ -z "$TARGET" && ( $# -lt 1 || $# -gt 2 ) ]]; then
  usage
  exit 1
fi

if [[ -z "$TARGET" && $# -ge 1 ]]; then
  TARGET="$1"
fi

if [[ "$ITERATIONS" == "3" && -z "$CXXFLAGS_OVERRIDE" && $# -eq 2 ]]; then
  ITERATIONS="$2"
fi

if [[ ! "$ITERATIONS" =~ ^[1-9][0-9]*$ ]]; then
  echo "ERROR: iterations must be a positive integer" >&2
  exit 1
fi

if [[ -z "$TARGET" ]]; then
  echo "ERROR: specify --snippet <name> or --all" >&2
  exit 1
fi

if [[ -z "$CXXFLAGS_OVERRIDE" ]]; then
  case "$OPT_LEVEL" in
    O0|O1|O2|O3|Og|Os|Oz)
      CXXFLAGS_OVERRIDE="$DEFAULT_WARN_FLAGS -$OPT_LEVEL"
      ;;
    *)
      echo "ERROR: --opt-level must be one of O0, O1, O2, O3, Og, Os, Oz" >&2
      exit 1
      ;;
  esac
fi

TMP_DIR="$(mktemp -d)"
cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

if [[ -n "$CSV_PATH" ]]; then
  mkdir -p "$(dirname "$CSV_PATH")"
  printf "snippet,iterations,baseline_file,nxpp_file,baseline_mean_s,baseline_median_s,nxpp_mean_s,nxpp_median_s,overhead_mean_pct,overhead_median_pct\n" > "$CSV_PATH"
fi

compile_seconds() {
  local source_file="$1"
  local output_file="$2"
  local timing_file stderr_file
  timing_file="$(mktemp)"
  stderr_file="$(mktemp)"

  # shellcheck disable=SC2086
  /usr/bin/time -f "%e" -o "$timing_file" \
    g++ -std=c++20 $CXXFLAGS_OVERRIDE "$source_file" -I"$ROOT_DIR" -o "$output_file" \
    2>"$stderr_file"

  cat "$timing_file"
  rm -f "$timing_file" "$stderr_file"
}

mean_file() {
  local data_file="$1"
  awk '{ sum += $1; count += 1 } END { if (count == 0) print "0.000"; else printf "%.3f", sum / count }' "$data_file"
}

median_file() {
  local data_file="$1"
  sort -n "$data_file" | awk '
    { a[NR] = $1 }
    END {
      if (NR == 0) {
        print "0.000"
      } else if (NR % 2 == 1) {
        printf "%.3f", a[(NR + 1) / 2]
      } else {
        printf "%.3f", (a[NR / 2] + a[NR / 2 + 1]) / 2
      }
    }'
}

percent_overhead_number() {
  local baseline="$1"
  local candidate="$2"
  awk -v b="$baseline" -v c="$candidate" 'BEGIN {
    if (b == 0) {
      print "n/a"
    } else {
      printf "%.2f", ((c - b) / b) * 100.0
    }
  }'
}

percent_overhead_display() {
  local baseline="$1"
  local candidate="$2"
  local value
  value="$(percent_overhead_number "$baseline" "$candidate")"
  if [[ "$value" == "n/a" ]]; then
    echo "n/a"
  else
    echo "${value}%"
  fi
}

resolve_case_files() {
  local case_dir="$1"
  local snippet_name="$2"
  local baseline_file="" nxpp_file=""
  local preferred_baseline="$case_dir/${snippet_name}.cpp"
  local preferred_nxpp="$case_dir/${snippet_name}_nxpp.cpp"
  mapfile -t cpp_files < <(find "$case_dir" -maxdepth 1 -type f -name '*.cpp' | sort)

  if [[ -f "$preferred_baseline" ]]; then
    baseline_file="$preferred_baseline"
  fi

  if [[ -f "$preferred_nxpp" ]]; then
    nxpp_file="$preferred_nxpp"
  fi

  for file in "${cpp_files[@]}"; do
    local base
    base="$(basename "$file")"
    if [[ "$base" == *_nxpp.cpp ]]; then
      [[ -z "$nxpp_file" ]] && nxpp_file="$file"
    else
      [[ -z "$baseline_file" ]] && baseline_file="$file"
    fi
  done

  if [[ -z "$baseline_file" || -z "$nxpp_file" ]]; then
    return 1
  fi

  printf "%s\n%s\n" "$baseline_file" "$nxpp_file"
}

benchmark_one() {
  local snippet_name="$1"
  local case_dir="$SNIPPET_ROOT/$snippet_name"
  local resolved baseline_file nxpp_file

  if [[ ! -d "$case_dir" ]]; then
    echo "ERROR: snippet folder not found: $case_dir" >&2
    return 1
  fi

  if ! resolved="$(resolve_case_files "$case_dir" "$snippet_name")"; then
    echo "NXPP compile benchmark"
    echo "Snippet: $snippet_name"
    echo "Case dir: $case_dir"
    echo "Iterations: $ITERATIONS"
    echo "CXXFLAGS: $CXXFLAGS_OVERRIDE"
    echo
    echo "Summary"
    echo "  skipped: missing baseline or nxpp C++ file"
    echo
    echo "SKIP $snippet_name: missing baseline or nxpp C++ file" >&2
    return 0
  fi

  baseline_file="$(echo "$resolved" | sed -n '1p')"
  nxpp_file="$(echo "$resolved" | sed -n '2p')"

  local baseline_times="$TMP_DIR/${snippet_name}_baseline.times"
  local nxpp_times="$TMP_DIR/${snippet_name}_nxpp.times"
  : > "$baseline_times"
  : > "$nxpp_times"

  echo "NXPP compile benchmark"
  echo "Snippet: $snippet_name"
  echo "Case dir: $case_dir"
  echo "Iterations: $ITERATIONS"
  echo "Baseline: $(basename "$baseline_file")"
  echo "NXPP: $(basename "$nxpp_file")"
  echo "CXXFLAGS: $CXXFLAGS_OVERRIDE"
  echo

  for ((i = 1; i <= ITERATIONS; ++i)); do
    local baseline_bin="$TMP_DIR/${snippet_name}_baseline_${i}.out"
    local nxpp_bin="$TMP_DIR/${snippet_name}_nxpp_${i}.out"
    local baseline_time nxpp_time overhead

    baseline_time="$(compile_seconds "$baseline_file" "$baseline_bin")"
    nxpp_time="$(compile_seconds "$nxpp_file" "$nxpp_bin")"

    echo "$baseline_time" >> "$baseline_times"
    echo "$nxpp_time" >> "$nxpp_times"

    overhead="$(percent_overhead_display "$baseline_time" "$nxpp_time")"
    printf "Run %d/%d: baseline=%ss nxpp=%ss overhead=%s\n" "$i" "$ITERATIONS" "$baseline_time" "$nxpp_time" "$overhead"
  done

  local baseline_mean baseline_median nxpp_mean nxpp_median overhead_mean overhead_median
  baseline_mean="$(mean_file "$baseline_times")"
  baseline_median="$(median_file "$baseline_times")"
  nxpp_mean="$(mean_file "$nxpp_times")"
  nxpp_median="$(median_file "$nxpp_times")"
  overhead_mean="$(percent_overhead_display "$baseline_mean" "$nxpp_mean")"
  overhead_median="$(percent_overhead_display "$baseline_median" "$nxpp_median")"

  echo
  echo "Summary"
  printf "  baseline mean:   %ss\n" "$baseline_mean"
  printf "  baseline median: %ss\n" "$baseline_median"
  printf "  nxpp mean:       %ss\n" "$nxpp_mean"
  printf "  nxpp median:     %ss\n" "$nxpp_median"
  printf "  overhead mean:   %s\n" "$overhead_mean"
  printf "  overhead median: %s\n" "$overhead_median"
  echo

  if [[ -n "$CSV_PATH" ]]; then
    printf "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" \
      "$snippet_name" \
      "$ITERATIONS" \
      "$(basename "$baseline_file")" \
      "$(basename "$nxpp_file")" \
      "$baseline_mean" \
      "$baseline_median" \
      "$nxpp_mean" \
      "$nxpp_median" \
      "$(percent_overhead_number "$baseline_mean" "$nxpp_mean")" \
      "$(percent_overhead_number "$baseline_median" "$nxpp_median")" \
      >> "$CSV_PATH"
  fi
}

if [[ "$TARGET" == "all" ]]; then
  mapfile -t snippet_dirs < <(find "$SNIPPET_ROOT" -mindepth 1 -maxdepth 1 -type d | sort)
  for dir in "${snippet_dirs[@]}"; do
    benchmark_one "$(basename "$dir")"
  done
  if [[ -n "$CSV_PATH" ]]; then
    echo "CSV written to: $CSV_PATH"
  fi
else
  benchmark_one "$TARGET"
  if [[ -n "$CSV_PATH" ]]; then
    echo "CSV written to: $CSV_PATH"
  fi
fi
