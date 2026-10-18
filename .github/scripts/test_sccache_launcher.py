import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import sccache_launcher as launcher


class SccacheLauncherTest(unittest.TestCase):
    def test_baseline_preserves_user_cache_identity(self):
        original = {"SCCACHE_C_CUSTOM_CACHE_BUSTER": "user-identity"}
        with patch.dict(os.environ, original, clear=True):
            self.assertEqual(launcher.cache_environment(["/O2", "/arch:AVX2"]), original)
            self.assertEqual(launcher.cache_environment(["-march=x86-64"]), original)

    def test_native_composes_identity_without_changing_parent_environment(self):
        with tempfile.TemporaryDirectory() as directory:
            fingerprint = Path(directory) / "cpu.json"
            fingerprint.write_text('{"cpu": "first"}', encoding="utf-8")
            original = {"WJR_CCACHE_NATIVE_FINGERPRINT": str(fingerprint),
                        "SCCACHE_C_CUSTOM_CACHE_BUSTER": "user-identity"}
            with patch.dict(os.environ, original, clear=True):
                first = launcher.cache_environment(["-march=native"])
                fingerprint.write_text('{"cpu": "second"}', encoding="utf-8")
                second = launcher.cache_environment(["-march=native"])
                self.assertNotEqual(first, second)
                self.assertTrue(first["SCCACHE_C_CUSTOM_CACHE_BUSTER"].startswith(
                    "user-identity:wjr-native:"))
                self.assertEqual(dict(os.environ), original)

    def test_response_files_use_existing_native_detection(self):
        with tempfile.TemporaryDirectory() as directory:
            flags = Path(directory) / "flags.rsp"
            flags.write_text('-march=native -I"path with spaces"', encoding="utf-8")
            with patch.dict(os.environ, {}, clear=True):
                with self.assertRaises(KeyError):
                    launcher.cache_environment(["@" + str(flags)])

    def test_missing_native_fingerprint_does_not_fall_back_to_shared_key(self):
        with patch.dict(os.environ, {"WJR_CCACHE_NATIVE_FINGERPRINT": "missing-cpu.json"},
                        clear=True):
            with self.assertRaises(FileNotFoundError):
                launcher.cache_environment(["-mtune=native"])


if __name__ == "__main__":
    unittest.main()
