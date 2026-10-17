import types
import unittest
from unittest.mock import patch

import compiler_fingerprint as fingerprint
import ccache_launcher as launcher


class CompilerFingerprintTest(unittest.TestCase):
    def test_frontend_retains_all_gcc_tuning_arguments(self):
        command = (' /usr/lib/gcc/cc1plus -march=alderlake -mavx2 -mno-avx512f '
                   '--param "l1-cache-size=48" -mtune=alderlake -fsyntax-only')
        with patch.object(fingerprint, "run", return_value=types.SimpleNamespace(stderr=command)):
            args = fingerprint.frontend("g++", "gcc", "c++", "-march=native")
        self.assertIn("--param", args)
        self.assertIn("l1-cache-size=48", args)
        self.assertEqual(fingerprint.target_value(args, "-march"), "alderlake")
        self.assertEqual(fingerprint.target_value(args, "-mtune"), "alderlake")

    def test_ambiguous_or_missing_frontend_is_rejected(self):
        for output in ("not a frontend", ' clang -cc1\n clang -cc1'):
            with self.subTest(output=output), patch.object(
                    fingerprint, "run", return_value=types.SimpleNamespace(stderr=output)):
                with self.assertRaises(RuntimeError):
                    fingerprint.frontend("clang++", "clang", "c++", "-march=native")

    def test_missing_native_cpu_features_or_tuning_is_rejected(self):
        actual = ["clang", "-cc1", "-target-cpu", "alderlake", "-target-feature", "+avx2"]
        tuning = [*actual, "-tune-cpu", "alderlake"]
        for native, tuned in ((actual[:2], tuning),
                              (["clang", "-cc1", "-target-cpu", "native"], tuning),
                              (actual[:4], tuning), (actual, actual)):
            with self.subTest(native=native, tuned=tuned), \
                    patch.object(fingerprint.shutil, "which", return_value="/usr/bin/clang"), \
                    patch.object(fingerprint, "run", return_value=types.SimpleNamespace(stdout="id")), \
                    patch.object(fingerprint, "frontend", side_effect=[native, tuned]):
                with self.assertRaises(RuntimeError):
                    fingerprint.identify("clang", "clang", "c")

    def test_clang_implicit_and_explicit_tuning_are_both_recorded(self):
        actual = ["clang", "-cc1", "-target-cpu", "alderlake", "-target-feature", "+avx2"]
        tuning = [*actual, "-tune-cpu", "alderlake"]
        with patch.object(fingerprint.shutil, "which", return_value="/usr/bin/clang"), \
                patch.object(fingerprint, "run", return_value=types.SimpleNamespace(stdout="id")), \
                patch.object(fingerprint, "frontend", side_effect=[actual, tuning]):
            identity = fingerprint.identify("clang", "clang", "c")
        self.assertEqual(identity["native"], actual)
        self.assertEqual(identity["native_tuning"], tuning)

    def test_cache_pool_identity_does_not_depend_on_host_cpu(self):
        compiler = dict(path="/usr/bin/clang", version="18", target="x86_64-linux-gnu", native=["cpu1"])
        document = dict(c=dict(compiler), cxx=dict(compiler))
        original = fingerprint.toolchain_hash(document)
        document["c"]["native"] = ["cpu2"]
        document["cxx"]["native"] = ["cpu2"]
        self.assertEqual(fingerprint.toolchain_hash(document), original)
        document["cxx"]["version"] = "19"
        self.assertNotEqual(fingerprint.toolchain_hash(document), original)

    def test_native_selection_uses_flags_and_response_files(self):
        self.assertFalse(launcher.uses_native(["-march=x86-64", "-Dnative=1"]))
        for flag in ("-march=native", "-mtune=native", "-mcpu=native"):
            self.assertTrue(launcher.uses_native([flag]))
            with patch.object(launcher.Path, "read_text", return_value=flag):
                self.assertTrue(launcher.uses_native(["@flags.rsp"]))
        with patch.object(launcher.Path, "read_text", return_value="@flags.rsp"):
            with self.assertRaises(RuntimeError):
                launcher.uses_native(["@flags.rsp"])

    def test_only_native_invocations_add_cpu_fingerprint(self):
        original = dict(WJR_CCACHE_NATIVE_FINGERPRINT="cpu.json", CCACHE_EXTRAFILES="other.json")
        with patch.dict(launcher.os.environ, original, clear=True), \
                patch.object(launcher.Path, "is_file", return_value=True):
            self.assertEqual(launcher.cache_environment(["-march=x86-64"]), original)
            native = launcher.cache_environment(["-march=native"])
            self.assertEqual(native["CCACHE_EXTRAFILES"], "other.json" + launcher.os.pathsep + "cpu.json")
        with patch.dict(launcher.os.environ, original, clear=True), \
                patch.object(launcher.Path, "is_file", return_value=False):
            with self.assertRaises(RuntimeError):
                launcher.cache_environment(["-march=native"])


if __name__ == "__main__":
    unittest.main()
