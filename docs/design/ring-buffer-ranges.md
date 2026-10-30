<!-- markdownlint-disable MD043 -->
<!-- These sections describe the range contract and its validation. -->

# Ring buffer C++20 ranges

`wjr::ring_buffer` accepts standard input ranges through `wjr::from_range`,
`assign_range` and `append_range`. This includes counted iterators with a
different sentinel type and single-pass stream views.

```cpp
std::istringstream input("2 3 5 7");
wjr::ring_buffer<int> values(
    wjr::from_range, std::ranges::istream_view<int>(input));
values.append_range(std::array{11, 13});
values.assign_range(std::views::iota(0, 4));
```

The element constraint reuses the vector range constructor's existing
`_container_compatible_range` concept. Assignment additionally requires the
actual element-assignment expression. It does not impose the stronger
`assignable_from` common-reference requirement on proxy sources.

Sized and forward construction allocates the destination buffer once.
Single-pass construction grows through the existing insertion path and cleans
up live elements and allocation if source iteration fails. Sized append reserves
the final capacity before traversal; unsized append visits the source once.
Assignment reuses existing elements and capacity. Common legacy iterator ranges
retain the existing segmented assignment implementation.

Sources passed to append or assignment must not overlap the destination's live
elements. Iteration and element operations may throw: completed appended values
remain owned, and assignment may have modified its prefix. No transactional
rollback of arbitrary user assignment is promised. Constructor failure destroys
all constructed elements and releases its allocation.

Iterators now meet the C++20 random-access requirements, including default
construction and ordering by logical position across wrapped storage. This makes
the buffer usable with standard ranges algorithms such as sort and reverse.
Ordering and subtraction require valid iterators from the same buffer.
Value-initialized iterators support equality comparison; arithmetic and ordering
on these singular values are outside the buffer iterator contract. Neither
operation adds a runtime equality test before logical position calculation.
These changes add no fields to iterators or buffers. Existing mutation
invalidation rules are unchanged.

Focused tests cover mutable/const iterator concepts, wrapped storage, counted
sentinels, streams, move-only elements, allocator identity and allocation count,
throwing sources, capacity reuse and oversized lazy ranges. No throughput claim
or additional benchmark is introduced by this interoperability change.
