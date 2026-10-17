"""Use CPU-specific cache inputs only for compiler arguments requesting native."""

import os
from pathlib import Path
import shlex
import subprocess
import sys


def uses_native(args, depth=0):
    if depth > 8:
        raise RuntimeError("Compiler response files are nested too deeply")
    for arg in args:
        if arg in {"-march=native", "-mtune=native", "-mcpu=native"}:
            return True
        if arg.startswith("@") and uses_native(
                shlex.split(Path(arg[1:]).read_text(encoding="utf-8")), depth + 1):
            return True
    return False


def cache_environment(args):
    env = dict(os.environ)
    if uses_native(args):
        fingerprint = env["WJR_CCACHE_NATIVE_FINGERPRINT"]
        if not Path(fingerprint).is_file():
            raise RuntimeError("Native CPU fingerprint is missing; refusing cached compilation")
        extra = env.get("CCACHE_EXTRAFILES")
        env["CCACHE_EXTRAFILES"] = extra + os.pathsep + fingerprint if extra else fingerprint
    return env


if __name__ == "__main__":
    raise SystemExit(subprocess.call(["ccache", *sys.argv[1:]], env=cache_environment(sys.argv[2:])))
