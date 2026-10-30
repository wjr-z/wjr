# Ordering intrusive nodes

`<wjr/container/intrusive_order.hpp>` provides non-owning order operations:

| Operation | Containers | Complexity |
| --- | --- | --- |
| `intrusive::reverse(head)` | ring and hlist | O(n) |
| `intrusive::merge(destination, source, compare)` | ring and hlist | O(n + m) comparisons |
| `intrusive::sort(head, compare)` | ring | O(n log n) comparisons |

All operations reconnect links without allocating, constructing, moving, or
destroying payloads. Nodes may be noncopyable and nonmovable. Element addresses
and references remain valid; other hooks are untouched. `reverse` also accepts
raw `void` hooks, is constexpr, and is noexcept.

```cpp
#include <wjr/container/intrusive_order.hpp>

struct task : wjr::intrusive::list_node<task> {
    int priority = 0;
};
wjr::intrusive::list_node<task> ready, incoming;
task first, second;
first.priority = 3;
second.priority = 1;
ready.push_back(&first);
incoming.push_back(&second);
auto less_priority = [](const task &a, const task &b) noexcept {
    return a.priority < b.priority;
};
wjr::intrusive::merge(ready, incoming, less_priority); // incoming is now empty
wjr::intrusive::reverse(ready);
wjr::intrusive::sort(ready, less_priority);
```

Merge requires two already sorted, disjoint lists. A self-merge is a no-op.
Equivalent elements from the destination precede equivalent source elements,
and order within either input is preserved. Transferred iterators follow the
destination. If comparison throws, prior transfers remain applied; both lists
remain valid and contain every original node between them.

Sort is a stable bottom-up merge sort using one local carry sentinel and
`numeric_limits<size_t>::digits` local bucket sentinels. It uses the same
comparator instance throughout. Empty and singleton rings return immediately.
If comparison throws, recovery returns every node to the input ring, in an
unspecified order. There is no temporary owner or hidden allocation.

Comparators must define a strict weak order and must not change links or destroy
linked objects. Merge and sort are constexpr and conditionally noexcept based
on comparison and conversion of its result to bool. The default comparator is
`std::less<>`; a custom comparator can compare a payload member as above.
