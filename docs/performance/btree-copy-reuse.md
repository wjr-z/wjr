# Single-pass indirect Btree copy reuse

`destination = source` now reuses compatible storage **during** a recursive
copy. There is no whole-tree same-shape preflight. Compatibility means the
existing node has the required storage kind/capacity, not that its values are
equal. Every source value is copy-constructed through its allocator, and every
old destination value is destroyed, including completely matching snapshots.

This serves ordinary map snapshots and indirect set/multiset keys without a new
API. Compact maps and inline-value sets keep their original copy path. Copy
construction, move and allocator propagation retain their existing protocols.

## Local ownership protocol

- Compatible inner nodes retain their allocation. Corresponding children are
  copied once; missing source branches are cloned and surplus old branches are
  released. The resulting child counts and separators match the source.
- Compatible leaves retain their node and `min(old_count,new_count)` value
  allocations. Surplus values are destroyed/deallocated; missing slots allocate
  and construct. Actual const-key pairs end and restart their complete lifetimes;
  no const key is assigned or reinterpreted.
- Different node kinds replace only that branch. Parent/position links are
  written for new nodes; reused children already occupy the correct parent slot.
- A retired leaf ring and a cursor track the next unconsumed old leaf. Reused
  leaves stay linked and advance the cursor. Fresh leaves insert before it;
  retired leaves advance it before unlink/free. This avoids rebuilding every
  existing leaf link on completely matching copies.

The destination publishes an empty root, size and sentry during reconstruction;
the source remains complete and unchanged. The completed graph is published
only at the end. SSO slots use the same value-prefix protocol; heap-to-SSO frees
the retired graph, and SSO-to-heap releases old inline-root values before cloning.
Existing allocator propagation still releases incompatible old storage through
its original owner before changing allocator state. The empty observation
guarantee applies inside the detached reconstruction engine; that pre-existing
propagation/release protocol is unchanged.

All operations remain inside the copy engine's existing `noexcept` boundary.
There is no new recovery guarantee or wider termination domain. Values may still
allocate internally, and copy assignment gains no reference-survival guarantee.
Unused child slots beyond the active count are not cleared or read.

## Frozen comparison

[Raw samples, ranges, CVs, memory passes and hashes](data/btree-copy-reuse.json)
compare main's engine `0a58a4d3`, the original #281 head `ce72ba29`, and the final
single-pass implementation `1a771930`. Linux
[run 41316931943](https://github.com/wjr-z/wjr/actions/runs/41316931943) runs GCC
and Clang on the same worker. Each profile uses the same fixture and N16, with
uint64 keys and uint64 or 64-byte array mapped values. Timing uses ordinary
`std::allocator`; counting is a separate executable. The order is
main/prior/candidate/candidate/prior/main, five raw repetitions per invocation,
minimum 0.005 seconds wall time. Ratios below one mean less time.

The 0/25/50/75/100 labels describe **paired leaf occupancy equality**, not value
equality or the fraction of reusable storage. The 455-leaf fixture has exactly
0/114/227/341/455 unchanged leaf occupancies. Inner kinds/counts remain equal,
so even the 0% occupancy case has reusable fixed-capacity nodes. Separate empty
and leaf-to-inner cases have no usable old graph. Other controls cover a late
50% mismatch, shuffled inner shapes, and alternating 4096/4095 sizes. Sources
alternate on every iteration, preventing a mismatch case from becoming matched
after its first copy.

| Profile / mapped | 0% | 25% | 50% early | 75% | 100% | 50% late | Random shape | Size mismatch |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Windows Clang / uint64 | .080 | .071 | .057 | .044 | .969 | .059 | .681 | .030 |
| Windows Clang / 64-byte | .109 | .095 | .089 | .079 | 1.027 | .092 | .650 | .065 |
| Linux GCC / uint64 | .144 | .131 | .115 | .102 | .891 | .115 | .726 | .086 |
| Linux GCC / 64-byte | .200 | .197 | .182 | .170 | .982 | .186 | .807 | .164 |
| Linux Clang / uint64 | .155 | .144 | .130 | .113 | .896 | .127 | .753 | .099 |
| Linux Clang / 64-byte | .208 | .198 | .186 | .168 | .935 | .187 | .818 | .165 |

These are candidate/original-#281 median wall-time ratios. Every partial,
random-shape and different-size observed envelope is below one. Completely
matching and all-fresh cases have envelopes crossing one: their previous gains
are retained without claiming a small additional improvement or zero overhead.
All raw comparisons against main are also retained in the JSON. Observed
`min(candidate)/max(reference)` to `max(candidate)/min(reference)` ranges are not
confidence intervals.

Windows uses Clang 18.1.8, MSVC STL 143.202408 and an i5-13500HX pinned to CPU0.
Linux GCC 13.3/Clang 18.1.3 use libstdc++ 20240904/20240908 on one EPYC 7763.
All use C++20, `-O3 -DNDEBUG -march=x86-64`. No cross-CPU absolute comparison or
MSVC-compiler throughput claim is made. Shuffle order may differ across STLs;
all engines within a profile consume the same trace.

## Allocation and peak costs

For the leaf-occupancy cases, main needs 4607 allocations/releases per copy.
Original #281 needs 4607 unless the whole graph matches. The single-pass copy
needs **228/171/114/57/0**, in either direction. Different-size copies need 0/1
instead of a complete rebuild. Random shapes still request roughly 2.8–3.0K
blocks in the Windows fixture: this is local reuse, not a global free-block pool.
Completely incompatible graphs retain the same fresh-copy allocation requests.

Final requested storage equals the source graph's footprint; no sparse extra
nodes are retained. Local growth can precede a later release. In the measured
redistribution fixture the temporary excess is at most two value blocks:
**32 bytes** for uint64 mapped values or **144 bytes** for 64-byte arrays.
This is a measured fixture bound, not a universal peak bound. Random/size/kind
controls retain every before/after/peak value. All memory passes finish with
zero live bytes and balanced allocation/deallocation counts. Requested bytes
exclude tree objects, allocator metadata and RSS.

The [initial evidence](data/btree-copy-reuse-initial.json) retains the rejected
broader compact/inline proposal and mature std/Abseil controls. In particular,
compact uint64 fallback had credible regressions; compact/inline remain excluded.
Abseil's lower retained footprint in that fixture remains a tradeoff. Historical
std/Abseil timings are not relabeled as measurements of this final implementation.

## Validation and reproduction

Final **14 MSVC Release**, **14 Windows ASan/UBSan**, and **14 Linux ASan/UBSan**
tests pass. Typed allocator ledgers verify parent/position, separator minima,
forward/backward leaf links, exact per-value copy/construct/destroy counts,
over-alignment, resource ownership and release. Coverage includes public self
copy, all occupancy rates early/late, a source/destination 8×8 size/kind/SSO
matrix, comparator changes, PMR indirect keys, and existing POCCA transitions.
No equal-value shortcut is used. Initial generic recursion incurred a credible
fully-matched slowdown; keeping reused ownership links and small leaf dispatch
removed it before this version was accepted.

Configure the existing btree benchmark profile and explicitly build
`btree_copy` and `btree_copy_memory`; both remain `EXCLUDE_FROM_ALL`. Their common
fixture separates timing/accounting. There is no new public configuration,
scheduled workflow or compiler matrix.

## Cursor ABI follow-up

The [bounded ABI experiment](btree-copy-cursor-abi.md) compares the current
scalar-key result plus cursor reference with returning the cursor directly.
The latter removes an indirect cursor store, but adds minimum propagation
through parent/position links and does not demonstrate a stable overall
benefit in Windows/Linux controls, including a fair GCC follow-up without the
avoidable clone-marker escape. The existing single-pass protocol is retained;
only the never-mutated clone insertion marker is passed by value.
