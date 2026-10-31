# Associative queries and deletion

The shared B-tree engine supplies `find`, `contains`, `equal_range`, key erase,
range erase and `value_comp` for `btree_map`, `btree_set`, `btree_multiset` and
the experimental compact map. These operations use comparator equivalence,
not `operator==`. Existing node layout, allocation, lookup and single-iterator
erase paths remain in use.

```cpp
#include <string>
#include <string_view>
#include <wjr/container/btree_map.hpp>

wjr::btree_map<std::string, int, std::less<>> items;
items.emplace("alpha", 1);
if (auto position = items.find(std::string_view("alpha")); position != items.end()) {
    position->second += 1;
}
const bool present = items.contains(std::string_view("alpha"));
const auto removed = items.erase(std::string_view("alpha"));
```

`find` returns `end()` if no equivalent key exists. A const tree returns a const
iterator; a mutable tree retains its existing iterator reference contract.
`equal_range` returns `[lower_bound(key), upper_bound(key))`, including the
correct insertion position for a missing key. Unique containers obtain the
range from one lower-bound search and an equivalence test. Duplicate-key trees
also supply `count`, which counts the complete equivalent range.

With a transparent comparator, these queries and key erase accept lookup types
that compare with keys in both directions, without constructing `key_type`.
The iterator erase overload remains available even for a comparator accepting
unrelated types. Comparator calls inside tree queries/deletion must not throw,
consistent with existing bounds operations.

`erase(key)` returns the number removed: zero or one for a unique container,
and the full equivalent count for a multiset. It finishes key comparisons before
destroying an element, so a key argument may refer to a key in the same tree.

`erase(first, last)` removes a valid half-open range and returns its successor,
or `end()`. Empty ranges are accepted. As with the existing engine, mutation can
invalidate other iterators and references. The implementation counts a partial
range before its first mutation and advances using the surviving successor;
it does not retain a potentially invalidated `last` as its stopping condition.
Range length is counted by leaf metadata, complete leaves can be removed at
once, and partial leaves are batched when they retain minimum occupancy.
Underflow uses the existing iterator erase/rebalance, and complete-tree erase
uses `clear`. There is no auxiliary allocation or new promise of standard-map
complexity or invalidation rules; see the [range erase comparison](../performance/btree-range-erase.md).

For maps, `value_compare`/`value_comp()` compare pair keys and ignore mapped
values. For sets, `value_compare` is `key_compare`. The returned comparator
retains a copy of the ordering state. Its construction/call exception
specifications follow the comparator expressions; the observer adds no state
to the tree object and does not change lookup's existing no-throw contract.
