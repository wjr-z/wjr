# Indexed multi-selection

`<wjr/np/argpartition.hpp>` provides:

```cpp
argpartition_to<sort_order::ascending, partition_method::nth_element>(
    indices, source, std::span(ranks));
```

It writes a permutation of `[0, N)`. At every requested position `r`,
`source[indices[r]]` has the value that would occur at `r` after sorting.
Earlier elements compare no greater and later elements no smaller, according
to the chosen direction. Elements within the resulting partitions are unsorted.
Ties have unspecified order, including equal NaNs and signed zeros.

The source remains unchanged. It is rank one and can be packed, padded or
positive-stride. Output is a packed integer view of equal length; its element
type is non-boolean and must represent `N - 1`. `uint8_t` therefore supports
256 source elements. Both ascending and descending place NaNs after all other
values; full-width integers are compared without conversion to floating point.

Ranks are nonnegative, in range and nondecreasing; duplicates are allowed.
Output storage is disjoint from source and ranks. The live output range must
fit `ptrdiff_t`, as required by the underlying standard algorithms. Length,
rank and index-width preconditions use L2 assertions, not a release validation
pass. There is no allocation, source copy, ownership retention or hidden buffer.

Initializing the permutation requires `N` writes, even when no ranks are
requested. Empty ranks produce the identity permutation and read no source
elements. Empty source/output with empty ranks can have null handles. This
differs from in-place `partition_at`, which leaves its input alone for no ranks.

## Shared implementation and cost

`argpartition_to` and `argsort_to` share the pointer/stride reader and ordering
implementation. Selection reuses `partition_at`'s private rank selector on the
index buffer, with comparison reading the borrowed source. The default balances
requested ranks with `std::nth_element`; `shared_pivot` remains explicit. It is
not automatically chosen based on array length or compiler.

Comparator construction state is passed only for indirect selection. The
existing value-selection and quantile paths still have no state argument;
their partition predicates retain a pivot-only capture. There is no virtual
call, type erasure, per-element mode switch or persistent plan.

Indirect selection must initialize indices and perform indirect value reads;
it is not necessarily faster than moving small scalar values. Its purpose is
to preserve the source and reuse the selected indices for several arrays.
The benchmark times initialization and selection together against handwritten
balanced selection and full index sorting. A descending partition at `k - 1`
selects the largest `k` values in an unsorted prefix; `take_to` can materialize
that prefix without requiring a separate top-k result type.

See [the runnable example](../../examples/np/argpartition.cpp).
