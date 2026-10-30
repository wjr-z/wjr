"""Import LLVM-MCA 18.1.8's JSON views as scheduling-model estimates."""

import math
import re

from ..model import is_integer, unknown

SUPPORTED_VERSION = "18.1.8"


def _number(value, pointer, integer=False):
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ValueError(f"{pointer}: expected a number")
    if not math.isfinite(value) or value < 0 or (integer and int(value) != value):
        raise ValueError(f"{pointer}: expected a finite nonnegative number")
    return value


def _index(value, length, pointer, allow_end=False):
    if isinstance(value, bool) or not isinstance(value, int):
        raise ValueError(f"{pointer}: expected an integer index")
    if not 0 <= value < length + int(allow_end):
        raise ValueError(f"{pointer}: index out of range")
    return value


SUMMARY_FIELDS = (
    ("BlockRThroughput", "block_rthroughput", "cycles/iteration", "steady_state_region", "lower"),
    ("TotalCycles", "total_cycles", "cycles", "simulation_run", "lower"),
    ("Instructions", "instructions", "count", "simulation_run", "none"),
    ("TotaluOps", "micro_ops", "count", "simulation_run", "none"),
    ("IPC", "ipc", "instructions/cycle", "simulation_run", "none"),
    ("uOpsPerCycle", "micro_ops_per_cycle", "uops/cycle", "simulation_run", "none"),
)


def _object(value, pointer):
    if not isinstance(value, dict):
        raise ValueError(pointer + ": expected an object")
    return value


def _strings(value, pointer):
    if not isinstance(value, list) or any(not isinstance(x, str) for x in value):
        raise ValueError(pointer + ": expected string array")
    return value


def _context(raw, manifest):
    _object(raw, "/")
    regions = raw.get("CodeRegions")
    if not isinstance(regions, list):
        raise ValueError("/CodeRegions: expected an array")
    target = _object(raw.get("TargetInfo", {}), "/TargetInfo")
    parameters = _object(raw.get("SimulationParameters", {}), "/SimulationParameters")
    resources = _strings(target.get("Resources", []), "/TargetInfo/Resources")
    if len(resources) != len(set(resources)):
        raise ValueError("/TargetInfo/Resources: duplicate names")
    options = manifest["producer"].get("options")
    if options is not None:
        _strings(options, "producer.options")
    return regions, resources, dict(
        analyzer="llvm-mca", analyzer_version=manifest["producer"].get("version"),
        analyzer_options=options, cpu_model=target.get("CPUName"),
        target_triple=parameters.get("-mtriple"), simulation_parameters=parameters,
        resource_names=resources, exit_status=manifest["producer"].get("exit_status"))


def _model_problems(metadata):
    problems = _option_limitations(metadata["analyzer_options"])
    if metadata["analyzer_version"] != SUPPORTED_VERSION:
        problems.append(("unsupported_analyzer_version", "Only LLVM-MCA 18.1.8 is verified; output is exploratory."))
    if unknown(metadata["cpu_model"]) or unknown(metadata["target_triple"]):
        problems.append(("missing_model", "CPU scheduling model or target triple is missing."))
    if metadata["analyzer_options"] is None:
        problems.append(("missing_analyzer_options", "Record all analyzer options; absent options do not prove defaults."))
    if not is_integer(metadata["exit_status"]) or metadata["exit_status"] != 0:
        problems.append(("analyzer_status", "Successful producer exit status is unknown or the output is partial/failed."))
    return problems


def _option_limitations(options):
    for option in options or []:
        key, _, value = option.partition("=")
        if key.lstrip("-") == "skip-unsupported-instructions" and value not in ("false", "0"):
            return [("ignored_instructions", "Ignoring unsupported instructions cannot establish a complete region model.")]
    return []


def _region_context(region, index, metadata):
    base = f"/CodeRegions/{index}"
    region = _object(region, base)
    instructions = _strings(region.get("Instructions"), base + "/Instructions")
    summary = _object(region.get("SummaryView", {}), base + "/SummaryView")
    iterations = summary.get("Iterations")
    if iterations is not None:
        _number(iterations, base + "/SummaryView/Iterations", integer=True)
        if iterations == 0:
            raise ValueError(base + "/SummaryView/Iterations: must be positive")
    metadata = dict(metadata, iterations=iterations, region_name=region.get("Name", ""),
                    static_instruction_count=len(instructions), dispatch_width=summary.get("DispatchWidth"))
    return base, instructions, summary, metadata


def _region_problems(instructions, metadata):
    problems = _model_problems(metadata)
    if metadata["iterations"] is None:
        problems.append(("missing_iterations", "Simulation iteration count is unknown."))
    if any(re.match(r"^\s*call[qwl]?\b", text, re.IGNORECASE) for text in instructions):
        problems.append(("call_in_region", "Call instructions do not model the callee or end-to-end execution."))
    return problems


def _observation(region_id, metadata, qualified, spec, value, pointer, extra=None, kind="estimate"):
    metric, unit, scope, direction = spec
    if value is not None:
        _number(value, pointer)
    quality, reason = _quality(value, metadata, qualified)
    return dict(region_id=region_id, metric=metric, kind=kind, unit=unit, scope=scope,
                direction=direction, samples=[] if value is None else [value], sample_level="simulation",
                quality=quality, missing_reason=reason,
                time_basis="model", source_locators=[pointer], metadata={**metadata, **(extra or {})})


def _quality(value, metadata, qualified):
    status = metadata["exit_status"]
    if is_integer(status) and status != 0:
        return "failed", "LLVM-MCA producer failed; output may be partial"
    if value is None:
        return "missing", "LLVM-MCA view or field was not emitted"
    return ("ok" if qualified else "exploratory"), None


def _summaries(region_id, region, base, instructions, summary, metadata, qualified):
    observations = []
    details = _instruction_details(region, instructions, base)
    for field, metric, unit, scope, direction in SUMMARY_FIELDS:
        extra = dict(instruction_details=details) if field == "BlockRThroughput" else None
        pointer = base + "/SummaryView/" + field
        if field not in summary:
            pointer = base + "/SummaryView" if "SummaryView" in region else base
        observations.append(_observation(region_id, metadata, qualified, (metric, unit, scope, direction),
                                          summary.get(field), pointer, extra))
    cycles, iterations = summary.get("TotalCycles"), metadata["iterations"]
    if cycles is not None and iterations is not None:
        derived = _observation(region_id, metadata, qualified,
            ("cycles_per_iteration", "cycles/iteration", "simulation_average", "lower"),
            cycles / iterations, base + "/SummaryView/TotalCycles", kind="derived")
        derived["metadata"]["derived_from"] = [base + "/SummaryView/TotalCycles", base + "/SummaryView/Iterations"]
        derived["metadata"]["inherits_kind"] = "estimate"
        observations.append(derived)
    return observations


def _pressure_cell(cell, pointer, count, resources):
    _object(cell, pointer)
    inst = _index(cell.get("InstructionIndex"), count, pointer, allow_end=True)
    resource = _index(cell.get("ResourceIndex"), len(resources), pointer)
    usage = _number(cell.get("ResourceUsage"), pointer + "/ResourceUsage")
    return inst, resource, usage


def _pressure(region_id, region, base, instructions, resources, metadata, qualified):
    pressure = region.get("ResourcePressureView")
    if pressure is None:
        diagnostic = dict(code="missing_resource_pressure", region_id=region_id,
                          pointer=base + "/ResourcePressureView", message="Resource pressure view was not emitted.")
        return [], None, [diagnostic]
    cells = _object(pressure, base + "/ResourcePressureView").get("ResourcePressureInfo")
    if not isinstance(cells, list):
        raise ValueError(base + "/ResourcePressureView/ResourcePressureInfo: expected an array")
    observations, per_instruction, seen = [], [], set()
    for index, cell in enumerate(cells):
        pointer = base + f"/ResourcePressureView/ResourcePressureInfo/{index}"
        inst, resource, usage = _pressure_cell(cell, pointer, len(instructions), resources)
        if (inst, resource) in seen:
            raise ValueError(pointer + ": duplicate pressure cell")
        seen.add((inst, resource))
        if inst == len(instructions):
            spec = ("resource_pressure:" + resources[resource], "resource_cycles/iteration", "steady_state_region", "none")
            observations.append(_observation(region_id, metadata, qualified, spec, usage,
                pointer + "/ResourceUsage", dict(resource_index=resource, resource_name=resources[resource])))
        else:
            per_instruction.append(dict(instruction_index=inst, resource_index=resource,
                resource_name=resources[resource], usage=usage, source_locator=pointer))
    return observations, per_instruction, []


def import_artifact(raw, manifest):
    """Bind regions by index; a simulation result is never a statistical repetition."""
    regions, resources, context = _context(raw, manifest)
    observations, diagnostics = [], []
    for binding in manifest["regions"]:
        index = _index(binding.get("raw_region_index"), len(regions), "/regions/raw_region_index")
        region = regions[index]
        base, instructions, summary, metadata = _region_context(region, index, context)
        problems = _region_problems(instructions, metadata)
        diagnostics.extend(dict(code=code, message=message, region_id=binding["id"]) for code, message in problems)
        summaries = _summaries(binding["id"], region, base, instructions, summary, metadata, not problems)
        pressure, instruction_pressure, extra = _pressure(binding["id"], region, base, instructions,
                                                        resources, metadata, not problems)
        summaries[0]["metadata"]["instruction_resource_pressure"] = instruction_pressure
        observations.extend(summaries + pressure)
        diagnostics.extend(extra)
    return observations, diagnostics


def _instruction_details(region, instructions, base):
    view = region.get("InstructionInfoView")
    if view is None:
        return None
    rows = view.get("InstructionList") if isinstance(view, dict) else None
    if not isinstance(rows, list):
        raise ValueError(
            f"{base}/InstructionInfoView/InstructionList: expected an array"
        )
    result, seen = [], set()
    for position, row in enumerate(rows):
        pointer = base + f"/InstructionInfoView/InstructionList/{position}"
        if not isinstance(row, dict):
            raise ValueError(f"{pointer}: expected an object")
        index = _index(row.get("Instruction"), len(instructions), pointer)
        if index in seen:
            raise ValueError(f"{pointer}: duplicate instruction index")
        seen.add(index)
        values = {}
        for key in ("Latency", "NumMicroOpcodes", "RThroughput"):
            if key in row:
                values[key] = _number(row[key], pointer + "/" + key)
        result.append(
            dict(
                index=index,
                assembly=instructions[index],
                source_locator=pointer,
                **values,
                **{
                    key: row[key]
                    for key in ("mayLoad", "mayStore", "hasUnmodeledSideEffects")
                    if key in row
                },
            )
        )
    return result
