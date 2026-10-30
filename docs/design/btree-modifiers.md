# Map updates and bulk insertion

Ordinary and compact maps support `insert_or_assign` and moved-key `operator[]`.
All B-tree adapters support iterator/sentinel and initializer-list insertion.
They reuse the existing insertion paths, node layout, comparator ordering and
unique/duplicate-key policy.

```cpp
#include <array>
#include <wjr/container/btree_map.hpp>

wjr::btree_map<int, int> counts;
counts.insert_or_assign(7, 1);  // Insert a new pair.
counts.insert_or_assign(7, 9);  // Assign the existing mapped value.
counts.insert({{2, 20}, {7, 70}}); // Existing key 7 retains 9.
const std::array<std::pair<short, short>, 2> batch{{{3, 30}, {4, 40}}};
counts.insert(batch.begin(), batch.end());
```

`insert_or_assign(key, source)` returns an iterator and an inserted flag. A single
existing try-emplace search determines whether to construct a new pair or assign
the existing mapped value. The source is forwarded only on the selected path.
An rvalue key is consumed only when a new pair is inserted. `operator[](Key&&)`
uses the same moved-key insertion and default-constructs a mapped value only for
a missing key. Compact insertion stages its value before relocating leaf slots,
so a source can refer to an existing mapped value.

The mapped source must construct the mapped type and support its assignment
expression. No common-reference relation or particular assignment return type is
required. Mapped assignment exceptions propagate; the mapped object's state
follows its own assignment guarantee. Construction, allocation and comparisons
inside the existing insertion engine retain its no-throw/termination contract;
these additions do not provide std::map's exception guarantees.

`insert(first, last)` accepts a single-pass input iterator and a compatible
sentinel when its reference type can construct `value_type`. It does not take a
preliminary distance or allocate staging storage. Noncommon sentinels and move
iterators are supported. Map and set duplicates are retained from the existing
tree/first successful input; multiset input inserts every value. The single-value
insert overload and its return type remain available; list/range insertion returns
void, as in standard associative containers.

Input dereference, comparison or increment exceptions propagate after the already
inserted prefix. Each value insertion retains the engine's existing failure
contract. Input iterators must remain valid throughout insertion; mutation of
these trees can invalidate their own iterators, so this is not a guarantee of
arbitrary self-range insertion. Existing pointer/reference invalidation rules
also apply to upsert when it inserts a new key.
