"""Native exact-count fixture and invalid/unavailable memory-pass boundaries."""

import contextlib
import copy
import hashlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from wjr_profiler.__main__ import main
from wjr_profiler.adapters.google_benchmark_memory import FIELDS, TOMBSTONE, import_artifact
from wjr_profiler.bundle import load_bundle
from wjr_profiler.compare import compare
from wjr_profiler.model import validate_observations

FIXTURE = Path(__file__).parent / "fixtures/memory_manager"


def fixture():
    return (json.loads((FIXTURE / "raw.json").read_bytes()),
            json.loads((FIXTURE / "manifest.json").read_text(encoding="utf-8")))


def imported(raw, manifest):
    observations, diagnostics = import_artifact(raw, manifest)
    validate_observations(observations, manifest)
    return dict(manifest=manifest, normalized=dict(observations=observations, diagnostics=diagnostics))


class MemoryManagerTests(unittest.TestCase):
    def test_real_fixture_hashes_and_source_identity(self):
        provenance = json.loads((FIXTURE / "provenance.json").read_text(encoding="utf-8"))
        self.assertTrue(provenance["real"])
        for name in ("raw", "stdout", "stderr"):
            file = "raw.json" if name == "raw" else name + ".txt"
            self.assertEqual(hashlib.sha256((FIXTURE / file).read_bytes()).hexdigest(), provenance[name + "_sha256"])
        self.assertEqual(hashlib.sha256((FIXTURE / "producer.cpp").read_bytes()).hexdigest(),
                         provenance["producer_source_sha256"])
        self.assertEqual(provenance["provider"]["commit"], "afa23b7699c17f1e26c88cbf95257b20d78d6247")

    def test_real_exact_counts_and_zero_control(self):
        raw, manifest = fixture()
        observations, diagnostics = import_artifact(raw, manifest)
        self.assertFalse(diagnostics)
        self.assertEqual(len(observations), 8)
        validate_observations(observations, manifest)
        expected = dict(allocs_per_iter=1, max_bytes_used=512,
                        total_allocated_bytes=8192, net_heap_growth=0)
        for row in observations:
            self.assertEqual(row["quality"], "ok")
            self.assertEqual(row["samples"], [expected[row["metric"]] if row["region_id"] == "tracked" else 0] * 5)
            self.assertEqual(row["metadata"]["pass_iterations"], 16)
            self.assertEqual(row["sample_level"], "memory_pass_repetition")
            self.assertTrue(all(pointer.endswith("/" + row["metric"]) for pointer in row["source_locators"]))

    def test_timing_denominator_and_units_are_not_reused(self):
        raw, manifest = fixture()
        for record in raw["benchmarks"]:
            if record["run_type"] == "iteration":
                record["iterations"] = 1000000000
                record["time_unit"] = "us"
        rows, _ = import_artifact(raw, manifest)
        total = next(row for row in rows if row["metric"] == "total_allocated_bytes" and row["region_id"] == "tracked")
        self.assertEqual(total["samples"], [8192] * 5)
        self.assertEqual(total["unit"], "bytes/pass")
        self.assertEqual(total["time_basis"], "memory")

    def test_unknown_and_corruptible_tool_versions_are_rejected(self):
        for version in ("1.9.0", "1.9.1", "1.9.3", "unknown"):
            raw, manifest = fixture()
            raw["context"]["library_version"] = version
            manifest["producer"]["version"] = version
            with self.assertRaisesRegex(ValueError, "requires verified.*1.9.2"):
                import_artifact(raw, manifest)

    def test_real_corrupted_190_byte_results_are_rejected(self):
        raw = json.loads((FIXTURE / "rejected-1.9.0.json").read_bytes())
        _, manifest = fixture()
        manifest["producer"]["version"] = "1.9.0"
        self.assertEqual(raw["context"]["library_version"], "v1.9.0")
        self.assertNotEqual(raw["benchmarks"][0]["max_bytes_used"], 512)
        with self.assertRaisesRegex(ValueError, "corrupt repeated memory results"):
            import_artifact(raw, manifest)

    def test_missing_optional_fields_are_not_zero(self):
        raw, manifest = fixture()
        for record in raw["benchmarks"]:
            record.pop("net_heap_growth", None)
        rows, _ = import_artifact(raw, manifest)
        for row in rows:
            self.assertEqual(row["quality"], "missing" if row["metric"] == "net_heap_growth" else "ok")
            if row["metric"] == "net_heap_growth":
                self.assertEqual(row["samples"], [])

    def test_invalid_values_and_tombstones_remain_missing(self):
        for metric, value in (("allocs_per_iter", True), ("allocs_per_iter", float("nan")),
                              ("max_bytes_used", -1), ("max_bytes_used", 512.0),
                              ("total_allocated_bytes", TOMBSTONE), ("net_heap_growth", TOMBSTONE),
                              ("net_heap_growth", -(1 << 64)), ("max_bytes_used", "512")):
            raw, manifest = fixture()
            raw["benchmarks"][0][metric] = value
            rows, diagnostics = import_artifact(raw, manifest)
            row = next(row for row in rows if row["region_id"] == "tracked" and row["metric"] == metric)
            self.assertEqual(row["quality"], "missing")
            self.assertEqual(len(row["samples"]), 4)
            self.assertTrue(diagnostics)
            json.dumps(rows, allow_nan=False)

    def test_negative_growth_is_signed_with_no_better_direction(self):
        raw, manifest = fixture()
        raw["benchmarks"][0]["net_heap_growth"] = -512
        rows, _ = import_artifact(raw, manifest)
        row = next(row for row in rows if row["region_id"] == "tracked" and row["metric"] == "net_heap_growth")
        self.assertEqual(row["samples"][0], -512)
        self.assertEqual(row["direction"], "none")

    def test_bad_or_incomplete_repetition_sets_cannot_be_promoted(self):
        for key, value in (("repetition_index", 1), ("repetition_index", True),
                           ("repetitions", 6), ("threads", True), ("iterations", 1)):
            raw, manifest = fixture()
            raw["benchmarks"][0][key] = value
            rows, _ = import_artifact(raw, manifest)
            self.assertTrue(all(row["quality"] == "missing" for row in rows if row["region_id"] == "tracked"))

    def test_errors_and_aggregates_never_add_valid_samples(self):
        raw, manifest = fixture()
        raw["benchmarks"][0].update(error_occurred=True, error_message="allocation failure")
        rows, _ = import_artifact(raw, manifest)
        self.assertTrue(all(row["quality"] == "failed" for row in rows if row["region_id"] == "tracked"))
        raw, manifest = fixture()
        raw["benchmarks"] = [row for row in raw["benchmarks"] if row["run_type"] == "aggregate"]
        rows, _ = import_artifact(raw, manifest)
        self.assertTrue(all(not row["samples"] and row["quality"] == "missing" for row in rows))

    def test_memory_contract_requires_explicit_scope_and_coverage(self):
        for key, value in (("threads", 2), ("threads", True), ("clock", "steady_clock"),
                           ("sample_level", "in_process_repetition"), ("instrumentation", "none")):
            raw, manifest = fixture()
            manifest["workloads"]["tracked"][key] = value
            with self.assertRaises(ValueError):
                import_artifact(raw, manifest)
        for key in ("collection_method", "manager_id", "coverage", "harness_policy", "pass_iterations"):
            raw, manifest = fixture()
            manifest["workloads"]["tracked"]["instrumentation"].pop(key)
            with self.assertRaises(ValueError):
                import_artifact(raw, manifest)

    def test_instrumentation_changes_block_comparison(self):
        for name in ("manager_id", "coverage", "harness_policy"):
            raw, manifest = fixture()
            left = imported(raw, manifest)
            right = copy.deepcopy(left)
            right["manifest"]["workloads"]["tracked"]["instrumentation"][name] = "different"
            rows = compare(left, right, exploratory=True)["comparisons"]
            self.assertTrue(all(row["classification"] == "inconclusive" and row["absolute_change"] is None
                                for row in rows if row["region_id"] == "tracked"))

    def test_uninterpreted_counters_stay_separate(self):
        raw, manifest = fixture()
        raw["benchmarks"][0]["custom_allocator_hint"] = 42
        rows, _ = import_artifact(raw, manifest)
        counters = rows[0]["metadata"]["records"][0]["user_counters_uninterpreted"]
        self.assertEqual(counters["custom_allocator_hint"], 42)
        self.assertFalse(set(counters) & FIELDS.keys())

    def test_cli_bundle_report_and_incompatible_missing_output(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            args = ["import", "--provider", "google-benchmark-memory", "--raw", str(FIXTURE / "raw.json"),
                    "--manifest", str(FIXTURE / "manifest.json"), "--out", str(root / "bundle")]
            self.assertEqual(main(args), 0)
            bundle = load_bundle(root / "bundle")
            self.assertEqual(len(bundle["normalized"]["observations"]), 8)
            self.assertEqual(main(["compare", str(root / "bundle"), str(root / "bundle"),
                                   "--out", str(root / "report")]), 0)
            text = (root / "report/report.md").read_text(encoding="utf-8")
            self.assertIn("16 iterations", text)
            self.assertIn("not RSS", text)
            self.assertIn("Declared hook coverage", text)
            self.assertTrue(all(row["classification"] == "contextual"
                                for row in compare(bundle, bundle)["comparisons"]))
            raw, manifest = fixture()
            manifest["producer"]["version"] = "1.9.0"
            (root / "bad.json").write_text(json.dumps(manifest), encoding="utf-8")
            args[args.index("--manifest") + 1] = str(root / "bad.json")
            args[-1] = str(root / "absent")
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(args), 2)
            self.assertFalse((root / "absent").exists())


if __name__ == "__main__":
    unittest.main()
