# Compact-map transfer between unequal allocators

Compact maps now consume detached source storage while rebuilding a tree for a
different allocator. This applies to allocator-extended move construction and
unequal-allocator move assignment. It reduces simultaneous live requests from
two complete trees to one tree plus the current rebuilding frontier.

Legacy indirect-value trees retain their original clone-then-clear path. The
prototype's Windows 64-byte legacy-map transfer was 1.436 times slower, so the
optimization is selected by the existing compact storage policy, not by a value
size threshold or compiler guess. Copy, compatible-allocator storage transfer,
default layout and allocator selection are unchanged.

## Ownership and failure boundary

Before the first migration hook, root and size are detached from the source,
which becomes a valid empty tree. The retired graph is private ownership state:
only child links/counts are traversed; its keys are never searched. A destination
value is fully constructed through the destination allocator before the old
value is destroyed through its original allocator. An old leaf is released only
after all of its values have migrated; an old inner node is released after its
children. New leaves are collected behind a local list sentinel. The complete
root, leaf list and size are published only after rebuilding succeeds.

For SSO, the old slots remain alive outside the source's published size until
each is consumed. Destination size is published last. During compact migration,
read-only allocation/value hooks observe valid empty source and destination
containers rather than partially ordered trees. Hooks must not reenter mutation
of either container. No public reference-stability promise is added.

This does not introduce a recoverable allocation/value exception guarantee.
The existing `_move_assign_tree`, `_assign_tree`, recursive clone, allocation,
value construction/destruction and node-release helpers were already declared
`noexcept`, including calls to custom allocator/value hooks. Exceptions inside
those boundaries already terminated. Old/new standalone probes with deliberate
allocation or move-constructor throws both enter the terminate handler; an outer
catch is never reached. No previously throwing helper is changed to `noexcept`.
This report does not claim cleanup, rollback or recovery after termination.
Potentially throwing comparator/allocator initialization and compatibility tests
before migration retain their previous behavior and precede detachment.

Successful migration releases every retired object exactly once. Tests cover
SSO and allocated roots, multi-level graphs, over-aligned move-only mapped
objects with self-address checks, custom hooks, allocator identity and PMR-owned
keys/values. Existing allocator-aware-only construction and immovable compatible
storage-transfer tests remain enabled. Legacy moves retain the two-phase path.

## Requested-byte peak

These are separate accounting passes over two distinct-identity PMR resources
that forward to `new_delete_resource`. They are live requested bytes, not RSS,
allocator usable sizes or committed pages. Pools/monotonic resources can retain
upstream slabs despite deallocation, so this is not a claim to reduce their
retention. Container/control objects and input buffers are excluded.

Each source has 4096 shuffled integer keys, N16, with uint64 or a 64-byte array
as mapped value. Both implementations preserve the same destination shape and
make the same number of typed allocations; no node packing or arena is added.

| Profile | Compact mapped type | Source before | Original peak | New peak |
| --- | --- | ---: | ---: | ---: |
| Windows Clang | uint64 | 114624 | 229248 | 115776 |
| Windows Clang | 64-byte array | 441664 | 883328 | 443712 |
| Linux GCC/Clang | uint64 | 112896 | 225792 | 114048 |
| Linux GCC/Clang | 64-byte array | 433664 | 867328 | 435712 |

Every pass returns source requests to zero after migration and all requests to
zero after destruction, with source/destination allocation counts balanced.
The JSON retains legacy accounting rows as controls; those original 2x peaks
remain in production because the CPU tradeoff did not support replacing them.

## Transfer time

Values below are median streaming/reference wall-time ratios, from five raw
repetitions. Brackets are the observed `min(stream)/max(reference)` through
`max(stream)/min(reference)`, not confidence intervals. Below one is less time.

| Profile | Compact uint64 | Compact 64-byte mapped |
| --- | ---: | ---: |
| Windows Clang 18 | 0.894 [0.864..0.924] | 0.892 [0.812..0.969] |
| Linux GCC 13 | 0.517 [0.513..0.542] | 0.558 [0.554..0.606] |
| Linux Clang 18 | 0.604 [0.592..0.636] | 0.616 [0.598..0.622] |

The legacy prototype was measured in the same executable and rejected:

| Profile | Legacy uint64 | Legacy 64-byte mapped |
| --- | ---: | ---: |
| Windows Clang 18 | 0.849 [0.791..1.042] | **1.436 [1.410..1.707]** |
| Linux GCC 13 | 0.733 [0.704..0.747] | 0.618 [0.614..0.626] |
| Linux Clang 18 | 0.741 [0.734..0.750] | 0.613 [0.611..0.620] |

The Windows uint64 legacy range also overlaps a tie. Fewer passes or lower peak
bytes alone would not establish universal throughput improvement. No hardware
counters were collected, so allocator locality or compiler code generation are
possible explanations rather than demonstrated causes of that negative case.

## Reproduction and limits

The frozen research source is `4f33cb11`, based on implementation `2381b221` and
main `566b6320`. A stateless comparator marker selects the original two-line
clone/clear reference; the standard comparator selects the streaming path.
Node layout, allocator/resource identity behavior and input trace are identical.
Before timing, complete target contents are compared with std::map and the
source is checked empty. Timing has no accounting counters.

Each timed region contains only allocator-extended move construction, including
source destruction/release work performed by the migration. Source build and
final target destruction are excluded using paused timing. Both independent
resources forward to the same ordinary upstream allocator; there is no pool.
All objects are properly constructed and no reference survives mutation.
The shared focused trace uses seed 1656023321, five repetitions and a 0.03-second
minimum. Standard-library shuffle can differ across STLs; within each profile,
reference/candidate share the exact same input.

Windows uses i5-13500HX, Clang 18.1.8, MSVC STL 143.202408, Windows 11 build
22631, CPU0 affinity. Linux uses GCC 13.3.0/libstdc++ 20240904 and Clang
18.1.3/libstdc++ 20240908 sequentially on the same EPYC 7763 Ubuntu 24.04 worker,
without affinity pinning. Both use C++20 Release baseline
`-O3 -DNDEBUG -march=x86-64`. Different CPUs prevent assigning cross-platform
absolute differences to the OS. MSVC correctness is covered; no MSVC speed claim
is made. Sizes, key distributions and resources outside this small experiment
are not characterized.

[All 120 raw timing samples and 24 accounting rows](data/btree-compact-transfer.json)
retain the rejected legacy results. Full commands, CPU/OS data and source hashes
are in [Linux research run 41182743821](https://github.com/wjr-z/wjr/actions/runs/41182743821).
An earlier research-only oracle-name compilation failure produced no timing
samples and is excluded. Research markers, benchmark scaffolding and workflow
changes are not included in the production change or default CI matrix.
