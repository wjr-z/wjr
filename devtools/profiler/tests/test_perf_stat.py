"""Real denied captures and explicitly synthetic, upstream-linted counter cases."""

import contextlib
import copy
import hashlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from wjr_profiler.__main__ import PROVIDERS, main
from wjr_profiler.adapters.perf_stat import decode_raw, import_artifact
from wjr_profiler.bundle import load_bundle
from wjr_profiler.compare import compare
from wjr_profiler.model import summarize

FIXTURE = ROOT / "tests/fixtures/perf_stat"


def fixture(name="synthetic"):
    raw_name = "synthetic.schema.jsonl" if name == "synthetic" else name + ".json"
    return (decode_raw((FIXTURE / raw_name).read_bytes()),
            json.loads((FIXTURE / (name + ".manifest.json")).read_text(encoding="utf-8")))


def synthetic_user_case():
    raw, manifest = fixture()
    manifest["producer"]["evidence"] = "user_supplied"
    return raw, manifest


def metric(observations, name):
    return next(row for row in observations if row["metric"] == name)


class PerfStatTests(unittest.TestCase):
    def test_real_unavailable_capture_provenance(self):
        provenance = json.loads((FIXTURE / "provenance.json").read_text(encoding="utf-8"))
        self.assertTrue(provenance["real"])
        self.assertFalse(provenance["positive_hardware_smoke"])
        self.assertEqual(provenance["version"], "perf version 6.8.12")
        for capture in provenance["captures"]:
            self.assertEqual(capture["exit_status"], 255)
            raw = (FIXTURE / (capture["name"] + ".json")).read_bytes()
            self.assertEqual(raw, b"")
            self.assertEqual(hashlib.sha256(raw).hexdigest(), capture["raw_sha256"])
            self.assertIn("perf_event_paranoid", (FIXTURE / (capture["name"] + ".stderr")).read_text())

    def test_real_denial_becomes_failed_not_zero_measurements(self):
        for name in ("hardware", "tool"):
            with self.subTest(name=name):
                raw, manifest = fixture(name)
                observations, diagnostics = import_artifact(raw, manifest)
                self.assertTrue(observations)
                self.assertTrue(all(row["quality"] == "failed" for row in observations))
                self.assertTrue(all(row["samples"] == [] for row in observations))
                self.assertTrue(all(row["code"] == "producer_failed" for row in diagnostics))

    def test_synthetic_source_lint_is_not_collection_evidence(self):
        provenance = json.loads((FIXTURE / "provenance.json").read_text(encoding="utf-8"))
        self.assertEqual(provenance["upstream_lint"]["exit_status"], 0)
        self.assertIn("v6.8.12", provenance["upstream_lint"]["source"]["url"])
        raw, manifest = fixture()
        observations, _ = import_artifact(raw, manifest)
        value = metric(observations, "instructions:u")
        self.assertEqual(value["quality"], "exploratory")
        self.assertIn("Synthetic", value["missing_reason"])
        self.assertEqual(value["metadata"]["evidence"], "synthetic")

    def test_scaling_native_units_and_event_modifiers(self):
        raw, manifest = fixture()
        observations, _ = import_artifact(raw, manifest)
        cycles = metric(observations, "cycles:u")
        self.assertEqual(cycles["samples"], [1000])
        self.assertEqual(cycles["metadata"]["running_percent"], 50)
        self.assertEqual(cycles["metadata"]["event_runtime"], 1000000)
        self.assertEqual(cycles["metadata"]["event_runtime_unit"], "ns")
        self.assertTrue(cycles["metadata"]["multiplexed"])
        self.assertIsNone(cycles["metadata"]["enabled_time_ns"])
        clock = metric(observations, "task-clock:u")
        self.assertEqual((clock["unit"], clock["samples"]), ("msec", [5.75]))

    def test_unsupported_not_counted_absent_and_true_zero_are_distinct(self):
        raw, manifest = fixture()
        observations, diagnostics = import_artifact(raw, manifest)
        codes = {row["code"] for row in diagnostics}
        self.assertTrue({"unsupported_event", "not_counted", "missing_event"} <= codes)
        self.assertEqual(metric(observations, "cache-references:u")["samples"], [0])
        self.assertEqual(metric(observations, "branches:u")["samples"], [])
        self.assertEqual(metric(observations, "cache-misses:u")["unit"], "unknown")

    def test_one_process_not_one_sample_per_event_or_internal_iteration(self):
        raw, manifest = synthetic_user_case()
        observations, _ = import_artifact(raw, manifest)
        instructions = metric(observations, "instructions:u")
        self.assertEqual(instructions["quality"], "insufficient_samples")
        statistics = summarize(instructions["samples"])
        self.assertEqual(statistics["n"], 1)
        self.assertIsNone(statistics["variance"])
        self.assertTrue(all(row["scope"] == "whole_process" for row in observations))

    def test_large_integral_counter_keeps_printed_precision(self):
        raw, manifest = synthetic_user_case()
        raw[1][1]["counter-value"] = "9007199254740993.000000"
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(metric(observations, "instructions:u")["samples"], [9007199254740993])

    def test_nonfinite_negative_locale_or_missing_values_are_unavailable(self):
        for value in (None, True, -1, "NaN", "Infinity", "-1.0", "1,000", "1_000", "", float("nan")):
            with self.subTest(value=value):
                raw, manifest = synthetic_user_case()
                raw[1][1]["counter-value"] = value
                observations, _ = import_artifact(raw, manifest)
                row = metric(observations, "instructions:u")
                self.assertEqual((row["quality"], row["samples"]), ("missing", []))
                json.dumps(observations, allow_nan=False)

    def test_missing_running_zero_runtime_and_invalid_percentage(self):
        for key, value in (("event-runtime", None), ("event-runtime", 0), ("pcnt-running", 0),
                           ("pcnt-running", 101), ("pcnt-running", float("inf"))):
            raw, manifest = synthetic_user_case()
            raw[1][1][key] = value
            observations, _ = import_artifact(raw, manifest)
            self.assertEqual(metric(observations, "instructions:u")["samples"], [])

    def test_duplicate_events_aggregate_and_interval_not_promoted_to_samples(self):
        raw, manifest = fixture()
        with self.assertRaisesRegex(ValueError, "duplicate"):
            import_artifact(raw + [raw[0]], manifest)
        for field in ("interval", "cpu", "variance", "aggregate-number"):
            raw, manifest = fixture()
            raw[0][1][field] = 1
            with self.assertRaisesRegex(ValueError, "unsupported"):
                import_artifact(raw, manifest)

    def test_derived_metric_rows_and_unselected_events_remain_raw_only(self):
        raw, manifest = fixture()
        raw.append((7, {"metric-value": 2.0, "metric-unit": "insn per cycle"}))
        manifest["regions"][0]["events"] = ["instructions:u"]
        observations, diagnostics = import_artifact(raw, manifest)
        self.assertEqual(len(observations), 1)
        self.assertIn("derived_metric_uninterpreted", {row["code"] for row in diagnostics})
        self.assertIn("unselected_event", {row["code"] for row in diagnostics})

    def test_explicit_scope_command_scaling_and_repeat_contract(self):
        for key, value in (("scope", "kernel"), ("repeat", True), ("repeat", 2),
                           ("interval", True), ("scaling", "unknown")):
            raw, manifest = fixture()
            manifest["producer"]["perf_stat"][key] = value
            with self.assertRaises(ValueError):
                import_artifact(raw, manifest)
        for option in ("-a", "-p123", "--pid=123", "--bpf-prog=1", "-I100", "-r5", "--metric-only", "--no-scale"):
            raw, manifest = fixture()
            manifest["producer"]["argv"].insert(2, option)
            with self.assertRaises(ValueError):
                import_artifact(raw, manifest)

    def test_raw_scale_is_preserved_and_unknown_versions_are_exploratory(self):
        raw, manifest = synthetic_user_case()
        manifest["producer"]["argv"].insert(2, "--no-scale")
        manifest["producer"]["perf_stat"]["scaling"] = "raw"
        manifest["producer"]["version"] = "7.0"
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(metric(observations, "cycles:u")["samples"], [1000])
        self.assertEqual(metric(observations, "instructions:u")["quality"], "exploratory")

    def test_jsonl_physical_lines_arrays_and_invalid_input(self):
        record = {"counter-value": "1", "event": "cycles:u"}
        self.assertEqual(decode_raw(("\n" + json.dumps(record)).encode())[0][0], 2)
        self.assertEqual(decode_raw(b""), [])
        for content in (b"[]", b"null", b"{", b"{}\nnot-json"):
            with self.assertRaises(ValueError):
                decode_raw(content)

    def test_offline_cli_real_failure_report_and_stderr_hash_integrity(self):
        self.assertTrue({"google-benchmark", "llvm-mca", "cprofile-json", "curl-json", "perf-stat"} <= PROVIDERS.keys())
        with tempfile.TemporaryDirectory() as temporary:
            bundle, report = Path(temporary) / "bundle", Path(temporary) / "report"
            arguments = ["import", "--provider", "perf-stat", "--raw", str(FIXTURE / "hardware.json"),
                         "--manifest", str(FIXTURE / "hardware.manifest.json"),
                         "--stderr", str(FIXTURE / "hardware.stderr"), "--out", str(bundle)]
            self.assertEqual(main(arguments), 0)
            self.assertEqual((bundle / "raw/stderr.txt").read_bytes(), (FIXTURE / "hardware.stderr").read_bytes())
            self.assertEqual(main(["perf-summary", str(bundle), "--out", str(report)]), 0)
            text = (report / "report.md").read_text(encoding="utf-8")
            self.assertIn("unavailable", text)
            self.assertIn("perf_event_paranoid", text)
            counters = json.loads((report / "report.json").read_text())["counters"]
            self.assertTrue(all(row["statistics"]["n"] == 0 for row in counters))
            (bundle / "raw/stderr.txt").write_bytes(b"changed")
            with self.assertRaisesRegex(ValueError, "stderr SHA-256"):
                load_bundle(bundle)

    def test_synthetic_and_machine_or_collection_mismatch_cannot_get_exploratory_delta(self):
        raw, manifest = fixture()
        observations, diagnostics = import_artifact(raw, manifest)
        before = dict(manifest=manifest, normalized=dict(observations=observations, diagnostics=diagnostics))
        after = copy.deepcopy(before)
        report = compare(before, after, exploratory=True)
        self.assertTrue(all(row["absolute_change"] is None for row in report["comparisons"]))
        self.assertTrue(any("synthetic" in reason for row in report["comparisons"] for reason in row["reasons"]))
        raw, manifest = synthetic_user_case()
        observations, diagnostics = import_artifact(raw, manifest)
        before = dict(manifest=manifest, normalized=dict(
            observations=[metric(observations, "instructions:u")], diagnostics=diagnostics))
        control = compare(before, copy.deepcopy(before), exploratory=True)
        self.assertEqual(control["comparisons"][0]["absolute_change"], 0)
        for change in ("host", "pmu", "filters", "inherit", "affinity", "kernel", "scaling"):
            after = copy.deepcopy(before)
            if change == "host":
                after["manifest"]["host"]["id"] = "different-machine"
            else:
                for row in after["normalized"]["observations"]:
                    row["metadata"]["capture"][change] = "different"
            report = compare(before, after, context_axes=["host.id"], exploratory=True)
            self.assertTrue(all(row["absolute_change"] is None for row in report["comparisons"]))

    def test_stderr_is_perf_only_and_invalid_output_does_not_create_bundle(self):
        with tempfile.TemporaryDirectory() as temporary:
            out = Path(temporary) / "bundle"
            with contextlib.redirect_stderr(io.StringIO()):
                result = main(["import", "--provider", "curl-json", "--raw", str(ROOT / "tests/fixtures/curl/normal.jsonl"),
                               "--manifest", str(ROOT / "tests/fixtures/curl/normal.manifest.json"),
                               "--stderr", str(FIXTURE / "hardware.stderr"), "--out", str(out)])
            self.assertEqual(result, 2)
            self.assertFalse(out.exists())

    def test_stderr_attachment_cannot_escape_the_bundle(self):
        with tempfile.TemporaryDirectory() as temporary:
            bundle = Path(temporary) / "bundle"
            self.assertEqual(main(["import", "--provider", "perf-stat", "--raw", str(FIXTURE / "hardware.json"),
                "--manifest", str(FIXTURE / "hardware.manifest.json"), "--stderr", str(FIXTURE / "hardware.stderr"),
                "--out", str(bundle)]), 0)
            path = bundle / "manifest.json"
            manifest = json.loads(path.read_text(encoding="utf-8"))
            manifest["raw_source"]["stderr"]["path"] = "../outside.txt"
            path.write_text(json.dumps(manifest), encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "stderr path escapes"):
                load_bundle(bundle)


if __name__ == "__main__":
    unittest.main()
