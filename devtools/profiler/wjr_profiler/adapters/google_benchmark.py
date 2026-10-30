"""Import the verified Google Benchmark v1.9.0 schema-1 subset."""

import math

from ..model import require

ADAPTER_VERSION = "1"
UNITS = {"ns": 1.0, "us": 1000.0, "ms": 1000000.0, "s": 1000000000.0}
ROW_FIELDS = {
    "name", "run_name", "run_type", "family_index", "per_family_instance_index",
    "repetitions", "repetition_index", "threads", "iterations", "real_time", "cpu_time",
    "time_unit", "aggregate_name", "aggregate_unit", "error_occurred", "error_message",
}


def safe_metadata(value):
    if isinstance(value, float) and not math.isfinite(value):
        return None
    if isinstance(value, dict):
        return {key: safe_metadata(item) for key, item in value.items()}
    if isinstance(value, list):
        return [safe_metadata(item) for item in value]
    return value


def import_artifact(raw, manifest):
    require(isinstance(raw, dict), "Google Benchmark JSON must be an object")
    context = raw.get("context")
    rows = raw.get("benchmarks")
    require(isinstance(context, dict), "Google Benchmark context must be an object")
    require(isinstance(rows, list), "Google Benchmark benchmarks must be a list")
    schema = context.get("json_schema_version")
    require(schema is None or type(schema) is int and schema == 1,
            "unsupported Google Benchmark json_schema_version; expected 1")
    diagnostics = []
    declared_version = manifest["producer"]["version"].removeprefix("v")
    reported_version = str(context.get("library_version", "unknown")).removeprefix("v")
    verified = schema == 1 and declared_version == "1.9.0" and reported_version == "1.9.0"
    if not verified:
        diagnostics.append(dict(code="unverified_tool_version", pointer="/context",
                                message="Only Google Benchmark v1.9.0 schema 1 is verified; exploratory only."))
    observations = []
    for region in manifest["regions"]:
        benchmark = region.get("benchmark")
        require(isinstance(benchmark, str) and bool(benchmark), "Google Benchmark region needs benchmark run_name")
        workload = manifest["workloads"][region["workload_id"]]
        selected = []
        for index, row in enumerate(rows):
            require(isinstance(row, dict), f"benchmarks/{index} must be an object")
            if row.get("run_name", row.get("name")) == benchmark:
                selected.append((index, row))
        for metric, time_basis in (("real_time", "wall"), ("cpu_time", "cpu")):
            samples, locators, records, aggregates, problems = [], [], [], [], []
            failed = False
            seen_repetitions = set()
            if workload.get("sample_level") != "in_process_repetition":
                problems.append("a native Google Benchmark file establishes in-process repetitions, not independent process runs")
            for index, row in selected:
                pointer = f"/benchmarks/{index}"
                locators.append(pointer + "/" + metric if metric in row else pointer)
                record = {key: safe_metadata(row[key]) for key in
                          ("iterations", "repetition_index", "threads", "repetitions") if key in row}
                record["pointer"] = pointer
                record["user_counters_uninterpreted"] = {
                    key: safe_metadata(value) for key, value in row.items() if key not in ROW_FIELDS
                }
                records.append(record)
                if any(type(v) is float and not math.isfinite(v)
                       for k, v in row.items() if k not in ROW_FIELDS):
                    diagnostics.append(dict(code="nonfinite_counter", region_id=region["id"], pointer=pointer,
                                            message="Non-finite user counter retained as null; no unit or Counter flags inferred."))
                if row.get("error_occurred"):
                    failed = True
                    problems.append(str(row.get("error_message", "benchmark reported an error")))
                    continue
                run_type = row.get("run_type")
                if run_type == "aggregate":
                    aggregates.append(dict(pointer=pointer, name=row.get("aggregate_name"),
                                           unit=row.get("aggregate_unit"), value=safe_metadata(row.get(metric))))
                    continue
                if run_type != "iteration":
                    problems.append("missing or unsupported run_type")
                    continue
                if row.get("threads") != workload.get("threads"):
                    problems.append("raw threads differ from the declared workload")
                    continue
                repetition = row.get("repetition_index")
                if type(repetition) is not int or repetition < 0 or repetition in seen_repetitions:
                    problems.append("missing, invalid or duplicate repetition_index")
                    continue
                seen_repetitions.add(repetition)
                if type(row.get("iterations")) is not int or row["iterations"] <= 0:
                    problems.append("missing or invalid internal iterations count")
                    continue
                unit = row.get("time_unit")
                scale = UNITS.get(unit) if isinstance(unit, str) else None
                value = row.get(metric)
                if scale is None:
                    problems.append("missing or unsupported time_unit")
                    continue
                if type(value) not in (int, float) or not math.isfinite(value) or value < 0:
                    problems.append(f"missing, negative or non-finite {metric}")
                    continue
                converted = value * scale
                if not math.isfinite(converted):
                    problems.append("time conversion overflow")
                    continue
                samples.append(converted)
            if failed:
                quality = "failed"
            elif problems:
                quality = "missing"
            elif not verified:
                quality = "exploratory"
            elif len(samples) < 5:
                quality = "insufficient_samples"
            else:
                quality = "ok"
            if not selected:
                problems.append("declared benchmark run_name was not present")
            elif not samples and aggregates:
                problems.append("only aggregates present; no raw repetition samples")
            elif len(samples) < 5:
                problems.append("fewer than five raw repetitions; internal iterations are not independent samples")
            for message in sorted(set(problems)):
                diagnostics.append(dict(code="observation_quality", region_id=region["id"],
                                        pointer=locators[0] if locators else "/benchmarks", message=message))
            observations.append(dict(
                region_id=region["id"], metric=metric, kind="measurement", unit="ns/iteration",
                scope=workload.get("scope", "unknown"), direction="lower", samples=samples,
                sample_level="in_process_repetition", quality=quality,
                missing_reason="; ".join(sorted(set(problems))) or None, time_basis=time_basis,
                source_locators=locators, metadata=dict(
                    benchmark=benchmark, context=safe_metadata(context), records=records,
                    aggregates=aggregates, adapter_version=ADAPTER_VERSION,
                    counter_policy="display only; unknown units/Counter flags never enter comparisons",
                ),
            ))
    return observations, diagnostics
