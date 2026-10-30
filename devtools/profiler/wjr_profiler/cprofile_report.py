"""A single instrumented profile is a hotspot report, not a repeated-run regression test."""

import json
from pathlib import Path

from .bundle import write_json
from .model import require
from .report import cell, number


def summarize_profile(bundle):
    require(bundle["manifest"]["producer"]["provider"] == "cprofile-json", "cprofile-summary requires a cprofile-json bundle")
    first = next(iter(bundle["normalized"]["observations"]), None)
    producer = first["metadata"]["producer"] if first else {}
    functions = {}
    for observation in bundle["normalized"]["observations"]:
        entry = functions.setdefault(observation["region_id"], dict(
            region_id=observation["region_id"], function=observation["metadata"]["function"],
            callers=observation["metadata"]["callers"], values={}, sources=[]))
        entry["values"][observation["metric"]] = next(iter(observation["samples"]), None)
        entry["sources"].extend(observation["source_locators"])
    rows = sorted(functions.values(), key=lambda row: (
        -(row["values"].get("self_time") or 0), row["function"]["file"],
        row["function"]["line"], row["function"]["name"]))
    return dict(schema_version=1, bundle_id=bundle["manifest"]["bundle_id"],
                raw_source=bundle["manifest"]["raw_source"], producer=producer,
                sample_level="independent_process_run", n=1, functions=rows,
                diagnostics=bundle["normalized"]["diagnostics"],
                limits=["One instrumented run; no independent between-run variance or regression claim.",
                        "Sorted by self seconds; cumulative seconds overlap and are never summed.",
                        "Caller edges preserve recursive primitive/total counts; same names in different files remain distinct.",
                        "Native extension internals and undeclared threads are outside cProfile coverage."])


def write_summary(bundle, destination):
    report = summarize_profile(bundle)
    producer = report["producer"]
    timing = f"Timer: {producer.get('timer', 'unknown')}; time basis: {producer.get('time_basis', 'unknown')}."
    coverage = f"Interpreter: {producer.get('version', 'unknown')}; thread coverage: {producer.get('thread_coverage', 'unknown')}."
    lines = ["# cProfile hotspots", "", cell(timing), cell(coverage), "", *report["limits"], "",
             "| Function (file:line) | Primitive / total calls | Self seconds | Cumulative seconds |",
             "| --- | ---: | ---: | ---: |"]
    for row in report["functions"]:
        identity, values = row["function"], row["values"]
        label = f"{identity['name']} ({identity['file']}:{identity['line']})"
        counts = f"{values.get('primitive_calls')} / {values.get('total_calls')}"
        lines.append("| " + cell(label) + " | " + counts + " | " +
                     number(values.get("self_time")) + " | " + number(values.get("cumulative_time")) + " |")
    lines += ["", "Raw SHA-256: " + report["raw_source"]["sha256"],
              "Raw locations and caller edges are preserved in report.json.", "", "## Diagnostics", ""]
    lines.extend("- " + cell(item["message"]) for item in report["diagnostics"])
    json.dumps(report, allow_nan=False)
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / "report.json", report)
    (output / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
