"""Eligibility first, descriptive changes second; no regression gate or significance test."""

import math

from .model import ENVIRONMENT_FIELDS, WORKLOAD_FIELDS, field, require, summarize, unknown
from .mca_compare import eligibility as mca_eligibility


def identity(observation):
    return tuple(observation[key] for key in ("region_id", "metric", "scope", "time_basis"))


def _contract_reasons(left, right, lregion, rregion):
    lc = left["semantic_contracts"][lregion["contract_id"]]
    rc = right["semantic_contracts"][rregion["contract_id"]]
    keys = (lc.keys() | rc.keys()) - {"correctness_evidence"}
    reasons = []
    if lregion["contract_id"] != rregion["contract_id"] or any(lc.get(k) != rc.get(k) for k in keys):
        reasons.append("semantic contract differs")
    for contract in (lc, rc):
        if any(unknown(contract.get(k)) for k in ("version", "semantics", "correctness_evidence")):
            reasons.append("semantic contract or correctness evidence is unknown")
    return reasons, bool(reasons)


def _workload_reasons(left, right, lregion, rregion):
    lw = left["workloads"][lregion["workload_id"]]
    rw = right["workloads"][rregion["workload_id"]]
    reasons, hard = [], False
    if lregion["workload_id"] != rregion["workload_id"] or lw != rw:
        reasons.append("workload/input/scope/concurrency/instrumentation differs")
        hard = True
    for name in WORKLOAD_FIELDS:
        if unknown(lw.get(name)) or unknown(rw.get(name)):
            reasons.append("unknown workload." + name)
            if name != "input_sha256":
                hard = True
    return reasons, hard


def _environment_reasons(left, right, context_axes):
    reasons, hard, contextual = [], False, False
    for path in ENVIRONMENT_FIELDS:
        lvalue, rvalue = field(left, path), field(right, path)
        if unknown(lvalue) or unknown(rvalue):
            reasons.append("unknown " + path)
        elif lvalue != rvalue:
            if path in context_axes:
                contextual = True
            else:
                reasons.append("environment differs at " + path + "; declare a contextual axis to display its delta")
                hard = True
    return reasons, hard, contextual


def _provenance_reasons(left, right):
    reasons, contextual = [], False
    for manifest in (left, right):
        if unknown(manifest.get("revision")) or unknown(manifest.get("dirty_patch_sha256")):
            reasons.append("source revision/dirty-patch provenance is unknown")
        if manifest.get("host", {}).get("stability") != "controlled":
            contextual = True
    if left["producer"]["version"] != right["producer"]["version"]:
        reasons.append("producer versions differ")
    return reasons, contextual


def _single_observation_reasons(observation):
    reasons, hard = [], False
    if observation["quality"] != "ok":
        detail = observation["missing_reason"] or "unverified provider/version"
        reasons.append(observation["quality"] + ": " + detail)
        hard = observation["quality"] == "missing"
    if observation["kind"] != "measurement":
        reasons.append("provider-specific comparison policy is required for non-runtime evidence")
        hard = True
    if len(observation["samples"]) < 5:
        reasons.append("fewer than five raw samples")
    return reasons, hard


def _observation_pair_reasons(before, after):
    reasons, hard, failed = [], False, False
    for observation in (before, after):
        if observation is not None:
            extra, blocked = _single_observation_reasons(observation)
            reasons.extend(extra)
            hard |= blocked
            failed |= observation["quality"] == "failed"
    if before and after:
        for name in ("kind", "unit", "sample_level", "direction"):
            if before[name] != after[name]:
                reasons.append(name + " differs")
                hard = True
    return reasons, hard, failed


def _region_reasons(left, right, lregion, rregion):
    reasons, hard = [], False
    if lregion and rregion:
        for check in (_contract_reasons, _workload_reasons):
            extra, blocked = check(left, right, lregion, rregion)
            reasons.extend(extra)
            hard |= blocked
    return reasons, hard


def _eligibility(left, right, lregion, rregion, before, after, context_axes):
    reasons, hard = _region_reasons(left, right, lregion, rregion)
    if left["producer"]["provider"] == right["producer"]["provider"] == "llvm-mca":
        extra, blocked, failed = mca_eligibility(left, right, before, after)
        reasons.extend(extra)
        return reasons, hard or blocked, False, failed
    environment, env_blocked, contextual = _environment_reasons(left, right, context_axes)
    provenance, shared = _provenance_reasons(left, right)
    observations, obs_blocked, failed = _observation_pair_reasons(before, after)
    reasons.extend(environment + provenance + observations)
    hard |= env_blocked | obs_blocked
    contextual |= shared
    if before is None or after is None:
        reasons.append("missing counterpart, possibly different scope or time_basis")
        hard = True
    if left["producer"]["provider"] != right["producer"]["provider"]:
        reasons.append("different providers are not comparable")
        hard = True
    return reasons, hard, contextual, failed


def _classification(reasons, contextual, failed):
    if failed:
        return "failed"
    if reasons:
        return "inconclusive"
    return "contextual" if contextual else "comparable"


def _delta(left, right, permitted):
    if not permitted or left["median"] is None or right["median"] is None:
        return None, None, []
    absolute = right["median"] - left["median"]
    if left["median"] == 0:
        return absolute, None, ["baseline median is zero; relative change is undefined"]
    relative = absolute / left["median"] * 100
    if not math.isfinite(relative):
        return absolute, None, ["relative change overflow; no finite ratio"]
    return absolute, relative, []


def _side(manifest, region, observation, summary):
    return dict(bundle_id=manifest["bundle_id"], variant=region["variant"] if region else None,
                revision=manifest["revision"], raw_source=manifest["raw_source"],
                source_sha256=region.get("source_sha256", "unknown") if region else None,
                statistics=summary, observation=observation)


def _find_region(manifest, region_id):
    return next((region for region in manifest["regions"] if region["id"] == region_id), None)


def _statistics(observation):
    return summarize(observation["samples"] if observation else [])


def _compare_pair(key, before, after, left, right, context_axes, exploratory):
    lregion, rregion = _find_region(left, key[0]), _find_region(right, key[0])
    reasons, hard, contextual, failed = _eligibility(
        left, right, lregion, rregion, before, after, context_axes)
    classification = _classification(reasons, contextual, failed)
    lsummary, rsummary = _statistics(before), _statistics(after)
    permitted = classification in ("comparable", "contextual")
    permitted |= exploratory and not hard and not failed
    absolute, relative, notes = _delta(lsummary, rsummary, permitted)
    source = before if before is not None else after
    return dict(region_id=key[0], metric=key[1], scope=key[2], time_basis=key[3],
                unit=source["unit"], kind=source["kind"],
                direction=source["direction"], classification=classification,
                reasons=sorted(set(reasons)), notes=notes,
                exploratory_delta=bool(permitted and classification == "inconclusive"),
                absolute_change=absolute, relative_change_percent=relative,
                baseline=_side(left, lregion, before, lsummary),
                candidate=_side(right, rregion, after, rsummary))


def compare(baseline, candidate, context_axes=(), exploratory=False):
    require(set(context_axes) <= set(ENVIRONMENT_FIELDS), "unsupported contextual comparison axis")
    left = {identity(o): o for o in baseline["normalized"]["observations"]}
    right = {identity(o): o for o in candidate["normalized"]["observations"]}
    rows = [_compare_pair(key, left.get(key), right.get(key), baseline["manifest"],
                          candidate["manifest"], context_axes, exploratory)
            for key in sorted(left.keys() | right.keys())]
    return dict(schema_version=1, baseline_id=baseline["manifest"]["bundle_id"],
                candidate_id=candidate["manifest"]["bundle_id"],
                contextual_axes=sorted(context_axes), exploratory_requested=exploratory, comparisons=rows,
                diagnostics=baseline["normalized"]["diagnostics"] + candidate["normalized"]["diagnostics"],
                limitations=["Descriptive statistics only; no statistical significance or regression gate.",
                             "In-process repetitions are not independent process runs; internal iterations are not sample n.",
                             "No frequency calibration, inferred work-item counts, counter-unit guesses or cross-provider score."])
