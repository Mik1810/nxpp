#!/usr/bin/env python3
"""Compare nxpp compile benchmark CSV files against a baseline."""

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path


KEY_COLUMNS = ("snippet", "baseline_file", "nxpp_file")
METRIC_COLUMNS = ("nxpp_mean_s", "nxpp_median_s", "overhead_mean_pct", "overhead_median_pct")


def read_rows(path: Path) -> dict[tuple[str, str, str], dict[str, float]]:
    rows: dict[tuple[str, str, str], dict[str, float]] = {}
    with path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        missing = [column for column in (*KEY_COLUMNS, *METRIC_COLUMNS) if column not in (reader.fieldnames or [])]
        if missing:
            raise ValueError(f"{path}: missing columns: {', '.join(missing)}")

        for line_number, row in enumerate(reader, start=2):
            key = tuple(row[column] for column in KEY_COLUMNS)
            if not all(key):
                raise ValueError(f"{path}:{line_number}: empty key column")
            if key in rows:
                raise ValueError(f"{path}:{line_number}: duplicate benchmark row for {key}")
            try:
                rows[key] = {column: float(row[column]) for column in METRIC_COLUMNS}
            except ValueError as exc:
                raise ValueError(f"{path}:{line_number}: non-numeric metric") from exc
    return rows


def regression_ratio(baseline: float, candidate: float) -> float:
    if baseline <= 0:
        return 0.0 if candidate <= baseline else float("inf")
    return candidate / baseline


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline_csv", type=Path)
    parser.add_argument("candidate_csv", type=Path)
    parser.add_argument("--threshold", type=float, default=1.15, help="maximum allowed candidate/baseline ratio")
    args = parser.parse_args()

    if args.threshold < 1.0:
        parser.error("--threshold must be at least 1.0")

    try:
        baseline_rows = read_rows(args.baseline_csv)
        candidate_rows = read_rows(args.candidate_csv)
    except (OSError, ValueError) as exc:
        print(f"benchmark comparison failed: {exc}", file=sys.stderr)
        return 2

    failures: list[str] = []
    compared = 0
    for key, candidate_metrics in candidate_rows.items():
        baseline_metrics = baseline_rows.get(key)
        if baseline_metrics is None:
            print(f"SKIP {key}: no baseline row")
            continue

        compared += 1
        for metric in METRIC_COLUMNS:
            ratio = regression_ratio(baseline_metrics[metric], candidate_metrics[metric])
            if ratio > args.threshold:
                failures.append(
                    f"{key} {metric}: baseline={baseline_metrics[metric]:.3f}, "
                    f"candidate={candidate_metrics[metric]:.3f}, ratio={ratio:.3f}"
                )

    if compared == 0:
        print("benchmark comparison failed: no overlapping benchmark rows", file=sys.stderr)
        return 2

    if failures:
        print("Benchmark regressions exceeded threshold:")
        for failure in failures:
            print(f"- {failure}")
        return 1

    print(f"Benchmark comparison passed for {compared} rows at threshold {args.threshold:.3f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
