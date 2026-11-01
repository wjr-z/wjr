"""Check LCOV path normalization without building the C++ test binary."""

import importlib.util
from argparse import Namespace
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import re


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


class CoverageSourcesTest(unittest.TestCase):
    def test_checkout_name_does_not_exclude_ordinary_sources(self):
        with tempfile.TemporaryDirectory() as directory:
            for name in ("benchmark-name", "build", "tests", "simd-sse", "repo [a]+(b)"):
                root = Path(directory) / name
                pattern = coverage_report.default_ignore_regex(root)
                source = root / "include/wjr/math/example.hpp"
                for path in (source.as_posix(), str(source).replace("/", "\\")):
                    self.assertIsNone(re.search(pattern, path), (name, path))

    def test_preserves_legacy_simd_exclusions_inside_source_root(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "benchmark-name"
            pattern = coverage_report.default_ignore_regex(root)
            for name in ("sse.hpp", "avx.hpp", "simd_cast.hpp"):
                source = root / "include/wjr/arch/x86/simd" / name
                for path in (source.as_posix(), str(source).replace("/", "\\")):
                    self.assertIsNotNone(re.search(pattern, path), path)

    def test_all_report_formats_share_include_and_src_selection(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            args = Namespace(llvm_cov="llvm-cov", binary="tests", profile="profile",
                             source_root=root, ignore_regex=None,
                             output_dir=directory, details_dir="details")
            with patch.object(coverage_report.subprocess, "run") as run:
                run.return_value.stdout = "SF:include/wjr/example.hpp\nDA:1,1\nend_of_record\n"
                coverage_report.write_html(args)
                coverage_report.run_report(args)
                coverage_report.write_lcov(args)
            commands = [call.args[0] for call in run.call_args_list]
            self.assertEqual([command[1] for command in commands], ["show", "report", "export"])
            for command in commands:
                self.assertEqual(command[-2:], [str(root / "include"), str(root / "src")])
                self.assertIn("-ignore-filename-regex=" +
                              coverage_report.default_ignore_regex(root), command)

    def test_custom_llvm_ignore_regex_is_preserved(self):
        args = Namespace(llvm_cov="llvm-cov", binary="tests", profile="profile",
                         source_root=Path.cwd(), ignore_regex="custom-pattern")
        self.assertIn("-ignore-filename-regex=custom-pattern",
                      coverage_report.coverage_command(args, "report"))


if __name__ == "__main__":
    unittest.main()
