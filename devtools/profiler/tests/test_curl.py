"""Real owned-localhost curl output; malformed cases are explicitly synthetic mutations."""

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
from wjr_profiler.adapters.curl_json import decode_raw, import_artifact
from wjr_profiler.bundle import import_bundle, load_bundle
from wjr_profiler.compare import compare
from wjr_profiler.curl_report import summarize_requests
from wjr_profiler.model import summarize

FIXTURE = ROOT / "tests/fixtures/curl"


def fixture(name="normal"):
    return (decode_raw((FIXTURE / (name + ".jsonl")).read_bytes()),
            json.loads((FIXTURE / (name + ".manifest.json")).read_text(encoding="utf-8")))


class CurlTests(unittest.TestCase):
    def test_real_provenance_bytes_processes_and_delayed_phases(self):
        provenance = json.loads((FIXTURE / "provenance.json").read_text(encoding="utf-8"))
        self.assertTrue(provenance["real"])
        self.assertTrue(provenance["curl_version"].startswith("curl 8.11.1 "))
        for capture in provenance["captures"]:
            self.assertEqual(capture["raw_sha256"], hashlib.sha256((FIXTURE / (capture["name"] + ".jsonl")).read_bytes()).hexdigest())
        raw, manifest = fixture()
        observations, _ = import_artifact(raw, manifest)
        by_name = {row["metric"]: row for row in observations}
        self.assertEqual(by_name["total"]["quality"], "ok")
        self.assertEqual(len(by_name["total"]["samples"]), 6)
        self.assertEqual(by_name["downloaded_bytes"]["samples"], [128] * 6)
        self.assertTrue(all(x > 10e6 for x in by_name["first_byte_wait"]["samples"]))
        self.assertTrue(all(x > 20e6 for x in by_name["body_transfer"]["samples"]))
        for index in range(6):
            pieces = sum(by_name[name]["samples"][index] for name in
                         ("dns", "tcp_connect", "protocol_setup", "first_byte_wait", "body_transfer"))
            self.assertAlmostEqual(pieces, by_name["total"]["samples"][index])
        self.assertEqual(by_name["tls"]["samples"], [])
        self.assertIn("not_applicable", by_name["tls"]["missing_reason"])

    def test_single_request_does_not_manufacture_variance(self):
        raw, manifest = fixture()
        manifest["regions"][0]["request_lines"] = [1]
        observations, diagnostics = import_artifact(raw, manifest)
        total = next(row for row in observations if row["metric"] == "total")
        self.assertEqual(total["quality"], "insufficient_samples")
        self.assertEqual(summarize(total["samples"])["n"], 1)
        self.assertIsNone(summarize(total["samples"])["variance"])
        self.assertTrue(any(row["code"] == "unselected_requests" for row in diagnostics))

    def test_real_partial_and_redirect_preserve_failure_without_phase_claim(self):
        for name in ("partial", "redirect"):
            with self.subTest(name=name):
                raw, manifest = fixture(name)
                observations, diagnostics = import_artifact(raw, manifest)
                self.assertTrue(all(not row["samples"] for row in observations))
                record = observations[0]["metadata"]["requests"][0]
                self.assertEqual(record["quality"], "failed")
                self.assertEqual(record["phases_ns"], {})
                self.assertTrue(diagnostics)
                self.assertEqual(record["downloaded_bytes"], 64 if name == "partial" else 128)

    def test_synthetic_unsupported_modes_never_subtract_phases(self):
        mutations = {"num_redirects": 1, "num_retries": 1, "proxy_used": 1,
                     "num_connects": 0, "http_version": "2", "scheme": "https",
                     "method": "POST", "size_upload": 1, "time_appconnect": .001}
        for field, value in mutations.items():
            with self.subTest(field=field):
                raw, manifest = fixture()
                raw[1][field] = value
                observations, diagnostics = import_artifact(raw, manifest)
                self.assertTrue(all(not row["samples"] for row in observations))
                self.assertIn(field, " ".join(row["message"] for row in diagnostics))

    def test_missing_nonfinite_negative_nonmonotonic_timestamps(self):
        for value in (None, -1, float("nan"), float("inf"), True, "0.1", 1e308):
            with self.subTest(value=value):
                raw, manifest = fixture()
                raw[1]["time_connect"] = value
                observations, diagnostics = import_artifact(raw, manifest)
                self.assertFalse(observations[0]["samples"])
                self.assertTrue(diagnostics)
                json.dumps(observations, allow_nan=False)

    def test_process_provenance_cannot_be_replaced_by_transfer_exit(self):
        raw, manifest = fixture()
        manifest["producer"]["processes"][0]["exit_status"] = 9
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["quality"], "failed")
        self.assertEqual(observations[0]["samples"], [])
        manifest["producer"]["processes"] = []
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["quality"], "failed")

    def test_unknown_versions_remain_exploratory(self):
        raw, manifest = fixture()
        manifest["producer"]["version"] = "8.12.0"
        observations, _ = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["quality"], "exploratory")
        self.assertEqual(len(observations[0]["samples"]), 6)

    def test_invalid_explicit_contract_and_line_selection(self):
        for field, value in (("process_per_request", 1), ("retries", True), ("proxy", True)):
            raw, manifest = fixture()
            manifest["producer"]["curl"][field] = value
            with self.assertRaises(ValueError):
                import_artifact(raw, manifest)
        raw, manifest = fixture()
        manifest["regions"][0]["request_lines"] = [1, 1]
        with self.assertRaisesRegex(ValueError, "duplicate"):
            import_artifact(raw, manifest)
        manifest["regions"][0]["request_lines"] = [999]
        observations, diagnostics = import_artifact(raw, manifest)
        self.assertEqual(observations[0]["quality"], "missing")
        self.assertIn("absent", diagnostics[0]["message"])

    def test_jsonl_schema_and_physical_lines(self):
        raw, _ = fixture()
        text = "\n" + json.dumps(raw[1]) + "\n"
        self.assertEqual(set(decode_raw(text.encode())), {2})
        for content in (b"", b"[]", b"null", b"{", b"{}\ninvalid"):
            with self.subTest(content=content), self.assertRaises(ValueError):
                decode_raw(content)

    def test_cli_offline_import_summary_integrity_and_existing_providers(self):
        self.assertTrue({"google-benchmark", "llvm-mca", "cprofile-json", "curl-json"} <= PROVIDERS.keys())
        with tempfile.TemporaryDirectory() as temporary:
            bundle_path, report_path = Path(temporary) / "bundle", Path(temporary) / "report"
            args = ["import", "--provider", "curl-json", "--raw", str(FIXTURE / "normal.jsonl"),
                    "--manifest", str(FIXTURE / "normal.manifest.json"), "--out", str(bundle_path)]
            self.assertEqual(main(args), 0)
            self.assertEqual((bundle_path / "raw/input.json").read_bytes(), (FIXTURE / "normal.jsonl").read_bytes())
            self.assertEqual(main(["curl-summary", str(bundle_path), "--out", str(report_path)]), 0)
            report = json.loads((report_path / "report.json").read_text())
            self.assertEqual(report["regions"][0]["statistics"]["total"]["n"], 6)
            self.assertIn("not server CPU", (report_path / "report.md").read_text())
            self.assertIn("not_applicable", (report_path / "report.md").read_text())
            before = load_bundle(bundle_path)
            after = copy.deepcopy(before)
            for row in after["normalized"]["observations"]:
                row["metadata"]["curl_versions"] = ["libcurl/8.11.1 another_backend"]
            compared = compare(before, after, exploratory=True)
            self.assertTrue(all(row["absolute_change"] is None for row in compared["comparisons"]))
            self.assertTrue(any("backend" in reason for row in compared["comparisons"] for reason in row["reasons"]))
            with contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(main(args), 2)
            (bundle_path / "raw/input.json").write_bytes(b"changed")
            with self.assertRaisesRegex(ValueError, "SHA-256"):
                load_bundle(bundle_path)


if __name__ == "__main__":
    unittest.main()
