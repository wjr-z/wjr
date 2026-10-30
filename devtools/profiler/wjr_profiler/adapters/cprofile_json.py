"""Read only the versioned JSON export; never load .prof/marshal input here."""

from ..model import is_finite_number, is_integer, require, unknown

ADAPTER_VERSION = "1"
METRICS = (
    ("self_time", "self_s", "s/run", "function exclusive", "lower"),
    ("cumulative_time", "cumulative_s", "s/run", "function inclusive", "lower"),
    ("primitive_calls", "primitive_calls", "count/run", "function primitive calls", "none"),
    ("total_calls", "total_calls", "count/run", "function total calls", "none"),
)


def _identity(record):
    require(isinstance(record, dict), "cProfile function identity must be an object")
    require(isinstance(record.get("file"), str), "cProfile file must be a string")
    require(isinstance(record.get("name"), str), "cProfile name must be a string")
    require(is_integer(record.get("line")) and record["line"] >= 0, "cProfile line must be a nonnegative integer")
    return record["file"], record["line"], record["name"]


def _counts_times(record):
    for key in ("primitive_calls", "total_calls"):
        require(is_integer(record.get(key)) and record[key] >= 0, "invalid cProfile " + key)
    require(record["total_calls"] >= record["primitive_calls"], "total calls cannot be below primitive calls")
    for key in ("self_s", "cumulative_s"):
        require(is_finite_number(record.get(key)) and record[key] >= 0, "invalid cProfile " + key)


def _function_index(raw):
    functions = raw.get("functions")
    require(isinstance(functions, list), "cProfile functions must be a list")
    result = {}
    for row in functions:
        key = _identity(row)
        require(key not in result, "duplicate cProfile (file,line,name) identity")
        _counts_times(row)
        require(isinstance(row.get("callers"), list), "cProfile callers must be a list")
        for caller in row["callers"]:
            _identity(caller)
            _counts_times(caller)
        result[key] = row
    return result


def _format(raw, manifest):
    require(isinstance(raw, dict), "cProfile JSON must be an object")
    require(raw.get("schema") == "wjr.cprofile-json/1", "unsupported cProfile JSON schema; expected wjr.cprofile-json/1")
    producer = raw.get("producer")
    require(isinstance(producer, dict), "cProfile producer must be an object")
    verified = producer.get("implementation") == "CPython" and producer.get("version") == "3.12.7"
    verified &= manifest["producer"]["version"] == "3.12.7"
    return producer, verified


def _quality(row, producer, verified):
    if row is None:
        return "missing", "explicit (file,line,name) mapping was not present; names/lines are not guessed"
    if not verified:
        return "exploratory", "only CPython 3.12.7 cProfile export is verified"
    for key in ("timer", "time_basis", "thread_coverage"):
        if unknown(producer.get(key)):
            return "exploratory", "unknown cProfile producer " + key
    return "insufficient_samples", "one profile is one instrumented process run; calls are not repetitions"


def _observations(region, row, index, producer, verified):
    quality, reason = _quality(row, producer, verified)
    result = []
    for metric, field, unit, scope, direction in METRICS:
        time_basis = producer.get("time_basis", "unknown") if field.endswith("_s") else "event_count"
        result.append(dict(
            region_id=region["id"], metric=metric, kind="measurement", unit=unit, scope=scope,
            direction=direction, samples=[row[field]] if row else [],
            sample_level="independent_process_run", quality=quality, missing_reason=reason,
            time_basis=time_basis, source_locators=[f"/functions/{index}/{field}"] if row else ["/functions"],
            metadata=dict(function=region["function"], producer=producer,
                          callers=row["callers"] if row else [], count_kind="exact_instrumented",
                          adapter_version=ADAPTER_VERSION,
                          limits=["Cumulative times overlap and must not be summed.",
                                  "Deterministic instrumentation perturbs execution; native extension interiors are not covered.",
                                  "One profile has no between-run variance; function identities require explicit cross-revision mapping."]),
        ))
    return result


def import_artifact(raw, manifest):
    producer, verified = _format(raw, manifest)
    functions = _function_index(raw)
    positions = {_identity(row): i for i, row in enumerate(raw["functions"])}
    observations, diagnostics = [], []
    for region in manifest["regions"]:
        key = _identity(region.get("function"))
        row = functions.get(key)
        observations.extend(_observations(region, row, positions.get(key), producer, verified))
        quality, reason = _quality(row, producer, verified)
        diagnostics.append(dict(code="cprofile_" + quality, region_id=region["id"],
                                pointer="/functions", message=reason))
    return observations, diagnostics
