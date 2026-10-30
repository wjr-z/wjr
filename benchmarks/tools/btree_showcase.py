#!/usr/bin/env python3
"""Collect and plot the fixed legacy-N16/Abseil ordered-container showcase."""

import argparse
import csv
import hashlib
import json
import math
import platform
import re
import statistics
from pathlib import Path

ABSEIL = "2a2d6aad626f75a32c8b7bb775fbff73d558b8b1"
CASES = {
    "u64_set": "uint64 set",
    "u64_map": "uint64 map",
    "large_map": "64-byte mapped map",
}
OPERATIONS = {
    "build_destroy": "Build + destroy",
    "lower_bound_50": "Lower bound (50% keys present)",
    "mixed_90_5_5": "Mixed 90/5/5",
    "scan": "Key scan",
}
BACKENDS = ("wjr16", "absl")
N = 4096


def read_text(path):
    raw = Path(path).read_bytes()
    return raw.decode(
        "utf-16" if raw.startswith((b"\xff\xfe", b"\xfe\xff")) else "utf-8-sig"
    )


def file_hash(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def collect(timing_path, memory_path, metadata_path, output):
    timing = json.loads(read_text(timing_path))
    context = timing["context"]
    metadata = json.loads(read_text(metadata_path))
    if (
        context["allocator"] != "std::allocator; no counters"
        or str(context["trace_seed"]) != "1656023321"
        or str(context["trace_blocks"]) != "256"
    ):
        raise ValueError("unexpected timing allocator or trace contract")
    if metadata["abseil_commit"] != ABSEIL:
        raise ValueError("showcase requires the reviewed Abseil commit")
    memory_lines = read_text(memory_path).splitlines()
    provenance = re.match(r"# source=(\S+) compiler=(\S+)", memory_lines[0])
    if not provenance or provenance.groups() != (
        context["wjr_source"],
        context["wjr_compiler"],
    ):
        raise ValueError("timing and memory source/compiler differ")
    if f"stdlib={context['standard_library']} flags=" not in memory_lines[0]:
        raise ValueError("timing and memory standard libraries differ")
    if f"flags={context['wjr_flags']} string_sizeof=" not in memory_lines[0]:
        raise ValueError("timing and memory compiler flags differ")
    groups = {}
    for row in timing["benchmarks"]:
        if row.get("run_type") == "aggregate":
            continue
        match = re.match(r"([^/]+)/([^/]+)/([^/]+)/(\d+)(?:/|$)", row["name"])
        if not match:
            continue
        case, backend, operation, size = match.groups()
        if (
            case not in CASES
            or backend not in BACKENDS
            or operation not in OPERATIONS
            or int(size) != N
        ):
            continue
        if row.get("error_occurred"):
            raise ValueError(row.get("error_message", "benchmark failed"))
        count = (
            2048
            if operation == "lower_bound_50"
            else 5120 if operation == "mixed_90_5_5" else N
        )
        scale = {"ns": 1, "us": 1000, "ms": 1000000, "s": 1000000000}[row["time_unit"]]
        groups.setdefault(f"{case}/{operation}/{backend}", []).append(
            row["real_time"] * scale / count
        )
    expected = {
        f"{case}/{operation}/{backend}"
        for case in CASES
        for operation in OPERATIONS
        for backend in BACKENDS
    }
    if set(groups) != expected or any(len(values) != 5 for values in groups.values()):
        raise ValueError(
            "expected exactly five raw repetitions for every selected case/backend"
        )
    if any(
        not math.isfinite(value) or value <= 0
        for values in groups.values()
        for value in values
    ):
        raise ValueError("timings must be finite and positive")
    memory = {}
    for row in csv.DictReader(memory_lines[1:]):
        case, backend = row["case"].split("/")
        if case in CASES and backend in BACKENDS and int(row["n"]) == N:
            memory[row["case"]] = {
                key: int(row[key])
                for key in (
                    "tree_size",
                    "iterator_size",
                    "build_live_requested",
                    "mixed_live_requested",
                    "peak_requested",
                    "total_requested",
                    "allocations",
                    "deallocations",
                    "after_clear_requested",
                    "after_destroy_requested",
                    "element_string_capacity_bytes",
                )
            }
            memory[row["case"]]["pool_reserved_bytes"] = row["pool_reserved_bytes"]
    if set(memory) != {f"{case}/{backend}" for case in CASES for backend in BACKENDS}:
        raise ValueError(
            "memory pass must contain both backends for every selected case"
        )
    for row in memory.values():
        if row["after_destroy_requested"] or row["allocations"] != row["deallocations"]:
            raise ValueError("unbalanced memory pass")
        if row["pool_reserved_bytes"] != "NA" or row["element_string_capacity_bytes"]:
            raise ValueError(
                "this numeric-key showcase does not measure pools or external strings"
            )
    result = {
        "schema": 1,
        "id": metadata["id"],
        "label": metadata["label"],
        "n": N,
        "provenance": {
            **metadata,
            **{
                key: context[key]
                for key in (
                    "wjr_source",
                    "wjr_compiler",
                    "standard_library",
                    "wjr_flags",
                    "allocator",
                    "trace_seed",
                    "trace_blocks",
                )
            },
            "timing_sha256": file_hash(timing_path),
            "memory_sha256": file_hash(memory_path),
            "measurement_date": context.get(
                "date", context.get("measurement_date", "unknown")
            ),
            "hardware_counters": "not collected",
        },
        "ns_per_operation_samples": groups,
        "memory": memory,
    }
    Path(output).write_text(
        json.dumps(result, indent=2) + "\n", encoding="utf-8", newline="\n"
    )


def ratio(values, reference):
    center = statistics.median(values) / statistics.median(reference)
    return center, min(values) / max(reference), max(values) / min(reference)


def render(inputs, output_dir, image_format="svg"):
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    profiles = [json.loads(read_text(path)) for path in inputs]
    for data in profiles:
        if (
            data["schema"] != 1
            or data["n"] != N
            or data["provenance"]["abseil_commit"] != ABSEIL
        ):
            raise ValueError("incompatible showcase input")
        expected = {
            f"{case}/{operation}/{backend}"
            for case in CASES
            for operation in OPERATIONS
            for backend in BACKENDS
        }
        if set(data["ns_per_operation_samples"]) != expected or any(
            len(x) != 5 for x in data["ns_per_operation_samples"].values()
        ):
            raise ValueError("incomplete five-repetition input")
    plt.rcParams.update(
        {
            "font.family": "DejaVu Sans",
            "font.size": 9,
            "svg.fonttype": "none",
            "svg.hashsalt": "wjr-btree-showcase-v1",
        }
    )
    destination = Path(output_dir)
    destination.mkdir(parents=True, exist_ok=True)
    image_metadata = (
        {"Date": None}
        if image_format == "svg"
        else {"Software": "WJR btree_showcase.py"}
    )
    figure, axes = plt.subplots(
        len(profiles), 3, figsize=(12, 2.6 * len(profiles)), squeeze=False
    )
    largest_envelope = max(
        ratio(
            data["ns_per_operation_samples"][f"{case}/{operation}/wjr16"],
            data["ns_per_operation_samples"][f"{case}/{operation}/absl"],
        )[2]
        for data in profiles
        for case in CASES
        for operation in OPERATIONS
    )
    summary = [
        "| Profile | Case | Operation | WJR median [min–max] ns/op | Abseil median [min–max] ns/op | WJR/Abseil |",
        "| --- | --- | --- | ---: | ---: | ---: |",
    ]
    for row_index, data in enumerate(profiles):
        samples = data["ns_per_operation_samples"]
        for column, (case, title) in enumerate(CASES.items()):
            axis = axes[row_index][column]
            centers, lower, upper = [], [], []
            for operation in OPERATIONS:
                values, reference = [
                    samples[f"{case}/{operation}/{backend}"] for backend in BACKENDS
                ]
                center, lo, hi = ratio(values, reference)
                centers.append(center)
                lower.append(center - lo)
                upper.append(hi - center)
                summary.append(
                    f"| {data['label']} | {title} | {OPERATIONS[operation]} | "
                    f"{statistics.median(values):.2f} [{min(values):.2f}–{max(values):.2f}] | "
                    f"{statistics.median(reference):.2f} [{min(reference):.2f}–{max(reference):.2f}] | {center:.3f} |"
                )
            axis.barh(
                ["Build + destroy", "Lower bound", "Mixed 90/5/5", "Key scan"],
                centers,
                xerr=[lower, upper],
                color="#35698f",
                capsize=3,
            )
            axis.tick_params(axis="y", labelleft=column == 0)
            axis.axvline(1, color="#303030", linestyle="--", linewidth=1)
            axis.invert_yaxis()
            axis.set_xlim(0, max(1.3, largest_envelope * 1.15))
            for index, value in enumerate(centers):
                axis.text(
                    value + upper[index] + 0.025,
                    index,
                    f"{value:.2f}×",
                    va="center",
                    fontsize=8,
                )
            axis.set_title(f"{data['label']}\n{title}")
            axis.set_xlabel("WJR / Abseil (lower is better)")
            axis.grid(axis="x", alpha=0.15)
    figure.suptitle(
        "Legacy N16 B+ tree baseline · 4096 elements · all four operations\n"
        "Bars: median ratio; whiskers: observed min/max envelope, not confidence intervals",
        fontsize=12,
    )
    figure.tight_layout(rect=(0, 0, 1, 0.94))
    figure.savefig(destination / f"btree-time.{image_format}", metadata=image_metadata)
    plt.close(figure)
    figure, axes = plt.subplots(
        1, len(profiles), figsize=(4.2 * len(profiles), 3.5), squeeze=False
    )
    for column, data in enumerate(profiles):
        axis = axes[0][column]
        for index, backend in enumerate(BACKENDS):
            values = [
                data["memory"][f"{case}/{backend}"]["mixed_live_requested"] / 1024
                for case in CASES
            ]
            positions = [x + (index - 0.5) * 0.35 for x in range(len(CASES))]
            bars = axis.bar(
                positions,
                values,
                width=0.35,
                label="WJR N16" if backend == "wjr16" else "Abseil",
                color=("#35698f", "#c58942")[index],
            )
            axis.bar_label(bars, fmt="%.1f", fontsize=8, padding=3)
        axis.set_xticks(range(len(CASES)), ["u64 set", "u64 map", "64B mapped"])
        axis.set_ylabel("Live requested after mixed (KiB)")
        axis.set_title(data["label"])
        axis.set_ylim(0, axis.get_ylim()[1] * 1.15)
        axis.legend()
    figure.suptitle(
        "Separate memory pass · no pools\n"
        "Requested bytes exclude object size\n"
        "and allocator overhead",
        fontsize=10,
    )
    figure.tight_layout(rect=(0, 0, 1, 0.82))
    figure.savefig(
        destination / f"btree-memory.{image_format}", metadata=image_metadata
    )
    plt.close(figure)
    if image_format == "svg":
        for name in ("btree-time.svg", "btree-memory.svg"):
            path = destination / name
            path.write_text(
                "\n".join(
                    line.rstrip()
                    for line in path.read_text(encoding="utf-8").splitlines()
                )
                + "\n",
                encoding="utf-8",
                newline="\n",
            )
    summary.extend(
        [
            "",
            "| Profile | Case | Backend | Live requested bytes after mixed | Peak requested bytes | Allocations (build + mixed) |",
            "| --- | --- | --- | ---: | ---: | ---: |",
        ]
    )
    for data in profiles:
        for case, title in CASES.items():
            for backend in BACKENDS:
                memory = data["memory"][f"{case}/{backend}"]
                summary.append(
                    f"| {data['label']} | {title} | {backend} | {memory['mixed_live_requested']} | {memory['peak_requested']} | {memory['allocations']} |"
                )
    (destination / "btree-samples.md").write_text(
        "\n".join(summary) + "\n", encoding="utf-8", newline="\n"
    )
    (destination / "generator.json").write_text(
        json.dumps(
            {"python": platform.python_version(), "matplotlib": matplotlib.__version__},
            indent=2,
        )
        + "\n",
        encoding="utf-8",
        newline="\n",
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    collector = subparsers.add_parser("collect")
    for flag in ("timing", "memory", "metadata", "output"):
        collector.add_argument(f"--{flag}", required=True)
    renderer = subparsers.add_parser("render")
    renderer.add_argument("inputs", nargs="+")
    renderer.add_argument("--output-dir", required=True)
    renderer.add_argument("--format", choices=("svg", "png"), default="svg")
    args = parser.parse_args()
    if args.command == "collect":
        collect(args.timing, args.memory, args.metadata, args.output)
    else:
        render(args.inputs, args.output_dir, args.format)


if __name__ == "__main__":
    main()
