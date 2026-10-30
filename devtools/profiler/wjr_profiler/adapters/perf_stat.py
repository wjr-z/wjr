"""Offline perf stat JSONL, following Linux v6.8.12's producer and JSON lint."""

import json
import math
import re
from decimal import Decimal, InvalidOperation

from ..model import is_integer, require, unknown

SOURCE_VERSION = "6.8.12"
UNSUPPORTED_LAYOUT = {"interval", "timestamp", "cpu", "core", "socket", "die", "node",
                      "thread", "cache", "cgroup", "aggregate-number", "variance"}
COMPARISON_FIELDS = ("pmu", "filters", "inherit", "affinity", "core_type", "kernel", "scaling")
LIMITS = ["Whole-process counters include startup and teardown; they are not per-kernel or per-call measurements.",
          "Each artifact is one process run, not one sample per event; no between-run variance is inferred.",
          "counter-value is retained as printed. Perf-scaled values are never scaled a second time.",
          "Running percentage is rounded by perf; exact time-enabled is not reconstructed from it.",
          "Schema source: Linux v6.8.12 producer and upstream JSON lint. Positive hardware smoke was unavailable on this worker."]


def decode_raw(content):
    """Preserve physical JSONL line numbers, including an empty failed capture."""
    records = []
    for number, line in enumerate(content.decode("utf-8-sig").splitlines(), 1):
        if not line.strip():
            continue
        try:
            record = json.loads(line)
        except ValueError as error:
            raise ValueError(f"perf stat line {number}: {error}") from error
        require(isinstance(record, dict), f"perf stat line {number}: expected an event object")
        records.append((number, record))
    return records


def _command_contract(producer, capture):
    argv = producer.get("argv")
    require(isinstance(argv, list) and all(isinstance(arg, str) for arg in argv), "producer.argv must record the perf command")
    require(len(argv) > 2 and argv[1] == "stat" and "--" in argv, "record an explicit perf stat -- owned-command argv")
    options = argv[2:argv.index("--")]
    forbidden = ("--pid", "--tid", "--interval", "--repeat", "--per-", "--cgroup", "--all-cpus",
                 "--bpf", "--for-each-cgroup")
    for option in options:
        require(option not in {"-a", "-p", "-t", "-A", "--no-aggr", "--metric-only"} and
                not option.startswith(forbidden) and not option.startswith(("-I", "-r", "-p", "-t", "-a")),
                "only one globally aggregated owned-command run is supported")
    require("--no-scale" not in options or capture["scaling"] == "raw", "--no-scale contradicts declared scaling")


def _context(manifest):
    require(len(manifest["regions"]) == 1, "perf stat needs exactly one whole-process region")
    capture = manifest["producer"].get("perf_stat")
    require(isinstance(capture, dict), "producer.perf_stat collection contract is required")
    expected = dict(mode="owned_command", scope="whole_process", aggregation="global", repeat=1, interval=False)
    for key, value in expected.items():
        require(type(capture.get(key)) is type(value) and capture[key] == value, f"unsupported perf_stat.{key}")
    require(capture.get("scaling") in {"perf_scaled", "raw"}, "declare perf_stat.scaling as perf_scaled or raw")
    require(isinstance(capture.get("inherit"), bool), "declare perf_stat.inherit explicitly")
    _command_contract(manifest["producer"], capture)
    region = manifest["regions"][0]
    workload = manifest["workloads"][region["workload_id"]]
    require(workload.get("scope") == "whole_process" and workload.get("sample_level") == "independent_process_run",
            "perf stat cannot assign whole-process counts to kernel regions or internal repetitions")
    events = region.get("events")
    require(isinstance(events, list) and bool(events) and all(isinstance(x, str) and x for x in events),
            "region.events must select exact native event names")
    require(len(events) == len(set(events)), "duplicate selected perf event")
    return region, capture, events


def _index(records):
    rows, diagnostics = {}, []
    for line, row in records:
        require(not UNSUPPORTED_LAYOUT.intersection(row), "interval/per-CPU/aggregate perf records are unsupported")
        if "counter-value" not in row and "metric-value" in row:
            diagnostics.append(dict(code="derived_metric_uninterpreted", pointer=f"/lines/{line}",
                                    message="Perf derived metric retained only in raw; no inferred unit or formula."))
            continue
        event = row.get("event")
        require(isinstance(event, str) and event, "perf event name is missing")
        require(event not in rows, "duplicate perf event; repeated/append output cannot establish independent samples")
        rows[event] = (line, row)
    return rows, diagnostics


def _number(value):
    if isinstance(value, bool) or not isinstance(value, (str, int, float)):
        return None
    try:
        if isinstance(value, str):
            if not re.fullmatch(r"[0-9]+(?:\.[0-9]+)?", value):
                return None
            decimal = Decimal(value)
            result = int(decimal) if decimal == decimal.to_integral_value() else float(decimal)
        else:
            result = value
        return result if math.isfinite(result) and result >= 0 else None
    except (ValueError, OverflowError, InvalidOperation):
        return None


def _reading(row):
    if row is None:
        return None, "missing_event", "Declared event is absent from the raw output."
    value = row.get("counter-value")
    if value == "<not supported>":
        return None, "unsupported_event", "Perf reports <not supported>; this is not a zero count."
    if value == "<not counted>":
        return None, "not_counted", "Perf reports <not counted>; this is not a zero count."
    number = _number(value)
    if number is None:
        return None, "invalid_counter", "Counter is missing, negative, nonfinite or not an ungrouped decimal."
    return number, None, None


def _coverage(row):
    runtime = _number(row.get("event-runtime"))
    percent = _number(row.get("pcnt-running"))
    if runtime is None or percent is None:
        return runtime, percent, "missing_running", "Event runtime or running percentage is missing/invalid."
    if percent > 100:
        return runtime, percent, "invalid_running", "Running percentage exceeds 100."
    if runtime == 0 or percent == 0:
        return runtime, percent, "not_counted", "Counter did not run; no whole-process count is available."
    return runtime, percent, None, None


def _quality(producer, value, code, multiplexed):
    status = producer.get("exit_status")
    if not is_integer(status) or status != 0:
        return "failed", "producer_failed", "Perf exit status is missing/nonzero; raw output may be partial."
    if code or value is None:
        return "missing", code, None
    if producer.get("evidence") == "synthetic":
        return "exploratory", "synthetic", "Synthetic schema fixture, not collected performance evidence."
    if producer["version"] != SOURCE_VERSION:
        return "exploratory", "unverified_version", "Perf version differs from the source-checked 6.8.12 schema."
    if multiplexed:
        return "exploratory", "multiplexed", "Counter ran for part of the interval; displayed scaling is preserved, not recomputed."
    return "insufficient_samples", "single_run", "One process run; no independent between-run variance."


def _observation(event, selected, region, capture, producer):
    line, row = selected if selected else (None, None)
    value, code, reason = _reading(row)
    runtime, percent = None, None
    if row is not None:
        runtime, percent, coverage_code, coverage_reason = _coverage(row)
        if not code:
            code, reason = coverage_code, coverage_reason
    multiplexed = None if percent is None else 0 < percent < 100
    quality, diagnostic, quality_reason = _quality(producer, value, code, multiplexed)
    reason = quality_reason or reason
    unit = row.get("unit", "") if row else "unknown"
    require(isinstance(unit, str), "perf unit must be a string")
    pointer = f"/lines/{line}" if line is not None else "/"
    observation = dict(region_id=region["id"], metric=event, kind="measurement", unit=unit or "count",
        scope="whole_process", direction="none", samples=[] if quality in {"failed", "missing"} else [value],
        sample_level="independent_process_run", quality=quality, missing_reason=reason,
        time_basis="perf_counter", source_locators=[pointer], metadata=dict(
            event=event, native_unit=unit, source_line=line, event_runtime=runtime, running_percent=percent,
            event_runtime_unit="us" if event.split(":", 1)[0] in {"user_time", "system_time"} else "ns",
            counter_value_text=str(row.get("counter-value")) if row else None,
            enabled_time_ns=None, multiplexed=multiplexed, capture=capture,
            evidence=producer.get("evidence", "user_supplied"),
            format_basis="Linux v6.8.12 producer source and upstream JSON lint; not a positive hardware smoke",
            scaling_policy="preserve printed counter-value; no second multiplex scaling"))
    detail = dict(code=diagnostic or code, region_id=region["id"], pointer=pointer, message=reason)
    return observation, detail


def import_artifact(raw, manifest):
    require(isinstance(raw, list), "decoded perf JSONL records expected")
    region, capture, events = _context(manifest)
    rows, diagnostics = _index(raw)
    observations = []
    for event in events:
        observation, detail = _observation(event, rows.get(event), region, capture, manifest["producer"])
        observations.append(observation)
        diagnostics.append(detail)
    stderr = manifest["producer"].get("stderr")
    require(stderr is None or isinstance(stderr, str), "producer.stderr must be text")
    if stderr:
        diagnostics.append(dict(code="producer_stderr", pointer="/", message=stderr))
    for event in rows.keys() - set(events):
        diagnostics.append(dict(code="unselected_event", pointer=f"/lines/{rows[event][0]}",
                                message=f"Unselected event {event} retained in raw only."))
    return observations, diagnostics


def comparison_reasons(left, right, before, after):
    """Do not extrapolate counters across machines, PMUs or incomplete coverage."""
    reasons = []
    for key in ("id", "cpu", "core"):
        values = [document.get("host", {}).get(key) for document in (left, right)]
        if any(unknown(value) for value in values) or values[0] != values[1]:
            reasons.append("perf requires the same known host." + key)
    if before is None or after is None:
        return reasons
    if any(observation["metadata"].get("evidence") == "synthetic" for observation in (before, after)):
        reasons.append("synthetic perf schema fixtures cannot establish performance changes")
    captures = [observation["metadata"]["capture"] for observation in (before, after)]
    for key in COMPARISON_FIELDS:
        values = [capture.get(key) for capture in captures]
        if any(unknown(value) for value in values) or values[0] != values[1]:
            reasons.append("perf collection differs or is unknown at " + key)
    if any(observation["metadata"]["multiplexed"] for observation in (before, after)):
        reasons.append("multiplexed perf counters are diagnostic evidence, not a full-coverage comparison")
    return reasons
