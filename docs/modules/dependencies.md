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
different sanitizer flags. GoogleTest's existing source/instrumentation policy is
unchanged. No benchmark runtime or performance methodology changes here.
