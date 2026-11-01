"""Producer-side export of owned live Snapshot objects, never native snapshot files."""

import hashlib
import json
import platform
from pathlib import Path
import sys
import tracemalloc

from ..adapters.tracemalloc_json import SCHEMA, SAMPLE_LEVEL, VERIFIED_PYTHON, checkpoint_policy, traceback_identity, validate_document
from ..model import require
from .cprofile_json import _output_paths, _write_new_json


def _runtime():
    # Avoid platform's first-call version parsing inside a measured checkpoint.
    version = sys.version_info
    require(sys.implementation.name == "cpython" and version.major == 3 and
            version.minor == 12 and version.micro == 7,
            "tracemalloc producer export is verified only on CPython 3.12.7")


def memory_checkpoint(*, peak_window, sampling_point):
    """Read global counters explicitly; never start, reset, clear or stop tracing."""
    _runtime()
    require(all(isinstance(value, str) and bool(value) for value in (peak_window, sampling_point)),
            "checkpoint peak_window and sampling_point need nonempty strings; use unknown explicitly")
    require(tracemalloc.is_tracing(), "tracemalloc must be active for a memory checkpoint")
    current, peak = tracemalloc.get_traced_memory()
    overhead = tracemalloc.get_tracemalloc_memory()
    return dict(current_bytes=current, peak_bytes=peak, tracer_overhead_bytes=overhead,
                peak_window=peak_window, sampling_point=sampling_point, domain_scope="all_traced_domains")


def _snapshot(snapshot):
    require(isinstance(snapshot, tracemalloc.Snapshot), "export requires owned live tracemalloc.Snapshot objects")
    groups = {}
    for trace in snapshot.traces:
        row = dict(domain=trace.domain, traceback=[dict(file=frame.filename, line=frame.lineno)
                   for frame in trace.traceback], total_nframe=trace.traceback.total_nframe)
        identity = traceback_identity(row)
        item = groups.get(identity)
        if item is None:
            item = groups[identity] = dict(row, size_bytes=0, blocks=0)
        item["size_bytes"] += trace.size
        item["blocks"] += 1
    records = sorted(groups.values(), key=lambda row: (row["domain"],
                     tuple((frame["file"], frame["line"]) for frame in row["traceback"]),
                     -1 if row["total_nframe"] is None else row["total_nframe"]))
    return dict(traceback_limit=snapshot.traceback_limit, records=records)


def default_manifest(document, digest):
    return dict(schema_version=1, bundle_id="tracemalloc-" + digest[:16], created_at="unknown",
        revision="unknown", dirty_patch_sha256="unknown", host={}, build={},
        producer=dict(provider="tracemalloc-json", version=document["producer"]["version"]),
        raw_source=dict(sha256=digest),
        workloads={"snapshot-pair": dict(version="1", parameters="unknown", input_sha256="unknown",
            scope="one declared in-process baseline/ending snapshot pair", threads="unknown",
            instrumentation=dict(kind=SCHEMA, **document["collection"],
                traceback_limit=document["baseline"]["traceback_limit"], checkpoint=checkpoint_policy(document)),
            clock="not_applicable", time_mode="not_applicable", sample_level=SAMPLE_LEVEL)},
        semantic_contracts={"tracemalloc-live-v1": dict(version="1", correctness_evidence="unknown",
            semantics="Tracked live blocks at endpoints, signed retained differences, optional separate global checkpoint; not RSS/allocation volume.")},
        regions=[dict(id="snapshot-pair", variant="profile", workload_id="snapshot-pair", contract_id="tracemalloc-live-v1",
                      source_sha256="unknown", snapshot_pair="baseline-to-ending")])


def export_snapshots(baseline, ending, output_path, *, coverage="unknown", harness_policy="unknown",
                     epoch_policy="unknown", checkpoint=None, manifest_output=None):
    """
    Export this producer's own same-epoch snapshots; caller declares selection/harness policy.

    A separately sampled checkpoint remains global and cannot be attributed to a
    traceback. Snapshot.dump/load are intentionally absent: that format is pickle.
    """
    _runtime()
    output = _output_paths(output_path, manifest_output)
    document = dict(schema=SCHEMA, producer=dict(implementation="CPython", version=VERIFIED_PYTHON,
        exporter_interpreter=sys.version, build=list(platform.python_build())),
        collection=dict(coverage=coverage, harness_policy=harness_policy, epoch_policy=epoch_policy),
        baseline=_snapshot(baseline), ending=_snapshot(ending), checkpoint=checkpoint)
    validate_document(document)
    json.dumps(document, allow_nan=False)
    _write_new_json(output, document)
    if manifest_output is not None:
        digest = hashlib.sha256(Path(output).read_bytes()).hexdigest()
        _write_new_json(manifest_output, default_manifest(document, digest))
    return document
