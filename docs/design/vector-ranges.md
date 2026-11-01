<!-- markdownlint-disable MD043 -->
<!-- A focused API and implementation contract. -->

# Vector range modifiers

`wjr::vector` and its existing storage specializations accept C++20 input ranges
through `assign_range`, `append_range`, `insert_range` and `replace_range`, alongside the existing `from_range`
constructor. These operations work in C++20 without depending on C++23 standard
library container-range support.

```cpp
wjr::vector<int> values;
values.append_range(std::views::iota(0, 8));
std::istringstream input("2 3 5 7");
values.assign_range(std::ranges::istream_view<int>(input));
auto batch = std::views::iota(11, 14);
auto first = values.insert_range(values.cbegin() + 1, batch);

std::istringstream updates("20 21 22");
values.replace_range(values.cbegin() + 1, values.cbegin() + 3,
                     std::ranges::istream_view<int>(updates));
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
capability; it does not claim a throughput improvement.

## Replacing an interval

`replace_range(from, to, range)` replaces the half-open interval `[from, to)` and
returns the vector by reference, matching the existing iterator-pair `replace`
operation. A queued sequence can apply a parsed stream or a lazy computed view
without first materializing a source container. The positions must be ordered
and belong to the target; the source must not overlap its live elements. The
element conversion and move/assignment requirements match range insertion.

A sized or forward source obtains its count before modifying the target. Final
size is checked before narrowing, and a sized oversized source is rejected before
calling begin. An unsized forward source may need a distance pass. The operation
assigns the common prefix, then either shifts the existing suffix left or inserts
the excess through the existing counted insertion core. A known count grows the
target at most once and reuses storage when capacity suffices.

Unsized input is consumed once. After filling the old interval, any remaining
input appends through that same counted core before rotation into position. This
also supports move-only C++20 input iterators and distinct sentinel types; no
temporary vector or source-sized storage is used. An empty or exactly matching
input leaves the untouched suffix in place rather than move-assigning it to
itself. Growth saturates at the size type's representable limit, so doubling a
narrow allocator size type near its limit does not degrade to repeated single
element reallocations. The existing maximum-size and fixed-capacity contracts
remain in effect.

Replacement provides a basic exception guarantee. Assignment, construction or
source iteration may already have changed values; allocation failure after
prefix assignment need not restore that prefix. All live elements remain owned,
uncommitted construction/allocation is cleaned up, and the vector stays usable.
Input failure can leave its completed tail at the old end before rotation. A
throwing rotation can leave values partly permuted, without abandoning live
elements. Destruction and deallocation must not throw, as for insertion.

Validation compares interval/length combinations against ordinary sequence
editing, checks non-common forward and single-pass views, moving/proxy sources,
storage reuse, allocator/element/iteration failures, and actual narrow size-type
growth and overflow. The installed-package consumer applies a stream update to
its existing work queue through this API.

## Single-element insertion ownership

`emplace_back`, `push_back`, `emplace` and single-element `insert` retain ownership
when construction, allocator hooks or element transfers throw. On growth, the
incoming object is constructed before moving its argument sources; this preserves
valid references into the old vector. Allocation and completed incoming/prefix/
suffix regions remain guarded wherever unwinding can occur. A throwing transfer
leaves the old objects alive, although their values can be moved from; no value-restoration
guarantee is added. Allocation failure before construction leaves the old vector
unchanged. Destruction and deallocation must not throw.
Recovery also assumes a nonthrowing storage commit, as provided by the built-in
growable storages; a custom storage retains its own `take_storage` contract.

With spare capacity, middle insertion guards the extra tail object until element
shifting and assignment finish. An assignment failure can partially change the
old values, while every live object remains owned and the old size stays valid.
The temporary incoming value keeps its existing allocator/lifetime behavior.

Existing paths whose incoming construction, allocator lifetime operations and
transfer cannot throw retain their direct implementation. The guarded growth
path shares the bulk-append transaction core; its optional middle offset adds no
runtime argument to append calls. Destructive relocation is used only with the
existing relocation permission and nonthrowing allocator/commit requirements.
Move-only values, construct-only end insertion, inline storage and allocator
hooks keep their existing domains. No initialization pass or intermediate vector
is introduced.

## Bulk insertion and replacement ownership

Count/value insertion and the legacy fill/forward-range `replace` overloads use
that same transaction when growth can unwind. Replacement skips the removed old
interval during transfer and destroys those old objects on commit. Its incoming
range is still constructed in full before any old element is moved; allocation
failure therefore does not assign an old prefix. This preserves the legacy
ordering, distinct from the prefix-first `replace_range` contract above.

With spare capacity, both shift layouts guard the complete newly constructed
tail until all construction and assignment finishes. A later allocator hook,
source increment, move or assignment failure destroys the uncommitted tail;
the old size remains valid. Values can be moved from or partly assigned, so this
is an ownership/basic guarantee. Existing nonthrowing construction/assignment
paths discard the guard at compile time and retain their direct growth code.
No intermediate vector or extra allocation is added.

Focused validation covers each incoming and surviving-element failure point,
both spare-capacity layouts, small/inplace storage, move-only resources,
self-referencing fill arguments and a throwing forward-source increment. Windows
Clang 18/C++20/baseline/O3 produces identical full assembly for the nine ordinary
integer, unique-pointer and small-vector callers in the
[caller probe](references/vector-emplace-codegen.cpp), including emitted helpers.
MSVC 19.42/O2/Ob2 produces identical instruction bodies and relocation multisets
for all 402 emitted functions/sections in this probe. These are caller/codegen
checks, not final library-size or exception-path throughput claims.

Compile the probe with `clang++ -std=c++20 -O3 -march=x86-64 -DNDEBUG
-DWJR_DEBUG_LEVEL=0 -Iinclude -Ithird-party/fmt/include -S
docs/design/references/vector-emplace-codegen.cpp`. For the reference, place
`include/wjr/container/vector.hpp` from commit `035303d5` in a separate include
overlay and put that overlay first. Keep the compiler, ISA and exception flags
equal. Linux CI provides the compiler and ASan coverage; local Windows EH testing
uses ordinary Release builds rather than the affected Clang 18 ASan runtime.
