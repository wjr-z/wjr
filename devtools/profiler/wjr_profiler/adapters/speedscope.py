"""Offline py-spy sampled stacks with recorded weights and explicit thread selection."""

import math

from ..model import is_finite_number, is_integer, require

SCHEMA = "https://www.speedscope.app/file-format-schema.json"
LIMITS = [
    "Weights describe recorded samples. CPU time and exact call counts require separate measurements.",
    "Inclusive weights count a frame once per stack. Recursive occurrences share that attribution.",
    "Profiles remain separate. Process wall time requires separate timing.",
    "One capture supplies one observation, regardless of its stack count. Between-run variance requires independent captures.",
    "Collector filters, sampling errors and native coverage require producer metadata.",
]


def validate_raw(raw):
    require(isinstance(raw, dict) and raw.get("$schema") == SCHEMA, "unsupported Speedscope schema")
    require(isinstance(raw.get("shared"), dict), "Speedscope shared must be an object")
    frames = raw["shared"].get("frames")
    require(isinstance(frames, list), "Speedscope frames must be a list")
    for frame in frames:
        require(isinstance(frame, dict) and isinstance(frame.get("name"), str), "invalid frame name")
        require(frame.get("file") is None or isinstance(frame["file"], str), "invalid frame file")
        for field in ("line", "col"):
            value = frame.get(field)
            require(value is None or is_integer(value) and value >= 0, "invalid frame " + field)
    profiles = raw.get("profiles")
    require(isinstance(profiles, list) and bool(profiles), "Speedscope profiles must be a nonempty list")
    for profile in profiles:
        _validate_profile(profile, len(frames))
    return frames, profiles


def _validate_profile(profile, frame_count):
    require(isinstance(profile, dict) and profile.get("type") == "sampled",
            "only sampled Speedscope profiles are supported")
    require(isinstance(profile.get("name"), str), "invalid profile name")
    require(profile.get("unit") == "seconds", "py-spy Speedscope weights must use seconds")
    for field in ("startValue", "endValue"):
        require(is_finite_number(profile.get(field)), "invalid profile " + field)
    require(profile["endValue"] >= profile["startValue"], "profile end precedes start")
    stacks, weights = profile.get("samples"), profile.get("weights")
    require(isinstance(stacks, list) and isinstance(weights, list) and len(stacks) == len(weights),
            "sample and weight lists must have equal lengths")
    for stack, weight in zip(stacks, weights):
        require(isinstance(stack, list) and all(is_integer(i) and 0 <= i < frame_count for i in stack),
                "invalid frame index in sampled stack")
        require(is_finite_number(weight) and weight >= 0, "invalid sampled weight")
    try:
        total = math.fsum(weights)
    except OverflowError as error:
        raise ValueError("sample weight total overflow") from error
    require(is_finite_number(total), "sample weight total overflow")


def _add_weight(values, corrections, index, weight):
    """Compensate rounding while retaining constant storage per shared frame."""
    adjusted = weight - corrections[index]
    previous = values[index]
    updated = previous + adjusted
    corrections[index] = (updated - previous) - adjusted
    values[index] = updated


def _hotspots(profile, frames):
    inclusive = [0.0] * len(frames)
    exclusive = [0.0] * len(frames)
    inclusive_corrections = [0.0] * len(frames)
    exclusive_corrections = [0.0] * len(frames)
    empty = []
    for stack, weight in zip(profile["samples"], profile["weights"]):
        if not stack:
            empty.append(weight)
            continue
        _add_weight(exclusive, exclusive_corrections, stack[-1], weight)
        for index in set(stack):
            _add_weight(inclusive, inclusive_corrections, index, weight)
    total = math.fsum(profile["weights"])
    rows = [dict(frame_index=index, frame=frame, self_weight=exclusive[index],
                 inclusive_weight=inclusive[index],
                 self_fraction=exclusive[index] / total if total else None,
                 inclusive_fraction=inclusive[index] / total if total else None,
                 source=f"/shared/frames/{index}")
            for index, frame in enumerate(frames) if inclusive[index] or exclusive[index]]
    rows.sort(key=lambda row: (-row["self_weight"], -row["inclusive_weight"], row["frame_index"]))
    return total, math.fsum(empty), rows


def import_artifact(raw, manifest):
    frames, profiles = validate_raw(raw)
    exporter = raw.get("exporter")
    verified = exporter == "py-spy@0.4.1" and manifest["producer"]["version"] == "0.4.1"
    observations, diagnostics = [], []
    selected = set()
    for region in manifest["regions"]:
        index = region.get("profile_index")
        require(is_integer(index) and 0 <= index < len(profiles), "region needs a valid profile_index")
        require(index not in selected, "each sampled profile must map to only one region")
        selected.add(index)
        profile = profiles[index]
        require(region.get("profile_name") == profile["name"], "profile_name differs from selected raw profile")
        total, empty, rows = _hotspots(profile, frames)
        reason = "one sampled capture; stack samples are not independent process runs"
        quality = "insufficient_samples" if verified else "exploratory"
        if not verified:
            reason = "only py-spy 0.4.1 sampled-seconds export is verified; " + reason
        if not total:
            quality = "missing"
            reason = "the selected profile has no positive recorded sample weight"
        metadata = dict(profile_index=index, profile_name=profile["name"], exporter=exporter,
                        stack_count=len(profile["samples"]), empty_stack_weight=empty,
                        start_value=profile["startValue"], end_value=profile["endValue"],
                        functions=rows, limits=LIMITS, adapter_version="1")
        observations.append(dict(
            region_id=region["id"], metric="recorded_sample_weight", kind="derived",
            unit="recorded s/run", scope="one sampled thread profile", direction="none",
            samples=[total] if total else [], sample_level="independent_process_run", time_basis="sample_weight",
            quality=quality, missing_reason=reason,
            source_locators=[f"/profiles/{index}/samples", f"/profiles/{index}/weights"], metadata=metadata))
        diagnostics.append(dict(code="sampled_capture", region_id=region["id"],
                                pointer=f"/profiles/{index}", message=reason))
    return observations, diagnostics


def default_manifest(raw, digest):
    _, profiles = validate_raw(raw)
    exporter = raw.get("exporter", "unknown")
    version = exporter.removeprefix("py-spy@") if isinstance(exporter, str) else "unknown"
    return dict(
        schema_version=1, bundle_id="sampled-capture", created_at="unknown", revision="unknown",
        dirty_patch_sha256="unknown", producer=dict(provider="py-spy-speedscope", version=version,
            sampling_policy=dict(rate_hz="unknown", gil_only="unknown", include_idle="unknown",
                                 native="unknown", nonblocking="unknown", errors="unknown")),
        raw_source=dict(sha256=digest), host={}, build={},
        workloads={"capture": dict(version="1", parameters={}, input_sha256="unknown",
                                   scope="sampled process", threads="unknown", instrumentation="unknown",
                                   clock="sample_weight", time_mode="sampled", sample_level="independent_process_run")},
        semantic_contracts={"capture": dict(version="1", semantics="unknown", correctness_evidence="unknown")},
        regions=[dict(id=f"thread-{index}", variant="sampled", workload_id="capture", contract_id="capture",
                      profile_index=index, profile_name=profile["name"])
                 for index, profile in enumerate(profiles)])
