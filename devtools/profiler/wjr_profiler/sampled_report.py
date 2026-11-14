"""Describe sampled hotspots without converting sampling weights into CPU timing."""

import json
from pathlib import Path

from .bundle import write_json
from .model import require
from .report import cell, number


def summarize_profile(bundle):
    require(bundle["manifest"]["producer"]["provider"] == "py-spy-speedscope",
            "speedscope-summary requires a py-spy-speedscope bundle")
    profiles = []
    for observation in bundle["normalized"]["observations"]:
        require(observation["metric"] == "recorded_sample_weight", "unexpected sampled-profile metric")
        metadata = observation["metadata"]
        profiles.append(dict(region_id=observation["region_id"], **metadata,
                             recorded_weight=next(iter(observation["samples"]), 0), quality=observation["quality"],
                             sources=observation["source_locators"]))
    return dict(schema_version=1, bundle_id=bundle["manifest"]["bundle_id"],
                raw_source=bundle["manifest"]["raw_source"], producer=bundle["manifest"]["producer"],
                n=1, profiles=profiles, diagnostics=bundle["normalized"]["diagnostics"])


def write_summary(bundle, destination, limit=20):
    require(isinstance(limit, int) and not isinstance(limit, bool) and limit > 0, "limit must be positive")
    report = summarize_profile(bundle)
    lines = ["# Sampled stack hotspots", "", "One capture per thread profile. Weights are recorded sampling weights.", ""]
    policy = report["producer"].get("sampling_policy", "unknown")
    lines += ["Declared sampling policy is " + cell(json.dumps(policy, sort_keys=True)) + ".", ""]
    for profile in report["profiles"]:
        lines += ["## " + cell(profile["profile_name"]), "", "\n\n".join(profile["limits"]), "",
                  "Recorded weight is " + number(profile["recorded_weight"]) +
                  " and empty-stack weight is " + number(profile["empty_stack_weight"]) + ".", "",
                  "Import quality is " + cell(profile["quality"]) + ".", "",
                  "| Frame (file and line) | Self weight | Inclusive weight | Self fraction |",
                  "| --- | ---: | ---: | ---: |"]
        for row in profile["functions"][:limit]:
            frame = row["frame"]
            label = f"{frame['name']} ({frame.get('file') or 'unknown'}:{frame.get('line')})"
            lines.append("| " + cell(label) + " | " + number(row["self_weight"]) + " | " +
                         number(row["inclusive_weight"]) + " | " + number(row["self_fraction"]) + " |")
        lines.append("")
    lines += ["Raw SHA-256 is " + report["raw_source"]["sha256"] + ".", "",
              "All frame identities, source pointers and selected thread profiles remain in report.json.", ""]
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / "report.json", report)
    (output / "report.md").write_text("\n".join(lines), encoding="utf-8", newline="\n")
