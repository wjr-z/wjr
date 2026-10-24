# Indexed reads

`<wjr/np/take.hpp>` provides `take_to(destination, source, indices)`.
The source has rank one; the destination has the indices' shape, including
rank zero. Each output element is `source[indices[...]]`, converted to the
destination element type. Repeated and unordered indices are supported.

All views borrow storage. The function does not allocate, initialize a temporary,
retain an owner, or copy the source. Packed, positive-stride and padded layouts
use the existing view mappings. Different destination and index layouts are
matched by logical coordinates. Packed compatible layouts use a direct pointer
loop with alias information justified by the disjoint-storage contract.

## Preconditions

- Source and destination elements are arithmetic types; destination is writable.
- Indices are non-boolean integers, nonnegative and smaller than the source size.
- Destination and indices have identical shapes.
- Destination storage is disjoint from both inputs. Inputs remain stable.
- Conversions to the destination type must be valid C++ arithmetic conversions.

Shape and index checks use level-two assertions, with no separate validation
pass. Release builds with those assertions disabled perform the gather directly.
An empty destination reads neither source elements nor index elements. Negative
index normalization, axis-dependent gathering and scatter writes are outside this
interface; they should not add work to the ordinary rank-one gather.

`argsort_to` can produce one index sequence for reordering several arrays using
`take_to`. The caller controls index width and output/workspace lifetime.

See [the runnable example](../../examples/np/take.cpp) and the same-contract
handwritten-loop comparison in `benchmarks/src/np_take.cpp`. No cross-platform
performance claim follows from the existence of the packed fast path alone.
