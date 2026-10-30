"""Explicit LLVM-MCA eligibility: model identity, never the analysis host's CPU."""

from .model import is_integer, unknown

MODEL_FIELDS = ("analyzer", "analyzer_version", "analyzer_options", "cpu_model", "target_triple",
                "simulation_parameters", "iterations", "dispatch_width", "resource_names", "exit_status")


def _observation_reasons(observation):
    if observation is None:
        return ["missing MCA observation counterpart or different region scope"]
    reasons = []
    if observation["quality"] != "ok":
        reasons.append("MCA output is " + observation["quality"] + "; incomplete/unverified model evidence cannot produce a delta")
    kind = observation["kind"]
    if kind == "derived":
        kind = observation["metadata"].get("inherits_kind")
    if kind != "estimate" or observation["time_basis"] != "model":
        reasons.append("MCA evidence must retain estimate/model semantics")
    if observation["sample_level"] != "simulation" or len(observation["samples"]) != 1:
        reasons.append("one MCA simulation result is required; iterations are not repetitions")
    return reasons


def _model_reasons(before, after):
    reasons = []
    for name in MODEL_FIELDS:
        left, right = before["metadata"].get(name), after["metadata"].get(name)
        if unknown(left) or unknown(right):
            reasons.append("unknown MCA " + name)
        elif left != right:
            reasons.append("MCA model/options differ at " + name)
    return reasons


def _producer_reasons(manifest, observation):
    reasons = []
    if manifest["producer"]["version"] != "18.1.8":
        reasons.append("only LLVM-MCA 18.1.8 comparison is verified")
    if observation is not None:
        status = observation["metadata"].get("exit_status")
        if not is_integer(status) or status != 0:
            reasons.append("MCA producer did not establish successful complete output")
    return reasons


def eligibility(left, right, before, after):
    reasons = _observation_reasons(before) + _observation_reasons(after)
    reasons.extend(_producer_reasons(left, before) + _producer_reasons(right, after))
    if before is not None and after is not None:
        reasons.extend(_model_reasons(before, after))
        for name in ("kind", "unit", "sample_level", "direction"):
            if before[name] != after[name]:
                reasons.append("MCA " + name + " differs")
    # Exploratory display never bypasses unknown or conflicting model inputs.
    failed = any(observation is not None and observation["quality"] == "failed"
                 for observation in (before, after))
    return reasons, bool(reasons), failed
