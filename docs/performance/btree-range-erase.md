# Leaf-batched B+ tree range erase

The shared B+ tree engine now erases complete leaves directly and batches
within-leaf deletion when the remaining leaf stays above its existing minimum.
This serves the existing `erase(first,last)` and duplicate-key `erase(key)`
callers for legacy map/set/multiset and experimental compact maps. No API,
capacity, storage layout or allocator policy is added.

```cpp
auto first = events.lower_bound(start_time);
auto last = events.lower_bound(finish_time);
auto successor = events.erase(first, last);
```

The old implementation counted the interval by visiting every element, then
called single-iterator erase for each value. Those erasures repeatedly shifted
pending values and repaired leaf occupancy. Range length is now computed from
leaf sizes and iterator positions. An entire covered leaf needs destruction and
one parent-child removal, without shifting its values into a neighboring leaf.
Partial leaves destroy a bounded slice and shift the suffix once; deletion that
would underflow still uses the existing single-element rebalance.

## Invariants and validation

Ordinary leaf/inner capacities and minimum occupancy are unchanged. Removing a
first child installs its successor as child zero and updates the nearest
ancestor minimum; the existing child-removal rebalance then removes child one.
Other child positions use the same rebalance directly. Whole-leaf deletion
updates the linked leaf chain and returns the surviving successor. Its parent
can merge or collapse without moving that successor leaf.

The range count is captured before mutation, so an invalidated `last` iterator
is not used as a stopping condition. Partial-batch relocation uses the existing
slot protocol, including actual const-key pair lifetimes and allocator hooks.
Indirect surviving value objects retain their addresses; iterator and inline
reference invalidation remain as documented for the existing engine. Key erase
finishes all comparisons before destroying an aliased key argument. Full-tree
range erase still uses `clear`, and single-iterator erase retains its original
implementation. There is no auxiliary allocation or new exception boundary.

In a counted, nontrivial mapped-type probe, erasing keys [1024,3072) from an
ascending 4096-element compact N16 map reduces mapped move constructions from
14018 to 30. This counts all moves during deletion, including values that may
be erased later; it is not only a count of surviving objects. Partial front
intervals still need boundary repair and do not promise the same move count.

Tests compare every small range and multi-level cuts against std containers,
including capacities 4/5/16, shuffled and reverse ordering, root collapse,
returned iterators, duplicate string-key groups, indirect keys and surviving
value addresses. Move-only over-aligned mapped objects check live counts and
self addresses; independent PMR accounting checks exact bytes/alignment on
deallocation. The final local checks are 117 Btree tests with Clang Release,
five focused tests with MSVC Release, and the same five with ASan+UBSan.

## Fixed workload results

Each ratio is `median(batch) / median(original)` wall time from five raw
repetitions. Below one is less time. The middle interval is [1024,3072); the
partial-front interval is [1,3071), retaining an initial anchor. Both start with
4096 ascending integer keys, N16. Mapped values are uint64 or a 64-byte array.

| Profile | Map | Middle | Partial front |
| --- | --- | ---: | ---: |
| Windows Clang | Legacy uint64 | 0.605 | 0.619 |
| Windows Clang | Compact uint64 | 0.312 | 0.446 |
| Windows Clang | Legacy 64-byte | 0.571 | 0.642 |
| Windows Clang | Compact 64-byte | 0.222 | 0.476 |
| Linux GCC | Legacy uint64 | 0.573 | 0.673 |
| Linux GCC | Compact uint64 | 0.341 | 0.587 |
| Linux GCC | Legacy 64-byte | 0.577 | 0.661 |
| Linux GCC | Compact 64-byte | 0.250 | 0.499 |
| Linux Clang | Legacy uint64 | 0.702 | 0.795 |
| Linux Clang | Compact uint64 | 0.433 | 0.556 |
| Linux Clang | Legacy 64-byte | 0.748 | 0.790 |
| Linux Clang | Compact 64-byte | 0.266 | 0.450 |

All 24 candidate/reference groups have observed ratio envelopes below one. The
JSON retains `min(batch)/max(original)` through `max(batch)/min(original)` and
every raw sample, not confidence intervals. These short bounded measurements
do not establish behavior for all interval widths, input orders or key types.

## Strong controls and remaining costs

std::map and Abseil's public map are measured with the same successful-operation
contract and allocator family. The reviewed Abseil revision remains
`2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`. No iterator or reference survives a
mutation in the benchmark; this does not equate their invalidation guarantees.

| Profile | Case | Batch/std middle, front | Batch/Abseil middle, front |
| --- | --- | ---: | ---: |
| Windows Clang | Legacy uint64 | 1.010, 1.108 | 3.832, 4.578 |
| Windows Clang | Compact uint64 | 0.239, 0.360 | 0.908, 1.487 |
| Windows Clang | Compact 64-byte | 0.253, 0.564 | 0.317, 0.602 |
| Linux GCC | Legacy uint64 | 0.671, 0.836 | 2.907, 3.709 |
| Linux GCC | Compact uint64 | 0.305, 0.554 | 1.321, 2.458 |
| Linux GCC | Compact 64-byte | 0.318, 0.822 | 0.451, 0.914 |
| Linux Clang | Legacy uint64 | 0.667, 0.783 | 2.813, 3.354 |
| Linux Clang | Compact uint64 | 0.286, 0.410 | 1.206, 1.754 |
| Linux Clang | Compact 64-byte | 0.278, 0.672 | 0.361, 0.703 |

The complete legacy-large rows are retained in the JSON too. Legacy small-value
maps still lose substantially to Abseil here, and compact uint64 is not uniformly
faster than Abseil. Legacy keeps its indirect-value storage and per-value release
work; this change does not remove those allocations or choose compact as the
default. No heap-density, RSS or hardware-counter improvement is claimed.

A separate Windows PMR counting pass verifies that each selected old/new pair
has identical requested bytes after erase, identical deallocation counts and
zero allocations during erase. For example, compact uint64 retains 73152 bytes
after the middle interval and 36576 after the partial-front interval; compact
64-byte mapped retains 276544/138720 bytes. All requests return to zero after
destruction. These counters are absent from timing; requested bytes are not RSS
or allocator usable size, and this equality is specific to the selected traces.

## Measurement provenance

Research source `a65bef5d68cf9ddce8de88961a3c4f41c457b31c` is based on production
implementation `77502091` and main `0ddcdd9f`. A stateless comparator marker
selects the original element-count/erase loops in the same executable; normal
std::less selects the batched path. Types have identical layout and ordering
semantics. Complete erased results are checked before timing for every backend.

Only the two lower-bound searches and erase are timed. Fresh ascending source
build and final surviving-tree destruction are paused. Timing uses std::allocator
without counters, 64 iterations per repetition, five repetitions and wall time.
Fixed iteration counts bound setup cost; no auto-calibrated minimum duration is
claimed. The shared allocation/deallocation environment is warmed by successive
iterations. Raw `real_time` is nanoseconds per interval operation, not per value.

Windows uses i5-13500HX, Clang 18.1.8/MSVC STL, Windows 11 build 22631, CPU0
affinity. Linux GCC 13.3.0 and Clang 18.1.3 run sequentially on the same EPYC 7763
Ubuntu 24.04 worker using their default libstdc++; the STL header date was not
sampled in this campaign. Both profiles use C++20 Release baseline
`-O3 -DNDEBUG -march=x86-64`. Different CPUs prevent attributing absolute
cross-platform differences to the OS. MSVC correctness is tested; no MSVC speed
claim is made.

[All 360 raw samples and ratios](data/btree-range-erase.json) include std/Abseil
controls and original-file hashes. Commands, source hashes, CPU/OS/compiler
details and Linux raw results are in
[run 41187389989](https://github.com/wjr-z/wjr/actions/runs/41187389989).
Research markers, workflow and benchmark scaffolding do not enter production
or add a default CI/benchmark matrix.
