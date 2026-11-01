# Range, list and allocator construction

`btree_map`, `btree_set`, `btree_multiset` and the experimental compact map accept
the following forms. Range inputs use the same single-pass input and compatible
sentinel contract as bulk insertion; dereferenced values must construct the stored
value type through the tree's allocator. Duplicate keys follow the adapter's existing policy.

| Form | Ordering / allocator |
| --- | --- |
| `Tree()` | Existing direct default construction |
| `Tree(compare)` | Explicit ordering, directly default-constructed allocator |
| `Tree(compare, allocator)` | Explicit ordering and allocator instance |
| `Tree(allocator)` | Default ordering and explicit allocator instance |
| `Tree(first, last, compare)` | Range with explicit ordering |
| `Tree(first, last)` | Range with default ordering |
| `Tree(first, last, compare, allocator)` | Range with both states supplied |
| `Tree(first, last, allocator)` | Range with default ordering and supplied allocator |
| `Tree({values}, compare, allocator)` | List with both states supplied |
| `Tree({values}, allocator)` | List with default ordering and supplied allocator |
| `Tree({values}, compare)` / `Tree({values})` | List with explicit/default ordering |

Allocator-only convenience requires a default-initializable comparator whose type
differs from the allocator type. If the types coincide, the existing comparator
constructor remains available; the explicit `compare, allocator` form identifies
both roles. A nondefault comparator can still use every form with an explicit
comparator argument.

```cpp
#include <memory_resource>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

std::pmr::unsynchronized_pool_resource pool;
using allocator = std::pmr::polymorphic_allocator<char>;
using map = wjr::btree_map<int, int, std::less<int>, allocator>;
map entries({{3, 30}, {1, 10}}, allocator(&pool));
const int keys[] = {4, 1, 4, 2};
wjr::btree_multiset<int, std::less<int>, allocator> duplicates(
    std::begin(keys), std::end(keys), allocator(&pool));
duplicates = {8, 2, 8}; // Retains the current comparator and resource.
```

The supplied allocator is rebound for nodes and legacy indirect values. Allocator-
aware mapped objects receive its resource through the existing construction
protocol. Over-aligned values retain typed allocation and leaf alignment. The
resource must outlive the tree and all objects using it; the example declares it
first so trees are destroyed before the resource.

Range loops run in adapter constructor bodies after the shared engine is fully
constructed. A source-iterator exception from an explicit-allocator range
constructor therefore unwinds the completed base and releases inserted values
and nodes. New set range constructors also propagate input-iterator exceptions.
Existing comparator-only map range/list constructors retain their previous
noexcept declaration. Value construction, allocation and comparator exceptions
inside insertion retain the existing engine's termination contract; standard
container exception guarantees are not added.

Initializer-list assignment on set/multiset, like map assignment, replaces values
through the current allocator and comparator instead of constructing and moving
a default-state temporary. Default and comparator-only empty construction still
construct the allocator directly: no extra temporary/copy/move is introduced.
These constructors add no tree state or node-layout changes.

## Allocator-extended range values

Range construction/insertion also accepts allocator-extended key/mapped copies
when their ordinary copy constructors are deleted. This is useful for moving
ownership from a temporary PMR resource to a persistent resource, or importing
values into a differently configured tree type. It uses the same actual
allocator construction as `emplace`; it does not create a temporary ordinary
copy before calling the allocator.

```cpp
#include <string>

struct resource_value {
    using allocator_type = std::pmr::polymorphic_allocator<char>;
    std::pmr::string text;
    resource_value(std::allocator_arg_t, allocator_type a, int n) : text(n, 'x', a) {}
    resource_value(std::allocator_arg_t, allocator_type a, const resource_value &v)
        : text(v.text, a) {}
    resource_value(const resource_value &) = delete;
    resource_value(resource_value &&) = delete;
};
using source_type = wjr::btree_map<int, resource_value, std::less<>, allocator, 16>;
using target_type = wjr::btree_map<int, resource_value, std::less<>, allocator, 32>;
source_type source(std::less<>{}, allocator(&pool));
source.try_emplace(1, 128);
std::pmr::monotonic_buffer_resource other;
target_type target(source.cbegin(), source.cend(), allocator(&other));
```

The example continues the allocator/pool declarations above. The source keeps
its values; the new mapped string owns storage through `other`. Resource lifetime
requirements remain unchanged. Map/set/multiset range forms share this admission;
compact maps still require their original ordinary nothrow-move storage domain.

Ordinary `constructible_from` values retain their existing path. For additional
allocator-extended cases, the private constraint checks the actual nontrivial
allocator-construction dispatch and the prefix/trailing `uses_allocator`
constructor forms. Pair key/mapped references are checked recursively, preserving
constness and rvalue references. An unconstrained PMR `construct` declaration alone
does not make an invalid key, mapped source or non-pair input admissible.

This models constructor arguments formed with the engine's rebound allocator;
it does not infer arbitrary argument rewriting to a different inner allocator
from a custom `construct` declaration. Existing ordinary input domains remain
available. Nothrow destruction requirements and node/value termination boundaries
are retained, while input-iterator exceptions preserve the constructor cleanup
and insertion-prefix behavior described above. The experimental packed importer
retains its separate, deliberately narrower input contract.
