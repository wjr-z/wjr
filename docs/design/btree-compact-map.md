# Compact scalar-key map prototype

`<wjr/container/experimental/compact_btree_map.hpp>` provides the opt-in
`wjr::experimental::compact_btree_map<Key, Value, Compare, Alloc, NodeSize>`.
`NodeSize` defaults to 16 slots. The ordinary `btree_map` remains unchanged.

```cpp
#include <cstdint>
#include <wjr/container/experimental/compact_btree_map.hpp>

wjr::experimental::compact_btree_map<std::uint64_t, std::uint64_t> counters;
counters.try_emplace(42, 1);
++counters.at(42);
```

This prototype targets small maps whose scalar keys can be copied into inner
separators. It stores actual `std::pair<const Key, Value>` objects in leaf slots,
including the existing two-slot storage inside the tree object. It does not
allocate a separate block for each pair. Allocations performed by mapped values
remain their own cost. The comparator and allocator instance constructors and
transparent lookup follow the existing map adapter.

The admitted key domain is unqualified scalar keys satisfying the existing
inline-key trait. The pair must be nothrow move constructible, and the mapped
value must be nothrow destructible. String keys and throwing relocation are not
admitted. Other constructors, allocator hooks and comparisons follow the existing
tree's failure contract: exceptions escaping noexcept operations terminate.
This does not provide the standard map's exception guarantees.

Every mutation may invalidate iterators, pointers and references. Existing
indirect map storage can retain the addresses of surviving value objects while
its leaf pointer slots move. Compact storage trades that mechanism for fewer
allocations and stores pairs at new addresses when elements shift or nodes split,
borrow or merge. It should not replace an address-sensitive use of the legacy map.

The slot protocol keeps unoccupied storage separate from live pairs. Right
shifts relocate backwards, left shifts relocate forwards after destroying the
erased value, and node transfers leave their source slots inactive. SSO moves
and swaps transfer only the live element count. Copy and allocator-unequal move
assignment construct destination pairs without prematurely ending source
lifetimes. Relocation uses the existing WJR customization contract for the actual
pair type; observable allocator construction and destruction hooks remain active.
No mutable-key pair is reinterpreted as a const-key pair.

Insertion constructs a stack temporary before shifting live slots, so arguments
can refer to an existing mapped value. Moving that temporary into the destination
adds a mapped-value move compared with constructing an indirect value directly
in its separately allocated block. Trivial relocation can avoid invoking a move
constructor during subsequent slot transfers; custom allocator hooks retain
their observable construction and destruction calls.

The existing linked leaves, parent and position metadata, compressed root
representation, two-element SSO, fixed-capacity searches and scalar separator
strategy remain in use. Legacy slot movement retains its pointer/trivial copy
specialization. Compact leaf alignment also accommodates over-aligned values.

The default is deliberately unchanged. Large mapped values increase both SSO
object size and the cost of unused leaf capacity and relocation. Whether avoiding
pair allocations improves total memory or time depends on value size, fill,
operation mix, allocator, compiler and platform. Compare the same slot capacity
with the focused B-tree benchmark and its separate memory-accounting pass;
allocation counts alone do not establish a throughput benefit.

Observed x64 Windows Clang/MSVC-ABI layout at capacity 16 (requested node bytes,
excluding allocator overhead):

| Key / mapped value | Legacy tree / leaf | Compact tree / leaf | Pair bytes |
| --- | ---: | ---: | ---: |
| uint64 / uint64 | 48 / 160 | 64 / 288 | 16 |
| uint64 / array<byte, 64> | 48 / 160 | 176 / 1184 | 72 |

Inner nodes are 288 bytes for both cases and strategies. Legacy maps additionally
request one pair-sized allocation per element. These layout facts do not predict
leaf fill or total resident memory.
