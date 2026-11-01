<!-- markdownlint-disable MD043 -->
<!-- This document specifies the LRU API and ownership contract. -->

# Owning fixed-capacity LRU cache

Include `<wjr/container/lru_cache.hpp>` and link the baseline header-only target
`wjr::lru_cache`. It is a core target, available after ordinary
`find_package(wjr CONFIG REQUIRED)`; it adds no ISA flags or third-party backend.
The [memoization example](../../examples/lru/README.md) is a complete package
consumer with ordinary strings, transparent lookup and an explicit PMR resource.

```cpp
wjr::lru_cache<int, std::string> cache(2);
cache.try_emplace(1, "one");
cache.try_emplace(2, "two");
cache.get(1);                // 1 becomes most recently used
cache.try_emplace(3, "three"); // evicts 2
```

The cache owns one Key and one T per entry. Its standard unordered-map node
also contains WJR's typed intrusive hook and a pointer to its own const key.
No separate list element, duplicate owning key, virtual policy, reference count,
automatic loading callback or custom allocator implementation is introduced.
The cache object cannot be copied or moved because its sentinel and key pointers
participate in that ownership graph. Payloads need not be copyable, movable,
or default constructible when their selected construction operation is valid.

## Operations and borrowing

Capacity counts entries, is fixed at construction, and does not bound the bytes
owned by keys/values, buckets, allocator caches or process RSS.

| Operation | Result and recency |
| --- | --- |
| `capacity`, `size`, `empty` | Observe without promotion or allocation |
| `get(key)` | Return `T*` or null; a hit becomes MRU |
| `peek(key) const` | Return `const T*` or null; no promotion |
| `try_emplace(key, args...)` | `pair<T*, bool>`; hits skip T construction |
| `insert_or_assign(key, value)` | Assign or insert; promote on success |
| `erase(key)` | Report removal; keep other entries in order |
| `clear` | Destroy entries; retain capacity and buckets |
| `get_allocator` | Return the allocator with its stored state |

There is no default-constructing `operator[]` or separate alias for `get`.
Capacity zero stores nothing and insertion returns `{nullptr, false}` without
constructing a stored key/value. Function arguments, implicit key conversions
and their side effects are still evaluated by C++ before the call. The backend
may allocate its ordinary empty-container bookkeeping; capacity zero does not
reserve an extra entry.

Pointers survive promotion and mutations to other entries, including insertion
that changes buckets. They expire when their own entry is erased/evicted, on
clear, or on cache destruction. Returned pointers are borrowed: retain/copy the
value independently if its lifetime must exceed the entry's lifetime. Arguments
borrowed from this cache remain valid until construction/assignment completes;
eviction happens afterward. Do not dereference an evicted argument after return.

This implementation is single-threaded. Even get on a hit changes links.
External synchronization is required for shared access. User Hash/Equal,
allocator operations and payload construction, assignment or destruction must
not reenter the same cache. Concurrent and sharded interfaces are separate work;
returning this raw pointer after releasing a lock would not extend its lifetime.

## Failure transaction

Hash must be callable without throwing, including conversion of its result to
`size_t`. Hash/Equal must satisfy the standard container equivalence, stable-key
and equal-keys/equal-hash requirements. Equal and key/value construction may throw.
Key/T destruction and allocator destroy/deallocate follow the standard
nonthrowing contract.

For positive capacity C, construction checks C+1 arithmetic and backend limits,
then reserves C+1 entries. Allocation or length failures propagate and completed
members clean up through RAII; the incomplete cache's destructor is not needed.
The map/load-factor/bucket interface is private and capacity cannot change.

A missing insertion first locates the LRU victim, if full, then constructs the
new node. Until insertion succeeds, the old membership and order remain intact.
After success, setting the key pointer and linking the MRU cannot throw; the
saved victim iterator is then unlinked and erased. Reserving C+1 prevents that
temporary admission from invalidating the iterator through rehash. The operation
may briefly own C+1 values; successful completion restores `size() <= C`.

The standard single-element insertion guarantee excludes exceptions from Hash,
which is why this first domain rejects potentially throwing hashes at compile
time. Pointer stability alone would not establish this transaction for a dense
backend. See the [storage design](../design/lru-cache.md) for the separate inline,
stable-pointer and node-table alternatives.

Failed lookup/erase leaves recency unchanged. Failed existing-value assignment
also leaves recency unchanged; the value keeps its assignment operator's own
guarantee, which may permit partial modification. User callback side effects and
modifications of aliased arguments cannot be rolled back. Neither all operations
nor an arbitrary user value assignment are advertised as noexcept or strongly
exception safe. Clear and destruction do not follow stale links through freed
objects: they reset the head and destroy nodes through the owning map.

## Allocators and heterogeneous lookup

The default allocator is `std::allocator<pair<const Key, T>>`; it is rebound to
the actual internal pair/entry type. The backend's allocator_traits construct/
destroy hooks manage real nodes. T is constructed with standard uses-allocator
arguments; a scoped allocator's inner allocator is selected for that payload.
This also supports allocator-aware values with no ordinary matching constructor.
No placement-new shortcut bypasses allocator node hooks.

For PMR, pass an explicit `polymorphic_allocator<pair<const Key, T>>` as the
fourth constructor argument, after Hash and Equal. The resource is borrowed and
must outlive every cache allocation and destruction. A PMR-aware T receives the
resource. A plain `std::string` remains a plain string; an unrelated allocator
does not automatically redirect its character buffer. Key construction follows
the standard map/pair allocator protocol as well. Do not release the resource
until the cache is destroyed: even an empty cache after clear can retain buckets.

Get, peek and erase accept heterogeneous queries only when both Hash and Equal
declare `is_transparent`, the query hash is nonthrowing, and equality is callable
in both directions. Hash/equality consistency across key and query types remains
a semantic precondition. This uses C++20 heterogeneous find and iterator erase;
it does not require C++23/C++26 heterogeneous erase or insertion. Insertion takes
Key lvalues/rvalues; it does not promise a transparent insertion facility.

## Cost and validation scope

Qualified hashing gives expected constant-time lookup; worst-case lookup remains
linear. Promotion does no allocation or payload movement and skips relinking an
already-MRU entry. A full miss still performs a new-key lookup, victim lookup,
map insertion and eviction. The extra bucket reservation and temporary node are
real costs. This capability introduces no universal throughput or memory-saving
claim against Boost.MultiIndex, Folly or dense hash implementations.

The standard backend was selected for the initial complete API because it adds
no dependency, owns each key once, preserves addresses and supplies the required
single-element insertion guarantee. Dense inline entries are not ruled out:
their move/erase hook repair, exception domain and weaker borrowing contract
must be proved with the actual fixed backend release before adoption. Concurrent
locking/sharding likewise cannot be inferred from this single-threaded type.
