# Effective insertion hints

The B-tree adapters accept `insert(hint, value)` and `emplace_hint(hint, args...)`.
Ordinary and compact maps also accept hinted `try_emplace` and `insert_or_assign`.
A hint must be a valid iterator in the same tree, including its current `end()`.
Existing mutation invalidation and comparator/value/allocator failure contracts
continue to apply.

```cpp
#include <wjr/container/btree_map.hpp>

wjr::btree_map<int, int> values;
auto position = values.try_emplace(values.cend(), 10, 1);
position = values.try_emplace(position, 20, 2);
position = values.insert_or_assign(position, 20, 3);
```

The engine checks the gap before the hint and an immediate neighboring gap. A
correct adjacent hint uses the existing insertion position directly without a
root-to-leaf search. A wrong hint falls back to the existing search. Shifts,
splits and parent updates still use the original insertion engine; avoiding a
search does not remove their cost or establish standard-map reference stability.

At a nonfirst leaf boundary, insertion is normalized to the preceding leaf's
end. This retains the insertion engine's existing minimum-key/separator
invariant, including indirect separator lifetimes. No node state, iterator
state or ownership checks are added. The caller must refresh hints after a
mutation that invalidates them.

For unique containers, equivalent keys return the existing element. Hinted
`try_emplace` does not construct the mapped value or consume an rvalue key on a
duplicate. Hinted `insert_or_assign` forwards the mapped source only to the
selected construction/assignment path; mapped assignment exceptions propagate
as in the ordinary overload.

Explicit multiset hints place a new equivalent element near the position before
the hint. Range insertion instead preserves the previous append-to-equivalent-
group policy and the relative input order. These policies use the existing lower
and upper searches when their hinted gap cannot be used.

Range/list insertion, initializer-list assignment and range/list construction use
fresh iterators returned by each insertion. The automatic cursor checks its
forward gap and retains append-to-equivalent-group order. No preliminary
distance or input staging is added, so single-pass inputs remain supported. The
actual caller is the range import API, including sorted iterators from existing
ordered maps/sets and batches composed of locally sorted runs.

The opt-in `btree_hint` benchmark target compares that range path with the
unchanged single-element emplace loop and standard-container ranges. It retains
sorted, shuffled 32-element runs and fully shuffled inputs at the same capacity
and allocator contract. It is excluded from the default build. Windows/Linux
timing and any automatic-range tradeoffs remain pending until the accompanying
measurement is complete; comparison-count tests alone do not establish speed.

```sh
cmake -S . -B build-hint -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree \
  -DWJR_CPU_TARGET=baseline -DWJR_CXX_STANDARD=20
cmake --build build-hint --target btree_hint --parallel
./build-hint/bin/btree_hint --benchmark_min_time=0.03s \
  --benchmark_repetitions=5 --benchmark_out=hint-timing.json
```
