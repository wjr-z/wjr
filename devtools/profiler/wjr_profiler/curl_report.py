"""Descriptive request stages; no network IO, server CPU attribution or regression gate."""

import json
from pathlib import Path

from .adapters.curl_json import LIMITS, PHASES
from .bundle import write_json
from .model import require, summarize
from .report import cell, number


def summarize_requests(bundle):
    require(bundle["manifest"]["producer"]["provider"] == "curl-json", "curl-summary requires a curl-json bundle")
    regions = {}
    for observation in bundle["normalized"]["observations"]:
        region = regions.setdefault(observation["region_id"], dict(region_id=observation["region_id"],
            requests=observation["metadata"]["requests"], statistics={}, quality={}))
        region["statistics"][observation["metric"]] = summarize(observation["samples"])
        region["quality"][observation["metric"]] = observation["quality"]
    return dict(schema_version=1, bundle_id=bundle["manifest"]["bundle_id"],
                raw_source=bundle["manifest"]["raw_source"], regions=list(regions.values()),
                diagnostics=bundle["normalized"]["diagnostics"], limits=LIMITS)


def write_summary(bundle, destination):
    report = summarize_requests(bundle)
    lines = ["# curl request stages", "", *LIMITS, "",
             "Times below are milliseconds; raw source values are seconds, normalized observations ns/request.", "",
             "| Region / source line | State | HTTP | Bytes | Name lookup | TCP | Setup | First-byte wait | Body | Total | TLS |",
             "| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |"]
    for region in report["regions"]:
        for row in region["requests"]:
            phases = row["phases_ns"]
            values = [number(phases.get(phase) / 1e6) if phase in phases else "unavailable" for phase in PHASES[:-1]]
            lines.append("| " + " | ".join([cell(f"{region['region_id']} / {row['line']}"),
                cell(row["quality"] + " / " + region["quality"]["total"]), cell(row.get("http_status")), cell(row["downloaded_bytes"]),
                *values, cell(row["tls"])]) + " |")
    lines += ["", "Raw SHA-256: " + report["raw_source"]["sha256"], "",
              "Statistics and physical JSONL line locators are in report.json; no composite latency score.", "", "## Diagnostics", ""]
    lines.extend("- " + cell(item["message"]) for item in report["diagnostics"])
    json.dumps(report, allow_nan=False)
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / "report.json", report)
    (output / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
