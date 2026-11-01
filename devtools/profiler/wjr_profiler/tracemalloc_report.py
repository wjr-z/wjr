"""One snapshot-pair diagnosis, keeping global peaks separate from live traceback deltas."""

import json
from pathlib import Path

from .bundle import write_json
from .model import require
from .report import cell
from .adapters.tracemalloc_json import LIMITS, SAMPLE_LEVEL


def summarize_memory(bundle):
    require(bundle["manifest"]["producer"]["provider"] == "tracemalloc-json",
            "tracemalloc-summary requires a tracemalloc-json bundle")
    regions = {}
    for observation in bundle["normalized"]["observations"]:
        metadata = observation["metadata"]
        row = regions.setdefault(observation["region_id"], dict(region_id=observation["region_id"],
            producer=metadata["producer"], collection=metadata["collection"], checkpoint=metadata["checkpoint"],
            traceback_limit=metadata["traceback_limit"], values={}, tracebacks=[], metric_quality={}))
        row["values"][observation["metric"]] = next(iter(observation["samples"]), None)
        row["metric_quality"][observation["metric"]] = dict(quality=observation["quality"],
                                                          reason=observation["missing_reason"])
        if observation["metric"] == "net_retained_bytes":
            row["tracebacks"] = metadata["tracebacks"]
    for row in regions.values():
        row["tracebacks"].sort(key=lambda site: (-site["net_retained_bytes"], site["domain"],
            tuple((frame["file"], frame["line"]) for frame in site["traceback"]),
            -1 if site["total_nframe"] is None else site["total_nframe"]))
        domains = {}
        for site in row["tracebacks"]:
            totals = domains.setdefault(site["domain"], dict(domain=site["domain"],
                baseline_bytes=0, ending_bytes=0, net_retained_bytes=0,
                baseline_blocks=0, ending_blocks=0, net_retained_blocks=0))
            for key in tuple(totals):
                if key != "domain":
                    totals[key] += site[key]
        row["domains"] = sorted(domains.values(), key=lambda item: item["domain"])
    return dict(schema_version=1, bundle_id=bundle["manifest"]["bundle_id"],
                raw_source=bundle["manifest"]["raw_source"], n=1, sample_level=SAMPLE_LEVEL,
                regions=list(regions.values()), diagnostics=bundle["normalized"]["diagnostics"], limits=LIMITS)


def _site_label(site):
    frames = " -> ".join(f"{frame['file']}:{frame['line']}" for frame in site["traceback"])
    depth = site["total_nframe"]
    detail = "depth unknown" if depth is None else f"{len(site['traceback'])}/{depth} frames"
    if site["truncated"]:
        detail += "; truncated"
    if site["unknown_frames"]:
        detail += "; unknown source frame"
    return f"domain {site['domain']}; {frames} ({detail})"


def _sites_table(sites, title, limit):
    lines = ["", "## " + title, "", "| Traceback (oldest to newest) | Before bytes / blocks | After bytes / blocks | Net bytes / blocks |",
             "| --- | ---: | ---: | ---: |"]
    for row in sites[:limit]:
        values = [f"{row[a]} / {row[b]}" for a, b in (("baseline_bytes", "baseline_blocks"),
                  ("ending_bytes", "ending_blocks"), ("net_retained_bytes", "net_retained_blocks"))]
        lines.append("| " + cell(_site_label(row)) + " | " + " | ".join(values) + " |")
    if len(sites) > limit:
        lines += ["", f"Showing {limit} of {len(sites)} sites; report.json retains all tracebacks."]
    return lines


def write_summary(bundle, destination, limit=20):
    require(isinstance(limit, int) and not isinstance(limit, bool) and limit > 0, "summary limit must be positive")
    report = summarize_memory(bundle)
    lines = ["# tracemalloc live-block diagnosis", "", *report["limits"], ""]
    for row in report["regions"]:
        lines += ["## " + cell(row["region_id"]), "", "| Metric | Value | Evidence |", "| --- | ---: | --- |"]
        for metric, value in row["values"].items():
            lines.append("| " + cell(metric) + " | " + ("missing" if value is None else str(value)) +
                         " | " + cell(row["metric_quality"][metric]["quality"]) + " |")
        checkpoint = row["checkpoint"]
        if checkpoint:
            lines += ["", "Global checkpoint window: " + cell(checkpoint["peak_window"]),
                      "Sampling point: " + cell(checkpoint["sampling_point"]),
                      "Peak covers all traced domains and is never assigned to the sites below."]
        lines += ["", "Snapshot selection: " + cell(row["collection"]["coverage"]),
                  "Harness policy: " + cell(row["collection"]["harness_policy"]),
                  "Domain 0 is Python tracing; other domains are extension-defined resources."]
        sites = row["tracebacks"]
        lines += _sites_table([site for site in sites if site["net_retained_bytes"] > 0], "Positive retained changes", limit)
        lines += _sites_table(sorted([site for site in sites if site["net_retained_bytes"] < 0],
                                    key=lambda site: site["net_retained_bytes"]), "Negative retained changes", limit)
        count_only = [site for site in sites if site["net_retained_bytes"] == 0 and site["net_retained_blocks"] != 0]
        if count_only:
            lines += _sites_table(count_only, "Block-count changes with unchanged bytes", limit)
    lines += ["", "Raw SHA-256: " + report["raw_source"]["sha256"], "", "## Diagnostics", ""]
    lines += ["- " + cell(item["message"]) for item in report["diagnostics"]]
    json.dumps(report, allow_nan=False)
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / "report.json", report)
    (output / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
