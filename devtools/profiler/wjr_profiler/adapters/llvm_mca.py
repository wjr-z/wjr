"""Import LLVM-MCA 18.1.8's JSON views as scheduling-model estimates."""

import math

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


def import_artifact(raw, manifest):
    """Return observations and diagnostics without executing a tool or reading files.

    Bind each declared region by ``raw_region_index`` rather than its display name:
    LLVM region names can be empty or repeated. A simulation is one model result,
    regardless of its iteration count. Missing optional views remain missing.
    """
    regions = raw.get("CodeRegions")
    if not isinstance(regions, list):
        raise ValueError("/CodeRegions: expected an array")
    target = raw.get("TargetInfo", {})
    parameters = raw.get("SimulationParameters", {})
    if not isinstance(target, dict) or not isinstance(parameters, dict):
        raise ValueError("TargetInfo and SimulationParameters must be objects")
    resources = target.get("Resources", [])
    if not isinstance(resources, list) or any(
        not isinstance(x, str) for x in resources
    ):
        raise ValueError("/TargetInfo/Resources: expected resource names")
    if len(resources) != len(set(resources)):
        raise ValueError("/TargetInfo/Resources: duplicate names")

    version = manifest["producer"].get("version")
    options = manifest["producer"].get("options")
    if options is not None and (
        not isinstance(options, list) or any(not isinstance(x, str) for x in options)
    ):
        raise ValueError("producer.options: expected explicit analyzer-option strings")
    model_known = bool(target.get("CPUName") and parameters.get("-mtriple"))
    qualified = version == SUPPORTED_VERSION and model_known and options is not None
    observations, diagnostics = [], []
    if version != SUPPORTED_VERSION:
        diagnostics.append(
            dict(
                code="unsupported_analyzer_version",
                message="Only LLVM-MCA 18.1.8 has a verified JSON fixture; output is exploratory.",
            )
        )
    if not model_known:
        diagnostics.append(
            dict(
                code="missing_model",
                message="CPU scheduling model or target triple is missing; output is exploratory.",
            )
        )
    if options is None:
        diagnostics.append(
            dict(
                code="missing_analyzer_options",
                message="Record all analyzer options; omitted options do not establish a default-model run.",
            )
        )

    for binding in manifest["regions"]:
        index = _index(
            binding.get("raw_region_index"), len(regions), "/regions/raw_region_index"
        )
        region = regions[index]
        if not isinstance(region, dict):
            raise ValueError(f"/CodeRegions/{index}: expected an object")
        region_id = binding["id"]
        base = f"/CodeRegions/{index}"
        instructions = region.get("Instructions")
        if not isinstance(instructions, list) or any(
            not isinstance(x, str) for x in instructions
        ):
            raise ValueError(f"{base}/Instructions: expected instruction strings")
        summary = region.get("SummaryView", {})
        if not isinstance(summary, dict):
            raise ValueError(f"{base}/SummaryView: expected an object")
        iterations = summary.get("Iterations")
        if iterations is not None:
            _number(iterations, base + "/SummaryView/Iterations", integer=True)
            if iterations == 0:
                raise ValueError(f"{base}/SummaryView/Iterations: must be positive")

        metadata = dict(
            analyzer="llvm-mca",
            analyzer_version=version,
            analyzer_options=options,
            cpu_model=target.get("CPUName"),
            simulation_parameters=parameters,
            iterations=iterations,
            region_name=region.get("Name", ""),
            static_instruction_count=len(instructions),
            dispatch_width=summary.get("DispatchWidth"),
        )
        details = _instruction_details(region, instructions, base)

        def emit(metric, unit, scope, direction, value, pointer, extra=None):
            missing = value is None
            if not missing:
                _number(value, pointer)
            observations.append(
                dict(
                    region_id=region_id,
                    metric=metric,
                    kind="estimate",
                    unit=unit,
                    scope=scope,
                    direction=direction,
                    samples=[] if missing else [float(value)],
                    sample_level="simulation",
                    quality=(
                        "missing" if missing else "ok" if qualified else "exploratory"
                    ),
                    missing_reason=(
                        "LLVM-MCA view or field was not emitted" if missing else None
                    ),
                    time_basis="model",
                    source_locators=[pointer],
                    metadata={**metadata, **(extra or {})},
                )
            )

        summary_start = len(observations)
        for field, metric, unit, scope, direction in (
            (
                "BlockRThroughput",
                "block_rthroughput",
                "cycles/iteration",
                "steady_state_region",
                "lower",
            ),
            ("TotalCycles", "total_cycles", "cycles", "simulation_run", "lower"),
            ("Instructions", "instructions", "count", "simulation_run", "none"),
            ("TotaluOps", "micro_ops", "count", "simulation_run", "none"),
            ("IPC", "ipc", "instructions/cycle", "simulation_run", "none"),
            (
                "uOpsPerCycle",
                "micro_ops_per_cycle",
                "uops/cycle",
                "simulation_run",
                "none",
            ),
        ):
            extra = (
                dict(instruction_details=details)
                if field == "BlockRThroughput"
                else None
            )
            emit(
                metric,
                unit,
                scope,
                direction,
                summary.get(field),
                base + "/SummaryView/" + field,
                extra,
            )

        pressure = region.get("ResourcePressureView")
        if pressure is None:
            diagnostics.append(
                dict(
                    code="missing_resource_pressure",
                    region_id=region_id,
                    pointer=base + "/ResourcePressureView",
                    message="Resource pressure view was not emitted.",
                )
            )
            continue
        cells = (
            pressure.get("ResourcePressureInfo") if isinstance(pressure, dict) else None
        )
        if not isinstance(cells, list):
            raise ValueError(
                f"{base}/ResourcePressureView/ResourcePressureInfo: expected an array"
            )
        seen = set()
        per_instruction = []
        for cell_index, cell in enumerate(cells):
            pointer = base + f"/ResourcePressureView/ResourcePressureInfo/{cell_index}"
            if not isinstance(cell, dict):
                raise ValueError(f"{pointer}: expected an object")
            inst = _index(
                cell.get("InstructionIndex"), len(instructions), pointer, allow_end=True
            )
            resource = _index(cell.get("ResourceIndex"), len(resources), pointer)
            usage = _number(cell.get("ResourceUsage"), pointer + "/ResourceUsage")
            if (inst, resource) in seen:
                raise ValueError(f"{pointer}: duplicate pressure cell")
            seen.add((inst, resource))
            # LLVM uses the index after the last instruction for region totals.
            if inst == len(instructions):
                emit(
                    "resource_pressure:" + resources[resource],
                    "resource_cycles/iteration",
                    "steady_state_region",
                    "none",
                    usage,
                    pointer + "/ResourceUsage",
                    dict(resource_index=resource, resource_name=resources[resource]),
                )
            else:
                per_instruction.append(
                    dict(
                        instruction_index=inst,
                        resource_index=resource,
                        resource_name=resources[resource],
                        usage=usage,
                        source_locator=pointer,
                    )
                )
        # Keep sparse per-instruction cells distinct from the block totals above.
        observations[summary_start]["metadata"][
            "instruction_resource_pressure"
        ] = per_instruction

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
