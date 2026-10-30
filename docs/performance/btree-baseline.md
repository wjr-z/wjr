# Focused ordered-container baseline

The `btree` benchmark profile compares WJR's unchanged N16/N32 engine with
`std::set/map`. It is a measurement facility, not evidence for changing defaults.
The existing engine keeps small integer sets inline, uses leaf-local search and
bulk movement, and stores ordinary map values indirectly. The latter preserves
value addresses during ordinary insertion but adds per-value allocations. These
costs need workload-specific measurements alongside search and traversal costs.

```sh
cmake -S . -B build-btree -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree \
  -DWJR_CPU_TARGET=baseline -DWJR_ENABLE_INSTALL=OFF \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build-btree --target benchmarks btree_memory --parallel
./build-btree/bin/benchmarks --benchmark_filter='/(64|4096)/' \
  --benchmark_min_time=0.01s --benchmark_out=timing.json
./build-btree/bin/btree_memory > memory.csv
```

The small first pilot uses 64 and 4096 elements. The executable also registers
262144; select it explicitly for a larger working set. Each case requests five
repetitions; retain the raw samples and report median and min/max, using
`real_time` for this wall-clock profile. Increase the minimum duration or repeat
the affected case when noise obscures a decision. CPU time remains in the JSON
but can be poorly resolved for short Windows runs.

Inputs are `set<uint64_t>`, `map<uint64_t,uint64_t>`, 12-character and
96-character `map<string,uint64_t>` keys, and `map<uint64_t,array<byte,64>>`.
Both string lengths are fixed; whether the short case uses SSO depends on the
selected STL. Mapped values are value-initialized. Comparators are `std::less<>`;
all implementations use their correctly rebound `std::allocator` during timing.
Each container starts independently; no shared warmed pool is involved.

All backends share the fixed seed, generated keys, insertion permutation and
trace in `benchmarks/src/btree_focused.hpp`. A reference pass checks lower-bound
results and complete final contents against the STL before timing. Operations:

- `build_destroy`: shuffled insertion into a newly constructed tree, including
  construction and destruction; inputs and permutation are prepared beforehand.
- `lower_bound_50`: 2048 shuffled same-key queries, exactly half present, with
  explicit before-first and after-last misses. String cases additionally run
  `lower_bound_view_50` using existing `string_view` queries without temporaries.
- `mixed_90_5_5`: 256 blocks of 18 lookups, one valid iterator-based deletion
  (including its lower-bound search), and one insertion. Each block restores
  the starting size; lookup queries are half present at their point in the trace.
  Setup and destruction are excluded; each timed replay starts from fresh state.
- `scan`: ordered traversal. Lookup/mixed/scan consume a deterministic key digest
  to keep results observable. That checksum work is part of the measured contract,
  including reading string characters; this is not an isolated comparison count.

The memory executable is a separate pass over 0/1/2/3 and the timed sizes. Its
counting allocator shares a pass-local counter across all typed rebinds. It
validates the trace before resetting counters, then records live requested bytes
after build/mixed, peak and cumulative requested bytes, allocation/deallocation
counts, and live bytes after clear/destruction. Object/iterator sizes are separate.
Allocation counts include the mixed trace and final clear/destruction. These are
allocator requests, not allocator usable size, metadata overhead, committed pages,
RSS, or cache footprints.

Ordinary `std::string` character allocation does not use the tree allocator.
`element_string_capacity_bytes` separately observes the final tree's external
character capacity including terminators, excluding in-object SSO storage. It is
an observed capacity total, **not** an exact character allocation count or peak.
Input/query string buffers are excluded. Tree node counts/occupancy are not exposed
by these public APIs and are not inferred from allocator sizes. This initial pass
has no pool: `pool_reserved_bytes` is `NA`, not zero. A pooled candidate must add
an upstream-resource accounting pass and report retained slabs after clear and
release after resource destruction before making pool-memory claims.

Abseil is explicitly opt-in. OFF performs no Abseil discovery or linking. ON uses
an existing, unmodified Git checkout of the reviewed commit; nothing downloads,
installs or publishes that source:

```sh
cmake -S . -B build-btree-absl -GNinja \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree \
  -DWJR_BENCHMARK_ABSEIL=ON \
  -DWJR_BENCHMARK_ABSEIL_SOURCE_DIR=/path/to/abseil-cpp \
  -DWJR_CPU_TARGET=baseline
```

The accepted commit is `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`. Comparisons
use public `absl::btree_set/map` with the same key/value types, comparator,
allocator, inputs and lifetime boundaries. This does not imply matching iterator
or reference invalidation guarantees. No iterator/reference survives mutation in
the trace. Timing tests successful operations; allocation-failure and exception
contracts are outside this baseline.

For Linux data, manually dispatch the existing Benchmarks workflow with profile
`btree`. One worker runs its GCC and Clang toolchains sequentially and uploads
timing JSON, memory CSV, actual compile commands, compiler/CPU/OS/source details
and executable code sizes. The scheduled profile stays `core`; Windows CI is
unchanged. Hardware counters are explicitly recorded as not collected.
For Windows, use the same filter, release/baseline/C++20 options and allocator
configuration with available Clang/MSVC, recording the STL version and CPU.
Compare candidate/reference ratios within each worker, then assess whether the
direction agrees across platforms. Different CPUs do not isolate an OS effect,
and raw durations from different machines must not be compared as if they did.

To add a reviewed candidate, add its container alias in `btree_focused.hpp`, then
register it next to the existing backends in `register_scenario` and
`memory_scenario`. Keep the shared trace and timing boundaries. Byte aliases,
compact storage and PMR candidates can be added without changing this baseline;
pool timing must apply equivalent independent resources to each compared backend.
