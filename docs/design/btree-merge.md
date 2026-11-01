# Merge compatible B+ tree owners

`destination.merge(source)` combines independently accumulated batches without
reconstructing legacy indirect values. It also accepts `std::move(source)`;
the source object itself is not replaced. A unique destination keeps existing
equivalent keys and leaves those conflicting elements in source. A multiset
destination transfers every element. Self merge has no effect.

```cpp
#include <cassert>
#include <wjr/container/btree_map.hpp>

struct record {
    int value;
    explicit record(int value) : value(value) {}
    record(const record&) = delete;
    record(record&&) = delete;
};

wjr::btree_map<int, record> index, incoming;
incoming.try_emplace(7, 42);
const auto* address = &incoming.begin()->second;
index.merge(incoming);
assert(incoming.empty());
assert(&index.at(7) == address);
```

This supports reducing batch/shard results into an index, including mapped
objects that cannot be copied or moved. Compare and NodeSize may differ; each
candidate is checked using the destination's comparator rather than assuming
the two ordering states are equivalent. Source comparator and allocator state
are retained. Source remains usable after merging.

An empty destination can adopt a heap graph directly when both trees have
identical traits and use `std::less<Key>`, `std::less<>`, `std::greater<Key>` or
`std::greater<>`. There are no conflicts in an empty destination, and these
comparators do not carry differing ordering states. The existing move-tree
storage operation transfers the root, size and leaf-list links while retaining
both comparator and allocator instances. No node or value is allocated,
destroyed or reconstructed. Existing objects keep their lifetimes, so allocator
value hooks are not called for adoption; eventual destruction still occurs
through an equal allocator. Source is empty and reusable afterward.

SSO, nonempty destinations, custom comparators and differing traits retain the
per-element loop. In particular, comparator type equality alone does not permit
adopting a custom comparator's graph: its two instances can order keys differently.
The heap-only boundary also retains the existing SSO construction/destruction
hooks. No automatic hinted per-element merge is enabled; a bounded experiment
found sparse-batch regressions. See the [graph-adoption measurements](../performance/btree-owner-adopt.md).

## Domain and allocator ownership

A named concept requires identical key/value and actual allocator types, plus
the same storage policy. Legacy map/set/multiset may merge with compatible
legacy counterparts; compact maps merge with compact maps. Mixed legacy/compact
storage is intentionally rejected at compile time. This avoids silently
relocating a value that the legacy storage policy keeps at a stable address.
Different value types use ordinary range conversion/insertion instead.

The allocators must compare equal, as in the
[standard associative merge requirements](https://eel.is/c++draft/associative.reqmts#113).
Debug builds check that precondition for stateful allocators. Unequal-resource
migration is served by the existing [allocator-extended constructors](btree-allocator-transfer.md),
which reconstruct through the supplied allocator. Merge does not propagate or
replace either allocator.

Legacy indirect values are extracted without destruction and their existing
pointers are inserted into the destination. No value allocation, copy, move or
value construction/destruction is required for a transferred indirect object.
Source and destination tree nodes still use their own typed allocation paths;
destination splits may allocate nodes. This is not a zero-total-allocation claim.
An adopted value that contains its own allocator/resource retains it; every
resource must outlive values that still refer to it, including transferred ones.

Inline legacy keys are copied through existing allocator construction and source
destruction hooks on the per-element path. Compact pairs use the existing allocator-aware temporary and
legal slot relocation protocol. Duplicate rejection happens before either path
consumes the candidate's value. A rejected compact value can still relocate
inside source when another element is removed, following its ordinary policy.

## Invalidation and failure behavior

Transferred indirect legacy values keep their addresses, including const keys
and immovable mapped objects. Iterators and inline/compact references follow
WJR's existing mutation invalidation rules; standard-container iterator-stability
guarantees are not added. No mutable const-key pair proxy or public node handle
is introduced.

The per-element path calls the existing key search, insertion and source rebalance engines.
Their comparison/allocation/value no-throw contract remains in force. Allocator
hooks must not reenter mutation of either tree. This operation does not add the
standard containers' recoverable exception guarantees or support for unequal
allocators. Successful transfer has exactly one owning tree for each object.

The source eraser has a compile-time destruction policy: extraction omits only
the selected indirect object's destruction. Ordinary erasure selects its original
destruction path without a runtime policy branch. A Windows Clang check of the
normal single-iterator erase wrapper/callees found identical assembly after only
normalizing the changed private template symbol and comments.
Using merge can instantiate an additional extraction eraser for the source type;
this adds code for the new operation rather than per-element runtime dispatch.

## Focused checks

Tests cover SSO and multi-level roots; destination equivalence differing from
source; typed allocator identity and exact deallocation bytes/alignment; source
conflicts and self/rvalue merge; scalar allocator hooks; immovable mapped values;
long string keys; over-aligned move-only payloads; and compact PMR strings with
shared resources. Compile tests reject const sources, mismatched values/allocators
and mixed storage policies.

For compatible legacy maps with 0/1/2/3/16/80/512 source elements, counters verify
zero additional value allocations, zero value constructions/destructions and
zero mapped moves during merge, while checking every transferred address.
These are ownership/cost properties, not a throughput comparison with std::map
or Abseil. No default layout or performance policy changes.
