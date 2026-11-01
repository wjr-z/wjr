"""Explicit Google Benchmark 1.9.2 MemoryManager pass metrics, separate from timing."""

from ..model import is_finite_number, is_integer, require
from . import google_benchmark as native

FIELDS = {
    "allocs_per_iter": ("allocations/iteration", "lower"),
    "max_bytes_used": ("bytes/pass", "lower"),
    "total_allocated_bytes": ("bytes/pass", "lower"),
    "net_heap_growth": ("bytes/pass", "none"),
}
TOMBSTONE = (1 << 63) - 1


def _contract(workload):
    require(is_integer(workload.get("threads")) and workload["threads"] == 1,
            "MemoryManager import supports single-thread benchmarks only")
    require(workload.get("clock") == workload.get("time_mode") == "not_applicable",
            "MemoryManager clock and time_mode must be not_applicable")
    require(workload.get("sample_level") == "memory_pass_repetition",
            "MemoryManager sample_level must be memory_pass_repetition")
    instrumentation = workload.get("instrumentation")
    require(isinstance(instrumentation, dict), "MemoryManager needs explicit instrumentation metadata")
    require(instrumentation.get("kind") == "google-benchmark-memory-manager/1",
            "unsupported MemoryManager instrumentation contract")
    require(instrumentation.get("collection_method") == "exact_instrumented",
            "MemoryManager import requires declared exact_instrumented hook counts")
    for name in ("manager_id", "coverage", "harness_policy"):
        require(isinstance(instrumentation.get(name), str) and bool(instrumentation[name]),
                "MemoryManager instrumentation needs " + name + "; use unknown explicitly")
    count = instrumentation.get("pass_iterations")
    require(is_integer(count) and 1 <= count <= 16,
            "MemoryManager pass_iterations must explicitly identify 1..16 iterations")
    return count


def _value(row, metric):
    value = row.get(metric)
    if metric == "allocs_per_iter":
        return value if is_finite_number(value) and value >= 0 else None
    if not is_integer(value) or not -(1 << 63) <= value <= TOMBSTONE:
        return None
    if metric != "net_heap_growth" and value < 0:
        return None
    if metric in ("total_allocated_bytes", "net_heap_growth") and value == TOMBSTONE:
        return None
    return value


def _row_problem(row, metric, pass_iterations, seen):
    if row.get("error_occurred"):
        return str(row.get("error_message", "benchmark reported an error"))
    if row.get("run_type") != "iteration":
        return "only raw iteration rows establish MemoryManager pass samples"
    problem = native._iteration_problem(row, 1, seen)
    if problem:
        return problem
    if not is_integer(row.get("threads")):
        return "invalid raw threads count"
    repeats = row.get("repetitions")
    if not is_integer(repeats) or repeats <= 0 or row["repetition_index"] >= repeats:
        return "invalid declared repetition count or repetition index"
    if min(16, row["iterations"]) != pass_iterations:
        return "timing row implies a different MemoryManager pass length at the verified 1.9.2 source"
    if _value(row, metric) is None:
        return "missing, tombstone or invalid native " + metric
    return None


def _observation(region, workload, selected, context, count, metric):
    samples, pointers, problems, records = [], [], [], []
    seen, failed = set(), False
    for index, row in selected:
        if row.get("run_type") == "aggregate" and not row.get("error_occurred"):
            continue
        pointer = f"/benchmarks/{index}"
        pointers.append(pointer + "/" + metric if metric in row else pointer)
        record = native._record(row, pointer)
        record["user_counters_uninterpreted"] = {
            key: value for key, value in record["user_counters_uninterpreted"].items() if key not in FIELDS
        }
        record["memory_fields"] = {key: native.safe_metadata(row[key]) for key in FIELDS if key in row}
        records.append(record)
        failed |= bool(row.get("error_occurred"))
        problem = _row_problem(row, metric, count, seen)
        if problem:
            problems.append(problem)
        else:
            samples.append(_value(row, metric))
    if not selected:
        problems.append("declared benchmark run_name was not present")
    elif not records:
        problems.append("only aggregates present; no raw MemoryManager pass samples")
    counts = {record["repetitions"] for record in records if is_integer(record.get("repetitions"))}
    if records and (len(counts) != 1 or not seen or min(seen) != 0 or
                    max(seen) != next(iter(counts)) - 1 or len(seen) != next(iter(counts))):
        problems.append("incomplete or inconsistent repetition set")
    quality = native._quality(dict(failed=failed, problems=problems, samples=samples), True)
    if len(samples) < 5:
        problems.append("fewer than five raw MemoryManager pass repetitions")
    diagnostics = [dict(code="memory_pass_quality", region_id=region["id"],
                        pointer=pointers[0] if pointers else "/benchmarks", message=text)
                   for text in sorted(set(problems))]
    unit, direction = FIELDS[metric]
    observation = dict(region_id=region["id"], metric=metric, kind="measurement", unit=unit,
                       scope=workload.get("scope", "unknown"), direction=direction, samples=samples,
                       sample_level="memory_pass_repetition", quality=quality,
                       missing_reason="; ".join(sorted(set(problems))) or None,
                       time_basis="allocation" if metric == "allocs_per_iter" else "memory",
                       source_locators=pointers or ["/benchmarks"],
                       metadata=dict(benchmark=region["benchmark"], context=native.safe_metadata(context),
                                     records=records, pass_iterations=count, adapter_version="1",
                                     instrumentation=workload["instrumentation"],
                                     pass_threads=1, counter_policy="only native MemoryManager fields interpreted",
                                     limitations=["Not RSS, pool reservation or timing-loop memory.",
                                                  "Hook coverage and harness policy are producer declarations.",
                                                  "Byte totals are per pass; no division by timing iterations."]))
    return observation, diagnostics


def import_artifact(raw, manifest):
    context, rows, verified = native._format(raw, manifest, verified_version="1.9.2")
    require(verified, "MemoryManager import requires verified Google Benchmark 1.9.2 schema 1; "
            "1.9.0/1.9.1 can corrupt repeated memory results (upstream #1939/#1941)")
    observations, diagnostics = [], []
    for region in manifest["regions"]:
        benchmark = region.get("benchmark")
        require(isinstance(benchmark, str) and bool(benchmark), "MemoryManager region needs benchmark run_name")
        selected = native._select(rows, benchmark)
        workload = manifest["workloads"][region["workload_id"]]
        count = _contract(workload)
        for metric in FIELDS:
            observation, extra = _observation(region, workload, selected, context, count, metric)
            observations.append(observation)
            diagnostics.extend(extra)
    return observations, diagnostics
