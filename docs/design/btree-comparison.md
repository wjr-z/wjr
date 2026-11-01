# Btree value comparisons

`btree_set`, `btree_multiset`, `btree_map` and the experimental
`compact_btree_map` compare their stored value sequences. They support equality
and C++20 three-way comparison, including defaulted comparisons in containing
application types:

```cpp
struct snapshot {
    wjr::btree_set<int> selected;
    wjr::btree_map<int, int> quantities;
    auto operator<=>(const snapshot &) const = default;
};
```

Equality compares corresponding values after checking sizes. Ordering compares
values lexicographically and uses sequence length only after an equivalent
prefix. Neither operation calls `key_compare`: a stateful or transparent key
comparator controls tree ordering and lookup, not value comparison. Containers
with different comparator state can consequently have different stored sequences.

The result preserves the element's strong, weak or partial comparison category.
Values supporting only `<` use synthesized weak ordering; ordering does not
require equality for these values. Conversely, equality-only values can be
compared for equality without defining ordering. This follows the C++20
[container comparison model](https://github.com/cplusplus/draft/blob/n4861/source/containers.tex)
and [synthesized three-way comparison](https://github.com/cplusplus/draft/blob/n4861/source/lib-intro.tex).

For example, an unordered mapped floating-point comparison propagates to the
container result. All four relational operators then return false for that
unordered result, matching C++20 `std::map`. This corrects the previous map
implementation of `<=` and `>=` as negated less-than comparisons. Floating-point
NaN keys remain outside the tree's strict weak key-ordering requirement.

Existing map and compact-map named relational operator templates remain usable,
including explicit template function addresses. Operations do not gain a
`noexcept` promise: element comparison exceptions propagate without modifying
either tree. There is no self-comparison shortcut, which would change NaN and
throwing-value semantics.

## Validation

Focused tests compare all six relations and comparison categories against
`std::set`, `std::multiset` and `std::map`, across empty/SSO/multiple-leaf sizes,
both map storage policies, nondefault comparator state, transparent string
lookup, equality-only/less-than-only values, NaN and throwing comparisons.
MSVC Release passed all nine new and existing comparison tests. Windows Clang
ASan/UBSan covers the nonthrowing cases; the exception test remains enabled in
the repository and Linux CI. Local Windows Clang 18 ASan has an independently
reproduced WinEH rethrow/cleanup failure, so that exception coverage is not
claimed locally.

The change does not modify search, insertion or storage. Clang 18 C++20 optimized
assembly was compared with the parent revision for baseline and native profiles:
direct searches, ordinary map/set insertion, map/set/compact bounds, transparent
string bounds, object sizes and iterator sizes were identical after removing
comments and blank lines. This is a code generation check, not a throughput
claim for the new comparison operations.
