# Reusing build dependencies

## fmt

WJR first reuses an existing `fmt::fmt` target supplied by its parent project.
Otherwise it searches for an installed fmt **11.2.1** config package, matching
the reviewed bundled version, before building `third-party/fmt`. An incompatible
or absent package uses the bundled fallback. This avoids recompiling an available
dependency without silently adopting a different fmt version.

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/fmt/prefix
```

`fmt_DIR` may instead name the directory containing `fmt-config.cmake`. To force
the bundled version in a standalone build:

```sh
cmake -S . -B build -DCMAKE_DISABLE_FIND_PACKAGE_fmt=ON
```

An existing parent target is always preserved, including when discovery is
disabled. The parent owns its compatibility and build configuration. A parent
requiring an external package can call `find_package(fmt 11.2.1 EXACT CONFIG
REQUIRED)` before adding WJR.

Installed WJR consumers must still be able to find fmt; externally supplied fmt
is not copied into WJR's install prefix. Keep the external dependency's compiler,
CRT, linkage and sanitizer requirements compatible with the consuming build.

## CpuFeatures

On x86, WJR reuses a parent-supplied `CpuFeatures::cpu_features` target first.
Otherwise it searches for the official installed CpuFeatures **0.10.1** config
package before building the pinned `cpu_features` source. The exact version keeps
the public `cpu_features::X86Info` ABI tied to the existing reviewed dependency.
Parent targets retain their scope and alias; the parent owns their compatibility.

Set `CMAKE_PREFIX_PATH` or `CpuFeatures_DIR` to the installed package. An explicit
`FETCHCONTENT_SOURCE_DIR_CPU_FEATURES` override takes precedence over discovery.
`CMAKE_DISABLE_FIND_PACKAGE_CpuFeatures=ON` or
`FETCHCONTENT_TRY_FIND_PACKAGE_MODE=NEVER` forces the pinned source fallback unless
the parent already supplies the target. Missing or incompatible packages also use
the source fallback. This works with WJR's CMake 3.20 minimum.

An installed provider is not copied into WJR's install prefix. Independent
installed WJR consumers must also be able to find CpuFeatures 0.10.1; a source-built
provider keeps its existing installation behavior. Match the external provider's
architecture, compiler, CRT, linkage and instrumentation to the consuming build.
CPU detection, baseline/native options and provider test isolation are unchanged.

## Google Benchmark

Both the regular and focused B+ tree benchmark profiles reuse a parent-supplied
`benchmark::benchmark` target. Otherwise they search for an installed **1.9.0**
config package before building the pinned source fallback. Only explicitly enabled
benchmark builds perform this search; normal library consumers do not depend on
Google Benchmark.

Point `CMAKE_PREFIX_PATH` or `benchmark_DIR` at an installed package. Explicit
`FETCHCONTENT_SOURCE_DIR_BENCHMARK` and a local `benchmarks/benchmark` source tree
take precedence over automatic discovery. Use
`-DCMAKE_DISABLE_FIND_PACKAGE_benchmark=ON` to force the source fallback. Existing
parent targets remain authoritative even when discovery is disabled; the parent
owns their version and build compatibility.

Installed binaries must match the compiler, CRT, linkage and instrumentation of
the consuming build. A prebuilt benchmark library is not silently rebuilt with
different sanitizer flags. No benchmark runtime or performance methodology changes
here.

## GoogleTest

Test builds reuse a parent-supplied `GTest::gtest_main` target first (including the
upstream `gtest_main` name). The parent owns its version, CRT and instrumentation;
WJR does not change that target or its source options.

Otherwise, single-configuration Release, RelWithDebInfo and MinSizeRel builds
without ASan or coverage search for an installed **1.17.0** config package. Point
`CMAKE_PREFIX_PATH` or `GTest_DIR` at the package. A compile/link check exercises
GoogleTest's registration, main and string ABI before accepting it. A missing,
wrong-version or non-linkable package uses the existing pinned source fallback.
This check cannot establish every ABI property; external packages must still match
the compiler, architecture, linkage and other build settings of the tests.

ASan, coverage, Debug, unspecified build types and multi-configuration generators
keep building GoogleTest from source. This preserves the existing instrumentation
and Windows CRT policy rather than assuming a prebuilt package matches it. An
explicit `FETCHCONTENT_SOURCE_DIR_GOOGLETEST` or local `test/googletest` source tree
also takes precedence over discovery. `CMAKE_DISABLE_FIND_PACKAGE_GTest=ON` or
`FETCHCONTENT_TRY_FIND_PACKAGE_MODE=NEVER` forces source selection unless the parent
already supplies the target. These paths work with CMake 3.20.

GoogleTest remains a private test dependency. A reused package is not copied into
WJR's install prefix and is not required by installed WJR consumers. The source
fallback retains upstream's existing installation behavior.
