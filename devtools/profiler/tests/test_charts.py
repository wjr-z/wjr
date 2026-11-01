"""Chart contract tests; synthetic data are never performance evidence."""

import contextlib
import copy
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock
import xml.etree.ElementTree as ET

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from test_profiler import FIXTURES, synthetic
from test_mca_compare import bundle as mca_bundle
from wjr_profiler.__main__ import main
from wjr_profiler.charts import _dots, render
from wjr_profiler.compare import compare
from wjr_profiler.report import write_report

HAS_MATPLOTLIB = importlib.util.find_spec("matplotlib") is not None


def comparison(values=(10, 20, 30, 40, 50)):
    left, _, _ = synthetic()
    right, _, _ = synthetic(values)
    for bundle in (left, right):
        bundle["manifest"]["raw_source"]["path"] = "raw/input.json"
    return compare(left, right)


class ChartPolicyTests(unittest.TestCase):
    def test_ineligible_and_exploratory_never_import_renderer(self):
        for state in ("inconclusive", "failed"):
            report = comparison()
            for row in report["comparisons"]:
                row["classification"] = state
                row["exploratory_delta"] = state == "inconclusive"
            with mock.patch("wjr_profiler.charts._plotter", side_effect=AssertionError("imported")):
                index, assets = render(report)
            self.assertFalse(assets)
            self.assertTrue(all(entry["reason"] for entry in index["entries"]))

    def test_real_contract_mismatch_cannot_produce_charts(self):
        left, _, _ = synthetic()
        right = copy.deepcopy(left)
        right["manifest"]["workloads"]["lookup64"]["threads"] = 2
        with mock.patch("wjr_profiler.charts._plotter", side_effect=AssertionError("imported")):
            self.assertFalse(render(compare(left, right, exploratory=True))[1])

    def test_missing_renderer_fails_before_creating_output(self):
        with tempfile.TemporaryDirectory() as temporary:
            destination = Path(temporary) / "absent"
            with mock.patch.dict(sys.modules, {"matplotlib": None}):
                with self.assertRaisesRegex(ValueError, "optional matplotlib"):
                    write_report(comparison(), destination, charts=True)
            self.assertFalse(destination.exists())

    def test_default_report_does_not_import_renderer(self):
        with tempfile.TemporaryDirectory() as temporary:
            destination = Path(temporary) / "plain"
            with mock.patch("wjr_profiler.charts._plotter", side_effect=AssertionError("imported")):
                write_report(comparison(), destination)
            self.assertFalse((destination / "charts").exists())

    def test_bounded_dots_do_not_rewrite_samples(self):
        samples = list(range(10000))
        selected = _dots(samples)
        self.assertEqual(len(selected), 256)
        self.assertEqual((selected[0], selected[-1]), (0, 9999))
        self.assertEqual(len(set(selected)), 256)
        self.assertEqual(len(samples), 10000)

    def test_cli_unknown_real_fixture_is_explicitly_skipped(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for side in ("baseline", "candidate"):
                self.assertEqual(main(["import", "--provider", "google-benchmark", "--raw",
                                       str(FIXTURES / "btree-v1.9.0.json"), "--manifest",
                                       str(FIXTURES / (side + ".manifest.json")), "--out", str(root / side)]), 0)
            with mock.patch("wjr_profiler.charts._plotter", side_effect=AssertionError("imported")):
                self.assertEqual(main(["compare", str(root / "baseline"), str(root / "candidate"),
                                       "--out", str(root / "report"), "--exploratory", "--charts"]), 0)
            index = json.loads((root / "report/charts/index.json").read_text())
            self.assertFalse(any(entry["file"] for entry in index["entries"]))
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(["compare", str(root / "baseline"), str(root / "candidate"),
                                       "--out", str(root / "report"), "--charts"]), 2)


@unittest.skipUnless(HAS_MATPLOTLIB, "optional matplotlib renderer is not installed")
class SvgTests(unittest.TestCase):
    def test_real_renderer_is_stable_and_preserves_source(self):
        report = comparison((20, 40, 60, 80, 100))
        source = copy.deepcopy(report)
        index, assets = render(report)
        self.assertEqual((index, assets), render(report))
        self.assertEqual(report, source)
        self.assertEqual(len(assets), 2)
        for entry in index["entries"]:
            content = assets[entry["file"]]
            self.assertEqual(hashlib.sha256(content).hexdigest(), entry["sha256"])
            text = " ".join(ET.fromstring(content).itertext())
            for marker in ("SYNTHETIC SCHEMA DATA", "NOT confidence intervals", "in_process_repetition",
                           "ns/iteration", "COMPARABLE", "lower is better", "ALL observations"):
                self.assertIn(marker, text)

    def test_contextual_environment_is_visible(self):
        left, _, _ = synthetic()
        right = copy.deepcopy(left)
        right["manifest"]["host"]["cpu"] = "a different CPU"
        report = compare(left, right, context_axes=["host.cpu"])
        report["comparisons"] = report["comparisons"][:1]
        index, assets = render(report)
        text = next(iter(assets.values())).decode()
        self.assertIn("CONTEXTUAL", text)
        self.assertIn("a different CPU", text)
        self.assertIn("no same-environment regression claim", text)
        self.assertEqual(index["contextual_axes"], ["host.cpu"])

    def test_real_mca_estimate_keeps_model_and_simulation_count(self):
        baseline, _ = mca_bundle()
        report = compare(baseline, copy.deepcopy(baseline))
        report["comparisons"] = report["comparisons"][:1]
        _, assets = render(report)
        text = " ".join(ET.fromstring(next(iter(assets.values()))).itertext())
        self.assertIn("MODEL ESTIMATE", text)
        self.assertIn("skylake", text)
        self.assertIn("not measured runtime", text)
        self.assertIn("n=1 (simulation)", text)
        self.assertNotIn("n=100", text)

    def test_zero_and_extreme_finite_values_have_linear_zero_origin(self):
        for values in ((0, 0, 0, 0, 0), (1e308,) * 5, (1e-300,) * 5):
            report = comparison(values)
            report["comparisons"] = report["comparisons"][:1]
            _, assets = render(report)
            self.assertEqual(len(assets), 1)
            labels = list(ET.fromstring(next(iter(assets.values()))).itertext())
            self.assertTrue(any(label in ("0", "0.0") for label in labels))

    def test_labels_are_literal_and_never_paths_or_markup(self):
        report = comparison()
        report["comparisons"] = report["comparisons"][:1]
        report["comparisons"][0]["region_id"] = '../../<script>alert(1)</script> $\\input{secret}$'
        _, assets = render(report)
        self.assertEqual(list(assets), ["comparison-0001.svg"])
        root = ET.fromstring(next(iter(assets.values())))
        self.assertFalse(any(node.tag.endswith("script") for node in root.iter()))
        self.assertIn("$\\input{secret}$", " ".join(root.itertext()))

    def test_report_links_and_chart_hash_index(self):
        with tempfile.TemporaryDirectory() as temporary:
            destination = Path(temporary) / "charts"
            report = comparison()
            report["comparisons"] = report["comparisons"][:1]
            write_report(report, destination, charts=True)
            index = json.loads((destination / "charts/index.json").read_text())
            self.assertEqual(index["report_sha256"], hashlib.sha256((destination / "report.json").read_bytes()).hexdigest())
            self.assertEqual(json.loads((destination / "report.json").read_text()), report)
            self.assertIn("charts/comparison-0001.svg", (destination / "report.md").read_text())


if __name__ == "__main__":
    unittest.main()
