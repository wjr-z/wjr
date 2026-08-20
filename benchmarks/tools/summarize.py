import json
import os
import sys


def load_rows(path):
    with open(path, encoding="utf-8") as file:
        data = json.load(file)

    return {
        row["name"]: row
        for row in data.get("benchmarks", [])
        if row.get("name")
        and row.get("aggregate_name") in (None, "median")
    }


def format_delta(delta):
    return f"{delta:+.2f}%"


def write_details(path, profile, threshold, baseline_available, comparisons):
    if not path:
        return

    details = {
        "profile": profile,
        "threshold_percent": threshold,
        "baseline_available": baseline_available,
        "compared_cases": len(comparisons),
        "comparisons": comparisons,
    }
    with open(path, "w", encoding="utf-8") as file:
        json.dump(details, file, indent=2)
        file.write("\n")


def print_rows(title, rows, threshold, display_limit):
    print(f"### {title} (>= {threshold:.1f}%)")
    print()
    if not rows:
        print(f"No benchmark {title.lower()} exceeded the threshold.")
        return

    print("| Benchmark | Previous | Current | Change | Unit |")
    print("| --- | ---: | ---: | ---: | --- |")
    displayed = rows if display_limit == 0 else rows[:display_limit]
    for row in displayed:
        name = row["name"].replace("|", "\\|")
        print(
            f"| `{name}` | {row['previous']:.3f} "
            f"| {row['current']:.3f} | {format_delta(row['delta_percent'])} "
            f"| {row['unit']} |"
        )

    omitted = len(rows) - len(displayed)
    if omitted:
        print()
        print(
            f"Showing {len(displayed)} of {len(rows)}. "
            f"The remaining {omitted} items are in the `benchmark-comparison.json` artifact."
        )


def main():
    if len(sys.argv) not in (2, 3, 4):
        raise SystemExit("usage: summarize.py results.json [profile] [baseline.json]")

    current_path = sys.argv[1]
    profile = sys.argv[2] if len(sys.argv) >= 3 else "unknown"
    baseline_path = sys.argv[3] if len(sys.argv) == 4 else None
    threshold = float(os.environ.get("BENCHMARK_REGRESSION_THRESHOLD", "10"))
    try:
        display_limit = int(os.environ.get("BENCHMARK_SUMMARY_LIMIT", "32"))
    except ValueError as error:
        raise SystemExit("BENCHMARK_SUMMARY_LIMIT must be a non-negative integer") from error
    if display_limit < 0:
        raise SystemExit("BENCHMARK_SUMMARY_LIMIT must be a non-negative integer")

    current = load_rows(current_path)
    details_path = os.environ.get("BENCHMARK_DETAILS_OUTPUT")

    print("## Benchmark Summary")
    print()
    print(f"- Profile: `{profile}`")
    print(f"- Cases: `{len(current)}`")
    print(f"- Regression threshold: `{threshold:.1f}%`")
    print(f"- Summary limit: `{'all' if display_limit == 0 else display_limit}`")
    print()

    if not baseline_path or not os.path.isfile(baseline_path):
        write_details(details_path, profile, threshold, False, [])
        print("No previous successful result is available for comparison.")
        return

    baseline = load_rows(baseline_path)
    regressions = []
    improvements = []
    comparisons = []
    for name, current_row in current.items():
        previous_row = baseline.get(name)
        if previous_row is None:
            continue

        previous_time = previous_row.get("real_time")
        current_time = current_row.get("real_time")
        if previous_time is None or current_time is None or previous_time <= 0:
            continue
        if previous_row.get("time_unit") != current_row.get("time_unit"):
            continue

        delta = (current_time - previous_time) / previous_time * 100
        if delta >= threshold:
            classification = "regression"
        elif delta <= -threshold:
            classification = "improvement"
        else:
            classification = "stable"

        comparisons.append(
            {
                "name": name,
                "previous": previous_time,
                "current": current_time,
                "delta_percent": delta,
                "unit": current_row.get("time_unit", "ns"),
                "classification": classification,
            }
        )
        if delta >= threshold:
            regressions.append(comparisons[-1])
        elif delta <= -threshold:
            improvements.append(comparisons[-1])

    regressions.sort(key=lambda row: (-row["delta_percent"], row["name"]))
    improvements.sort(key=lambda row: (row["delta_percent"], row["name"]))
    comparisons.sort(key=lambda row: row["name"])
    write_details(details_path, profile, threshold, True, comparisons)
    print(f"- Compared cases: `{len(comparisons)}`")
    print(f"- Regressions: `{len(regressions)}`")
    print(f"- Improvements: `{len(improvements)}`")
    print()
    print_rows("Regressions", regressions, threshold, display_limit)
    print()
    print_rows("Improvements", improvements, threshold, display_limit)


if __name__ == "__main__":
    main()