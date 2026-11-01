"""Offline live-block evidence; no Snapshot/pickle loading or timing inference."""

from ..model import is_integer, require, unknown

SCHEMA = "wjr.tracemalloc-json/1"
VERIFIED_PYTHON = "3.12.7"
SAMPLE_LEVEL = "in_process_snapshot_pair"
COLLECTION_FIELDS = ("coverage", "harness_policy", "epoch_policy")
CHECKPOINT_FIELDS = ("current_bytes", "peak_bytes", "tracer_overhead_bytes")
LIMITS = [
    "One in-process snapshot pair is n=1; blocks/frames are not repetitions.",
    "Snapshot order, tracing epoch and filtering are caller declarations, not verified Snapshot metadata.",
    "Net retained bytes/blocks are endpoint differences, not allocation volume or a leak verdict.",
    "Snapshots omit blocks freed before both endpoints and allocations outside tracing coverage.",
    "Traced sizes and tracing overhead are not RSS, allocator reservation or copy counts.",
    "Global peak has no per-traceback attribution and need not equal the ending snapshot size.",
    "Truncated or unknown tracebacks cannot identify an unrecorded caller.",
]


def _text(record, names, prefix):
    for name in names:
        require(isinstance(record.get(name), str) and bool(record[name]),
                prefix + "." + name + " must be a nonempty string; use unknown explicitly")


def _unsigned(value, label, minimum=0):
    require(is_integer(value) and value >= minimum, label + " must be an integer >= " + str(minimum))


def traceback_identity(row):
    """Retain domains and original depth; Snapshot.statistics discards these axes."""
    require(isinstance(row, dict), "traceback record must be an object")
    _unsigned(row.get("domain"), "trace domain")
    require(row["domain"] <= 0xffffffff, "trace domain exceeds CPython's unsigned domain")
    frames = row.get("traceback")
    require(isinstance(frames, list) and bool(frames), "traceback must contain frames")
    key = []
    for frame in frames:
        require(isinstance(frame, dict), "frame must be an object")
        require(isinstance(frame.get("file"), str), "frame.file must be a string")
        _unsigned(frame.get("line"), "frame line")
        key.append((frame["file"], frame["line"]))
    depth = row.get("total_nframe")
    require("total_nframe" in row, "traceback must explicitly preserve total_nframe or null")
    if depth is not None:
        _unsigned(depth, "total_nframe", len(frames))
    return row["domain"], tuple(key), depth


def _snapshot(value, name):
    require(isinstance(value, dict), name + " must be a snapshot object")
    _unsigned(value.get("traceback_limit"), name + ".traceback_limit", 1)
    require(isinstance(value.get("records"), list), name + ".records must be a list")
    index = {}
    for position, row in enumerate(value["records"]):
        identity = traceback_identity(row)
        require(identity not in index, "duplicate domain/traceback/depth identity in " + name)
        require(len(row["traceback"]) <= value["traceback_limit"], "traceback exceeds snapshot limit")
        _unsigned(row.get("size_bytes"), "traceback size_bytes")
        _unsigned(row.get("blocks"), "traceback blocks", 1)
        index[identity] = (position, row)
    return index


def validate_document(raw):
    require(isinstance(raw, dict) and raw.get("schema") == SCHEMA, "unsupported tracemalloc JSON schema")
    producer = raw.get("producer")
    require(isinstance(producer, dict), "tracemalloc producer must be an object")
    _text(producer, ("implementation", "version"), "producer")
    collection = raw.get("collection")
    require(isinstance(collection, dict), "tracemalloc collection must be an object")
    _text(collection, COLLECTION_FIELDS, "collection")
    before = _snapshot(raw.get("baseline"), "baseline")
    after = _snapshot(raw.get("ending"), "ending")
    require(raw["baseline"]["traceback_limit"] == raw["ending"]["traceback_limit"],
            "snapshot traceback limits differ; use the same tracing configuration")
    checkpoint = raw.get("checkpoint")
    if checkpoint is not None:
        require(isinstance(checkpoint, dict), "checkpoint must be an object or null")
        for key in CHECKPOINT_FIELDS:
            _unsigned(checkpoint.get(key), "checkpoint." + key)
        require(checkpoint["peak_bytes"] >= checkpoint["current_bytes"], "peak cannot be below current bytes")
        _text(checkpoint, ("peak_window", "sampling_point"), "checkpoint")
        require(checkpoint.get("domain_scope") == "all_traced_domains", "checkpoint must cover all traced domains")
    return before, after


def _site_rows(before, after):
    rows = []
    for key in before.keys() | after.keys():
        old, new = before.get(key), after.get(key)
        source = new[1] if new else old[1]
        pointers = ([f"/baseline/records/{old[0]}"] if old else [])
        pointers += ([f"/ending/records/{new[0]}"] if new else [])
        old_bytes, old_blocks = (old[1]["size_bytes"], old[1]["blocks"]) if old else (0, 0)
        new_bytes, new_blocks = (new[1]["size_bytes"], new[1]["blocks"]) if new else (0, 0)
        depth = source["total_nframe"]
        rows.append(dict(domain=source["domain"], traceback=source["traceback"], total_nframe=depth,
                         truncated=None if depth is None else depth > len(source["traceback"]),
                         unknown_frames=any(frame["file"] in ("", "<unknown>") or frame["line"] == 0
                                            for frame in source["traceback"]),
                         baseline_bytes=old_bytes, ending_bytes=new_bytes,
                         net_retained_bytes=new_bytes - old_bytes,
                         baseline_blocks=old_blocks, ending_blocks=new_blocks,
                         net_retained_blocks=new_blocks - old_blocks, source_locators=pointers))
    return sorted(rows, key=lambda row: (row["domain"],
                  tuple((frame["file"], frame["line"]) for frame in row["traceback"]),
                  -1 if row["total_nframe"] is None else row["total_nframe"]))


def _quality(raw, manifest):
    producer = raw["producer"]
    if (producer["implementation"] != "CPython" or producer["version"] != VERIFIED_PYTHON or
            manifest["producer"]["version"] != VERIFIED_PYTHON):
        return "exploratory", "only CPython 3.12.7 producer export is verified"
    if any(unknown(raw["collection"][name]) for name in COLLECTION_FIELDS):
        return "exploratory", "snapshot selection/epoch/harness policy is unknown"
    return "insufficient_samples", "one in-process snapshot pair; no between-run variance"


def checkpoint_policy(raw):
    checkpoint = raw.get("checkpoint")
    return {key: checkpoint[key] for key in ("peak_window", "sampling_point", "domain_scope")} if checkpoint else None


def _contract(workload, raw):
    require(workload.get("clock") == workload.get("time_mode") == "not_applicable",
            "tracemalloc clock/time_mode must be not_applicable")
    require(workload.get("sample_level") == SAMPLE_LEVEL, "tracemalloc needs in_process_snapshot_pair sample level")
    instrumentation = workload.get("instrumentation")
    require(isinstance(instrumentation, dict) and instrumentation.get("kind") == SCHEMA,
            "tracemalloc workload must declare its snapshot instrumentation")
    require(all(instrumentation.get(name) == raw["collection"][name] for name in COLLECTION_FIELDS),
            "manifest snapshot policy does not match the raw collection")
    require(instrumentation.get("traceback_limit") == raw["baseline"]["traceback_limit"],
            "manifest traceback limit does not match the raw snapshots")
    require(instrumentation.get("checkpoint") == checkpoint_policy(raw),
            "manifest checkpoint scope does not match the raw checkpoint")


def import_artifact(raw, manifest):
    before, after = validate_document(raw)
    sites = _site_rows(before, after)
    quality, reason = _quality(raw, manifest)
    before_bytes = sum(row[1]["size_bytes"] for row in before.values())
    after_bytes = sum(row[1]["size_bytes"] for row in after.values())
    before_blocks = sum(row[1]["blocks"] for row in before.values())
    after_blocks = sum(row[1]["blocks"] for row in after.values())
    metrics = [
        ("baseline_live_bytes", before_bytes, "bytes/snapshot", "measurement", ["/baseline"]),
        ("ending_live_bytes", after_bytes, "bytes/snapshot", "measurement", ["/ending"]),
        ("net_retained_bytes", after_bytes - before_bytes, "bytes/snapshot-pair", "derived", ["/baseline", "/ending"]),
        ("baseline_live_blocks", before_blocks, "blocks/snapshot", "measurement", ["/baseline"]),
        ("ending_live_blocks", after_blocks, "blocks/snapshot", "measurement", ["/ending"]),
        ("net_retained_blocks", after_blocks - before_blocks, "blocks/snapshot-pair", "derived", ["/baseline", "/ending"]),
    ]
    checkpoint = raw.get("checkpoint")
    for name in CHECKPOINT_FIELDS:
        metrics.append(("traced_" + name if name != "tracer_overhead_bytes" else name,
                        checkpoint[name] if checkpoint else None, "bytes/checkpoint", "measurement",
                        ["/checkpoint/" + name] if checkpoint else ["/checkpoint"]))
    observations, diagnostics = [], []
    for region in manifest["regions"]:
        require(region.get("snapshot_pair") == "baseline-to-ending", "region must explicitly select baseline-to-ending")
        _contract(manifest["workloads"][region["workload_id"]], raw)
        for metric, value, unit, kind, locators in metrics:
            is_checkpoint = unit == "bytes/checkpoint"
            problem = "no checkpoint supplied; snapshots cannot establish a peak/current counter" if value is None else reason
            item_quality = "missing" if value is None else quality
            if is_checkpoint and checkpoint and (unknown(checkpoint["peak_window"]) or unknown(checkpoint["sampling_point"])):
                item_quality, problem = "exploratory", "checkpoint window/sampling point is unknown"
            metadata = dict(producer=raw["producer"], collection=raw["collection"],
                            traceback_limit=raw["baseline"]["traceback_limit"],
                            checkpoint=checkpoint, adapter_version="1", limits=LIMITS)
            if metric == "net_retained_bytes":
                metadata["tracebacks"] = sites
            observations.append(dict(region_id=region["id"], metric=metric, kind=kind, unit=unit,
                scope="all traced domains at declared checkpoint" if is_checkpoint else "all supplied snapshot domains",
                direction="none", samples=[] if value is None else [value], sample_level=SAMPLE_LEVEL,
                quality=item_quality, missing_reason=problem,
                time_basis="allocation" if "blocks" in metric else "memory",
                source_locators=locators, metadata=metadata))
        diagnostics.append(dict(code="tracemalloc_" + quality, region_id=region["id"], pointer="/", message=reason))
        if any(row["truncated"] is not False or row["unknown_frames"] for row in sites):
            diagnostics.append(dict(code="tracemalloc_partial_tracebacks", region_id=region["id"],
                                    pointer="/", message="Truncated/unknown traceback depth is retained; missing callers are not inferred."))
    return observations, diagnostics
