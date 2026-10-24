# Explicit-boundary histogram

`<wjr/np/histogram.hpp>` provides `histogram_to(counts, values, edges)` using
caller-owned rank-one views. It first clears every logical count, then visits
values once and increments the matching bin. It allocates no storage, retains
no input ownership, and does not materialize an array of bin indices.

Bin `i` covers `[edges[i], edges[i+1])`; the final bin includes its right endpoint.
For edges `[0, 1, 1, 3]`, value 1 belongs to bin 2 and bin 1 remains empty.
If the last bin has equal endpoints, it contains that endpoint: edges `[2,2,2]`
and value 2 increment the last bin. Signed zero compares equal. NaN values and
values outside the closed overall domain are ignored; infinite edges are allowed.

## Contracts

- Counts are writable nonvolatile integral elements, excluding `bool`.
- Values and edges have the same arithmetic dtype, ignoring `const`; bool and
  volatile inputs are excluded. Comparisons preserve that dtype, including
  full-width integers. There is no intermediate floating-point conversion.
- All views are rank one, with packed, positive-stride, or padded layouts.
  Counts may have holes; only their logical elements are cleared and incremented.
- At least two edges are required, with exactly one more edge than bins.
  This metadata relationship uses a level-two boundary assertion with the
  overflow-safe form `edge_count >= 2 && edge_count - 1 == bin_count`.
- Edges are nondecreasing and contain no NaNs. These are trusted preconditions;
  there is no repeated sortedness or NaN validation scan.
- Every final bin count must be representable in the output dtype. Total input
  length may exceed that dtype's maximum when values occupy different bins.
  Incrementing a bin has no overflow check.
- Output storage is disjoint from both inputs. Values and edges may overlap each
  other, and remain stable for the call. No alias scan or defensive copy is made.
- Empty values still clear all counts, then return without reading edge elements.

## Implementation

After the closed-domain test `first <= value && value <= last`, only the internal
boundaries `edges[1..bin_count)` need searching. The upper-bound position in this
range is already the output bin index. This is equivalent to the documented
half-open intervals and closed final interval, including duplicate edges, while
avoiding a per-value special case or clamp for the final endpoint.

Packed and padded rank-one edges use `std::upper_bound`. Positive-stride edges
reuse the existing private `searchsorted_index<right, false>` binary search.
The domain test already rejects NaN values, so no second NaN dispatcher is used.
There are no automatic edge strategies, weights, density normalization, sorting,
thread scheduling, SIMD policy or always-on validation framework in this API.

Time is `O(bins + values * log(bins + 1))`, excluding early rejection of out-of-domain
values. Extra storage is constant. Packed standard-library pointer ranges must
also satisfy the usual representable iterator-distance requirement.

The example uses explicit floating-point bins. The benchmark includes the
necessary count-clearing pass and compares complete calls to a handwritten
upper-bound implementation with identical domain/end-point/NaN semantics and
alias constraints. Performance evidence is reported separately from source-level
allocation and traversal properties.
