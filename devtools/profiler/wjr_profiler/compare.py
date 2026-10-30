"""Eligibility first, descriptive changes second; no regression gate or significance test."""

import math

from .model import ENVIRONMENT_FIELDS, WORKLOAD_FIELDS, field, require, summarize, unknown


def identity(observation):
    return tuple(observation[key] for key in ("region_id", "metric", "scope", "time_basis"))


def compare(baseline, candidate, context_axes=(), exploratory=False):
    require(set(context_axes) <= set(ENVIRONMENT_FIELDS), "unsupported contextual comparison axis")
    left, right = baseline["manifest"], candidate["manifest"]
    left_regions = {r["id"]: r for r in left["regions"]}
    right_regions = {r["id"]: r for r in right["regions"]}
    left_obs = {identity(o): o for o in baseline["normalized"]["observations"]}
    right_obs = {identity(o): o for o in candidate["normalized"]["observations"]}
    rows = []
    for key in sorted(left_obs.keys() | right_obs.keys()):
        before, after = left_obs.get(key), right_obs.get(key)
        reasons, hard, contextual = [], False, False
        if before is None or after is None:
            reasons.append("missing counterpart, possibly different scope or time_basis")
            hard = True
        lregion, rregion = left_regions.get(key[0]), right_regions.get(key[0])
        if left["producer"]["provider"] != right["producer"]["provider"]:
            reasons.append("different providers are not comparable")
            hard = True
        if lregion and rregion:
            lc = left["semantic_contracts"][lregion["contract_id"]]
            rc = right["semantic_contracts"][rregion["contract_id"]]
            semantic_keys = (lc.keys() | rc.keys()) - {"correctness_evidence"}
            if lregion["contract_id"] != rregion["contract_id"] or any(lc.get(k) != rc.get(k) for k in semantic_keys):
                reasons.append("semantic contract differs")
                hard = True
            for contract in (lc, rc):
                if any(unknown(contract.get(k)) for k in ("version", "semantics", "correctness_evidence")):
                    reasons.append("semantic contract or correctness evidence is unknown")
                    hard = True
            lw = left["workloads"][lregion["workload_id"]]
            rw = right["workloads"][rregion["workload_id"]]
            if lregion["workload_id"] != rregion["workload_id"] or lw != rw:
                reasons.append("workload/input/scope/concurrency/instrumentation differs")
                hard = True
            for name in WORKLOAD_FIELDS:
                if unknown(lw.get(name)) or unknown(rw.get(name)):
                    reasons.append("unknown workload." + name)
                    if name != "input_sha256":
                        hard = True
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
        for manifest in (left, right):
            if unknown(manifest.get("revision")) or unknown(manifest.get("dirty_patch_sha256")):
                reasons.append("source revision/dirty-patch provenance is unknown")
            if manifest.get("host", {}).get("stability") != "controlled":
                contextual = True
        if left["producer"]["version"] != right["producer"]["version"]:
            reasons.append("producer versions differ")
        failed = False
        for observation in (before, after):
            if observation is None:
                continue
            failed |= observation["quality"] == "failed"
            if observation["quality"] != "ok":
                reasons.append(observation["quality"] + ": " + (observation["missing_reason"] or "unverified provider/version"))
                if observation["quality"] == "missing":
                    hard = True
            if observation["kind"] != "measurement":
                reasons.append("provider-specific comparison policy is required for non-runtime evidence")
                hard = True
            if len(observation["samples"]) < 5:
                reasons.append("fewer than five raw samples")
        if before and after:
            for name in ("kind", "unit", "sample_level", "direction"):
                if before[name] != after[name]:
                    reasons.append(name + " differs")
                    hard = True
        classification = "failed" if failed else "inconclusive" if reasons else "contextual" if contextual else "comparable"
        lsummary = summarize(before["samples"] if before else [])
        rsummary = summarize(after["samples"] if after else [])
        permitted = classification in ("comparable", "contextual") or exploratory and not hard and not failed
        absolute = relative = None
        notes = []
        if permitted and lsummary["median"] is not None and rsummary["median"] is not None:
            absolute = rsummary["median"] - lsummary["median"]
            if lsummary["median"] == 0:
                notes.append("baseline median is zero; relative change is undefined")
            else:
                relative = absolute / lsummary["median"] * 100
                if not math.isfinite(relative):
                    relative = None
                    notes.append("relative change overflow; no finite ratio")
        def side(manifest, region, observation, summary):
            return dict(bundle_id=manifest["bundle_id"], variant=region["variant"] if region else None,
                        revision=manifest["revision"], raw_source=manifest["raw_source"],
                        source_sha256=region.get("source_sha256", "unknown") if region else None,
                        statistics=summary, observation=observation)
        rows.append(dict(region_id=key[0], metric=key[1], scope=key[2], time_basis=key[3],
                         unit=(before or after)["unit"], kind=(before or after)["kind"],
                         direction=(before or after)["direction"], classification=classification,
                         reasons=sorted(set(reasons)), notes=notes,
                         exploratory_delta=bool(permitted and classification == "inconclusive"),
                         absolute_change=absolute, relative_change_percent=relative,
                         baseline=side(left, lregion, before, lsummary),
                         candidate=side(right, rregion, after, rsummary)))
    return dict(schema_version=1, baseline_id=left["bundle_id"], candidate_id=right["bundle_id"],
                contextual_axes=sorted(context_axes), exploratory_requested=exploratory, comparisons=rows,
                diagnostics=baseline["normalized"]["diagnostics"] + candidate["normalized"]["diagnostics"],
                limitations=["Descriptive statistics only; no statistical significance or regression gate.",
                             "In-process repetitions are not independent process runs; internal iterations are not sample n.",
                             "No frequency calibration, inferred work-item counts, counter-unit guesses or cross-provider score."])
