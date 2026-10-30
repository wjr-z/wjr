"""Real LLVM-MCA output and malformed/sparse view boundaries."""

import copy
import json
import unittest
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from wjr_profiler.adapters.llvm_mca import import_artifact

FIXTURE = Path(__file__).parent / "fixtures" / "llvm_mca"


class LLVMImportTests(unittest.TestCase):
    def setUp(self):
        self.raw = json.loads((FIXTURE / "raw.json").read_text(encoding="utf-8"))
        self.manifest = {
            "producer": {
                "provider": "llvm-mca",
                "version": "18.1.8",
                "exit_status": 0,
                "options": [
                    "-iterations=100",
                    "-mcpu=skylake",
                    "-mtriple=x86_64-unknown-linux-gnu",
                ],
            },
            "regions": [
                {"id": "dependent", "raw_region_index": 0},
                {"id": "independent", "raw_region_index": 1},
            ],
        }

    def test_real_two_region_estimates(self):
        observations, diagnostics = import_artifact(self.raw, self.manifest)
        self.assertEqual(diagnostics, [])
        totals = [x for x in observations if x["metric"] == "total_cycles"]
        self.assertEqual([x["samples"] for x in totals], [[104.0], [103.0]])
        for observation in observations:
            if observation["kind"] == "derived":
                self.assertEqual(observation["metadata"]["inherits_kind"], "estimate")
            else:
                self.assertEqual(observation["kind"], "estimate")
            self.assertEqual(observation["time_basis"], "model")
            self.assertEqual(observation["sample_level"], "simulation")
            self.assertEqual(len(observation["samples"]), 1)
            self.assertEqual(observation["metadata"]["iterations"], 100)
            self.assertEqual(observation["metadata"]["cpu_model"], "skylake")

    def test_pressure_totals_are_not_instruction_rows(self):
        observations, _ = import_artifact(self.raw, self.manifest)
        blocks = [x for x in observations if x["metric"] == "block_rthroughput"]
        self.assertEqual(len(blocks), 2)
        for block in blocks:
            rows = block["metadata"]["instruction_resource_pressure"]
            self.assertEqual(len(rows), 4)
            self.assertTrue(all(row["instruction_index"] < 2 for row in rows))
            self.assertEqual(len(block["metadata"]["instruction_details"]), 2)
        pressure = [
            x for x in observations if x["metric"].startswith("resource_pressure:")
        ]
        self.assertEqual(len(pressure), 8)
        self.assertTrue(all(x["scope"] == "steady_state_region" for x in pressure))
        # Sparse output does not imply fabricated entries for all ten CPU resources.
        self.assertTrue(all(x["samples"] == [0.5] for x in pressure))

    def test_name_is_not_region_identity(self):
        for region in self.raw["CodeRegions"]:
            region["Name"] = ""
        observations, _ = import_artifact(self.raw, self.manifest)
        self.assertEqual(
            {x["region_id"] for x in observations}, {"dependent", "independent"}
        )

    def test_unknown_version_and_missing_model_are_exploratory(self):
        for change in ("version", "model"):
            with self.subTest(change=change):
                raw, manifest = copy.deepcopy(self.raw), copy.deepcopy(self.manifest)
                if change == "version":
                    manifest["producer"]["version"] = "unknown"
                else:
                    del raw["TargetInfo"]["CPUName"]
                observations, diagnostics = import_artifact(raw, manifest)
                self.assertTrue(diagnostics)
                self.assertTrue(
                    all(x["quality"] == "exploratory" for x in observations)
                )

    def test_missing_views_and_fields_do_not_become_zero(self):
        region = self.raw["CodeRegions"][0]
        del region["SummaryView"]["TotalCycles"]
        del region["ResourcePressureView"]
        del region["InstructionInfoView"]
        observations, diagnostics = import_artifact(self.raw, self.manifest)
        missing = next(
            x
            for x in observations
            if x["region_id"] == "dependent" and x["metric"] == "total_cycles"
        )
        self.assertEqual(missing["quality"], "missing")
        self.assertEqual(missing["samples"], [])
        self.assertTrue(missing["missing_reason"])
        self.assertEqual(diagnostics[0]["code"], "missing_resource_pressure")
        self.assertTrue(any(x["region_id"] == "independent" for x in observations))

    def test_bad_indices_duplicate_cells_and_nonfinite_metrics_fail(self):
        cases = []
        for field, value in (
            ("ResourceIndex", 10),
            ("InstructionIndex", 3),
            ("InstructionIndex", True),
            ("ResourceUsage", float("nan")),
        ):
            raw = copy.deepcopy(self.raw)
            raw["CodeRegions"][0]["ResourcePressureView"]["ResourcePressureInfo"][0][
                field
            ] = value
            cases.append(raw)
        raw = copy.deepcopy(self.raw)
        cells = raw["CodeRegions"][0]["ResourcePressureView"]["ResourcePressureInfo"]
        cells.append(copy.deepcopy(cells[0]))
        cases.append(raw)
        raw = copy.deepcopy(self.raw)
        raw["CodeRegions"][0]["InstructionInfoView"]["InstructionList"][0][
            "Instruction"
        ] = 2
        cases.append(raw)
        for index, raw in enumerate(cases):
            with self.subTest(index=index), self.assertRaises(ValueError):
                import_artifact(raw, self.manifest)

    def test_bad_region_binding_fails(self):
        for index in (-1, 2, True, "0", None):
            self.manifest["regions"][0]["raw_region_index"] = index
            with self.subTest(index=index), self.assertRaises(ValueError):
                import_artifact(self.raw, self.manifest)


if __name__ == "__main__":
    unittest.main()
