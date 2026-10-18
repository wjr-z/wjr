"""Keep native CPU identity out of portable sccache object keys."""

import hashlib
import os
from pathlib import Path
import subprocess
import sys

from ccache_launcher import uses_native


def cache_environment(args):
    env = dict(os.environ)
    if uses_native(args):
        fingerprint = Path(env["WJR_CCACHE_NATIVE_FINGERPRINT"])
        identity = hashlib.sha256(fingerprint.read_bytes()).hexdigest()
        previous = env.get("SCCACHE_C_CUSTOM_CACHE_BUSTER", "")
        env["SCCACHE_C_CUSTOM_CACHE_BUSTER"] = previous + ":wjr-native:" + identity
    return env


if __name__ == "__main__":
    raise SystemExit(subprocess.call(["sccache", *sys.argv[1:]],
                                    env=cache_environment(sys.argv[2:])))
