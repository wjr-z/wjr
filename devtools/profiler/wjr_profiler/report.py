"""Stable JSON and Markdown views of the same comparison model."""

import html
import json
from pathlib import Path

from .bundle import write_json


def cell(value):
    return html.escape(str(value)).replace("|", "\\|").replace("\n", " ")


def number(value):
    return "unknown" if value is None else f"{value:.6g}"


def _table_row(item):
    left, right = item["baseline"]["statistics"], item["candidate"]["statistics"]
    label = item["classification"] + ("; exploratory delta" if item["exploratory_delta"] else "")
    relative = number(item["relative_change_percent"])
    if item["relative_change_percent"] is not None:
        relative += "%"
    values = (item["region_id"] + " / " + item["metric"], label,
              number(left["median"]), number(right["median"]), number(item["absolute_change"]),
              relative, f"{left['n']}/{right['n']}")
    return "| " + " | ".join(cell(value) for value in values) + " |"


def _side_description(name, side):
    stats = side["statistics"]
    identity = f"{name}: {cell(side['bundle_id'])}, variant {cell(side['variant'])}; "
    spread = (f"median/min/max/MAD: {number(stats['median'])} / {number(stats['minimum'])} / "
              f"{number(stats['maximum'])} / {number(stats['mad'])}; ")
    return identity + spread + f"sample stddev: {number(stats['sample_stddev'])}."


def _counter_lines(observation):
    counters = [record["user_counters_uninterpreted"]
                for record in observation["metadata"].get("records", [])
                if record.get("user_counters_uninterpreted")]
    if not counters:
        return []
    return ["Uninterpreted counters/extension fields (display only):", "",
            "```json", json.dumps(counters, sort_keys=True, allow_nan=False), "```"]


def _side_lines(name, side):
    raw = side["raw_source"]
    lines = ["", _side_description(name, side),
             f"Raw: {cell(raw['path'])}; SHA-256: {cell(raw['sha256'])}."]
    observation = side["observation"]
    if observation:
        lines.append("Locations: " + cell(", ".join(observation["source_locators"])))
        lines.append("Sample level: " + cell(observation["sample_level"]))
        lines.extend(_counter_lines(observation))
    return lines


def _detail_lines(item):
    description = (f"Kind: {cell(item['kind'])}; unit: {cell(item['unit'])}; "
                   f"basis: {cell(item['time_basis'])}; scope: {cell(item['scope'])}; "
                   f"direction: {cell(item['direction'])}.")
    lines = ["", "## " + cell(item["region_id"] + " / " + item["metric"]), "", description]
    lines.extend("- " + cell(reason) for reason in item["reasons"] + item["notes"])
    for name in ("baseline", "candidate"):
        lines.extend(_side_lines(name, item[name]))
    return lines


def markdown(report):
    lines = ["# Offline profiling comparison", "", "Eligibility is reported before any descriptive delta.", "",
             "| Region / metric | Eligibility | Baseline median | Candidate median | Absolute change | Relative change | n (base/candidate) |",
             "| --- | --- | ---: | ---: | ---: | ---: | ---: |"]
    lines.extend(_table_row(item) for item in report["comparisons"])
    for item in report["comparisons"]:
        lines.extend(_detail_lines(item))
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
