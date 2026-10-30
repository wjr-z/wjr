#!/usr/bin/env python3
"""Summarize raw repetitions within one focused btree worker/compiler profile."""

import json
import re
import statistics
import sys
from collections import defaultdict
from pathlib import Path


def summarize(path):
    report = json.loads(Path(path).read_text(encoding="utf-8"))
    context = report.get("context", {})
    for key in ("wjr_source", "wjr_compiler", "standard_library", "wjr_flags"):
        print(f"{key}: {context.get(key, 'unknown')}")
    samples = defaultdict(list)
    units = {"ns": 1, "us": 1000, "ms": 1000000, "s": 1000000000}
    for row in report["benchmarks"]:
        if row.get("run_type") == "aggregate":
            continue
        if row.get("error_occurred"):
            raise ValueError(row.get("error_message", "benchmark failed"))
        match = re.match(r"([^/]+)/([^/]+)/([^/]+)/(\d+)(?:/|$)", row["name"])
        if not match:
            raise ValueError(f"Not a focused btree result: {row['name']}")
        case, backend, operation, count = match.groups()
        n = int(count)
        operations = 2048 if operation.startswith("lower_bound") else 5120 if operation == "mixed_90_5_5" else n
        samples[(case, operation, n, backend)].append(
            row["real_time"] * units[row["time_unit"]] / operations
        )
    if not samples:
        raise ValueError("No raw repetitions; run with --benchmark_report_aggregates_only=false")
    print("\n| Case | Operation | n | Backend | Reps | Median ns/op | Min-max ns/op | Time/std |")
    print("| --- | --- | ---: | --- | ---: | ---: | ---: | ---: |")
    for (case, operation, n, backend), values in sorted(samples.items()):
        median = statistics.median(values)
        reference = samples.get((case, operation, n, "std"))
        ratio = f"{median / statistics.median(reference):.3f}" if reference else "NA"
        print(f"| {case} | {operation} | {n} | {backend} | {len(values)} | "
              f"{median:.3f} | {min(values):.3f}-{max(values):.3f} | {ratio} |")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: summarize_btree.py timing.json")
    summarize(sys.argv[1])
