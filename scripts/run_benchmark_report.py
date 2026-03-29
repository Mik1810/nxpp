#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import math
import shutil
import subprocess
from datetime import UTC, datetime
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parent.parent
BENCHMARK_DIR = ROOT_DIR / "benchmark"
IMGS_DIR = BENCHMARK_DIR / "imgs"
BACKUP_ROOT = ROOT_DIR / "backups" / "benchmark"
SERIAL_SCRIPT = ROOT_DIR / "scripts" / "benchmark_snippet_compile.sh"
PARALLEL_SCRIPT = ROOT_DIR / "scripts" / "benchmark_snippet_compile_parallel.sh"
REPORT_PATH = BENCHMARK_DIR / "BENCHMARK.md"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run the serial benchmark first, then the parallel benchmark, and build a Markdown report with plots."
    )
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--snippet", help="Benchmark a single snippet folder")
    group.add_argument("--all", action="store_true", help="Benchmark every snippet folder")
    parser.add_argument("--iterations", type=int, default=3, help="Compile runs per snippet")
    parser.add_argument(
        "--jobs",
        type=int,
        default=None,
        help="Parallel jobs for the parallel benchmark (default: nproc in the shell script)",
    )
    parser.add_argument("--opt-level", default="O3", help="Optimization level passed to the benchmark scripts")
    parser.add_argument("--cxxflags", help="Override compile flags passed to the benchmark scripts")
    parser.add_argument("target", nargs="?", help="Positional target alternative to --snippet/--all")
    parser.add_argument(
        "positional_iterations",
        nargs="?",
        type=int,
        help="Positional iteration count alternative to --iterations",
    )
    return parser.parse_args()


def resolve_target(args: argparse.Namespace) -> str:
    if args.snippet:
        return args.snippet
    if args.all:
        return "all"
    if args.target:
        return args.target
    return "all"


def resolve_iterations(args: argparse.Namespace) -> int:
    return args.positional_iterations if args.positional_iterations is not None else args.iterations


def ensure_positive(value: int, name: str) -> None:
    if value <= 0:
        raise ValueError(f"{name} must be a positive integer")


def backup_existing_outputs(timestamp: str) -> Path | None:
    BENCHMARK_DIR.mkdir(parents=True, exist_ok=True)
    existing = [entry for entry in BENCHMARK_DIR.iterdir() if entry.name != ".gitkeep"]
    if not existing:
        return None
    backup_dir = BACKUP_ROOT / timestamp
    backup_dir.mkdir(parents=True, exist_ok=True)
    for entry in existing:
        shutil.move(str(entry), str(backup_dir / entry.name))
    return backup_dir


def build_common_cli(target: str, iterations: int, opt_level: str, cxxflags: str | None) -> list[str]:
    cli: list[str] = []
    if target == "all":
        cli.append("--all")
    else:
        cli.extend(["--snippet", target])
    cli.extend(["--iterations", str(iterations), "--opt-level", opt_level])
    if cxxflags:
        cli.extend(["--cxxflags", cxxflags])
    return cli


def run_command(command: list[str]) -> None:
    subprocess.run(command, cwd=ROOT_DIR, check=True)


def read_csv_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as handle:
        return list(csv.DictReader(handle))


def to_float(row: dict[str, str], key: str) -> float:
    return float(row[key])


def mean(values: list[float]) -> float:
    return sum(values) / len(values) if values else 0.0


def median(values: list[float]) -> float:
    if not values:
        return 0.0
    ordered = sorted(values)
    mid = len(ordered) // 2
    if len(ordered) % 2:
        return ordered[mid]
    return (ordered[mid - 1] + ordered[mid]) / 2.0


def stddev(values: list[float]) -> float:
    if not values:
        return 0.0
    avg = mean(values)
    return math.sqrt(sum((value - avg) ** 2 for value in values) / len(values))


def sort_rows(rows: list[dict[str, str]], key: str, reverse: bool = False) -> list[dict[str, str]]:
    return sorted(rows, key=lambda row: to_float(row, key), reverse=reverse)


def paired_rows(serial_rows: list[dict[str, str]], parallel_rows: list[dict[str, str]]) -> list[dict[str, float | str]]:
    parallel_map = {row["snippet"]: row for row in parallel_rows}
    pairs: list[dict[str, float | str]] = []
    for serial_row in serial_rows:
        snippet = serial_row["snippet"]
        parallel_row = parallel_map.get(snippet)
        if parallel_row is None:
            continue
        pairs.append(
            {
                "snippet": snippet,
                "serial_overhead": to_float(serial_row, "overhead_mean_pct"),
                "parallel_overhead": to_float(parallel_row, "overhead_mean_pct"),
                "overhead_delta": to_float(parallel_row, "overhead_mean_pct") - to_float(serial_row, "overhead_mean_pct"),
                "serial_ratio": to_float(serial_row, "nxpp_mean_s") / to_float(serial_row, "baseline_mean_s"),
                "parallel_ratio": to_float(parallel_row, "nxpp_mean_s") / to_float(parallel_row, "baseline_mean_s"),
                "serial_gap": abs(to_float(serial_row, "overhead_mean_pct") - to_float(serial_row, "overhead_median_pct")),
                "parallel_gap": abs(to_float(parallel_row, "overhead_mean_pct") - to_float(parallel_row, "overhead_median_pct")),
            }
        )
    return pairs


def make_svg_bar_chart(
    rows: list[dict[str, str]],
    value_key: str,
    title: str,
    output_path: Path,
    color: str,
    suffix: str,
) -> None:
    width = 1220
    top_margin = 70
    bottom_margin = 40
    left_margin = 220
    right_margin = 90
    row_height = 28
    chart_width = width - left_margin - right_margin
    height = top_margin + bottom_margin + row_height * len(rows)

    values = [to_float(row, value_key) for row in rows]
    min_value = min(0.0, min(values) if values else 0.0)
    max_value = max(0.0, max(values) if values else 0.0)
    span = max(max_value - min_value, 1.0)
    zero_x = left_margin + ((0.0 - min_value) / span) * chart_width

    lines = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
        "<style>",
        'text { font-family: "Segoe UI", Arial, sans-serif; fill: #1f2937; }',
        '.title { font-size: 24px; font-weight: 700; }',
        '.label { font-size: 14px; }',
        '.value { font-size: 13px; fill: #374151; }',
        '.axis { stroke: #94a3b8; stroke-width: 1.2; }',
        "</style>",
        f'<text x="{left_margin}" y="38" class="title">{title}</text>',
        f'<line x1="{zero_x:.1f}" y1="{top_margin - 12}" x2="{zero_x:.1f}" y2="{height - bottom_margin + 10}" class="axis"/>',
    ]

    for index, row in enumerate(rows):
        value = to_float(row, value_key)
        y = top_margin + index * row_height
        label_y = y + 18
        x_value = left_margin + ((value - min_value) / span) * chart_width
        bar_x = min(zero_x, x_value)
        bar_width = max(abs(x_value - zero_x), 1.0)
        text_x = x_value + 8 if value >= 0 else x_value - 92
        lines.append(f'<text x="{left_margin - 12}" y="{label_y}" text-anchor="end" class="label">{row["snippet"]}</text>')
        lines.append(
            f'<rect x="{bar_x:.1f}" y="{y + 6}" width="{bar_width:.1f}" height="14" fill="{color}" rx="3" ry="3"/>'
        )
        lines.append(f'<text x="{text_x:.1f}" y="{label_y}" class="value">{value:.2f}{suffix}</text>')

    lines.append("</svg>")
    output_path.write_text("\n".join(lines), encoding="utf-8")


def make_compare_svg(serial_rows: list[dict[str, str]], parallel_rows: list[dict[str, str]], output_path: Path) -> None:
    serial_map = {row["snippet"]: to_float(row, "overhead_mean_pct") for row in serial_rows}
    parallel_map = {row["snippet"]: to_float(row, "overhead_mean_pct") for row in parallel_rows}
    snippets = [row["snippet"] for row in sort_rows(serial_rows, "overhead_mean_pct")]

    width = 1400
    top_margin = 70
    bottom_margin = 40
    left_margin = 220
    right_margin = 120
    row_height = 34
    bar_height = 10
    group_gap = 4
    chart_width = width - left_margin - right_margin
    height = top_margin + bottom_margin + row_height * len(snippets)

    values = list(serial_map.values()) + list(parallel_map.values())
    min_value = min(0.0, min(values) if values else 0.0)
    max_value = max(0.0, max(values) if values else 0.0)
    span = max(max_value - min_value, 1.0)
    zero_x = left_margin + ((0.0 - min_value) / span) * chart_width
    serial_color = "#2563eb"
    parallel_color = "#f97316"

    lines = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
        "<style>",
        'text { font-family: "Segoe UI", Arial, sans-serif; fill: #1f2937; }',
        '.title { font-size: 24px; font-weight: 700; }',
        '.label { font-size: 14px; }',
        '.legend { font-size: 13px; }',
        '.axis { stroke: #94a3b8; stroke-width: 1.2; }',
        "</style>",
        f'<text x="{left_margin}" y="38" class="title">Serial vs Parallel Mean Overhead</text>',
        f'<line x1="{zero_x:.1f}" y1="{top_margin - 12}" x2="{zero_x:.1f}" y2="{height - bottom_margin + 10}" class="axis"/>',
        f'<rect x="{width - 210}" y="18" width="16" height="16" fill="{serial_color}" rx="3" ry="3"/>',
        f'<text x="{width - 186}" y="31" class="legend">Serial</text>',
        f'<rect x="{width - 120}" y="18" width="16" height="16" fill="{parallel_color}" rx="3" ry="3"/>',
        f'<text x="{width - 96}" y="31" class="legend">Parallel</text>',
    ]

    for index, snippet in enumerate(snippets):
        y = top_margin + index * row_height
        label_y = y + 20
        serial_value = serial_map[snippet]
        parallel_value = parallel_map.get(snippet, 0.0)
        lines.append(f'<text x="{left_margin - 12}" y="{label_y}" text-anchor="end" class="label">{snippet}</text>')
        for offset, value, color in ((0, serial_value, serial_color), (bar_height + group_gap, parallel_value, parallel_color)):
            x_value = left_margin + ((value - min_value) / span) * chart_width
            bar_x = min(zero_x, x_value)
            bar_width = max(abs(x_value - zero_x), 1.0)
            lines.append(
                f'<rect x="{bar_x:.1f}" y="{y + 4 + offset}" width="{bar_width:.1f}" height="{bar_height}" fill="{color}" rx="3" ry="3"/>'
            )

    lines.append("</svg>")
    output_path.write_text("\n".join(lines), encoding="utf-8")


def make_dual_time_svg(rows: list[dict[str, str]], title: str, output_path: Path) -> None:
    ordered = sorted(rows, key=lambda row: to_float(row, "nxpp_mean_s") - to_float(row, "baseline_mean_s"), reverse=True)
    width = 1400
    top_margin = 70
    bottom_margin = 40
    left_margin = 220
    right_margin = 120
    row_height = 34
    bar_height = 10
    group_gap = 4
    chart_width = width - left_margin - right_margin
    height = top_margin + bottom_margin + row_height * len(ordered)

    values = [to_float(row, "baseline_mean_s") for row in ordered] + [to_float(row, "nxpp_mean_s") for row in ordered]
    max_value = max(max(values) if values else 1.0, 1.0)
    baseline_color = "#0f766e"
    nxpp_color = "#7c3aed"

    lines = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
        "<style>",
        'text { font-family: "Segoe UI", Arial, sans-serif; fill: #1f2937; }',
        '.title { font-size: 24px; font-weight: 700; }',
        '.label { font-size: 14px; }',
        '.legend { font-size: 13px; }',
        "</style>",
        f'<text x="{left_margin}" y="38" class="title">{title}</text>',
        f'<rect x="{width - 220}" y="18" width="16" height="16" fill="{baseline_color}" rx="3" ry="3"/>',
        f'<text x="{width - 196}" y="31" class="legend">Baseline</text>',
        f'<rect x="{width - 120}" y="18" width="16" height="16" fill="{nxpp_color}" rx="3" ry="3"/>',
        f'<text x="{width - 96}" y="31" class="legend">nxpp</text>',
    ]

    for index, row in enumerate(ordered):
        y = top_margin + index * row_height
        label_y = y + 20
        lines.append(f'<text x="{left_margin - 12}" y="{label_y}" text-anchor="end" class="label">{row["snippet"]}</text>')
        for offset, value, color in (
            (0, to_float(row, "baseline_mean_s"), baseline_color),
            (bar_height + group_gap, to_float(row, "nxpp_mean_s"), nxpp_color),
        ):
            bar_width = max(1.0, (value / max_value) * chart_width)
            lines.append(
                f'<rect x="{left_margin}" y="{y + 4 + offset}" width="{bar_width:.1f}" height="{bar_height}" fill="{color}" rx="3" ry="3"/>'
            )
            lines.append(f'<text x="{left_margin + bar_width + 8:.1f}" y="{y + 14 + offset}" class="legend">{value:.2f}s</text>')

    lines.append("</svg>")
    output_path.write_text("\n".join(lines), encoding="utf-8")


def make_pair_delta_svg(pairs: list[dict[str, float | str]], title: str, output_path: Path) -> None:
    rows = sorted(pairs, key=lambda row: float(row["overhead_delta"]))
    width = 1220
    top_margin = 70
    bottom_margin = 40
    left_margin = 220
    right_margin = 90
    row_height = 28
    chart_width = width - left_margin - right_margin
    height = top_margin + bottom_margin + row_height * len(rows)
    values = [float(row["overhead_delta"]) for row in rows]
    min_value = min(0.0, min(values) if values else 0.0)
    max_value = max(0.0, max(values) if values else 0.0)
    span = max(max_value - min_value, 1.0)
    zero_x = left_margin + ((0.0 - min_value) / span) * chart_width

    lines = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
        "<style>",
        'text { font-family: "Segoe UI", Arial, sans-serif; fill: #1f2937; }',
        '.title { font-size: 24px; font-weight: 700; }',
        '.label { font-size: 14px; }',
        '.value { font-size: 13px; fill: #374151; }',
        '.axis { stroke: #94a3b8; stroke-width: 1.2; }',
        "</style>",
        f'<text x="{left_margin}" y="38" class="title">{title}</text>',
        f'<line x1="{zero_x:.1f}" y1="{top_margin - 12}" x2="{zero_x:.1f}" y2="{height - bottom_margin + 10}" class="axis"/>',
    ]

    for index, row in enumerate(rows):
        value = float(row["overhead_delta"])
        color = "#059669" if value <= 0 else "#dc2626"
        y = top_margin + index * row_height
        label_y = y + 18
        x_value = left_margin + ((value - min_value) / span) * chart_width
        bar_x = min(zero_x, x_value)
        bar_width = max(abs(x_value - zero_x), 1.0)
        text_x = x_value + 8 if value >= 0 else x_value - 92
        lines.append(f'<text x="{left_margin - 12}" y="{label_y}" text-anchor="end" class="label">{row["snippet"]}</text>')
        lines.append(
            f'<rect x="{bar_x:.1f}" y="{y + 6}" width="{bar_width:.1f}" height="14" fill="{color}" rx="3" ry="3"/>'
        )
        lines.append(f'<text x="{text_x:.1f}" y="{label_y}" class="value">{value:.2f} pts</text>')

    lines.append("</svg>")
    output_path.write_text("\n".join(lines), encoding="utf-8")


def make_ratio_svg(rows: list[dict[str, str]], title: str, output_path: Path) -> None:
    decorated = [
        {"snippet": row["snippet"], "ratio": to_float(row, "nxpp_mean_s") / to_float(row, "baseline_mean_s")}
        for row in rows
    ]
    decorated.sort(key=lambda row: float(row["ratio"]), reverse=True)
    width = 1220
    top_margin = 70
    bottom_margin = 40
    left_margin = 220
    right_margin = 90
    row_height = 28
    chart_width = width - left_margin - right_margin
    height = top_margin + bottom_margin + row_height * len(decorated)
    max_value = max((float(row["ratio"]) for row in decorated), default=1.0)

    lines = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
        "<style>",
        'text { font-family: "Segoe UI", Arial, sans-serif; fill: #1f2937; }',
        '.title { font-size: 24px; font-weight: 700; }',
        '.label { font-size: 14px; }',
        '.value { font-size: 13px; fill: #374151; }',
        "</style>",
        f'<text x="{left_margin}" y="38" class="title">{title}</text>',
    ]

    for index, row in enumerate(decorated):
        value = float(row["ratio"])
        y = top_margin + index * row_height
        label_y = y + 18
        bar_width = max(1.0, (value / max(max_value, 1.0)) * chart_width)
        lines.append(f'<text x="{left_margin - 12}" y="{label_y}" text-anchor="end" class="label">{row["snippet"]}</text>')
        lines.append(
            f'<rect x="{left_margin}" y="{y + 6}" width="{bar_width:.1f}" height="14" fill="#8b5cf6" rx="3" ry="3"/>'
        )
        lines.append(f'<text x="{left_margin + bar_width + 8:.1f}" y="{label_y}" class="value">{value:.2f}x</text>')

    lines.append("</svg>")
    output_path.write_text("\n".join(lines), encoding="utf-8")


def format_top(rows: list[dict[str, str]], key: str, count: int, suffix: str) -> str:
    ordered = sort_rows(rows, key, reverse=True)[: min(count, len(rows))]
    return "\n".join(f"- `{row['snippet']}`: `{to_float(row, key):.2f}{suffix}`" for row in ordered)


def format_bottom(rows: list[dict[str, str]], key: str, count: int, suffix: str) -> str:
    ordered = sort_rows(rows, key, reverse=False)[: min(count, len(rows))]
    return "\n".join(f"- `{row['snippet']}`: `{to_float(row, key):.2f}{suffix}`" for row in ordered)


def format_pair_entries(pairs: list[dict[str, float | str]], key: str, count: int, reverse: bool, suffix: str) -> str:
    ordered = sorted(pairs, key=lambda row: float(row[key]), reverse=reverse)[: min(count, len(pairs))]
    return "\n".join(f"- `{row['snippet']}`: `{float(row[key]):.2f}{suffix}`" for row in ordered)


def write_report(
    target: str,
    iterations: int,
    jobs: int,
    opt_level: str,
    serial_csv: Path,
    parallel_csv: Path,
    serial_rows: list[dict[str, str]],
    parallel_rows: list[dict[str, str]],
    pairs: list[dict[str, float | str]],
    plots: dict[str, Path],
    backup_dir: Path | None,
) -> None:
    serial_mean_values = [to_float(row, "overhead_mean_pct") for row in serial_rows]
    parallel_mean_values = [to_float(row, "overhead_mean_pct") for row in parallel_rows]
    serial_gap_values = [abs(to_float(row, "overhead_mean_pct") - to_float(row, "overhead_median_pct")) for row in serial_rows]
    parallel_gap_values = [abs(to_float(row, "overhead_mean_pct") - to_float(row, "overhead_median_pct")) for row in parallel_rows]
    serial_gap_rows = [{"snippet": row["snippet"], "gap": f"{abs(to_float(row, 'overhead_mean_pct') - to_float(row, 'overhead_median_pct')):.4f}"} for row in serial_rows]
    parallel_gap_rows = [{"snippet": row["snippet"], "gap": f"{abs(to_float(row, 'overhead_mean_pct') - to_float(row, 'overhead_median_pct')):.4f}"} for row in parallel_rows]

    report = f"""# Benchmark Report

Generated: `{datetime.now(UTC).strftime('%Y-%m-%d %H:%M:%S UTC')}`

## Run Configuration

- target: `{target}`
- iterations: `{iterations}`
- optimization: `{opt_level}`
- parallel jobs: `{jobs}`
- serial CSV: `{serial_csv.name}`
- parallel CSV: `{parallel_csv.name}`
"""
    if backup_dir is not None:
        report += f"- previous benchmark outputs moved to: `{backup_dir.relative_to(ROOT_DIR)}`\n"

    report += f"""
## Statistical Overview

- snippets analyzed: `{len(serial_rows)}`
- serial mean-overhead average: `{mean(serial_mean_values):.2f}%`
- serial mean-overhead median: `{median(serial_mean_values):.2f}%`
- serial overhead stddev across snippets: `{stddev(serial_mean_values):.2f}`
- parallel mean-overhead average: `{mean(parallel_mean_values):.2f}%`
- parallel mean-overhead median: `{median(parallel_mean_values):.2f}%`
- parallel overhead stddev across snippets: `{stddev(parallel_mean_values):.2f}`
- average serial mean-vs-median gap: `{mean(serial_gap_values):.2f}` points
- average parallel mean-vs-median gap: `{mean(parallel_gap_values):.2f}` points

## Serial Benchmark

![Serial mean overhead](imgs/{plots['serial_overhead'].name})

![Serial absolute compile times](imgs/{plots['serial_times'].name})

![Serial nxpp/baseline ratio](imgs/{plots['serial_ratio'].name})

Lowest serial overhead:

{format_bottom(serial_rows, 'overhead_mean_pct', 5, '%')}

Highest serial overhead:

{format_top(serial_rows, 'overhead_mean_pct', 5, '%')}

Fastest serial nxpp compile times:

{format_bottom(serial_rows, 'nxpp_mean_s', 5, 's')}

Slowest serial nxpp compile times:

{format_top(serial_rows, 'nxpp_mean_s', 5, 's')}

## Parallel Benchmark

![Parallel mean overhead](imgs/{plots['parallel_overhead'].name})

![Parallel absolute compile times](imgs/{plots['parallel_times'].name})

![Parallel nxpp/baseline ratio](imgs/{plots['parallel_ratio'].name})

Lowest parallel overhead:

{format_bottom(parallel_rows, 'overhead_mean_pct', 5, '%')}

Highest parallel overhead:

{format_top(parallel_rows, 'overhead_mean_pct', 5, '%')}

Fastest parallel nxpp compile times:

{format_bottom(parallel_rows, 'nxpp_mean_s', 5, 's')}

Slowest parallel nxpp compile times:

{format_top(parallel_rows, 'nxpp_mean_s', 5, 's')}

## Serial vs Parallel Comparison

![Serial vs parallel mean overhead](imgs/{plots['compare_overhead'].name})

![Parallel minus serial overhead delta](imgs/{plots['delta_overhead'].name})

Most improved in parallel:

{format_pair_entries(pairs, 'overhead_delta', 5, False, ' pts')}

Most degraded in parallel:

{format_pair_entries(pairs, 'overhead_delta', 5, True, ' pts')}

## Mean vs Median Consistency

![Serial mean-vs-median gap](imgs/{plots['serial_gap'].name})

![Parallel mean-vs-median gap](imgs/{plots['parallel_gap'].name})

Largest serial mean-vs-median gaps:

{format_top(serial_gap_rows, 'gap', 5, ' pts')}

Largest parallel mean-vs-median gaps:

{format_top(parallel_gap_rows, 'gap', 5, ' pts')}

## Interpretation

- This report intentionally uses only the two final CSVs produced by the serial and parallel benchmark scripts.
- That keeps the output simple: one serial CSV, one parallel CSV, one Markdown report, and plots under `benchmark/imgs/`.
- Even with just those two CSVs, we still get a useful cross-section of the benchmark story: aggregate overhead, absolute compile times, nxpp-to-baseline ratios, serial-vs-parallel deltas, and mean-vs-median consistency per snippet.
- A true per-iteration variance study would need raw run data, but this tool now deliberately avoids generating extra CSV artifacts.
"""
    REPORT_PATH.write_text(report.strip() + "\n", encoding="utf-8")


def main() -> int:
    args = parse_args()
    target = resolve_target(args)
    iterations = resolve_iterations(args)
    jobs = args.jobs if args.jobs is not None else 1
    ensure_positive(iterations, "iterations")
    ensure_positive(jobs, "jobs")

    timestamp = datetime.now(UTC).strftime("%Y%m%d_%H%M%S")
    backup_dir = backup_existing_outputs(timestamp)
    BENCHMARK_DIR.mkdir(parents=True, exist_ok=True)
    IMGS_DIR.mkdir(parents=True, exist_ok=True)

    target_slug = "all" if target == "all" else target
    serial_csv = BENCHMARK_DIR / f"serial_{target_slug}_i{iterations}_{timestamp}.csv"
    parallel_csv = BENCHMARK_DIR / f"parallel_{target_slug}_i{iterations}_j{jobs}_{timestamp}.csv"
    common_cli = build_common_cli(target, iterations, args.opt_level, args.cxxflags)

    run_command(["bash", str(SERIAL_SCRIPT), *common_cli, "--csv", str(serial_csv)])
    run_command(["bash", str(PARALLEL_SCRIPT), *common_cli, "--jobs", str(jobs), "--csv", str(parallel_csv)])

    serial_rows = read_csv_rows(serial_csv)
    parallel_rows = read_csv_rows(parallel_csv)
    pairs = paired_rows(serial_rows, parallel_rows)

    plots = {
        "serial_overhead": IMGS_DIR / f"serial_overhead_{target_slug}_{timestamp}.svg",
        "parallel_overhead": IMGS_DIR / f"parallel_overhead_{target_slug}_{timestamp}.svg",
        "compare_overhead": IMGS_DIR / f"compare_overhead_{target_slug}_{timestamp}.svg",
        "delta_overhead": IMGS_DIR / f"delta_overhead_{target_slug}_{timestamp}.svg",
        "serial_times": IMGS_DIR / f"serial_times_{target_slug}_{timestamp}.svg",
        "parallel_times": IMGS_DIR / f"parallel_times_{target_slug}_{timestamp}.svg",
        "serial_ratio": IMGS_DIR / f"serial_ratio_{target_slug}_{timestamp}.svg",
        "parallel_ratio": IMGS_DIR / f"parallel_ratio_{target_slug}_{timestamp}.svg",
        "serial_gap": IMGS_DIR / f"serial_gap_{target_slug}_{timestamp}.svg",
        "parallel_gap": IMGS_DIR / f"parallel_gap_{target_slug}_{timestamp}.svg",
    }

    make_svg_bar_chart(sort_rows(serial_rows, "overhead_mean_pct"), "overhead_mean_pct", "Serial Mean Overhead", plots["serial_overhead"], "#2563eb", "%")
    make_svg_bar_chart(sort_rows(parallel_rows, "overhead_mean_pct"), "overhead_mean_pct", "Parallel Mean Overhead", plots["parallel_overhead"], "#f97316", "%")
    make_compare_svg(serial_rows, parallel_rows, plots["compare_overhead"])
    make_pair_delta_svg(pairs, "Parallel Minus Serial Mean Overhead", plots["delta_overhead"])
    make_dual_time_svg(serial_rows, "Serial Absolute Compile Times", plots["serial_times"])
    make_dual_time_svg(parallel_rows, "Parallel Absolute Compile Times", plots["parallel_times"])
    make_ratio_svg(serial_rows, "Serial nxpp/Baseline Compile-Time Ratio", plots["serial_ratio"])
    make_ratio_svg(parallel_rows, "Parallel nxpp/Baseline Compile-Time Ratio", plots["parallel_ratio"])
    serial_gap_rows = [{"snippet": row["snippet"], "gap": f"{abs(to_float(row, 'overhead_mean_pct') - to_float(row, 'overhead_median_pct')):.4f}"} for row in serial_rows]
    parallel_gap_rows = [{"snippet": row["snippet"], "gap": f"{abs(to_float(row, 'overhead_mean_pct') - to_float(row, 'overhead_median_pct')):.4f}"} for row in parallel_rows]
    make_svg_bar_chart(sort_rows(serial_gap_rows, "gap"), "gap", "Serial Mean-vs-Median Overhead Gap", plots["serial_gap"], "#dc2626", " pts")
    make_svg_bar_chart(sort_rows(parallel_gap_rows, "gap"), "gap", "Parallel Mean-vs-Median Overhead Gap", plots["parallel_gap"], "#b45309", " pts")

    write_report(target, iterations, jobs, args.opt_level, serial_csv, parallel_csv, serial_rows, parallel_rows, pairs, plots, backup_dir)

    print(f"Serial CSV: {serial_csv.relative_to(ROOT_DIR)}")
    print(f"Parallel CSV: {parallel_csv.relative_to(ROOT_DIR)}")
    print(f"Report: {REPORT_PATH.relative_to(ROOT_DIR)}")
    print(f"Image directory: {IMGS_DIR.relative_to(ROOT_DIR)}")
    if backup_dir is not None:
        print(f"Backed up previous benchmark outputs to: {backup_dir.relative_to(ROOT_DIR)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
