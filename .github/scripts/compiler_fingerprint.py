"""Fingerprint the compiler's native target before restoring compiled objects."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys


def run(compiler, *args):
    return subprocess.run(
        [compiler, *args], input="", text=True, encoding="utf-8", capture_output=True,
        check=True, timeout=30, env={**os.environ, "LC_ALL": "C"},
    )


def frontend(compiler, family, language, *flags):
    result = run(compiler, "-###", "-O3", *flags, "-fsyntax-only", "-x", language, "-")
    commands = []
    for line in result.stderr.splitlines():
        if not line.startswith(" "):
            continue
        args = shlex.split(line)
        if args and ((family == "clang" and "-cc1" in args) or
                     (family == "gcc" and Path(args[0]).name in
                      {"cc1", "cc1plus", "cc1.exe", "cc1plus.exe"})):
            commands.append(args)
    if len(commands) != 1:
        raise RuntimeError(f"Cannot identify one {family} frontend command for {compiler}")
    return commands[0]


def target_value(args, option):
    if option.startswith("-m"):
        values = [arg.split("=", 1)[1] for arg in args if arg.startswith(option + "=")]
    else:
        values = [args[i + 1] for i, arg in enumerate(args[:-1]) if arg == option]
    if len(values) != 1 or not values[0] or values[0] == "native":
        raise RuntimeError(f"Unresolved {option}: refusing an ambiguous compiler cache key")
    return values[0]


def identify(program, family, language):
    compiler = shutil.which(program)
    if compiler is None:
        raise RuntimeError(f"Compiler not found: {program}")
    identity = {
        "path": str(Path(compiler).resolve()),
        "version": run(compiler, "--version").stdout.strip(),
        "target": run(compiler, "-dumpmachine").stdout.strip(),
    }
    if not identity["version"] or not identity["target"]:
        raise RuntimeError(f"Incomplete compiler identity: {program}")
    # Keep the complete frontend arguments, including GCC cache-tuning --param
    # values. A syntax-only probe avoids randomized assembler/output filenames.
    args = frontend(compiler, family, language, "-march=native")
    cpu_option, tune_option = (("-target-cpu", "-tune-cpu") if family == "clang"
                               else ("-march", "-mtune"))
    resolved_cpu = target_value(args, cpu_option)
    if family == "clang":
        has_features = "-target-feature" in args
    else:
        has_features = any(arg.startswith("-mno-") for arg in args)
    if not has_features:
        raise RuntimeError(f"Native features were not expanded by {program}")
    # Clang may omit its implicit tuning CPU. Record a separate explicit native
    # tuning probe as well; these probe flags do not change the real build flags.
    tuning = frontend(compiler, family, language, "-march=native", "-mtune=native")
    resolved_tune = target_value(tuning, tune_option)
    identity.update(native=args, native_tuning=tuning)
    print(f"{language}: {family} native CPU={resolved_cpu}, tune={resolved_tune}", file=sys.stderr)
    return identity


def toolchain_hash(document):
    identity = {language: {key: document[language][key] for key in ("path", "version", "target")}
                for language in ("c", "cxx")}
    return hashlib.sha256(json.dumps(identity, sort_keys=True).encode("utf-8")).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--family", choices=("gcc", "clang"), required=True)
    parser.add_argument("--cc", required=True)
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    document = {
        "schema": 1,
        "family": args.family,
        "c": identify(args.cc, args.family, "c"),
        "cxx": identify(args.cxx, args.family, "c++"),
    }
    contents = (json.dumps(document, sort_keys=True, indent=2) + "\n").encode("utf-8")
    args.output.write_bytes(contents)
    print(f"toolchain={toolchain_hash(document)}")
    print(f"native={hashlib.sha256(contents).hexdigest()}")


if __name__ == "__main__":
    main()
