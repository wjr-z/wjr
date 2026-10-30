"""Stable JSON and Markdown views of the same comparison model."""

import html
import json
from pathlib import Path

from .bundle import write_json


def cell(value):
    return html.escape(str(value)).replace("|", "\\|").replace("\n", " ")


def number(value):
    return "unknown" if value is None else f"{value:.6g}"


def markdown(report):
    lines = ["# Offline profiling comparison", "", "Eligibility is reported before any descriptive delta.", "",
             "| Region / metric | Eligibility | Baseline median | Candidate median | Absolute change | Relative change | n (base/candidate) |",
             "| --- | --- | ---: | ---: | ---: | ---: | ---: |"]
    for item in report["comparisons"]:
        left, right = item["baseline"]["statistics"], item["candidate"]["statistics"]
        label = item["classification"] + ("; exploratory delta" if item["exploratory_delta"] else "")
        relative = number(item["relative_change_percent"])
        if item["relative_change_percent"] is not None:
            relative += "%"
        lines.append("| " + " | ".join(cell(x) for x in (
            item["region_id"] + " / " + item["metric"], label,
            number(left["median"]), number(right["median"]), number(item["absolute_change"]),
            relative, f"{left['n']}/{right['n']}",
        )) + " |")
    for item in report["comparisons"]:
        lines += ["", "## " + cell(item["region_id"] + " / " + item["metric"]), "",
                  f"Kind: {cell(item['kind'])}; unit: {cell(item['unit'])}; basis: {cell(item['time_basis'])}; "
                  f"scope: {cell(item['scope'])}; direction: {cell(item['direction'])}."]
        for reason in item["reasons"] + item["notes"]:
            lines.append("- " + cell(reason))
        for name in ("baseline", "candidate"):
            side = item[name]
            observation = side["observation"]
            stats = side["statistics"]
            lines += ["", f"{name}: {cell(side['bundle_id'])}, variant {cell(side['variant'])}; "
                      f"median/min/max/MAD: {number(stats['median'])} / {number(stats['minimum'])} / "
                      f"{number(stats['maximum'])} / {number(stats['mad'])}; "
                      f"sample stddev: {number(stats['sample_stddev'])}.",
                      f"Raw: {cell(side['raw_source']['path'])}; SHA-256: {cell(side['raw_source']['sha256'])}."]
            if observation:
                lines.append("Locations: " + cell(", ".join(observation["source_locators"])))
                lines.append("Sample level: " + cell(observation["sample_level"]))
                counters = [record["user_counters_uninterpreted"]
                            for record in observation["metadata"].get("records", [])
                            if record.get("user_counters_uninterpreted")]
                if counters:
                    lines += ["Uninterpreted counters/extension fields (display only):", "",
                              "```json", json.dumps(counters, sort_keys=True, allow_nan=False), "```"]
    lines += ["", "## Diagnostics", ""]
    for diagnostic in report["diagnostics"]:
        lines.append("- " + cell(diagnostic.get("code", "diagnostic")) + ": " +
                     cell(diagnostic.get("message", "")) + " @ " + cell(diagnostic.get("pointer", "unknown")))
    lines += ["", "## Limits", ""] + ["- " + cell(text) for text in report["limitations"]]
    return "\n".join(lines) + "\n"


def write_report(report, destination):
    text = markdown(report)
    json.dumps(report, allow_nan=False)
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / "report.json", report)
    (output / "report.md").write_text(text, encoding="utf-8", newline="\n")
