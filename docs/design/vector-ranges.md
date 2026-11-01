<!-- markdownlint-disable MD043 -->
<!-- A focused API and implementation contract. -->

# Vector range modifiers

`wjr::vector` and its existing storage specializations accept C++20 input ranges
through `assign_range`, `append_range` and `insert_range`, alongside the existing `from_range`
constructor. These operations work in C++20 without depending on C++23 standard
library container-range support.

```cpp
wjr::vector<int> values;
values.append_range(std::views::iota(0, 8));
std::istringstream input("2 3 5 7");
values.assign_range(std::ranges::istream_view<int>(input));
auto batch = std::views::iota(11, 14);
auto first = values.insert_range(values.cbegin() + 1, batch);
```

The existing container-compatible range concept controls element conversion.
Assignment additionally requires the actual assignment expression, rather than
the stronger common-reference rules of `assignable_from`. Different iterator
and sentinel types, single-pass sources and moving ranges are supported.
Assignment and append return `void`. Insertion returns an iterator to the first
inserted element, or the supplied position when the source is empty. Insertion
also requires move construction/assignment and the actual source assignment
expression; it does not impose the stronger `assignable_from` common-reference rule.

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

## Insertion and ownership

`insert_range(position, range)` accepts spans, lazy views, proxy references,
different iterator/sentinel types and move-only C++20 input iterators. A queued
sequence can splice a parsed stream or computed batch at its current cursor;
the [package consumer](../../test/consumer/vector_ranges.cpp) uses both without
building an intermediate container. Include `<wjr/vector.hpp>`; the ordinary
baseline `wjr::assert` target suffices for this consumer.

Sized or forward ranges insert through one counted bulk core, also used by the
existing forward iterator-pair insertion. A sized single-pass source calls begin
once and is not pre-consumed; an unsized forward source may have a distance pass.
The target grows at most once for a known count and reuses capacity otherwise.
No source-sized temporary vector is created. Size addition is checked before
narrowing or beginning traversal of a known oversized source.

Unsized input is consumed once, appending through the same counted core with the
existing geometric growth policy, then rotating the appended values into position.
The saved position is an offset, so it remains valid if storage changes. Input
failure can leave its completed prefix appended at the old end; no rotation has
then occurred. A throwing rotation may modify values but leaves every live element
owned by the vector. The operation does not promise rollback of input-stream effects.

Bulk insertion tracks each newly constructed segment separately until size/storage
publication. If construction, assignment or source iteration fails, it destroys
the new tail/prefix and releases uncommitted storage. Existing values can be
moved-from or partially assigned; allocation failure before construction preserves
them. This ownership cleanup also covers the reused legacy bulk-insert core.
Element/allocator destruction and deallocation must not throw. Fixed-capacity
storage keeps its existing sufficient-capacity precondition.

Rollback storage is compile-time disabled for pointer input with nonthrowing
element construction/movement, trivial allocator hooks and nonthrowing storage
transfer. Pointer cursors remain values; noncopyable input cursors are advanced
by reference. A Windows Clang 18 C++20 `-O3` raw-span probe and an equivalent
checked iterator-pair caller both lower to the same counted core, with no range
materialization. That integer core emits no rollback cleanup handler; this is a
scoped code-generation check, not a throughput or universal code-size claim.

Sources must not overlap the destination's live elements. Assignment can leave
a modified prefix after an exception; allocation failure before replacement
preserves the previous values. Fixed-capacity storage retains its existing
capacity precondition. Sized append checks size addition before reservation or
source traversal; oversized input does not wrap the destination size.

Focused validation covers non-common sentinels, forward and stream ranges,
moving values/iterators, proxy references, stateful allocator hooks, storage reuse,
returned positions, balanced lifetimes under constructor/assignment/iteration
failures, rotation failure and oversized ranges. The installed-package consumer
is included in the normal consumer tests. This is a range interoperability
capability; it does not claim a throughput improvement or change growth policy.
