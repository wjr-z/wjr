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


def main():
    if len(sys.argv) not in (2, 3, 4):
        raise SystemExit("usage: summarize.py results.json [profile] [baseline.json]")

    current_path = sys.argv[1]
    profile = sys.argv[2] if len(sys.argv) >= 3 else "unknown"
    baseline_path = sys.argv[3] if len(sys.argv) == 4 else None
    threshold = float(os.environ.get("BENCHMARK_REGRESSION_THRESHOLD", "10"))

    current = load_rows(current_path)

    print("## Benchmark Summary")
    print()
    print(f"- Profile: `{profile}`")
    print(f"- Cases: `{len(current)}`")
    print(f"- Regression threshold: `{threshold:.1f}%`")
    print()

    if not baseline_path or not os.path.isfile(baseline_path):
        print("No previous successful result is available for comparison.")
        return

    baseline = load_rows(baseline_path)
    regressions = []
    compared = 0
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

        compared += 1
        delta = (current_time - previous_time) / previous_time * 100
        if delta >= threshold:
            regressions.append((delta, name, previous_row, current_row))

    regressions.sort(reverse=True)
    print(f"- Compared cases: `{compared}`")
    print(f"- Regressions: `{len(regressions)}`")
    print()
    print(f"### Regressions (>= {threshold:.1f}%)")
    print()
    if not regressions:
        print("No benchmark regression exceeded the threshold.")
        return

    print("| Benchmark | Previous | Current | Change | Unit |")
    print("| --- | ---: | ---: | ---: | --- |")
    for delta, name, previous_row, current_row in regressions[:20]:
        name = name.replace("|", "\\|")
        unit = current_row.get("time_unit", "ns")
        print(
            f"| `{name}` | {previous_row['real_time']:.3f} "
            f"| {current_row['real_time']:.3f} | {format_delta(delta)} | {unit} |"
        )


if __name__ == "__main__":
    main()