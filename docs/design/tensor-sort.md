# Value sorting

`<wjr/np/sort.hpp>` provides two rank-one operations:

```cpp
np::sort_inplace<np::sort_order::ascending>(values);
np::sort_to<np::sort_order::descending>(destination, source);
```

Both reuse `std::sort`, with no WJR workspace or result allocation. Sorting is
unstable: equal values, signed zeros and NaN payloads have no stable-order
guarantee. NaNs sort last in either direction. Infinities participate normally,
and integers retain their full precision rather than converting to double.

`sort_inplace` accepts writable packed arithmetic views. `sort_to` accepts a
packed writable destination and a packed, positive-stride or padded source,
preserving the source by copying directly into destination before sorting.
Both views have the same length and arithmetic dtype; volatile is excluded.
Output storage is disjoint from source. Conversion should be performed explicitly
before an in-place sort, rather than silently changing comparison precision.

Empty ranges can have null handles. Empty/singleton in-place sorting does not
read elements; a singleton `sort_to` still copies its value. The live packed
range must fit `ptrdiff_t`. Shape compatibility is an L2 caller precondition;
there is no release validation, alias scan or automatic temporary. Same-buffer
sorting belongs to `sort_inplace`.

The source-copy path knows that output is disjoint: it uses `std::copy_n` for
contiguous input and one stride-aware loop otherwise, without routing through
`copy_to`'s additional identity/overlap handling. Sorting delegates to the
standard implementation with a stateless compile-time order comparator. No
automatic ISA, stable-sort workspace, scheduler or custom sort backend is added.

The benchmark compares source-preserving copy plus sorting against the same
handwritten operations, including copying on every timed iteration. Repeated
timing therefore does not accidentally measure an already-sorted result. It
covers packed/stride input, random/ordered/reversed/duplicate-heavy/NaN values,
and empty, singleton, small and large arrays. This measures wrapper overhead
against the reused mechanism, not superiority over specialized radix/SIMD sorts.

See [the runnable example](../../examples/np/sort.cpp). Use `argsort_to` when
indices need to reorder several arrays, and `argpartition_to` when only selected
positions are required. Ordinary sorting followed by `run_offsets_to` provides
boundaries for the existing segmented reductions, without a grouping container.
