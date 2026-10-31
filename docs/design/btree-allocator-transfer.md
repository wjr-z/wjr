# Copy and move into a supplied allocator

The map, set, multiset and experimental compact-map adapters support
`Tree(const Tree&, allocator)` and `Tree(Tree&&, allocator)`. These operate on the
same configured tree type. Existing default, comparator-only and ordinary
copy/move construction paths are unchanged.

Copy construction copies the comparator and clones values/nodes through the
supplied allocator. It does not call `select_on_container_copy_construction`.
The original tree retains its values and allocator.

Move construction copies the supplied allocator and moves the comparator. If
allocator compatibility is guaranteed by `is_always_equal` or runtime equality,
it uses the existing storage-transfer path. Allocated leaves are stolen without
per-element migration or new allocation. Compact SSO still relocates its at-most-
two values into the destination object; this does not promise std::map reference
stability for inline storage.

For unequal allocators, values are constructed through the destination allocator.
Legacy trees clear the original tree through its original allocator after
successful migration. Compact maps detach the source into private ownership state
and release its nodes incrementally; see the [peak-memory and CPU comparison](../performance/btree-compact-transfer.md).
These helpers are also used by unequal-allocator move assignment.
Previously, moving ordinary string keys could leave a unique set containing
multiple moved-from empty keys. Clearing the source restores a valid empty tree
without changing its allocator; it can be reused with its moved-from comparator.

Construction follows the actual allocator protocol, including allocator-aware
key/mapped construction. Ordinary value copy/move traits are not substituted for
that protocol: a type with allocator-extended constructors can be legal even
when its ordinary copy/move constructors are deleted. Conversely, a branch that
migrates elements must be supported by the destination allocator. A type-level
always-equal allocator can transfer indirect storage without requiring the
mapped object itself to move.

```cpp
#include <memory_resource>
#include <optional>
#include <wjr/container/btree_map.hpp>

std::pmr::unsynchronized_pool_resource persistent;
using allocator = std::pmr::polymorphic_allocator<char>;
using tree = wjr::btree_map<int, std::pmr::string, std::less<>, allocator>;
std::optional<tree> retained;
{
    std::pmr::monotonic_buffer_resource temporary;
    tree source(std::less<>{}, allocator(&temporary));
    source.try_emplace(1, 128, 'x');
    retained.emplace(std::move(source), allocator(&persistent));
} // The allocator-aware owning string now uses persistent, not temporary.
```

Resources must outlive objects that still use them. `clear` deallocates through
the proper resource, while a pool or monotonic resource can retain upstream
slabs until release/destruction. Allocation/value operations inside the existing
engine retain its no-throw/termination contract; these overloads do not add the
standard containers' exception guarantees or new fancy-pointer support.

The additions do not change node/tree layout, introduce an arena, or add
alignment calculations. Compatible storage transfer and unequal-allocator clone
reuse the existing engine. Focused tests cover SSO/allocated roots, comparator
state, allocator selection/identity, PMR const keys and mapped strings, source
resource destruction, over-aligned payloads, move-only/immovable values and
allocator-aware-only value construction.
