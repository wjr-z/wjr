"""Validate the installed optional component against the reviewed native release."""
from pathlib import Path
import io
import json
import os
import subprocess
import tarfile
import urllib.request


source = Path(__file__).resolve().parents[3]
root = Path(os.environ["RUNNER_TEMP"]) / "wjr-yyjson-validation"
root.mkdir(exist_ok=True)
artifacts = source / "yyjson-validation-results"
artifacts.mkdir(exist_ok=True)
commit = "6447536015f3d600f3d65323b10976103b337ca7"
archive = urllib.request.urlopen("https://api.github.com/repos/ibireme/yyjson/tarball/" + commit).read()
with tarfile.open(fileobj=io.BytesIO(archive), mode="r:gz") as package:
    for member in package.getmembers():
        assert (root / member.name).resolve().is_relative_to(root.resolve())
    package.extractall(root, filter="data")
provider_source = next(root.glob("ibireme-yyjson-*"))


def run(command, **kwargs):
    print("+", " ".join(map(str, command)), flush=True)
    return subprocess.run(list(map(str, command)), check=True, **kwargs)


deps = {}
for name, url, revision in [
    ("cpu_features", "https://github.com/google/cpu_features.git",
     "d3b2440fcfc25fe8e6d0d4a85f06d68e98312f5b"),
    ("googletest", "https://github.com/google/googletest.git",
     "52eb8108c5bdec04579160ae17225d66034bd723"),
]:
    directory = root / name
    run(["git", "init", directory])
    run(["git", "-C", directory, "fetch", "--depth", "1", url, revision])
    run(["git", "-C", directory, "checkout", "FETCH_HEAD"])
    deps[name] = directory

for name, cc, cxx, sanitize in [("gcc11", "gcc-11", "g++-11", False),
                                 ("clang18-asan", "clang-18", "clang++-18", True)]:
    profile = root / name
    native_build, native_prefix = profile / "native-build", profile / "native"
    build, prefix = profile / "build", profile / "install"
    flags = "-fsanitize=address,undefined" if sanitize else ""
    common = ["-G", "Ninja", "-DCMAKE_BUILD_TYPE=Release",
              "-DCMAKE_C_COMPILER=" + cc, "-DCMAKE_CXX_COMPILER=" + cxx,
              "-DCMAKE_C_FLAGS=" + flags, "-DCMAKE_CXX_FLAGS=" + flags]
    run(["cmake", "-S", provider_source, "-B", native_build, *common,
         "-DCMAKE_INSTALL_PREFIX=" + str(native_prefix), "-DBUILD_SHARED_LIBS=OFF",
         "-DYYJSON_INSTALL=ON", "-DYYJSON_BUILD_TESTS=OFF"])
    run(["cmake", "--build", native_build, "--target", "install", "--parallel", "4"])
    overrides = ["-DFETCHCONTENT_SOURCE_DIR_CPU_FEATURES=" + str(deps["cpu_features"]),
                 "-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=" + str(deps["googletest"])]
    run(["cmake", "-S", source, "-B", build, *common, *overrides,
         "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", "-DWJR_CPU_TARGET=baseline",
         "-DWJR_ENABLE_ASSEMBLY=OFF", "-DWJR_BUILD_TESTS=ON", "-DWJR_USE_GMP_TESTS=OFF",
         "-DWJR_ENABLE_YYJSON=ON", "-DWJR_ENABLE_ASAN=" + ("ON" if sanitize else "OFF"),
         "-DCMAKE_PREFIX_PATH=" + str(native_prefix), "-DCMAKE_INSTALL_PREFIX=" + str(prefix)])
    run(["cmake", "--build", build, "--target", "yyjson_readonly_tests", "wjr-json", "fmt",
         "--parallel", "4"])
    environment = os.environ.copy()
    environment["ASAN_OPTIONS"] = "detect_leaks=1:halt_on_error=1"
    environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
    run(["ctest", "--test-dir", build, "-R", "^yyjson_readonly_tests$", "--output-on-failure"],
        env=environment)
    run(["cmake", "--install", build])
    consumer = profile / "consumer"
    run(["cmake", "-S", source / "test/consumer", "-B", consumer, *common,
         "-DCMAKE_PREFIX_PATH=" + str(prefix) + ";" + str(native_prefix), "-DWJR_TEST_YYJSON=ON"])
    run(["cmake", "--build", consumer, "--target", "wjr_yyjson_consumer", "--parallel", "2"])
    run(["ctest", "--test-dir", consumer, "-R", "^wjr_yyjson_", "--output-on-failure"],
        env=environment)
    with (artifacts / (name + "-twitter.txt")).open("w") as stream:
        run([consumer / "wjr_yyjson_consumer", source / "test/data/success/twitter.json"],
            env=environment, stdout=stream)
    output = artifacts / (name + "-codegen.o")
    run([cxx, "-std=c++20", "-O3", "-DNDEBUG", "-DWJR_DEBUG_LEVEL=0", "-DWJR_YYJSON_013=1",
         "-ffunction-sections", *(["-fno-ipa-icf"] if name == "gcc11" else []),
         "-I" + str(source / "include"), "-I" + str(native_prefix / "include"),
         "-I" + str(source / "third-party/fmt/include"), "-c",
         source / "docs/design/references/yyjson-codegen.cpp", "-o", output])
    (artifacts / (name + "-compile_commands.json")).write_bytes((build / "compile_commands.json").read_bytes())
    trap = profile / "no-provider"
    trap.mkdir()
    (trap / "yyjson-config.cmake").write_text('message(FATAL_ERROR "Default OFF searched yyjson")\n')
    off = profile / "off-project"
    off.mkdir()
    (off / "CMakeLists.txt").write_text(f'''cmake_minimum_required(VERSION 3.20)
project(default_off LANGUAGES CXX)
set(WJR_ENABLE_YYJSON OFF CACHE BOOL "" FORCE)
set(WJR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_ASSEMBLY OFF CACHE BOOL "" FORCE)
set(WJR_CPU_TARGET baseline CACHE STRING "" FORCE)
set(yyjson_DIR "{trap}" CACHE PATH "" FORCE)
add_subdirectory("{source}" wjr)
if(TARGET wjr::yyjson OR TARGET yyjson::yyjson)
  message(FATAL_ERROR "Default OFF imported yyjson")
endif()
''')
    run(["cmake", "-S", off, "-B", profile / "off-build", *common, *overrides,
         "-DCMAKE_PREFIX_PATH=" + str(prefix)])

    if name == "gcc11":
        original = (native_prefix / "include/yyjson.h").read_text()
        library = next(native_prefix.rglob("libyyjson.a"))
        for case in ("wrong-version", "disabled-utf8"):
            fake = profile / case
            (fake / "include").mkdir(parents=True)
            text = original.replace("0x000D00", "0x000C00") if case == "wrong-version" else original
            (fake / "include/yyjson.h").write_text(text)
            definitions = ("set_property(TARGET yyjson::yyjson PROPERTY INTERFACE_COMPILE_DEFINITIONS "
                           "YYJSON_DISABLE_UTF8_VALIDATION=1)") if case == "disabled-utf8" else ""
            (fake / "yyjson-config.cmake").write_text(f'''add_library(yyjson::yyjson UNKNOWN IMPORTED)
set_target_properties(yyjson::yyjson PROPERTIES IMPORTED_LOCATION "{library}"
    INTERFACE_INCLUDE_DIRECTORIES "{fake / 'include'}")
{definitions}
''')
            for optional in ("ON", "OFF"):
                command = ["cmake", "-S", str(source / "test/consumer/component"),
                           "-B", str(profile / (case + optional)), *common,
                           "-DCMAKE_PREFIX_PATH=" + str(prefix), "-Dyyjson_DIR=" + str(fake),
                           "-DWJR_REQUEST_COMPONENT=yyjson", "-DWJR_OPTIONAL_COMPONENT=" + optional]
                result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                (artifacts / (case + optional + ".log")).write_bytes(result.stdout)
                assert (result.returncode == 0) == (optional == "ON")

(artifacts / "provider.json").write_text(json.dumps({"version": "0.13.0", "commit": commit}, indent=2))
