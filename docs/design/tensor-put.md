# Indexed writes

`<wjr/np/put.hpp>` provides `put_to(destination, indices, values)` for rank-one
views. It assigns `destination[indices[i]] = static_cast<D>(values[i])` in
increasing logical `i` order. Repeated indices keep the last update; all other
destination elements retain their original value. This is assignment, not a
scatter reduction. The function does not allocate, clear the destination,
normalize negative indices, sort updates, or use atomics.

All three views accept packed, positive-stride and supported padded mappings.
The indices and values have identical shapes; their layouts may differ.
Destination extent is independent of the update count. Indices are non-boolean
integers, nonnegative and smaller than the destination extent. Destination is
writable arithmetic storage; source values are arithmetic and their conversion
to the destination element type must be valid under C++ rules.

Destination storage is disjoint from both inputs. The read-only inputs may
overlap one another and remain stable during the call. Views borrow existing
objects and do not retain an owner. An empty update sequence accesses no data,
including when all handles are null. Unaddressed logical elements and mapping
holes are untouched.

Shape and index preconditions use level-two assertions. There is no validation
pass or always-on check. With these assertions disabled, the loop performs one
index read, one value read and one indexed destination write per update, plus
the addressing required by the layouts. The separate written pointer carries
the alias information justified by the contract; the read-only inputs are not
required to be disjoint from each other.

The operation is `constexpr` and `noexcept`. Signed negative indices, out-of-range
indices, invalid conversions, and input/output overlap violate its preconditions.
Higher-rank scatter needs a defined update order before it can be added without
changing duplicate-index semantics; it is outside this initial interface.

See [the runnable example](../../examples/np/put.cpp) and the same-contract
handwritten comparisons in `benchmarks/src/np_put.cpp`. A direct loop alone does
not establish a cross-platform performance guarantee.
