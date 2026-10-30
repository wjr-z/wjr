"""Offline contract tests. Synthetic cases are schema tests, never performance evidence."""

import contextlib
import copy
import hashlib
import io
import json
import math
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from wjr_profiler.__main__ import main
from wjr_profiler.adapters.google_benchmark import import_artifact
from wjr_profiler.bundle import import_bundle, load_bundle, write_json
from wjr_profiler.compare import compare
from wjr_profiler.model import summarize, validate_manifest
from wjr_profiler.report import markdown, write_report

FIXTURES = ROOT / "tests/fixtures"


def fixture_manifest():
    return json.loads((FIXTURES / "baseline.manifest.json").read_text())


def synthetic(values=(10, 20, 30, 40, 50), unit="ns"):
    raw = dict(context=dict(library_version="v1.9.0", json_schema_version=1), benchmarks=[
        dict(name="synthetic/reference", run_name="synthetic/reference", run_type="iteration",
             repetition_index=i, repetitions=len(values), threads=1, iterations=100000,
             real_time=value, cpu_time=value / 2, time_unit=unit, uninterpreted_counter=123)
        for i, value in enumerate(values)
    ])
    manifest = fixture_manifest()
    manifest["synthetic"] = True
    manifest["regions"][0]["benchmark"] = "synthetic/reference"
    manifest["host"]["stability"] = "controlled"
    manifest["workloads"]["lookup64"]["input_sha256"] = "2" * 64
    raw_bytes = json.dumps(raw, allow_nan=True).encode()
    manifest["raw_source"]["sha256"] = hashlib.sha256(raw_bytes).hexdigest()
    observations, diagnostics = import_artifact(raw, manifest)
    return dict(manifest=manifest, normalized=dict(observations=observations, diagnostics=diagnostics)), raw, raw_bytes


class ImportTests(unittest.TestCase):
    def test_real_fixture_and_provenance(self):
        raw_bytes = (FIXTURES / "btree-v1.9.0.json").read_bytes()
        provenance = json.loads((FIXTURES / "btree-v1.9.0.provenance.json").read_text())
        self.assertTrue(provenance["real"])
        self.assertEqual(hashlib.sha256(raw_bytes).hexdigest(), provenance["raw_sha256"])
        for name in ("baseline", "candidate"):
            manifest = json.loads((FIXTURES / (name + ".manifest.json")).read_text())
            observations, diagnostics = import_artifact(json.loads(raw_bytes), manifest)
            self.assertFalse(diagnostics)
            self.assertEqual(len(observations), 2)
            self.assertEqual({o["time_basis"] for o in observations}, {"wall", "cpu"})
            for observation in observations:
                self.assertEqual(len(observation["samples"]), 5)
                self.assertEqual(len(observation["metadata"]["aggregates"]), 4)
                self.assertEqual(observation["quality"], "ok")
                self.assertTrue(all(x.startswith("/benchmarks/") for x in observation["source_locators"]))

    def test_units_do_not_divide_by_internal_iterations(self):
        for unit, scale in (("ns", 1), ("us", 1000), ("ms", 1000000), ("s", 1000000000)):
            with self.subTest(unit=unit):
                bundle, _, _ = synthetic(unit=unit)
                sample = bundle["normalized"]["observations"][0]
                self.assertEqual(summarize(sample["samples"])["median"], 30 * scale)
                self.assertEqual(len(sample["samples"]), 5)
                self.assertEqual(sample["unit"], "ns/iteration")

    def test_aggregate_only_is_not_repetitions(self):
        bundle, raw, _ = synthetic()
        raw["benchmarks"] = [dict(raw["benchmarks"][0], run_type="aggregate", aggregate_name="median")]
        observations, _ = import_artifact(raw, bundle["manifest"])
        self.assertEqual(observations[0]["samples"], [])
        self.assertEqual(observations[0]["quality"], "insufficient_samples")
        self.assertIn("only aggregates", observations[0]["missing_reason"])
        self.assertIsNone(summarize([])["variance"])

    def test_error_does_not_become_zero(self):
        bundle, raw, _ = synthetic()
        raw["benchmarks"][0] = dict(raw["benchmarks"][0], error_occurred=True, error_message="input failure")
        observations, _ = import_artifact(raw, bundle["manifest"])
        self.assertEqual(observations[0]["quality"], "failed")
        self.assertEqual(len(observations[0]["samples"]), 4)
        self.assertIn("input failure", observations[0]["missing_reason"])

    def test_nonfinite_duration_counter_and_strict_output(self):
        bundle, raw, _ = synthetic()
        raw["benchmarks"][0]["real_time"] = math.nan
        raw["benchmarks"][1]["cpu_time"] = math.inf
        raw["benchmarks"][2]["custom"] = -math.inf
        observations, diagnostics = import_artifact(raw, bundle["manifest"])
        self.assertEqual(len(observations[0]["samples"]), 4)
        self.assertEqual(observations[0]["quality"], "missing")
        self.assertTrue(any(d["code"] == "nonfinite_counter" for d in diagnostics))
        self.assertIsNone(observations[0]["metadata"]["records"][2]["user_counters_uninterpreted"]["custom"])
        json.dumps(observations, allow_nan=False)

    def test_missing_invalid_fields_have_reasons(self):
        for key, value in (("real_time", None), ("time_unit", "ticks"), ("time_unit", []),
                           ("threads", 2), ("iterations", 0), ("real_time", -1)):
            with self.subTest(key=key, value=value):
                bundle, raw, _ = synthetic()
                raw["benchmarks"][0][key] = value
                observations, diagnostics = import_artifact(raw, bundle["manifest"])
                self.assertEqual(observations[0]["quality"], "missing")
                self.assertTrue(diagnostics)

    def test_duplicate_repetition_is_not_extra_evidence(self):
        bundle, raw, _ = synthetic()
        raw["benchmarks"][1]["repetition_index"] = 0
        observations, _ = import_artifact(raw, bundle["manifest"])
        self.assertEqual(len(observations[0]["samples"]), 4)
        self.assertIn("duplicate", observations[0]["missing_reason"])

    def test_unknown_version_is_exploratory_and_major_schema_rejected(self):
        bundle, raw, _ = synthetic()
        raw["context"]["library_version"] = "v2.0.0"
        observations, diagnostics = import_artifact(raw, bundle["manifest"])
        self.assertEqual(observations[0]["quality"], "exploratory")
        self.assertTrue(diagnostics)
        raw["context"]["json_schema_version"] = 2
        with self.assertRaisesRegex(ValueError, "schema"):
            import_artifact(raw, bundle["manifest"])

    def test_empty_or_unmapped_input(self):
        bundle, raw, _ = synthetic()
        raw["benchmarks"] = []
        observations, _ = import_artifact(raw, bundle["manifest"])
        self.assertEqual(observations[0]["samples"], [])
        self.assertIn("not present", observations[0]["missing_reason"])


class CompareTests(unittest.TestCase):
    def test_eligible_statistics_and_separate_clocks(self):
        left, _, _ = synthetic()
        right, _, _ = synthetic((20, 40, 60, 80, 100))
        report = compare(left, right)
        self.assertEqual(len(report["comparisons"]), 2)
        for row in report["comparisons"]:
            self.assertEqual(row["classification"], "comparable")
            self.assertEqual(row["relative_change_percent"], 100)
            self.assertEqual(row["baseline"]["statistics"]["n"], 5)
        stats = summarize([10, 20, 30, 40, 50])
        self.assertEqual((stats["median"], stats["minimum"], stats["maximum"], stats["mad"]), (30, 10, 50, 10))
        self.assertEqual(stats["variance"], 250)
        self.assertIsNone(summarize([1])["sample_stddev"])

    def test_contract_workload_units_and_instrumentation_never_bypassed(self):
        for mutation in ("contract", "n", "alignment", "seed", "scope", "threads", "instrumentation", "unit", "kind"):
            with self.subTest(mutation=mutation):
                left, _, _ = synthetic()
                right = copy.deepcopy(left)
                workload = right["manifest"]["workloads"]["lookup64"]
                if mutation == "contract":
                    right["manifest"]["semantic_contracts"]["lower-bound-v1"]["semantics"] = "different result"
                elif mutation in ("n", "alignment", "seed"):
                    workload["parameters"][mutation] = 1234
                elif mutation in ("unit", "kind"):
                    for row in right["normalized"]["observations"]:
                        row[mutation] = "cycles/block" if mutation == "unit" else "estimate"
                else:
                    workload[mutation] = "different"
                row = compare(left, right, exploratory=True)["comparisons"][0]
                self.assertEqual(row["classification"], "inconclusive")
                self.assertIsNone(row["relative_change_percent"])

    def test_host_flags_mismatch_and_explicit_context_axis(self):
        left, _, _ = synthetic()
        right = copy.deepcopy(left)
        right["manifest"]["host"]["cpu"] = "different CPU"
        self.assertEqual(compare(left, right)["comparisons"][0]["classification"], "inconclusive")
        row = compare(left, right, context_axes=["host.cpu"])["comparisons"][0]
        self.assertEqual(row["classification"], "contextual")
        right["manifest"]["build"]["flags"] += " /different"
        self.assertEqual(compare(left, right, context_axes=["host.cpu"])["comparisons"][0]["classification"], "inconclusive")
        with self.assertRaises(ValueError):
            compare(left, right, context_axes=["workload.threads"])

    def test_unknown_environment_is_explicitly_exploratory(self):
        left, _, _ = synthetic()
        right = copy.deepcopy(left)
        right["manifest"]["host"]["core"] = "unknown"
        ordinary = compare(left, right)["comparisons"][0]
        exploratory = compare(left, right, exploratory=True)["comparisons"][0]
        self.assertEqual(ordinary["classification"], "inconclusive")
        self.assertIsNone(ordinary["absolute_change"])
        self.assertTrue(exploratory["exploratory_delta"])
        self.assertEqual(exploratory["classification"], "inconclusive")
        self.assertIn("unknown host.core", exploratory["reasons"])

    def test_zero_baseline_no_ratio_and_failed_observation(self):
        left, _, _ = synthetic((0, 0, 0, 0, 0))
        right, _, _ = synthetic()
        row = compare(left, right)["comparisons"][1]
        self.assertIsNone(row["relative_change_percent"])
        self.assertIsNotNone(row["absolute_change"])
        right["normalized"]["observations"][0]["quality"] = "failed"
        failed = next(x for x in compare(left, right, exploratory=True)["comparisons"] if x["metric"] == "real_time")
        self.assertEqual(failed["classification"], "failed")
        self.assertIsNone(failed["absolute_change"])

    def test_repetition_level_and_few_samples(self):
        left, _, _ = synthetic((10,))
        right, _, _ = synthetic((20,))
        row = compare(left, right)["comparisons"][0]
        self.assertEqual(row["classification"], "inconclusive")
        self.assertEqual(row["baseline"]["statistics"]["n"], 1)
        self.assertEqual(row["baseline"]["observation"]["sample_level"], "in_process_repetition")

    def test_process_level_cannot_be_promoted_by_manifest(self):
        bundle, raw, _ = synthetic()
        bundle["manifest"]["workloads"]["lookup64"]["sample_level"] = "independent_process_run"
        observations, _ = import_artifact(raw, bundle["manifest"])
        self.assertEqual(observations[0]["sample_level"], "in_process_repetition")
        self.assertEqual(observations[0]["quality"], "missing")

    def test_revision_variant_and_source_changes_are_allowed(self):
        left, _, _ = synthetic()
        right = copy.deepcopy(left)
        right["manifest"]["revision"] = "new revision"
        right["manifest"]["regions"][0].update(variant="new implementation", source_sha256="a" * 64)
        self.assertEqual(compare(left, right)["comparisons"][0]["classification"], "comparable")

    def test_finite_samples_cannot_emit_nonfinite_dispersion(self):
        stats = summarize([1e308, 1e308, 0, 0])
        json.dumps(stats, allow_nan=False)
        self.assertEqual(stats["median"], 5e307)
        self.assertIsNone(stats["variance"])


class BundleAndCliTests(unittest.TestCase):
    def test_real_end_to_end_and_stable_reports(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for name in ("baseline", "candidate"):
                args = ["import", "--provider", "google-benchmark", "--raw", str(FIXTURES / "btree-v1.9.0.json"),
                        "--manifest", str(FIXTURES / (name + ".manifest.json")), "--out", str(root / name)]
                self.assertEqual(main(args), 0)
                self.assertEqual((root / name / "raw/input.json").read_bytes(), (FIXTURES / "btree-v1.9.0.json").read_bytes())
            for output in ("report1", "report2"):
                self.assertEqual(main(["compare", str(root / "baseline"), str(root / "candidate"),
                                       "--out", str(root / output), "--exploratory"]), 0)
            for name in ("report.json", "report.md"):
                self.assertEqual((root / "report1" / name).read_bytes(), (root / "report2" / name).read_bytes())
            report = json.loads((root / "report1/report.json").read_text(),
                                parse_constant=lambda value: self.fail("non-standard JSON: " + value))
            self.assertTrue(all(x["exploratory_delta"] for x in report["comparisons"]))
            text = (root / "report1/report.md").read_text()
            self.assertIn("/benchmarks/", text)
            self.assertIn("Uninterpreted counters", text)
            self.assertIn("SHA-256", text)
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(args), 2)
                self.assertEqual(main(["compare", str(root / "baseline"), str(root / "candidate"),
                                       "--out", str(root / "report1")]), 2)

    def test_hashes_and_damaged_json_fail_without_output(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            manifest = fixture_manifest()
            manifest["raw_source"]["sha256"] = "0" * 64
            write_json(root / "manifest.json", manifest)
            with self.assertRaisesRegex(ValueError, "SHA-256"):
                import_bundle("google-benchmark", import_artifact, FIXTURES / "btree-v1.9.0.json",
                              root / "manifest.json", root / "bad-hash")
            damaged = b"{ damaged JSON"
            (root / "damaged.json").write_bytes(damaged)
            manifest["raw_source"]["sha256"] = hashlib.sha256(damaged).hexdigest()
            write_json(root / "manifest.json", manifest)
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(["import", "--provider", "google-benchmark", "--raw", str(root / "damaged.json"),
                                       "--manifest", str(root / "manifest.json"), "--out", str(root / "bad-json")]), 2)
            self.assertFalse((root / "bad-hash").exists())
            self.assertFalse((root / "bad-json").exists())

    def test_modified_normalized_raw_and_escape_rejected(self):
        for mutation in ("normalized", "raw", "escape"):
            with self.subTest(mutation=mutation), tempfile.TemporaryDirectory() as temporary:
                root = Path(temporary)
                import_bundle("google-benchmark", import_artifact, FIXTURES / "btree-v1.9.0.json",
                              FIXTURES / "baseline.manifest.json", root / "bundle")
                if mutation == "escape":
                    manifest = json.loads((root / "bundle/manifest.json").read_text())
                    manifest["raw_source"]["path"] = "../outside.json"
                    write_json(root / "bundle/manifest.json", manifest)
                else:
                    file = root / ("bundle/normalized.json" if mutation == "normalized" else "bundle/raw/input.json")
                    file.write_bytes(b"{}")
                with self.assertRaises(ValueError):
                    load_bundle(root / "bundle")

    def test_manifest_major_duplicates_and_bad_references(self):
        for mutation in ("major", "duplicate", "reference"):
            with self.subTest(mutation=mutation):
                manifest = fixture_manifest()
                if mutation == "major":
                    manifest["schema_version"] = 2
                elif mutation == "duplicate":
                    manifest["regions"] *= 2
                else:
                    manifest["regions"][0]["workload_id"] = "absent"
                with self.assertRaises(ValueError):
                    validate_manifest(manifest)

    def test_module_entrypoint_needs_no_collector(self):
        result = subprocess.run([sys.executable, "-m", "wjr_profiler", "--help"], cwd=ROOT,
                                capture_output=True, text=True, check=False)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("compare", result.stdout)


if __name__ == "__main__":
    unittest.main()
