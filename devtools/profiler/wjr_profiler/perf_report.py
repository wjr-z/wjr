"""Describe a single perf process run without inventing per-kernel attribution."""

import json
from pathlib import Path

from .adapters.perf_stat import LIMITS
from .bundle import write_json
from .model import require, summarize
from .report import cell, number


def summarize_counters(bundle):
    require(bundle["manifest"]["producer"]["provider"] == "perf-stat", "perf-summary requires a perf-stat bundle")
    rows = []
    for observation in bundle["normalized"]["observations"]:
        rows.append(dict(event=observation["metric"], unit=observation["unit"], quality=observation["quality"],
                         reason=observation["missing_reason"], statistics=summarize(observation["samples"]),
                         sources=observation["source_locators"], metadata=observation["metadata"]))
    manifest = bundle["manifest"]
    return dict(schema_version=1, bundle_id=manifest["bundle_id"], raw_source=manifest["raw_source"],
                producer=manifest["producer"], host=manifest.get("host", {}), counters=rows,
                diagnostics=bundle["normalized"]["diagnostics"], limits=LIMITS)


def write_summary(bundle, destination):
    report = summarize_counters(bundle)
    producer, host = report["producer"], report["host"]
    capture = producer["perf_stat"]
    lines = ["# perf stat process counters", "", *LIMITS, "",
             cell(f"Perf {producer['version']}; exit status {producer.get('exit_status', 'unknown')}; "
                  f"CPU {host.get('cpu', 'unknown')}; core {host.get('core', 'unknown')}."), "",
             cell(f"PMU {capture.get('pmu', 'unknown')}; filters {capture.get('filters', 'unknown')}; "
                  f"inherit {capture['inherit']}; scaling {capture['scaling']}; kernel {capture.get('kernel', 'unknown')}."), "",
             "| Event | Printed value | Unit | State | Event runtime | Running % | Multiplexed | Source |",
             "| --- | ---: | --- | --- | ---: | ---: | --- | --- |"]
    for row in report["counters"]:
        metadata = row["metadata"]
        value = row["statistics"]["median"]
        printed = "unavailable" if value is None else str(value)
        lines.append("| " + " | ".join(map(cell, (row["event"], printed, row["unit"], row["quality"],
            number(metadata["event_runtime"]) + " " + metadata["event_runtime_unit"], number(metadata["running_percent"]),
            "unknown" if metadata["multiplexed"] is None else metadata["multiplexed"], ", ".join(row["sources"])))) + " |")
    lines += ["", "Raw SHA-256: " + report["raw_source"]["sha256"], "", "## Diagnostics", ""]
    lines.extend("- " + cell(item["message"]) for item in report["diagnostics"])
    json.dumps(report, allow_nan=False)
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / "report.json", report)
    (output / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
