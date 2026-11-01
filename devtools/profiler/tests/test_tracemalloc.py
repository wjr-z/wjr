"""Owned CPython captures and explicitly synthetic edge cases; no native dump loader."""

import ast
import contextlib
import copy
import hashlib
import io
import json
from pathlib import Path
import platform
import sys
import tempfile
import tracemalloc
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from wjr_profiler.__main__ import main
from wjr_profiler.adapters.tracemalloc_json import import_artifact, SAMPLE_LEVEL
from wjr_profiler.bundle import import_bundle, load_bundle
from wjr_profiler.compare import compare
from wjr_profiler.exporters.tracemalloc_json import default_manifest, export_snapshots, memory_checkpoint, _snapshot
from wjr_profiler.model import summarize
from wjr_profiler.tracemalloc_report import summarize_memory

FIXTURE = ROOT / "tests/fixtures/tracemalloc"
PRODUCER = platform.python_implementation() == "CPython" and platform.python_version() == "3.12.7"


def fixture(name="full"):
    return (json.loads((FIXTURE / (name + ".json")).read_text()),
            json.loads((FIXTURE / (name + ".manifest.json")).read_text()))


def observations(raw):
    return import_artifact(raw, default_manifest(raw, "0" * 64))[0]


def sites(items):
    return next(row for row in items if row["metric"] == "net_retained_bytes")["metadata"]["tracebacks"]


def function_line(name):
    tree = ast.parse((FIXTURE / "program.py").read_text())
    function = next(node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name == name)
    return function.body[0].lineno


def leaf_sites(rows):
    return [row for row in rows if row["traceback"][-1]["file"].endswith("program.py") and
            row["traceback"][-1]["line"] == function_line("leaf")]


class TracemallocTests(unittest.TestCase):
    def test_real_provenance_and_native_bytes_match(self):
        provenance = json.loads((FIXTURE / "provenance.json").read_text())
        self.assertTrue(provenance["real"])
        self.assertEqual((provenance["implementation"], provenance["python"]), ("CPython", "3.12.7"))
        for relative, digest in provenance["source_sha256"].items():
            self.assertEqual(hashlib.sha256((ROOT.parents[1] / relative).read_bytes()).hexdigest(), digest)
        for capture in provenance["captures"]:
            self.assertEqual(capture["exit_status"], 0)
            for kind in ("raw", "manifest", "stdout", "stderr"):
                self.assertEqual(hashlib.sha256((FIXTURE / capture[kind]).read_bytes()).hexdigest(),
                                 capture[kind + "_sha256"])
            raw, manifest = fixture(capture["name"])
            self.assertEqual(manifest["raw_source"]["sha256"], capture["raw_sha256"])
            self.assertEqual(capture["checkpoint"], raw["checkpoint"])
            self.assertEqual(capture["domains"], [0])

    def test_real_retention_release_and_full_callers(self):
        raw, manifest = fixture()
        items, _ = import_artifact(raw, manifest)
        selected = leaf_sites(sites(items))
        retained = [row for row in selected if row["net_retained_bytes"] > 0]
        released = [row for row in selected if row["net_retained_bytes"] < 0]
        self.assertEqual(len(retained), 2)
        self.assertEqual(len(released), 1)
        self.assertGreaterEqual(min(row["net_retained_bytes"] for row in retained), 64 * 1024)
        self.assertGreaterEqual(-released[0]["net_retained_bytes"], 128 * 1024)
        self.assertNotEqual(retained[0]["traceback"], retained[1]["traceback"])
        self.assertEqual(retained[0]["traceback"][-1], retained[1]["traceback"][-1])
        self.assertTrue(all(row["truncated"] is False for row in selected))

    def test_temporary_peak_has_no_surviving_site_or_peak_attribution(self):
        raw, _ = fixture()
        checkpoint = raw["checkpoint"]
        self.assertGreaterEqual(checkpoint["peak_bytes"] - checkpoint["current_bytes"], 700 * 1024)
        selected = sites(observations(raw))
        temporary_line = function_line("temporary")
        self.assertFalse(any(frame["file"].endswith("program.py") and frame["line"] == temporary_line
                             for row in selected for frame in row["traceback"]))
        self.assertTrue(all("peak_bytes" not in row for row in selected))

    def test_real_truncation_preserves_original_depth_without_guessing_callers(self):
        raw, _ = fixture("truncated")
        selected = leaf_sites(sites(observations(raw)))
        self.assertTrue(all(len(row["traceback"]) == 1 and row["truncated"] for row in selected))
        self.assertGreaterEqual(len({row["total_nframe"] for row in selected}), 2)
        self.assertEqual(len([row for row in selected if row["net_retained_bytes"] > 0]), 1)

    def test_blocks_and_tracebacks_are_not_repetitions(self):
        raw, _ = fixture()
        items = observations(raw)
        for item in items:
            self.assertEqual(item["sample_level"], SAMPLE_LEVEL)
            self.assertEqual(summarize(item["samples"])["n"], 1)
            self.assertIsNone(summarize(item["samples"])["variance"])
            self.assertEqual(item["quality"], "insufficient_samples")
        net = next(item for item in items if item["metric"] == "net_retained_bytes")
        self.assertEqual(net["kind"], "derived")
        self.assertEqual(net["samples"], [sum(row["size_bytes"] for row in raw["ending"]["records"]) -
                                          sum(row["size_bytes"] for row in raw["baseline"]["records"])])

    def test_domains_depth_and_unknown_frames_are_separate_identities(self):
        raw, _ = fixture()
        row = dict(domain=0, traceback=[dict(file="<unknown>", line=0)], total_nframe=None,
                   size_bytes=0, blocks=2)
        other = dict(row, domain=7, size_bytes=17)
        known = dict(row, total_nframe=1, size_bytes=5)
        raw["baseline"]["records"] = [row, other, known]
        raw["ending"]["records"] = [dict(row, blocks=1)]
        items = observations(raw)
        selected = sites(items)
        self.assertEqual(len(selected), 3)
        self.assertEqual({row["domain"] for row in selected}, {0, 7})
        unknown = next(row for row in selected if row["domain"] == 0 and row["total_nframe"] is None)
        self.assertIsNone(unknown["truncated"])
        self.assertTrue(unknown["unknown_frames"])
        self.assertEqual(unknown["net_retained_blocks"], -1)
        self.assertEqual(unknown["net_retained_bytes"], 0)
        raw["baseline"]["records"][0]["traceback"][0]["file"] = ""
        self.assertTrue(all(row["unknown_frames"] for row in sites(observations(raw))))

    def test_empty_zero_and_missing_checkpoint_stay_distinct(self):
        raw, _ = fixture()
        raw["baseline"]["records"] = raw["ending"]["records"] = []
        raw["checkpoint"] = None
        items = {item["metric"]: item for item in observations(raw)}
        self.assertEqual(items["net_retained_bytes"]["samples"], [0])
        self.assertEqual(items["ending_live_blocks"]["samples"], [0])
        self.assertEqual(items["traced_peak_bytes"]["samples"], [])
        self.assertEqual(items["traced_peak_bytes"]["quality"], "missing")
        raw["checkpoint"] = dict(current_bytes=0, peak_bytes=0, tracer_overhead_bytes=0,
                                 peak_window="known empty window", sampling_point="known checkpoint",
                                 domain_scope="all_traced_domains")
        peak = next(item for item in observations(raw) if item["metric"] == "traced_peak_bytes")
        self.assertEqual(peak["samples"], [0])
        self.assertNotEqual(peak["quality"], "missing")

    def test_unknown_producer_or_policy_is_exploratory(self):
        for name in ("version", "coverage", "peak_window"):
            with self.subTest(name=name):
                raw, _ = fixture()
                if name == "version":
                    raw["producer"][name] = "3.12.8"
                elif name == "coverage":
                    raw["collection"][name] = "unknown"
                else:
                    raw["checkpoint"][name] = "unknown"
                peak = next(row for row in observations(raw) if row["metric"] == "traced_peak_bytes")
                self.assertEqual(peak["quality"], "exploratory")

    def test_malformed_values_duplicates_and_limits_are_rejected(self):
        def change(raw, name):
            record = raw["baseline"]["records"][0]
            if name == "schema": raw["schema"] += "x"
            elif name == "domain": record["domain"] = -1
            elif name == "domain-bool": record["domain"] = True
            elif name == "domain-overflow": record["domain"] = 1 << 32
            elif name == "size": record["size_bytes"] = -1
            elif name == "size-float": record["size_bytes"] = 1.5
            elif name == "count": record["blocks"] = 0
            elif name == "count-bool": record["blocks"] = True
            elif name == "frame": record["traceback"] = []
            elif name == "line": record["traceback"][0]["line"] = -1
            elif name == "depth": record["total_nframe"] = len(record["traceback"]) - 1
            elif name == "missing-depth": del record["total_nframe"]
            elif name == "duplicate": raw["baseline"]["records"].append(copy.deepcopy(record))
            elif name == "limits": raw["ending"]["traceback_limit"] += 1
            elif name == "small-limit": raw["baseline"]["traceback_limit"] = 0
            elif name == "peak": raw["checkpoint"]["peak_bytes"] = raw["checkpoint"]["current_bytes"] - 1
            elif name == "peak-nan": raw["checkpoint"]["peak_bytes"] = float("nan")
            elif name == "checkpoint-domain": raw["checkpoint"]["domain_scope"] = "only-python"
        for name in ("schema", "domain", "domain-bool", "domain-overflow", "size", "size-float", "count", "count-bool",
                     "frame", "line", "depth", "missing-depth", "duplicate", "limits", "small-limit", "peak", "peak-nan", "checkpoint-domain"):
            with self.subTest(name=name):
                raw, manifest = fixture()
                change(raw, name)
                with self.assertRaises(ValueError): import_artifact(raw, manifest)

    def test_manifest_policy_and_selector_cannot_relabel_raw_evidence(self):
        for name in ("coverage", "traceback_limit", "checkpoint", "selector", "clock", "sample_level"):
            with self.subTest(name=name):
                raw, manifest = fixture()
                workload = manifest["workloads"]["snapshot-pair"]
                if name in ("clock", "sample_level"): workload[name] = "independent_process_run"
                elif name == "selector": manifest["regions"][0]["snapshot_pair"] = "guessed"
                else: workload["instrumentation"][name] = "changed"
                with self.assertRaises(ValueError): import_artifact(raw, manifest)

    def test_bundle_summary_comparison_and_exclusive_cli_outputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            raw_path, manifest_path = FIXTURE / "full.json", FIXTURE / "full.manifest.json"
            self.assertEqual(main(["import", "--provider", "tracemalloc-json", "--raw", str(raw_path),
                                   "--manifest", str(manifest_path), "--out", str(root / "bundle")]), 0)
            bundle = load_bundle(root / "bundle")
            self.assertEqual((root / "bundle/raw/input.json").read_bytes(), raw_path.read_bytes())
            report = summarize_memory(bundle)
            self.assertEqual(report["n"], 1)
            self.assertEqual(report["sample_level"], SAMPLE_LEVEL)
            self.assertEqual(report["regions"][0]["domains"][0]["domain"], 0)
            self.assertTrue(all(item["classification"] == "inconclusive" for item in compare(bundle, bundle)["comparisons"]))
            self.assertEqual(main(["tracemalloc-summary", str(root / "bundle"), "--out", str(root / "report"), "--limit", "1"]), 0)
            text = (root / "report/report.md").read_text()
            self.assertIn("never assigned", text)
            self.assertIn("report.json retains all", text)
            self.assertIn("not RSS", text)
            self.assertNotIn("Self seconds", text)
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(["tracemalloc-summary", str(root / "bundle"), "--out", str(root / "report")]), 2)
                self.assertEqual(main(["tracemalloc-summary", str(root / "bundle"), "--out", str(root / "bad"), "--limit", "0"]), 2)
            self.assertFalse((root / "bad").exists())

    def test_native_dump_or_untrusted_pickle_is_never_loaded(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / "untrusted.snapshot"
            path.write_bytes(b"\x80\x04not JSON or a trusted Snapshot")
            with patch("pickle.load", side_effect=AssertionError("must not load pickle")), \
                 patch("pickle.loads", side_effect=AssertionError("must not load pickle")), \
                 patch.object(tracemalloc.Snapshot, "load", side_effect=AssertionError("must not load Snapshot files")):
                with self.assertRaises(ValueError): export_snapshots(path, path, root / "out.json")
                self.assertFalse((root / "out.json").exists())
                _, manifest = fixture()
                manifest["raw_source"]["sha256"] = hashlib.sha256(path.read_bytes()).hexdigest()
                manifest_path = root / "manifest.json"
                manifest_path.write_text(json.dumps(manifest))
                with contextlib.redirect_stderr(io.StringIO()):
                    self.assertEqual(main(["import", "--provider", "tracemalloc-json", "--raw", str(path),
                        "--manifest", str(manifest_path), "--out", str(root / "untrusted-bundle")]), 2)
                self.assertFalse((root / "untrusted-bundle").exists())

    @unittest.skipUnless(PRODUCER, "live producer exporter verified on CPython 3.12.7")
    def test_owned_snapshots_export_after_stopping_without_changing_tracer(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            tracemalloc.start(4)
            try:
                before = tracemalloc.take_snapshot()
                value = bytearray(8192)
                checkpoint = memory_checkpoint(peak_window="since start", sampling_point="before ending snapshot")
                after = tracemalloc.take_snapshot()
            finally:
                tracemalloc.stop()
            raw = export_snapshots(before, after, root / "raw.json", coverage="unfiltered owned snapshots",
                harness_policy="included", epoch_policy="no stop/clear between endpoints", checkpoint=checkpoint,
                manifest_output=root / "manifest.json")
            self.assertFalse(tracemalloc.is_tracing())
            self.assertGreaterEqual(sum(row["size_bytes"] for row in raw["ending"]["records"]), len(value))
            import_bundle("tracemalloc-json", import_artifact, root / "raw.json", root / "manifest.json", root / "bundle")
            with self.assertRaises(ValueError): export_snapshots(before, after, root / "raw.json")
            with self.assertRaises(ValueError): export_snapshots(before, after, root / "same", manifest_output=root / "same")
            with self.assertRaises(ValueError): memory_checkpoint(peak_window="none", sampling_point="inactive")

    @unittest.skipUnless(PRODUCER, "live producer exporter verified on CPython 3.12.7")
    def test_export_grouping_retains_domains_and_depth_using_synthetic_snapshots(self):
        snapshot = tracemalloc.Snapshot([(0, 7, (("leaf.py", 2),), 3), (7, 9, (("leaf.py", 2),), 3),
                                         (0, 11, (("leaf.py", 2),), 4), (0, 13, (("leaf.py", 2),), 3)], 1)
        rows = _snapshot(snapshot)["records"]
        self.assertEqual(len(rows), 3)
        grouped = next(row for row in rows if row["domain"] == 0 and row["total_nframe"] == 3)
        self.assertEqual((grouped["size_bytes"], grouped["blocks"]), (20, 2))

    def test_default_manifest_has_no_observed_values_in_instrumentation(self):
        raw, _ = fixture()
        manifest = default_manifest(raw, "0" * 64)
        self.assertEqual(manifest["host"], {})
        self.assertEqual(manifest["workloads"]["snapshot-pair"]["sample_level"], SAMPLE_LEVEL)
        policy = manifest["workloads"]["snapshot-pair"]["instrumentation"]["checkpoint"]
        self.assertNotIn("peak_bytes", policy)


if __name__ == "__main__":
    unittest.main()
