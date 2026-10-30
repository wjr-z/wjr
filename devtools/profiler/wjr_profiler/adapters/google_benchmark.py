"""Import the verified Google Benchmark v1.9.0 schema-1 subset."""

import math

from ..model import is_finite_number, is_integer, require

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


def _format(raw, manifest):
    require(isinstance(raw, dict), "Google Benchmark JSON must be an object")
    context, rows = raw.get("context"), raw.get("benchmarks")
    require(isinstance(context, dict), "Google Benchmark context must be an object")
    require(isinstance(rows, list), "Google Benchmark benchmarks must be a list")
    schema = context.get("json_schema_version")
    require(schema is None or is_integer(schema) and schema == 1,
            "unsupported Google Benchmark json_schema_version; expected 1")
    declared = manifest["producer"]["version"].removeprefix("v")
    reported = str(context.get("library_version", "unknown")).removeprefix("v")
    verified = schema == 1 and declared == "1.9.0" and reported == "1.9.0"
    return context, rows, verified


def _record(row, pointer):
    record = {key: safe_metadata(row[key]) for key in
              ("iterations", "repetition_index", "threads", "repetitions") if key in row}
    record["pointer"] = pointer
    record["user_counters_uninterpreted"] = {
        key: safe_metadata(value) for key, value in row.items() if key not in ROW_FIELDS
    }
    return record


def _nonfinite_counters(row):
    return any(isinstance(value, float) and not math.isfinite(value)
               for key, value in row.items() if key not in ROW_FIELDS)


def _iteration_problem(row, expected_threads, seen):
    if row.get("threads") != expected_threads:
        return "raw threads differ from the declared workload"
    repetition = row.get("repetition_index")
    if not is_integer(repetition) or repetition < 0 or repetition in seen:
        return "missing, invalid or duplicate repetition_index"
    seen.add(repetition)
    if not is_integer(row.get("iterations")) or row["iterations"] <= 0:
        return "missing or invalid internal iterations count"
    return None


def _convert_time(row, metric):
    unit = row.get("time_unit")
    scale = UNITS.get(unit) if isinstance(unit, str) else None
    if scale is None:
        return None, "missing or unsupported time_unit"
    value = row.get(metric)
    if not is_finite_number(value) or value < 0:
        return None, f"missing, negative or non-finite {metric}"
    converted = value * scale
    if not math.isfinite(converted):
        return None, "time conversion overflow"
    return converted, None


def _consume_row(row, pointer, metric, expected_threads, seen):
    if row.get("error_occurred"):
        return None, None, str(row.get("error_message", "benchmark reported an error")), True
    if row.get("run_type") == "aggregate":
        aggregate = dict(pointer=pointer, name=row.get("aggregate_name"),
                         unit=row.get("aggregate_unit"), value=safe_metadata(row.get(metric)))
        return None, aggregate, None, False
    if row.get("run_type") != "iteration":
        return None, None, "missing or unsupported run_type", False
    problem = _iteration_problem(row, expected_threads, seen)
    if problem:
        return None, None, problem, False
    value, problem = _convert_time(row, metric)
    return value, None, problem, False


def _initial_collection(workload):
    result = dict(samples=[], source_locators=[], records=[], aggregates=[], problems=[], failed=False)
    if workload.get("sample_level") != "in_process_repetition":
        result["problems"].append(
            "a native Google Benchmark file establishes in-process repetitions, not independent process runs")
    return result


def _collect(selected, region, workload, metric):
    result, diagnostics, seen = _initial_collection(workload), [], set()
    for index, row in selected:
        pointer = f"/benchmarks/{index}"
        locator = pointer + "/" + metric if metric in row else pointer
        result["source_locators"].append(locator)
        result["records"].append(_record(row, pointer))
        if _nonfinite_counters(row):
            diagnostics.append(dict(code="nonfinite_counter", region_id=region["id"], pointer=pointer,
                                    message="Non-finite user counter retained as null; no unit or Counter flags inferred."))
        value, aggregate, problem, failed = _consume_row(row, pointer, metric, workload.get("threads"), seen)
        result["failed"] |= failed
        if problem:
            result["problems"].append(problem)
        if aggregate is not None:
            result["aggregates"].append(aggregate)
        if value is not None:
            result["samples"].append(value)
    return result, diagnostics


def _quality(result, verified):
    if result["failed"]:
        return "failed"
    if result["problems"]:
        return "missing"
    if not verified:
        return "exploratory"
    return "insufficient_samples" if len(result["samples"]) < 5 else "ok"


def _sample_problems(result, selected):
    if not selected:
        return ["declared benchmark run_name was not present"]
    if not result["samples"] and result["aggregates"]:
        return ["only aggregates present; no raw repetition samples"]
    if len(result["samples"]) < 5:
        return ["fewer than five raw repetitions; internal iterations are not independent samples"]
    return []


def _quality_diagnostics(region, result):
    pointer = result["source_locators"][0] if result["source_locators"] else "/benchmarks"
    return [dict(code="observation_quality", region_id=region["id"], pointer=pointer, message=message)
            for message in sorted(set(result["problems"]))]


def _observation(region, workload, selected, context, verified, metric, time_basis):
    result, diagnostics = _collect(selected, region, workload, metric)
    quality = _quality(result, verified)
    result["problems"].extend(_sample_problems(result, selected))
    diagnostics.extend(_quality_diagnostics(region, result))
    observation = dict(
        region_id=region["id"], metric=metric, kind="measurement", unit="ns/iteration",
        scope=workload.get("scope", "unknown"), direction="lower", samples=result["samples"],
        sample_level="in_process_repetition", quality=quality,
        missing_reason="; ".join(sorted(set(result["problems"]))) or None, time_basis=time_basis,
        source_locators=result["source_locators"], metadata=dict(
            benchmark=region["benchmark"], context=safe_metadata(context), records=result["records"],
            aggregates=result["aggregates"], adapter_version=ADAPTER_VERSION,
            counter_policy="display only; unknown units/Counter flags never enter comparisons",
        ),
    )
    return observation, diagnostics


def _select(rows, benchmark):
    result = []
    for index, row in enumerate(rows):
        require(isinstance(row, dict), f"benchmarks/{index} must be an object")
        if row.get("run_name", row.get("name")) == benchmark:
            result.append((index, row))
    return result


def import_artifact(raw, manifest):
    context, rows, verified = _format(raw, manifest)
    observations, diagnostics = [], []
    if not verified:
        diagnostics.append(dict(code="unverified_tool_version", pointer="/context",
                                message="Only Google Benchmark v1.9.0 schema 1 is verified; exploratory only."))
    for region in manifest["regions"]:
        benchmark = region.get("benchmark")
        require(isinstance(benchmark, str) and bool(benchmark), "Google Benchmark region needs benchmark run_name")
        workload = manifest["workloads"][region["workload_id"]]
        selected = _select(rows, benchmark)
        for metric, time_basis in (("real_time", "wall"), ("cpu_time", "cpu")):
            observation, extra = _observation(region, workload, selected, context, verified, metric, time_basis)
            observations.append(observation)
            diagnostics.extend(extra)
    return observations, diagnostics
