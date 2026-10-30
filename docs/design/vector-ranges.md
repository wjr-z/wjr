<!-- markdownlint-disable MD043 -->
<!-- A focused API and implementation contract. -->

# Vector range modifiers

`wjr::vector` and its existing storage specializations accept C++20 input ranges
through `assign_range` and `append_range`, alongside the existing `from_range`
constructor. These operations work in C++20 without depending on C++23 standard
library container-range support.

```cpp
wjr::vector<int> values;
values.append_range(std::views::iota(0, 8));
std::istringstream input("2 3 5 7");
values.assign_range(std::ranges::istream_view<int>(input));
```

The existing container-compatible range concept controls element conversion.
Assignment additionally requires the actual assignment expression, rather than
the stronger common-reference rules of `assignable_from`. Different iterator
and sentinel types, single-pass sources and moving ranges are supported.
The operations return `void`, matching the standard vector range modifiers.

Ordinary common-iterator assignment delegates to the existing assignment
implementation. Other sources reuse live elements and capacity. A known larger
replacement allocates before discarding old values, without first moving values
that will immediately be overwritten. Unsized input is consumed once and grows
through the existing insertion mechanism.

For append, sized or forward sources reserve the final capacity once and use the
existing allocator-aware uninitialized-copy primitive. Forward ranges may be
traversed once to obtain their distance; input ranges are never pre-consumed.
Bulk-copy failure destroys the new prefix before publishing its size. A failed
unsized single-pass append retains its completed prefix. Reservation follows the
existing vector relocation and exception contract and may invalidate iterators.
No temporary vector, per-element capacity check on the bulk path, new state, or
change to legacy `assign` / `append` is introduced.

Sources must not overlap the destination's live elements. Assignment can leave
a modified prefix after an exception; allocation failure before replacement
preserves the previous values. Fixed-capacity storage retains its existing
capacity precondition. Sized append checks size addition before reservation or
source traversal; oversized input does not wrap the destination size.

Focused validation covers non-common counted sentinels, forward and stream
ranges, moving elements, stateful allocators, storage reuse, lifetime balance,
throwing sources and oversized lazy ranges. This is a range interoperability
capability; it does not claim a throughput improvement or change growth policy.
