# Argsort: borrowing values, sorting indices

`<wjr/np/argsort.hpp>` provides:

```cpp
argsort_to<sort_order::ascending, sort_stability::unstable>(indices, values);
```

The rank-one source is borrowed, remains unchanged and may use packed,
positive-stride or padded storage. The destination is a packed, writable integer
view of the same length. Its element type excludes `bool`; its largest value
must represent `size - 1`. Consequently `uint8_t` can index 256 elements.
The views must have disjoint storage. Empty views may have null handles.
Length and index-width preconditions use level-2 assertions, with no per-element
release validation or implicit workspace allocation.

The implementation initializes indices to `[0, size)` and applies `std::sort`
with an indirect comparator. Integers retain their original precision; values
are not converted to double. NaNs sort after every non-NaN in both directions,
including infinities. Signed zeros compare equal.

`sort_stability::stable` compares original indices when values tie, including
ties between NaNs. This produces stable results using the same index buffer,
without `std::stable_sort`'s potential temporary allocation. It adds equality
and tie-breaking work; unstable ordering leaves equivalent indices unordered.
This is not an assertion that both modes have equal cost.

Packed readers capture only the source pointer. Strided readers additionally
capture their stride; no source copy or owner retention is introduced. Sorting
uses `O(N log N)` comparisons and the standard sort implementation's call stack;
there is no heap workspace. The function is constexpr with a C++20 standard
library supporting constexpr sorting.

The benchmark compares the whole operation, including index initialization,
against an equivalent direct pointer implementation of `std::sort`. Cases
include packed/strided input, both stability choices, duplicate-heavy values,
NaNs and empty, singleton, short and large inputs. Allocations for fixtures are
outside timing. This establishes abstraction cost against the reused sorting
mechanism, not optimality over specialized radix or SIMD sorting algorithms.
