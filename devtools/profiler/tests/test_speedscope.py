"""Sample attribution, strict input rejection, real capture and offline CLI entry points."""

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

from wjr_profiler.__main__ import main
from wjr_profiler.adapters.speedscope import SCHEMA, default_manifest, import_artifact
from wjr_profiler.bundle import import_bundle, load_bundle
from wjr_profiler.compare import compare
from wjr_profiler.model import summarize
from wjr_profiler.sampled_report import summarize_profile

FIXTURE = ROOT / "tests/fixtures/speedscope"


def synthetic():
    raw = dict(exporter="py-spy@0.4.1", shared=dict(frames=[
        dict(name="recursive", file="a.py", line=7), dict(name="leaf", file="a.py", line=12),
        dict(name="leaf", file="b.py", line=12)]), profiles=[
        dict(type="sampled", name="worker", unit="seconds", startValue=0, endValue=10,
             samples=[[0, 0, 1], [0, 2], []], weights=[2, 3, 5]),
        dict(type="sampled", name="other", unit="seconds", startValue=0, endValue=1,
             samples=[[2]], weights=[1])])
    raw["$schema"] = SCHEMA
    return raw, default_manifest(raw, "0" * 64)


class SampledStackTests(unittest.TestCase):
    def test_recursion_leaf_weights_empty_samples_and_thread_separation(self):
        raw, manifest = synthetic()
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(len(observations), 2)
        self.assertEqual(observations[0]["samples"], [10])
        self.assertEqual(observations[1]["samples"], [1])
        self.assertEqual(observations[0]["metadata"]["empty_stack_weight"], 5)
        rows = {row["frame_index"]: row for row in observations[0]["metadata"]["functions"]}
        self.assertEqual(rows[0]["inclusive_weight"], 5)
        self.assertEqual(rows[0]["self_weight"], 0)
        self.assertEqual(rows[1]["self_weight"], 2)
        self.assertEqual(rows[2]["self_weight"], 3)
        self.assertEqual(rows[2]["self_fraction"], .3)
        self.assertNotEqual(rows[1]["frame"]["file"], rows[2]["frame"]["file"])
        self.assertEqual(summarize(observations[0]["samples"])["n"], 1)
        self.assertIsNone(summarize(observations[0]["samples"])["variance"])

    def test_malformed_profiles_are_rejected_even_when_not_selected(self):
        mutations = [
            lambda raw: raw.update({"$schema": "future"}),
            lambda raw: raw["profiles"][1].update(type="evented"),
            lambda raw: raw["profiles"][1].update(unit="bytes"),
            lambda raw: raw["profiles"][1].update(weights=[]),
            lambda raw: raw["profiles"][1].update(weights=[-1]),
            lambda raw: raw["profiles"][1].update(weights=[True]),
            lambda raw: raw["profiles"][1].update(weights=[float("nan")]),
            lambda raw: raw["profiles"][1].update(samples=[[True]]),
            lambda raw: raw["profiles"][1].update(samples=[[-1]]),
            lambda raw: raw["profiles"][1].update(samples=[[3]]),
            lambda raw: raw["profiles"][1].update(endValue=-1),
            lambda raw: raw["shared"]["frames"][0].update(line=True),
        ]
        for mutate in mutations:
            raw, manifest = synthetic()
            manifest["regions"] = manifest["regions"][:1]
            mutate(raw)
            with self.subTest(mutate=mutate), self.assertRaises(ValueError):
                import_artifact(raw, manifest)

    def test_explicit_thread_identity_and_unknown_exporter(self):
        for mutation in ("name", "index", "duplicate"):
            raw, manifest = synthetic()
            if mutation == "name":
                manifest["regions"][0]["profile_name"] = "another"
            elif mutation == "index":
                manifest["regions"][0]["profile_index"] = True
            else:
                manifest["regions"].append(copy.deepcopy(manifest["regions"][0]))
            with self.subTest(mutation=mutation), self.assertRaises(ValueError):
                import_artifact(raw, manifest)
        raw, manifest = synthetic()
        raw["exporter"] = "py-spy@future"
        observations, _ = import_artifact(raw, manifest)
        self.assertTrue(all(row["quality"] == "exploratory" for row in observations))

    def test_zero_weights_and_empty_profiles_are_reportable(self):
        raw, manifest = synthetic()
        raw["profiles"][0].update(samples=[], weights=[], endValue=0)
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["samples"], [])
        self.assertEqual(observations[0]["quality"], "missing")
        self.assertEqual(observations[0]["metadata"]["functions"], [])

    def test_zero_weight_frames_and_float_overflow(self):
        raw, manifest = synthetic()
        raw["profiles"][0]["weights"] = [0, 0, 0]
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["quality"], "missing")
        raw["profiles"][0]["weights"] = [1e308, 1e308, 0]
        with self.assertRaises(ValueError):
            import_artifact(raw, manifest)

    def test_actual_py_spy_capture_and_offline_cli(self):
        raw_path = FIXTURE / "profile.json"
        raw = json.loads(raw_path.read_bytes())
        provenance = json.loads((FIXTURE / "provenance.json").read_text())
        self.assertEqual(hashlib.sha256(raw_path.read_bytes()).hexdigest(), provenance["raw_sha256"])
        self.assertTrue(provenance["real"])
        self.assertEqual(raw["exporter"], "py-spy@0.4.1")
        self.assertTrue(any(row["name"] == "leaf" for row in raw["shared"]["frames"]))
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.assertEqual(main(["speedscope-manifest", "--raw", str(raw_path), "--out", str(root / "manifest.json")]), 0)
            self.assertEqual(main(["import", "--provider", "py-spy-speedscope", "--raw", str(raw_path),
                                   "--manifest", str(root / "manifest.json"), "--out", str(root / "bundle")]), 0)
            self.assertEqual(main(["speedscope-summary", str(root / "bundle"), "--out", str(root / "report"), "--limit", "2"]), 0)
            report = summarize_profile(load_bundle(root / "bundle"))
            self.assertEqual(report["n"], 1)
            self.assertTrue(report["profiles"][0]["functions"])
            bundle = load_bundle(root / "bundle")
            comparison = compare(bundle, bundle, exploratory=True)
            self.assertTrue(all(row["classification"] == "inconclusive" for row in comparison["comparisons"]))
            self.assertTrue(all(row["absolute_change"] is None for row in comparison["comparisons"]))
            self.assertIn("sample", (root / "report/report.md").read_text())
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(["speedscope-summary", str(root / "bundle"), "--out", str(root / "report")]), 2)
                self.assertEqual(main(["speedscope-manifest", "--raw", str(raw_path), "--out", str(root / "manifest.json")]), 2)
                self.assertEqual(main(["speedscope-summary", str(root / "bundle"), "--out", str(root / "negative"), "--limit", "0"]), 2)

    def test_hash_binding_and_wrong_provider_remain_enforced(self):
        raw, manifest = synthetic()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "raw.json").write_text(json.dumps(raw), encoding="utf-8")
            (root / "manifest.json").write_text(json.dumps(manifest), encoding="utf-8")
            with self.assertRaises(ValueError):
                import_bundle("py-spy-speedscope", import_artifact, root / "raw.json", root / "manifest.json", root / "bad")
            self.assertFalse((root / "bad").exists())
        with self.assertRaises(ValueError):
            summarize_profile(dict(manifest=dict(producer=dict(provider="cprofile-json"))))


if __name__ == "__main__":
    unittest.main()
