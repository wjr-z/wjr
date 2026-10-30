# Intrusive list iteration

`wjr::intrusive::list_node<T>` supplies a circular doubly linked hook;
`hlist_node<T>` supplies a null-terminated singly linked hook.
The hook is embedded in an externally owned object, and a separate hook can
serve as the list head. Linking and iteration do not allocate or retain ownership.
The caller keeps linked objects alive and preserves the existing link preconditions.

## Typed iteration

For an object derived from `list_node<T>`, mutable and const iterators satisfy
the C++20 bidirectional iterator concept, including reverse iteration.
The corresponding `hlist_node<T>` iterators satisfy the forward iterator concept.
Both expose `std::ptrdiff_t` as their difference type, so standard iterator traits,
ranges algorithms, and iterator adapters can use the existing traversal operations.

A mutable iterator can become a const iterator. The reverse conversion is rejected,
and a const node pointer cannot construct a mutable iterator.
Mutable and const iterators compare in either operand order.
These rules affect access through the iterator; they do not add ownership tracking.

`front()` returns the same hook pointer as `next()`, with the head's const qualification.
For `hlist_node<T>`, this is null when empty. For the circular list, an empty head
points to itself; it must not be dereferenced as a stored `T`.

## Storage and validation

The list hook stores two pointers, the hlist hook one, and each iterator one.
The associated types and constructor restrictions add no fields, allocations,
per-step validation, or traversal branches.
The `T = void` raw-hook use case retains its link operations; this iteration contract
does not require dereferencing a void iterator.

Tests check iterator concepts, one-way conversion, mixed comparison, ranges traversal,
reverse traversal, typed `front()`, empty lists, and the pointer-sized representations.
These interface corrections need compile and behavioral checks rather than a benchmark.

## Follow-up boundary

Future link-lifecycle work should start from the existing B-tree leaf chain,
`automatic_free_pool` chunk list, and `object_pool` raw hlist users.
Tag-based multiple hooks, splice, safe-iteration macros, and empty-node swapping
need separate contracts and focused tests. They are outside this interface correction.
