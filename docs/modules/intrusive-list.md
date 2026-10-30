# Intrusive list transfers

`<wjr/container/list.hpp>` provides a non-owning circular doubly-linked ring.
The sentinel and each element contain two pointers; there is no stored size,
allocation, ownership tracking, or automatic unlinking on destruction.
Keep every linked object alive until it is removed or the ring is discarded.

The destination node supplies the insertion position:

```cpp
#include <wjr/container/list.hpp>

struct job : wjr::intrusive::list_node<job> {
    int priority = 0;
};

wjr::intrusive::list_node<job> ready, deferred;
job first, second;
ready.push_back(&first);
ready.push_back(&second);

// Defer one job, then return the entire deferred batch to ready's end.
deferred.splice_before(&first);
ready.splice_all_before(&deferred); // ready: second, first; deferred: empty

// Move the prefix [second, first) before deferred's sentinel.
deferred.splice_before(&second, &first); // ready: first; deferred: second
```

All three operations are `constexpr`, `noexcept`, and O(1). They reconnect
boundary pointers without walking the elements or accessing payloads. Addresses,
references, and iterators to elements remain valid. Incrementing a transferred
iterator follows the destination ring; the source sentinel stays in the source.
Payloads need not be copyable or movable.

For `position.splice_before(first, last)`, the half-open range follows `next()`
links and must not cross the source sentinel. `last` may be the source sentinel.
An empty range, `position == first`, and `position == last` are no-ops.
Otherwise, `position` must lie outside `[first, last)`. This permits reordering
within one ring as well as transfers between rings. These are trusted
preconditions: there is no linear membership check.

`position.splice_before(node)` moves one linked element; `node` must not be a
sentinel. Moving an element before itself or its immediate successor is a no-op.

`position.splice_all_before(head)` moves every element of the source sentinel
`head`, leaving it empty and reusable. The source may be empty; transferring a
head before itself is a no-op. Every other destination must be outside the
source ring. The operation never moves the source sentinel.

These additions preserve existing low-level contracts: `remove()` does not reset
the removed node's links, and construction with `default_construct_t` does not
initialize links. Initialize a ring before using it as a source or destination.

## Independent tagged hooks

An object can inherit multiple `list_node<T, Tag>` or `hlist_node<T, Tag>` hooks.
Each tag identifies a separate base and iterator type. Select the hook explicitly
when linking an object into several queues:

```cpp
struct ready_tag {};
struct timer_tag {};
struct task;
using ready_hook = wjr::intrusive::list_node<task, ready_tag>;
using timer_hook = wjr::intrusive::list_node<task, timer_tag>;
struct task : ready_hook, timer_hook {};

ready_hook ready;
timer_hook timers;
task current;
ready.push_back(static_cast<ready_hook *>(&current));
timers.push_back(static_cast<timer_hook *>(&current));
static_cast<ready_hook &>(current).remove(); // timers still contains current
```

Traversal through either hook returns the complete payload object, including
when that hook is not its first base. Other hooks are unaffected by insertion,
removal, or transfers. Tags require no stored state: list hooks remain two
pointers, hlist hooks one pointer, and iterators one pointer. Different tags
cannot be mixed in an insertion or iterator conversion. Omitting the tag retains
the default `void` tag and existing one-hook source syntax.

## Filtering and adjacent duplicates

Include `<wjr/container/intrusive_algorithms.hpp>` for `intrusive::remove_if`
and `intrusive::unique`. Both accept a typed `list_node<T, Tag>` or
`hlist_node<T, Tag>` sentinel by reference and return the number unlinked.

```cpp
auto removed = wjr::intrusive::remove_if(ready, [](const job &j) {
    return j.priority < 0;
});
auto duplicates = wjr::intrusive::unique(ready, [](const job &a, const job &b) {
    return a.priority == b.priority;
});
```

`unique` retains the first element of each adjacent equivalent group; it does
not sort. Both algorithms take linear time and allocate nothing. They leave
payloads alive and do not reset removed links, so callers retain responsibility
for lifetime and may reinsert removed hooks. Other hooks in the same payload
are unaffected. Predicates must not change links or destroy linked objects.
If a predicate throws, earlier removals remain applied and the remaining ring
or chain is valid. Calls are `constexpr` and conditionally `noexcept` according
to predicate invocation, including its result's conversion to bool.
