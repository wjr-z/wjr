from pathlib import Path
import hashlib
import json
import os
import subprocess
import urllib.request


source = Path.cwd()
root = Path(os.environ["RUNNER_TEMP"]) / "direct-readonly"
root.mkdir(exist_ok=True)
out = source / "json-direct-readonly-results"
out.mkdir(exist_ok=True)


def run(command, **kwargs):
    return subprocess.run(command, check=True, **kwargs)


cpu = root / "cpu_features"
run(["git", "init", str(cpu)])
run(["git", "-C", str(cpu), "fetch", "--depth", "1",
     "https://github.com/google/cpu_features.git",
     "d3b2440fcfc25fe8e6d0d4a85f06d68e98312f5b"])
run(["git", "-C", str(cpu), "checkout", "FETCH_HEAD"])
yy = root / "yyjson-0.10.0"
yy.mkdir(exist_ok=True)
hashes = {
    "yyjson.h": "adce02aef5dfd868f7c0b632bd23bc0e79fa710b70d93bfe334c2bd15b21a41d",
    "yyjson.c": "98489c068ab65298320710e5be4c1090af1b5676e41301a3525b2e732641aace",
}
for name, expected in hashes.items():
    url = "https://raw.githubusercontent.com/ibireme/yyjson/9ddba001a4ea88e93b46932e5c5b87b222e19a5f/src/" + name
    data = urllib.request.urlopen(url).read()
    assert hashlib.sha256(data).hexdigest() == expected
    (yy / name).write_bytes(data)

project = root / "project"
project.mkdir(exist_ok=True)
(project / "CMakeLists.txt").write_text(f'''cmake_minimum_required(VERSION 3.20)
project(json_direct_readonly_research LANGUAGES C CXX)
set(WJR_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_ASSEMBLY OFF CACHE BOOL "" FORCE)
set(WJR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WJR_CPU_TARGET baseline CACHE STRING "" FORCE)
set(FETCHCONTENT_SOURCE_DIR_CPU_FEATURES {cpu} CACHE PATH "" FORCE)
add_subdirectory({source / 'third-party/fmt'} fmt EXCLUDE_FROM_ALL)
add_subdirectory({source} wjr EXCLUDE_FROM_ALL)
add_library(simdjson_reference STATIC {source / 'examples/demo/src/simdjson.cpp'})
target_include_directories(simdjson_reference PUBLIC {source / 'examples/demo/src'})
add_library(yyjson_reference STATIC {yy / 'yyjson.c'})
target_include_directories(yyjson_reference PUBLIC {yy})
foreach(target IN ITEMS readonly_bench readonly_count)
  add_executable(${{target}} {source / '.github/research/json-direct-readonly.cpp'})
  target_link_libraries(${{target}} PRIVATE wjr::json simdjson_reference yyjson_reference)
  target_compile_definitions(${{target}} PRIVATE WJR_WITH_MATURE=1 WJR_DIRECT_RESERVE=1)
endforeach()
target_compile_definitions(readonly_count PRIVATE WJR_READONLY_COUNT=1)
''')

flags = "-march=x86-64-v3 -mpclmul"
build = root / "release"
run(["cmake", "-S", str(project), "-B", str(build), "-G", "Ninja",
     "-DCMAKE_CXX_COMPILER=clang++-18", "-DCMAKE_C_COMPILER=clang-18",
     "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
     "-DCMAKE_CXX_FLAGS=" + flags, "-DCMAKE_C_FLAGS=" + flags])
run(["cmake", "--build", str(build), "--target", "readonly_bench", "readonly_count",
     "--parallel", "4"])
(out / "compile_commands.json").write_bytes((build / "compile_commands.json").read_bytes())
run([str(build / "readonly_bench")])
with (out / "counts.csv").open("w") as stream:
    run([str(build / "readonly_count"), "count"], stdout=stream)
affinity = str(min(os.sched_getaffinity(0)))
for mode in ("forward", "reverse"):
    with (out / (mode + ".csv")).open("w") as stream:
        run(["taskset", "-c", affinity, str(build / "readonly_bench"), "bench"] +
            (["reverse"] if mode == "reverse" else []), stdout=stream)

san = root / "sanitize"
san_flags = flags + " -fsanitize=address,undefined -fno-sanitize=builtin"
run(["cmake", "-S", str(project), "-B", str(san), "-G", "Ninja",
     "-DCMAKE_CXX_COMPILER=clang++-18", "-DCMAKE_C_COMPILER=clang-18",
     "-DCMAKE_BUILD_TYPE=Release", "-DWJR_ENABLE_ASAN=ON",
     "-DCMAKE_CXX_FLAGS=" + san_flags, "-DCMAKE_C_FLAGS=" + san_flags])
run(["cmake", "--build", str(san), "--target", "readonly_bench", "--parallel", "4"])
environment = os.environ.copy()
environment["ASAN_OPTIONS"] = "detect_leaks=1:halt_on_error=1"
environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
with (out / "sanitizer.log").open("w") as stream:
    run([str(san / "readonly_bench")], env=environment, stdout=stream, stderr=subprocess.STDOUT)

metadata = {"affinity": affinity, "flags": flags, "yyjson_release": "0.10.0",
            "yyjson_commit": "9ddba001a4ea88e93b46932e5c5b87b222e19a5f",
            "yyjson_sha256": hashes, "simdjson_version": "3.9.1", "simdjson_isa": "haswell"}
for name in ("simdjson.hpp", "simdjson.cpp"):
    metadata[name + "_sha256"] = hashlib.sha256((source / "examples/demo/src" / name).read_bytes()).hexdigest()
(out / "metadata.json").write_text(json.dumps(metadata, indent=2))
