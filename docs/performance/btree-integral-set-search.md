# Integer-set search at capacity 32

Capacity 32 reduces node overhead compared with capacity 16, but the larger
node also changes the search strategy. This change improves the explicit N32
integer set/multiset configuration. It does not change the default capacity,
node layout, allocation policy or map search.

The new path applies only when Key and Value are the same integral type, the
query has that exact type, and Compare is `std::less<Key>` or `std::less<>`.
It checks pivots 16 and 8 within the selected half, then reuses the existing
step-two scan. All partial node sizes 1..32 remain valid. Other capacities,
custom comparators, heterogeneous queries, strings and maps keep the existing
searcher. Comparator recognition is by exact type; an arbitrary callable is
not assumed cheap or replaced by a built-in comparison.

```cpp
#include <cstdint>
#include <wjr/container/btree_set.hpp>

using compact_nodes = wjr::btree_set<std::uint64_t, std::less<>,
                                    std::allocator<char>, 32>;
compact_nodes keys;
keys.insert(42);
const auto found = keys.lower_bound(std::uint64_t{42});
```

This is a workload choice, not a universal recommendation to use N32. In the
Linux Clang profile below, optimized N32 lookup still costs 1.177 times N16 for
the selected integer set. The same profile's mixed trace costs 0.698 times N16.
The change itself does not save memory relative to old N32: all storage and
insertion positions are unchanged.

## Fixed-capacity comparison

Ratios below are median candidate time / median original N32 time; below one
means less elapsed time. Each bracket is `min(candidate)/max(reference)` through
`max(candidate)/min(reference)` across five raw repetitions. These are observed
envelopes, not confidence intervals. The JSON retains every sample and each
group's min/max relative to its own median, including map results rejected for
the production path.

| Profile | Set build + destroy | Set lower bound | Set mixed 90/5/5 |
| --- | ---: | ---: | ---: |
| Windows Clang 18 | 0.781 [0.664..0.819] | 0.559 [0.486..0.615] | 0.626 [0.602..0.650] |
| Linux GCC 13 | 0.784 [0.778..0.823] | 0.663 [0.658..0.692] | 0.598 [0.566..0.604] |
| Linux Clang 18 | 1.027 [0.976..1.063] | 0.950 [0.945..0.957] | 0.463 [0.448..0.528] |

Lookup and mixed updates improve in all three profiles. Linux Clang build is
not separated from a tie by the observed ranges; its 2.7% median increase is
retained. The map prototype was rejected: Linux Clang map lower bound costs
1.140 [1.128..1.144] times the original N32 path, despite mixed-update gains.
This is why the selector requires Key == Value. These results do not justify a
compiler-specific default or a general cheap-comparator map optimization.

The N16 context is measured in the same executable, not inferred from an older
campaign:

| Profile | Set build N32/N16 | Set lookup N32/N16 | Set mixed N32/N16 |
| --- | ---: | ---: | ---: |
| Windows Clang 18 | 1.036 | 0.905 | 0.900 |
| Linux GCC 13 | 0.751 | 0.756 | 0.967 |
| Linux Clang 18 | 1.053 | 1.177 | 0.698 |

## Contract and provenance

The reference and candidate have identical N32 node/value storage, empty
standard comparator semantics, allocator type, input permutation and operation
trace. A research-only comparator marker chooses the alternative private
searcher; it adds no state and invokes the same standard integer comparison.
Complete results are checked against std containers before timing. The N16
context uses the original implementation. No iterator/reference survives a
mutation; no std::set stability or exception guarantee is implied.

Only integer set and integer map, N4096, are measured. The existing focused
trace and successful-operation contract are reused: shuffled build + destroy;
2048 lower-bound queries with half the query keys present; and 5120 mixed events
(90% lookup, 5% erase, 5% insert). Inputs and the complete-content oracle are
outside timing. Mixed setup/destruction are excluded; build includes them.
Timing uses `std::allocator` without counters, five repetitions, 0.03-second
minimum, wall time, C++20 Release baseline. There is no ISA-specific kernel.

| Profile | CPU / tools | Flags |
| --- | --- | --- |
| Windows | i5-13500HX; Clang 18.1.8; MSVC STL 143.202408; Windows 11 build 22631; CPU0 affinity | `-O3 -DNDEBUG -march=x86-64` |
| Linux GCC | EPYC 9V74; GCC 13.3.0; libstdc++ 20240904; Ubuntu 24.04 | Same baseline flags |
| Linux Clang | Same Linux worker; Clang 18.1.3; libstdc++ 20240908 | Same baseline flags |

Linux compilers run sequentially on the same worker without affinity pinning.
Windows uses a different CPU and STL: compare ratios within profiles, not
absolute durations across operating systems. The trace uses seed 1656023321;
std::shuffle can differ across STLs, while each profile's backends share inputs.
MSVC correctness is tested; this report contains no MSVC timing claim. The data
do not establish throughput at other sizes, capacities or key distributions.

The frozen research source is `a5739b0861f125b9c3271d37729c0b8bf3701ad9`, based
on main `ac00a85d`. The production source uses the same two-pivot function and a
strict compile-time selector instead of the marker. Windows Clang's standalone
helper assembly is byte identical between the frozen and production functions.
Debug precondition checks are retained without adding release optimizer
assumptions that changed the measured helper's generated scan. N16, map N32,
custom-comparator N32 and heterogeneous-query N32 query-wrapper assembly is
byte identical before/after. These checks establish the intended implementation
seam, not a hardware-level explanation for every timing difference.

[Checked-in raw samples, ratios and dispersion](data/btree-integral-set-search.json)
retain 270 samples and original-file hashes. Full source, compiler commands,
CPU/OS details and Linux raw output are available in the
[bounded research artifact](https://github.com/wjr-z/wjr/actions/runs/41092492178).
The research-only workflow is not part of this change. It runs only the two
types, three operations and fixed N4096; no new default benchmark or CI matrix
is introduced. Existing std/Abseil comparisons remain in the
[baseline report](btree-showcase.md); this experiment isolates a same-capacity
search change and does not claim a new ranking against those libraries.
