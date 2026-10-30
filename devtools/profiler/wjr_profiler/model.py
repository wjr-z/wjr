"""The small manifest and observation contract used by the offline CLI."""

import math
import re
import statistics

SCHEMA_VERSION = 1
ENVIRONMENT_FIELDS = (
    "host.id", "host.os", "host.arch", "host.abi", "host.cpu", "host.core",
    "build.compiler", "build.flags", "build.fingerprint", "build.standard",
    "build.sanitizer", "build.lto", "build.pgo",
)
WORKLOAD_FIELDS = (
    "version", "parameters", "input_sha256", "scope", "threads",
    "instrumentation", "clock", "time_mode", "sample_level",
)


def unknown(value):
    if value is None or isinstance(value, str) and value.strip().lower() in ("", "unknown", "unspecified"):
        return True
    if isinstance(value, dict):
        return any(unknown(v) for v in value.values())
    if isinstance(value, list):
        return any(unknown(v) for v in value)
    return False


def field(document, path):
    result = document
    for key in path.split("."):
        if not isinstance(result, dict) or key not in result:
            return None
        result = result[key]
    return result


def require(condition, message):
    if not condition:
        raise ValueError(message)


def is_integer(value):
    """JSON integers exclude bool, even though Python bool subclasses int."""
    return isinstance(value, int) and not isinstance(value, bool)


def is_finite_number(value):
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        return False
    try:
        return math.isfinite(value)
    except OverflowError:
        return False


def _string_fields(document, names, prefix):
    for name in names:
        require(isinstance(document.get(name), str) and bool(document[name]),
                f"{prefix}.{name} must be a nonempty string; use unknown explicitly")


def _manifest_producer(manifest):
    producer = manifest.get("producer")
    require(isinstance(producer, dict), "manifest.producer must be an object")
    _string_fields(producer, ("provider", "version"), "manifest.producer")
    digest = field(manifest, "raw_source.sha256")
    require(isinstance(digest, str) and re.fullmatch(r"[0-9a-fA-F]{64}", digest) is not None,
            "manifest.raw_source.sha256 must be a SHA-256 hex digest")


def _manifest_maps(manifest):
    for key in ("workloads", "semantic_contracts"):
        require(isinstance(manifest.get(key), dict) and bool(manifest[key]),
                f"manifest.{key} must be a nonempty object indexed by id")
        require(all(isinstance(k, str) and isinstance(v, dict)
                    for k, v in manifest[key].items()), f"invalid manifest.{key} entries")


def _manifest_regions(manifest):
    regions = manifest.get("regions")
    require(isinstance(regions, list) and bool(regions), "manifest.regions must be a nonempty list")
    seen = set()
    for region in regions:
        require(isinstance(region, dict), "region must be an object")
        _string_fields(region, ("id", "variant", "workload_id", "contract_id"), "region")
        require(region["id"] not in seen, f"duplicate region id: {region['id']}")
        seen.add(region["id"])
        require(region["workload_id"] in manifest["workloads"], "unknown region workload_id")
        require(region["contract_id"] in manifest["semantic_contracts"], "unknown region contract_id")


def validate_manifest(manifest):
    require(isinstance(manifest, dict), "manifest must be an object")
    require(is_integer(manifest.get("schema_version")) and manifest["schema_version"] == 1,
            "unsupported manifest schema_version; expected 1")
    _string_fields(manifest, ("bundle_id", "created_at", "revision", "dirty_patch_sha256"), "manifest")
    _manifest_producer(manifest)
    _manifest_maps(manifest)
    _manifest_regions(manifest)
    return manifest


def _observation_fields(item, regions):
    require(isinstance(item, dict), "observation must be an object")
    require(item.get("region_id") in regions, "observation references an unknown region")
    _string_fields(item, ("metric", "unit", "scope", "sample_level", "time_basis"), "observation")
    require(item.get("kind") in {"measurement", "estimate", "derived"}, "invalid observation kind")
    require(item.get("direction") in {"lower", "higher", "none"}, "invalid metric direction")
    require(item.get("quality") in {"ok", "missing", "insufficient_samples", "failed", "exploratory"},
            "invalid observation quality")
    require(item.get("missing_reason") is None or isinstance(item["missing_reason"], str),
            "invalid observation missing_reason")
    require(isinstance(item.get("metadata"), dict), "observation metadata must be an object")


def _observation_samples_sources(item):
    samples = item.get("samples")
    require(isinstance(samples, list) and all(is_finite_number(x) for x in samples),
            "invalid finite observation samples")
    require(isinstance(item.get("source_locators"), list) and
            all(isinstance(x, str) and x.startswith("/") for x in item["source_locators"]),
            "source locators must be JSON pointers")


def validate_observations(observations, manifest):
    require(isinstance(observations, list), "adapter observations must be a list")
    regions = {region["id"] for region in manifest["regions"]}
    seen = set()
    for item in observations:
        _observation_fields(item, regions)
        _observation_samples_sources(item)
        identity = (item["region_id"], item["metric"], item["scope"], item["time_basis"])
        require(identity not in seen, f"duplicate observation identity: {identity}")
        seen.add(identity)


def _median(samples):
    ordered = sorted(samples)
    middle = len(ordered) // 2
    return ordered[middle] if len(ordered) % 2 else ordered[middle - 1] / 2 + ordered[middle] / 2


def _finite_stat(function, samples):
    try:
        value = function(samples)
        return value if math.isfinite(value) else None
    except (OverflowError, ValueError):
        return None


def summarize(samples):
    if not samples:
        return dict(n=0, median=None, minimum=None, maximum=None, mad=None,
                    sample_stddev=None, variance=None)
    median = _median(samples)
    return dict(n=len(samples), median=median, minimum=min(samples), maximum=max(samples),
                mad=statistics.median(abs(x - median) for x in samples),
                sample_stddev=_finite_stat(statistics.stdev, samples) if len(samples) >= 2 else None,
                variance=_finite_stat(statistics.variance, samples) if len(samples) >= 2 else None)
