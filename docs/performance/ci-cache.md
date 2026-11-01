# CI compiler cache

Linux caches C/C++ object compilation through the CMake compiler launchers,
including `tests`, `np_tests` and dependencies built by the same CMake project.
Windows now uses the official Mozilla sccache action, pinned to action v0.0.9
and backend v0.18.0, for its existing MSVC and Clang jobs. The FetchContent cache
is separate: restoring dependency sources alone does not restore compiled tests.
The compiler backend handles compiler identity, options and included headers;
NASM and linking retain their ordinary commands.

Ninja still prints `Building CXX object ...` when a compiler launcher returns a
cached object. Use the backend's statistics, rather than that progress label,
to distinguish compilation from a hit. Both cache actions report statistics
after each job. Windows uses the backend's GitHub Actions storage and its
existing branch/cache access rules; it does not cache or share CMake build trees.

## Native identity

Both launchers reuse the same native-option and response-file detection.
Only invocations containing `-march=native`, `-mtune=native` or `-mcpu=native`
include the full compiler-resolved CPU/features/tuning fingerprint. sccache's
`SCCACHE_C_CUSTOM_CACHE_BUSTER` composes the fingerprint hash with any existing
user value. Missing native metadata fails before accessing the shared cache.
Baseline compilation has no extra CPU identity; it can reuse compatible objects
on a different CPU. MSVC's existing CI profile remains baseline.

The cache is content-addressed, not divided by checkout or build directory.
Actual compiler flags, header changes, sanitizers and toolchains still affect
whether an object is reusable. No sloppiness, ignored headers or removed debug
information is used. This workflow does not enable `SCCACHE_BASEDIRS`: CI checkout
paths are stable, and stripping arbitrary paths can conflate path-dependent
`__FILE__` strings. A cross-checkout local profile should first make its source
and include spelling intentionally equivalent, then verify path observables.

## Observed baseline and verification

The last uncached Windows runs before this change spent the following time in
the Build step; checkout, tool installation, configuration and test execution
are separate costs. These runner observations are not a controlled throughput
comparison or a guaranteed future saving.

| PR | MSVC Build | Clang Build |
| --- | ---: | ---: |
| [#78](https://github.com/wjr-z/wjr/pull/78) | 119 s | 94 s |
| [#80](https://github.com/wjr-z/wjr/pull/80) | 159 s | 87 s |
| [#81](https://github.com/wjr-z/wjr/pull/81) | 122 s | 51 s |

For #80, Linux GCC 11 baseline reported 130 hits/137 cacheable invocations;
Clang 20 native reported 54/166. Those totals do not identify each miss's cause.
The main `tests` executable currently links the umbrella target and inherits
its numerical ISA requirements; this includes utility test translation units.
Splitting that target needs an audit of shared helpers, inline CPU macros and
module dependencies. It is not safe to remove its native flag merely to increase
the hit rate. A changed template header can also require real recompilation.

Local validation used the real `test/math/bit.cpp` translation unit and two
differently named source worktrees, with independent build directories and
equivalent relative source/include spelling. MSVC and Clang each missed cold,
hit in the second tree, missed after an included-header change, missed after
an optimization-option change and hit after restoration. CMake/Ninja builds of
the real bit tests also passed with each compiler and reused the object in a
second independent build directory. A simulated changed native fingerprint
missed for native compilation while the baseline object remained reusable.
This does not claim testing a second physical CPU.

References: [sccache CMake and compiler support](https://github.com/mozilla/sccache/tree/v0.18.0#usage),
[cache identity extension](https://github.com/mozilla/sccache/tree/v0.18.0#separating-caches-between-invocations),
[GitHub Actions storage](https://github.com/mozilla/sccache/blob/v0.18.0/docs/GHA.md).

## Scheduling and test execution

Measure runner queue time separately from execution. In successful Linux run
[41025660538](https://github.com/wjr-z/wjr/actions/runs/41025660538), jobs waited
11 min 10 s to 21 min 30 s after workflow creation before starting; actual job
execution took 1 min 52 s to 5 min 24 s. Build steps ranged from 50 s to 225 s,
and compiler setup took 15 s to 53 s. These are observations of a busy batch,
not a general performance guarantee. GCC 14 native reported 53 hits/140 calls;
Clang 20 native reported 164/232, including its additional consumer validation.

In Windows run
[41026678303](https://github.com/wjr-z/wjr/actions/runs/41026678303), MSVC and
Clang waited 11 min 38 s and 11 min 11 s respectively, then executed for 124 s
and 150 s. Their Build steps took 53 s and 31 s, with each backend reporting
132 hits/138 requests. Improving cache hits alone cannot remove that queue.

Linux now probes the requested versioned C/C++ drivers and compiles an ASan
header/runtime probe at the matrix's C++ standard. An available matching major
version avoids reinstalling the compiler; otherwise the existing setup action
still runs. Reused Clang drivers retain setup-clang's `/usr/local/bin/clang`
and `clang++` aliases: Clang's reported `InstalledDir` belongs to the existing
fingerprint, so switching directly to a versioned driver would unnecessarily
select a cold cache pool. Compiler contents remain part of the object-cache
identity. Installed consumers use the selected C++ driver too, rather than the
runner's unversioned Clang default.
The matrix requests compiler major versions. A preinstalled patch release can
differ from the package repository's latest patch (observed GCC 14.2.0 versus
14.3.0); these genuinely different compilers correctly use different cache pools.

Linux and Windows use two CTest workers. The seven provider-scope configuration
tests each have an independent build directory and reuse dependency sources
without updating them. In the Windows Clang run above they consumed 34 s of a
45.68 s serial test step. Parallelism retains all tests and sanitizer settings.
Local Windows Clang 18 validation passed all 19 tests with two workers; a pair
of fresh configuration directories took 29.24 s serial and 15.22 s with two
workers. This local result does not establish the same saving on every runner.

The main unit executable can also use GoogleTest's built-in sharding protocol.
`WJR_ENABLE_TEST_SHARDING` defaults to `OFF`, preserving the single `wjr_tests`
CTest entry. Linux and Windows CI enable it and register `wjr_tests_shard_0`
and `wjr_tests_shard_1`, both running the same executable in the existing test
working directory. GoogleTest assigns each enabled test case to exactly one
process through `GTEST_TOTAL_SHARDS=2` and `GTEST_SHARD_INDEX`; no source-path or
case-name lists are maintained. The existing `ctest --parallel 2` runs the two
shards within the same job. Compiler objects, linking, the Windows matrix and
the other CTest entries retain their current coverage.

For a local run:

```sh
cmake -S . -B build -DWJR_BUILD_TESTS=ON -DWJR_ENABLE_TEST_SHARDING=ON
cmake --build build --target tests
ctest --test-dir build --parallel 2 --output-on-failure
```

Filtering with `ctest -R wjr_tests` includes both shards. A single externally supplied
`GTEST_OUTPUT` file is unsuitable for parallel shards; leave it unset and use
CTest's `--output-junit` (CTest 3.21 or newer) for an aggregated CTest report.
If coverage and sharding are both enabled, each CTest shard writes a separate
`LLVM_PROFILE_FILE` containing
its shard index and process ID. The `coverage` target still directly runs the
complete executable once and generates its existing combined report.

This reduces the unit executable's elapsed time, rather than selecting or
skipping tests. Build, tool installation, configuration fixtures and runner
queue time remain separate costs; a percentage improvement of the unit
executable does not imply the same improvement for the complete CI job.

Bounded controls ran the same compiled executable three times in each mode,
interleaving single-process and two-shard runs. Every run's case names were
checked for missing or duplicated cases. Windows used an otherwise idle local
Core i5-13500HX with Clang 18.1.8, C++20 baseline, without ASan or GMP. The
[Linux controls](https://github.com/wjr-z/wjr/actions/runs/41279745425) used
C++20 native, assertion level 3, ASan and GMP on the same host within each job.

| Profile | Cases | One process median | Two shards median | Peak sampled aggregate RSS, one/two |
| --- | ---: | ---: | ---: | ---: |
| Windows Clang 18 | 676 | 4.79 s | 3.43 s | 208 / 211 MB |
| Linux Clang 18 | 704 | 32.83 s | 15.66 s | 1.10 / 1.53 GB |
| Linux GCC 13 | 704 | 40.51 s | 20.41 s | 1.05 / 1.48 GB |

RSS was sampled every 5 ms on Windows and 10 ms on Linux, so these are observed
peaks rather than allocation bounds. In particular, concurrent ASan processes
use more memory. These measurements do not include build or provider-fixture
time and are not whole-job speedup guarantees.

Reference: [GoogleTest sharding protocol](https://google.github.io/googletest/advanced.html#distributing-test-functions-to-multiple-machines).

Linux, Windows and pre-commit cancel superseded workflow runs for the same PR.
Different PRs, main pushes and manual runs have separate concurrency groups.
This releases capacity when a PR is revised; it does not reduce the matrix or
skip the latest revision. Prepare and validate a coherent revision locally
before pushing, and avoid repeated intermediate pushes that each enqueue the
full matrix. Separate PRs still require their complete compiler/standard/ISA
coverage. Additional concurrency limits can lengthen feedback time and should
not be added without measuring the repository's available runner capacity.
