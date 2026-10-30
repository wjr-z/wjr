"""Real CPython fixture plus isolated malformed JSON cases; no foreign marshal fixtures."""

import contextlib
import copy
import cProfile
import hashlib
import io
import json
from pathlib import Path
import platform
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from wjr_profiler.__main__ import main
from wjr_profiler.adapters.cprofile_json import import_artifact
from wjr_profiler.bundle import import_bundle, load_bundle
from wjr_profiler.cprofile_report import summarize_profile
from wjr_profiler.exporters.cprofile_json import default_manifest, export_profile
from wjr_profiler.model import summarize

FIXTURE = ROOT / "tests/fixtures/cprofile"


def fixture():
    raw = json.loads((FIXTURE / "profile.json").read_text())
    manifest = json.loads((FIXTURE / "profile.manifest.json").read_text())
    return raw, manifest


class CProfileTests(unittest.TestCase):
    def test_real_identity_recursion_edges_and_unrounded_time(self):
        raw, manifest = fixture()
        provenance = json.loads((FIXTURE / "provenance.json").read_text())
        self.assertTrue(provenance["real"])
        self.assertEqual(provenance["raw_sha256"], hashlib.sha256((FIXTURE / "profile.json").read_bytes()).hexdigest())
        duplicates = [row for row in raw["functions"] if row["name"] == "duplicate"]
        self.assertEqual(len(duplicates), 2)
        self.assertNotEqual(duplicates[0]["file"], duplicates[1]["file"])
        recursive = next(row for row in raw["functions"] if row["name"] == "recurse")
        self.assertEqual((recursive["primitive_calls"], recursive["total_calls"]), (7, 35))
        edge = next(caller for caller in recursive["callers"] if caller["name"] == "recurse")
        self.assertEqual((edge["primitive_calls"], edge["total_calls"]), (7, 28))
        tiny = next(row for row in raw["functions"] if row["name"] == "tiny")
        self.assertGreater(tiny["self_s"], 0)
        self.assertLess(tiny["self_s"], 0.0005)
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(len(observations), len(raw["functions"]) * 4)
        self.assertTrue(all(len(row["samples"]) == 1 for row in observations))
        self.assertTrue(all(row["quality"] == "insufficient_samples" for row in observations))

    def test_calls_never_become_statistical_n(self):
        raw, manifest = fixture()
        observations, _ = import_artifact(raw, manifest)
        calls = next(row for row in observations if row["metric"] == "total_calls" and row["samples"] == [35])
        self.assertEqual(summarize(calls["samples"])["n"], 1)
        self.assertIsNone(summarize(calls["samples"])["variance"])
        self.assertEqual(calls["metadata"]["count_kind"], "exact_instrumented")

    def test_only_explicit_file_line_name_mapping(self):
        raw, manifest = fixture()
        manifest["regions"][0]["function"]["line"] += 1
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["samples"], [])
        self.assertEqual(observations[0]["quality"], "missing")
        self.assertIn("not guessed", observations[0]["missing_reason"])

    def test_unknown_versions_and_timer_are_exploratory(self):
        for field, value in (("version", "3.12.8"), ("timer", "unknown"), ("thread_coverage", "unknown")):
            with self.subTest(field=field):
                raw, manifest = fixture()
                raw["producer"][field] = value
                observations, _ = import_artifact(raw, manifest)
                self.assertTrue(all(row["quality"] == "exploratory" for row in observations))

    def test_bad_schema_identity_counts_and_nonfinite_are_rejected(self):
        for mutation in ("schema", "duplicate", "bool", "negative", "nonfinite"):
            with self.subTest(mutation=mutation):
                raw, manifest = fixture()
                if mutation == "schema":
                    raw["schema"] = "wjr.cprofile-json/2"
                elif mutation == "duplicate":
                    raw["functions"].append(copy.deepcopy(raw["functions"][0]))
                elif mutation == "bool":
                    raw["functions"][0]["total_calls"] = True
                elif mutation == "negative":
                    raw["functions"][0]["self_s"] = -1
                else:
                    raw["functions"][0]["self_s"] = float("nan")
                with self.assertRaises(ValueError):
                    import_artifact(raw, manifest)

    def test_json_import_and_hotspot_cli_are_offline(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.assertEqual(main(["import", "--provider", "cprofile-json", "--raw", str(FIXTURE / "profile.json"),
                                   "--manifest", str(FIXTURE / "profile.manifest.json"), "--out", str(root / "bundle")]), 0)
            self.assertEqual(main(["cprofile-summary", str(root / "bundle"), "--out", str(root / "report")]), 0)
            report = json.loads((root / "report/report.json").read_text())
            self.assertEqual(len(report["functions"]), 6)
            self.assertEqual(report["producer"]["version"], "3.12.7")
            self.assertEqual(report["producer"]["time_basis"], "wall")
            self.assertEqual(report["n"], 1)
            self.assertEqual(sum(row["function"]["name"] == "duplicate" for row in report["functions"]), 2)
            self.assertNotIn("total_cumulative_time", report)
            values = [row["values"]["self_time"] for row in report["functions"]]
            self.assertEqual(values, sorted(values, reverse=True))
            text = (root / "report/report.md").read_text()
            self.assertIn("never summed", text)
            self.assertIn("no independent between-run variance", text)
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(["cprofile-summary", str(root / "bundle"), "--out", str(root / "report")]), 2)

    def test_exporter_requires_exact_declared_producer(self):
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaises(ValueError):
                export_profile("absent.prof", Path(directory) / "out.json", "3.11.0")

    @unittest.skipUnless(platform.python_implementation() == "CPython" and platform.python_version() == "3.12.7",
                         "producer-side export is verified only on CPython 3.12.7")
    def test_own_native_profile_roundtrip_and_exclusive_outputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            profiler = cProfile.Profile()
            profiler.runcall(sum, [1, 2, 3])
            profiler.dump_stats(str(root / "own.prof"))
            document = export_profile(root / "own.prof", root / "own.json", "3.12.7",
                                      "cProfile default real-time timer", "wall", "current thread",
                                      root / "own.manifest.json")
            self.assertEqual(document["producer"]["version"], "3.12.7")
            import_bundle("cprofile-json", import_artifact, root / "own.json", root / "own.manifest.json", root / "bundle")
            report = summarize_profile(load_bundle(root / "bundle"))
            self.assertTrue(report["functions"])
            with self.assertRaises(ValueError):
                export_profile(root / "own.prof", root / "own.json", "3.12.7")
            with self.assertRaises(ValueError):
                export_profile(root / "own.prof", root / "same.json", "3.12.7", manifest_output=root / "same.json")

    def test_default_manifest_has_explicit_functions_unknown_environment(self):
        raw, _ = fixture()
        manifest = default_manifest(raw, "0" * 64)
        self.assertEqual(manifest["host"], {})
        self.assertEqual(len(manifest["regions"]), len(raw["functions"]))
        self.assertEqual(len({row["id"] for row in manifest["regions"]}), len(raw["functions"]))


if __name__ == "__main__":
    unittest.main()
