# Compact N16 map against legacy N16

The experimental compact map improves build/destroy and mixed mutation in these
profiles, but lookup can regress substantially and large mapped values can use
more requested memory. These results support keeping it opt-in; they do not
select a new default or establish an advantage for every ordered-map workload.

Both candidates use capacity 16, the same comparator, inputs, trace and allocator
within each compiler/STL profile. `u64_map` is `map<uint64_t,uint64_t>`;
`large_map` is `map<uint64_t,array<byte,64>>`. The compact map stores actual
const-key pairs in leaves; the legacy map stores pointers to separately allocated
pairs. Their different reference-invalidation behavior remains part of the
[storage contract](../design/btree-compact-map.md).

## Time

Each number is `median(compact16) / median(wjr16)` wall time from five samples;
below 1 means compact took less time. Element count is separate from the fixed
16-slot node capacity. All measured groups, including ties and regressions, are
retained. The [compact source data](data/btree-compact-n16.json) contains all 480
normalized raw samples, so medians and observed min/max can be recomputed.
Five samples are not a confidence interval; values near 1 do not demonstrate an
improvement.

| Profile | Map | Elements | Build/destroy | Lower bound | Mixed 90/5/5 | Key scan |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Windows Clang | u64_map | 64 | 0.221 | 2.195 | 0.887 | 0.972 |
| Windows Clang | u64_map | 4096 | 0.437 | 1.421 | 0.822 | 1.001 |
| Windows Clang | large_map | 64 | 0.241 | 2.513 | 0.882 | 1.021 |
| Windows Clang | large_map | 4096 | 0.558 | 1.572 | 0.840 | 1.042 |
| Linux GCC | u64_map | 64 | 0.557 | 1.980 | 0.930 | 1.001 |
| Linux GCC | u64_map | 4096 | 0.741 | 0.775 | 0.861 | 0.967 |
| Linux GCC | large_map | 64 | 0.805 | 0.960 | 0.895 | 0.992 |
| Linux GCC | large_map | 4096 | 0.816 | 0.747 | 0.873 | 0.926 |
| Linux Clang | u64_map | 64 | 0.505 | 0.933 | 0.954 | 0.916 |
| Linux Clang | u64_map | 4096 | 0.803 | 1.039 | 0.904 | 0.964 |
| Linux Clang | large_map | 64 | 0.827 | 1.072 | 0.971 | 0.923 |
| Linux Clang | large_map | 4096 | 0.829 | 0.836 | 0.878 | 0.970 |

At 4096 elements, build/destroy ratios range from 0.437 to 0.829 and mixed ratios
from 0.822 to 0.904. Lookup has no common winning direction: Windows ratios are
1.421 and 1.572, while Linux GCC ratios are 0.775 and 0.747. Small Windows maps
regress further, to 2.195 and 2.513. Compiler, STL, CPU and size matter; the two
operating systems use different CPUs, so these differences do not isolate an OS
effect. No instruction-cache, branch or cache-miss explanation is established by
this timing pass.

A focused Windows Clang wrapper inspection found one call to the existing
noinline search helper for each layout, no calls within that helper, and no added
runtime slot/trait dispatch, pair copy or allocator operation. The same N16
hybrid search accesses pointer slots with key indirection in the legacy leaf and
16-byte/72-byte pair strides directly in compact leaves. Address generation
differs in the generated code. This static check does not explain the full
lookup regression or establish a corrective optimization.

## Requested memory

This is a separate allocator-counting pass. Heap columns show live requested
bytes after build and after one mixed replay. Allocation counts cover the entire
build + one mixed replay + clear/destruction window, not only initial build.
All 24 selected rows balance allocation/deallocation counts and return requested
live bytes to zero after clear and destruction. Linux GCC and Clang have identical
memory rows for these selected traces and are grouped below.

| Profile | Map | Elements | Object bytes legacy / compact | Build heap legacy / compact | Mixed heap legacy / compact | Allocation calls legacy / compact |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Windows Clang | u64_map | 64 | 48 / 64 | 2112 / 1728 | 2112 / 1728 | 326 / 6 |
| Windows Clang | u64_map | 4096 | 48 / 64 | 133440 / 114624 | 133600 / 114912 | 4751 / 399 |
| Windows Clang | large_map | 64 | 48 / 176 | 5696 / 6208 | 5696 / 6208 | 326 / 6 |
| Windows Clang | large_map | 4096 | 48 / 176 | 362816 / 441664 | 362976 / 442848 | 4751 / 399 |
| Linux GCC/Clang | u64_map | 64 | 48 / 64 | 2272 / 2016 | 2432 / 2304 | 328 / 8 |
| Linux GCC/Clang | u64_map | 4096 | 48 / 64 | 132608 / 112896 | 132768 / 113184 | 4745 / 393 |
| Linux GCC/Clang | large_map | 64 | 48 / 176 | 5856 / 7392 | 6016 / 8576 | 328 / 8 |
| Linux GCC/Clang | large_map | 4096 | 48 / 176 | 361984 / 433664 | 362144 / 434848 | 4745 / 393 |

Compact removes per-pair allocation, but unused leaf capacity grows with the
mapped value. At 4096 elements, integer-map build requests decrease by 14.1%
on Windows and 14.9% on Linux; large-map requests increase by 21.7% and 19.8%.
The large-map object also grows from 48 to 176 bytes. Fewer calls do not imply
less total memory.

These are allocator requests, excluding allocator metadata, size-class rounding,
RSS, input/query buffers and the separately reported tree object. The integer
keys and mapped values own no external buffers. Peak and cumulative requests and
iterator sizes remain in the source data. This pass has no pool; reserved pool
bytes are `NA`. Node fill is not exposed by the public API and is not inferred
from these totals.

## Workload and provenance

- The [focused baseline contract](btree-baseline.md) defines the shared trace and
  reference validation. Timing uses correctly rebound `std::allocator` with no
  counters; memory uses the separate counting-allocator executable.
- Build includes empty construction, shuffled insertion and destruction, excluding
  input generation. Lower bound runs 2048 queries, half with present keys, including
  before-first and after-last misses. It does not imply half return `end()`.
- Each mixed replay has 256 blocks of 18 lookups, one valid iterator deletion
  including its search, and one insertion: 5120 operations total. Setup/destruction
  are excluded. Each replay begins with fresh state.
- Scan consumes keys and a digest, not mapped payloads. No iterator, reference or
  pointer survives mutation. Successful operations and complete final contents
  are checked against the STL before timing; exception/failure paths are outside
  the timing contract.
- Google Benchmark 1.9.0, C++20 Release, baseline x86-64; five raw repetitions,
  0.05-second minimum time. Seed `1656023321`, 256 trace blocks. The same generated
  permutation is used within a profile; `std::shuffle` can differ across STLs.
- Windows 11 22631: Intel Core i5-13500HX, process pinned to CPU 0; Clang 18.1.8,
  MSVC STL `143.202408`, `/O2 /Ob2 /DNDEBUG`. This is Windows Clang runtime data;
  the separate MSVC correctness checks are not an MSVC performance measurement.
- Linux: Ubuntu 24.04, Intel Xeon Platinum 8370C, unpinned shared runner. GCC 13.3.0
  with libstdc++ `20240904`, and Clang 18.1.3 with libstdc++ `20240908`, ran
  sequentially on the same worker with `-O3 -DNDEBUG -march=x86-64`.
- Frozen feature source `0575e0cbe2a2586a63bb788d87ec63870ef9d082`; measurement
  source `e3b59a81bcb37433510e8dede0fe850a19f326c5` adds only registration and
  measurement workflow settings. Functional header blobs match the frozen source.
  [Linux run 41047957402](https://github.com/wjr-z/wjr/actions/runs/41047957402)
  retains raw timing, CSV, compile commands and CPU/compiler details. Local Windows
  data was collected on 2026-10-30. The checked-in compact source retains every
  selected raw sample/memory row, source blobs and raw-file hashes for both.
- Hardware counters were not collected. These observations do not establish
  allocator overhead, RSS, cold-cache behavior or a hardware-level cause.

## Reproduce the candidate comparison

Compact benchmark registration is **OFF by default** and available only in the
`btree` profile. It adds only integer and 64-byte-mapped N16 map candidates:

```sh
cmake -S . -B build-compact -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree \
  -DWJR_BENCHMARK_COMPACT=ON -DWJR_CPU_TARGET=baseline \
  -DWJR_ENABLE_INSTALL=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build-compact --target benchmarks btree_memory --parallel
./build-compact/bin/benchmarks \
  --benchmark_filter='^(u64_map|large_map)/(wjr16|compact16)/.*/(64|4096)/' \
  --benchmark_repetitions=5 --benchmark_report_aggregates_only=false \
  --benchmark_min_time=0.05s --benchmark_out=timing.json
./build-compact/bin/btree_memory > memory.csv
```

The manual **Benchmarks** workflow accepts `profile=btree`, `compact=true` and
`showcase=false`; its general pilot retains the shorter existing 0.01-second
minimum. Scheduled/default profiles do not enable compact. Keep the raw samples,
actual flags, CPU and STL when repeating a comparison; do not combine absolute
durations from different machines.
