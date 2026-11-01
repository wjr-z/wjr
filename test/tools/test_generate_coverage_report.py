"""Check LCOV path normalization without building the C++ test binary."""

import importlib.util
from pathlib import Path
import tempfile
import unittest


SCRIPT = Path(__file__).resolve().parents[2] / "tools/generate_coverage_report.py"
SPEC = importlib.util.spec_from_file_location("coverage_report", SCRIPT)
coverage_report = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(coverage_report)


class NormalizeLcovTest(unittest.TestCase):
    def test_retains_coverage_and_uses_repository_relative_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "include/wjr/example.hpp"
            report = f"SF:{source}\r\nDA:7,3\r\nBRDA:7,0,0,2\r\nend_of_record\r\n"
            self.assertEqual(
                coverage_report.normalize_lcov(report, root),
                "SF:include/wjr/example.hpp\nDA:7,3\nBRDA:7,0,0,2\nend_of_record\n",
            )

    def test_omits_external_files_and_parent_traversal(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "repo"
            report = (
                f"SF:{root.parent / 'external.hpp'}\nDA:1,1\nend_of_record\n"
                "SF:../external.hpp\nDA:1,1\nend_of_record\n"
                "SF:src/example.cpp\nDA:2,0\nend_of_record\n"
            )
            self.assertEqual(
                coverage_report.normalize_lcov(report, root),
                "SF:src/example.cpp\nDA:2,0\nend_of_record\n",
            )

    def test_rejects_empty_repository_report(self):
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaisesRegex(ValueError, "repository source files"):
                coverage_report.normalize_lcov("", Path(directory))


if __name__ == "__main__":
    unittest.main()
