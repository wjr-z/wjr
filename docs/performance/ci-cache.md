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
