# Reusing indirect Btree copy-assignment storage

Repeatedly refreshing an owning snapshot with `destination = source` used to
destroy and reallocate every node and indirect value, even when the two trees
had identical shapes. At N16 and 4096 integer keys, a legacy map requested and
released 4607 blocks per assignment: 4096 value blocks and 511 tree nodes.

The copy-assignment engine now reuses those blocks when values are indirect,
sizes match, and every corresponding node has the same child/slot count.
This serves ordinary map snapshots and nontrivial set/multiset keys without a
new API. Copy construction, move, mismatched shapes/sizes, compact maps and
inline-value sets retain their existing implementations. The shape check scans
metadata only; there is no comparator-based search, allocation or persistent
cache/configuration field.

```cpp
wjr::btree_map<unsigned long long, unsigned long long> current;
// Populate current, then retain an independently owned snapshot.
auto published = current;
// After updating mapped values, reuse published's compatible storage.
published = current;
```

## Ownership and lifetime

Reuse never assigns into a const key. Each complete value object is destroyed
and copy-constructed in its existing owned allocation through allocator hooks.
The allocation and node objects remain alive. Separators are rebuilt from the
new leaf minima, including indirect key pointers and changed comparator state.
The source remains a complete, unchanged tree. During reconstruction the
destination publishes an empty root/size/sentry; hooks cannot observe a partly
rekeyed active graph. The detached graph is republished only after all values
and separators are complete. Small-object slots follow the same value lifetime
protocol without inventing heap nodes.

Allocator propagation still runs first. An incompatible propagating allocator
releases its old storage through the original allocator, then uses the existing
fresh-copy path. Nonpropagating allocators retain their destination resource.
Matching the complete shape preserves the source's node counts, occupancy and
requested storage footprint; a larger, sparser destination is not retained.

These operations remain inside the existing copy engine's `noexcept` domain:
comparator assignment, value construction/destruction and allocator hook
failures already terminated there. This does not add exception recovery,
new termination boundaries, or a pointer/reference survival guarantee for copy
assignment. Element-owned allocations, such as long PMR strings, can still be
destroyed and recreated; zero tree allocations does not mean zero total work.

## Bounded measurements

The [machine-readable evidence](data/btree-copy-reuse.json) retains every raw
sample, observed ranges, CVs, memory passes and source hashes. The frozen
prototype is `3f241c9d`, reference `035303d5`, with Linux
[run 41304495942](https://github.com/wjr-z/wjr/actions/runs/41304495942).
That prototype also tried compact storage; the final scope deliberately rejects
it. Windows Clang baseline/native assembly checks confirm that the selected
indirect specializations are identical to the measured prototype, while final
compact/inline-set copy assignment and legacy copy construction are identical
to the original (only comments/blank lines removed). The timings below are of
the frozen prototype, not an additional measurement of the narrowed branch.

All cases use N16, 4096 ascending uint64 keys and either uint64 or 64-byte array
mapped values. Timing uses ordinary `std::allocator`, with counters in a separate
pass. Reference/candidate run in RCCR order, five raw repetitions per invocation,
minimum 0.005 seconds wall time. Thus each ratio uses ten samples per side.
The ranges are observed `min(candidate)/max(reference)` through
`max(candidate)/min(reference)`, not confidence intervals.

Windows uses an i5-13500HX, CPU0, Clang 18.1.8 and MSVC STL 143.202408.
Linux GCC 13.3 and Clang 18.1.3 share one EPYC 7763 runner, with libstdc++
20240904/20240908. All use C++20, `-O3 -DNDEBUG -march=x86-64`.
Different CPUs/STLs preclude attributing absolute differences to the OS.
MSVC is covered functionally; there is no MSVC throughput claim.

| Profile | Mapped | Matched candidate/reference | Observed range |
| --- | --- | ---: | --- |
| Windows Clang | uint64 | 0.023 | 0.018–0.042 |
| Windows Clang | 64-byte | 0.056 | 0.050–0.079 |
| Linux GCC | uint64 | 0.103 | 0.081–0.118 |
| Linux GCC | 64-byte | 0.171 | 0.147–0.230 |
| Linux Clang | uint64 | 0.110 | 0.089–0.131 |
| Linux Clang | 64-byte | 0.182 | 0.159–0.235 |

Two negative controls alternate between source trees on every iteration, so
they cannot become matched copies after the first assignment. Shape mismatch
moves one key between late leaves, exercising a nearly complete failed shape
scan; size mismatch alternates 4096/4095 values and bypasses that scan.

| Profile | Mapped | Shape mismatch ratio [range] | Size mismatch ratio [range] |
| --- | --- | --- | --- |
| Windows Clang | uint64 | 0.990 [0.898, 1.175] | 1.256 [0.559, 1.652] |
| Windows Clang | 64-byte | 1.037 [0.917, 1.179] | 1.060 [0.902, 1.334] |
| Linux GCC | uint64 | 0.981 [0.872, 1.083] | 0.989 [0.869, 1.132] |
| Linux GCC | 64-byte | 1.001 [0.873, 1.158] | 1.005 [0.900, 1.119] |
| Linux Clang | uint64 | 1.025 [0.978, 1.134] | 1.016 [0.943, 1.150] |
| Linux Clang | 64-byte | 1.026 [0.887, 1.194] | 1.011 [0.886, 1.143] |

These short controls do not prove fallback cost is zero. Windows uint64 size
mismatch has particularly high dispersion; its unfavorable median is retained.
Shape mismatch necessarily adds metadata traversal before the old allocation
path, so applications that always change shape should measure that cost.

The rejected compact prototype has credible regressions despite matched-copy
gains: uint64 shape mismatch is 1.063 [1.025, 1.096] on GCC and
1.070 [1.043, 1.102] on Linux Clang; Windows uint64 size mismatch is
1.464 [1.184, 1.800]. Compact and inline values therefore keep their original
copy paths at compile time, without a runtime threshold or policy switch.

## Mature controls and memory

The same fixture measures `std::map` and pinned Abseil `btree_map`
`2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`. No iterator/reference survives a
mutation. Their broader exception/reference guarantees remain different from
WJR; these are successful-copy timing comparisons, not full substitutability.

| Profile | Mapped | Selected WJR/std matched time | Selected WJR/Abseil matched time |
| --- | --- | ---: | ---: |
| Windows Clang | uint64 | 0.030 | 0.144 |
| Windows Clang | 64-byte | 0.052 | 0.146 |
| Linux GCC | uint64 | 0.234 | 0.219 |
| Linux GCC | 64-byte | 0.336 | 0.208 |
| Linux Clang | uint64 | 0.197 | 0.283 |
| Linux Clang | 64-byte | 0.291 | 0.215 |

For selected WJR maps, matched assignment requests/releases **4607 → 0 tree
blocks** on each profile. Fallback retains its original requests. Requested
bytes before/after/peak are unchanged: the three simultaneously owned uint64
trees use 463392 bytes, and the 64-byte mapped trees use 1151520 bytes.
This is allocator-requested heap storage, excluding tree objects and allocator
metadata; it is not RSS. Every independent memory pass ends at zero live bytes.

Abseil retains less heap in this fixture: 220416/961464 bytes for the same three
uint64/64-byte trees. WJR's faster matched refresh does not erase that memory
tradeoff. libstdc++ already reuses std-map nodes here (zero copy allocations),
whereas the tested MSVC STL requests 4096 nodes per assignment. Allocation-count
reduction alone would not establish the measured timing result.

## Correctness and reproduction

The final 12 focused tests pass MSVC Release and Windows Clang ASan/UBSan.
They cover indirect/inline map/set/multiset, duplicates, SSO/multilevel graphs,
self-assignment, mismatched shapes/sizes, heap-to-SSO fallback, changed comparator
state, PMR indirect string keys, over-aligned nontrivial values, balanced typed
hooks, valid empty destination observation, and existing allocator propagation.
The frozen broader prototype's 11 tests also passed Linux ASan/UBSan; the final
additional indirect-set test remains in the normal compiler CI matrix.

Configure the existing `btree` benchmark profile, then explicitly build
`btree_copy` and `btree_copy_memory`; both are excluded from the default build.
The existing reviewed Abseil option adds those controls. Timing and memory use
the same fixture with separate executables, so accounting does not affect time.
There is no new scheduled workflow or compiler matrix.
