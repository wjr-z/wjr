"""Export a producer's own CPython 3.12.7 cProfile data without lossy name aggregation."""

import hashlib
import json
import platform
import pstats
from pathlib import Path
import sys

from ..model import require

SUPPORTED_PYTHON = "3.12.7"


def function_identity(key):
    filename, line, name = key
    return dict(file=filename, line=line, name=name)


def function_id(identity):
    data = json.dumps(identity, sort_keys=True).encode()
    return "function-" + hashlib.sha256(data).hexdigest()


def _caller(key, values):
    # CPython cProfile caller tuples use total, primitive, self, cumulative order.
    require(isinstance(values, tuple) and len(values) == 4, "unsupported cProfile caller tuple")
    total, primitive, self_s, cumulative_s = values
    return dict(function_identity(key), total_calls=total, primitive_calls=primitive,
                self_s=self_s, cumulative_s=cumulative_s)


def _function(key, values):
    require(isinstance(values, tuple) and len(values) == 5, "unsupported cProfile function tuple")
    primitive, total, self_s, cumulative_s, callers = values
    return dict(function_identity(key), primitive_calls=primitive, total_calls=total,
                self_s=self_s, cumulative_s=cumulative_s,
                callers=[_caller(caller, edge) for caller, edge in sorted(callers.items())])


def default_manifest(document, digest):
    regions = [dict(id=function_id({k: row[k] for k in ("file", "line", "name")}),
                    variant="profile", workload_id="profile-run", contract_id="cprofile-function-v1",
                    source_sha256="unknown", function={k: row[k] for k in ("file", "line", "name")})
               for row in document["functions"]]
    return dict(
        schema_version=1, bundle_id="cprofile-" + digest[:16], created_at="unknown",
        revision="unknown", dirty_patch_sha256="unknown",
        producer=dict(provider="cprofile-json", version=document["producer"]["version"]),
        raw_source=dict(sha256=digest), host={}, build={},
        workloads={"profile-run": dict(version="1", parameters="unknown", input_sha256="unknown",
                    scope="one instrumented process run; functions have separate self/inclusive scopes",
                    threads="unknown", instrumentation="cProfile deterministic",
                    clock=document["producer"]["timer"], time_mode="instrumented",
                    sample_level="independent_process_run")},
        semantic_contracts={"cprofile-function-v1": dict(version="1",
                    semantics="Per-function cProfile call counts and exclusive/inclusive instrumented seconds; cumulative times overlap.",
                    correctness_evidence="unknown")}, regions=regions,
    )


def _producer_version(producer_version):
    runtime = platform.python_version()
    require(sys.implementation.name == "cpython" and runtime == SUPPORTED_PYTHON,
            "cProfile exporter is verified only on CPython 3.12.7")
    require(producer_version == runtime, "declare the matching producer interpreter version; foreign .prof is unsupported")


def _output_paths(output_path, manifest_output):
    output = Path(output_path)
    require(not output.exists(), "export output already exists")
    if manifest_output is not None:
        manifest = Path(manifest_output)
        require(not manifest.exists(), "manifest output already exists")
        require(manifest.resolve() != output.resolve(), "JSON output and manifest output must differ")
    return output


def _write_new_json(path, document):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("x", encoding="utf-8", newline="\n") as stream:
        stream.write(json.dumps(document, indent=2, sort_keys=True, allow_nan=False) + "\n")


def export_profile(input_path, output_path, producer_version, timer="unknown",
                   time_basis="unknown", thread_coverage="unknown", manifest_output=None):
    """Explicit trusted-local conversion; .prof itself cannot establish producer metadata."""
    _producer_version(producer_version)
    output = _output_paths(output_path, manifest_output)
    try:
        stats = pstats.Stats(str(input_path))
    except (TypeError, EOFError) as error:
        raise ValueError("input is not a supported local cProfile file") from error
    document = dict(schema="wjr.cprofile-json/1", producer=dict(
        implementation="CPython", version=producer_version, exporter_interpreter=sys.version,
        build=list(platform.python_build()), timer=timer, time_basis=time_basis,
        thread_coverage=thread_coverage,
        provenance="Producer version/timer/coverage are explicit declarations; .prof does not embed them."),
        input_prof_sha256=hashlib.sha256(Path(input_path).read_bytes()).hexdigest(),
        functions=[_function(key, values) for key, values in sorted(stats.stats.items())])
    json.dumps(document, allow_nan=False)
    _write_new_json(output, document)
    if manifest_output is not None:
        digest = hashlib.sha256(output.read_bytes()).hexdigest()
        _write_new_json(manifest_output, default_manifest(document, digest))
    return document
