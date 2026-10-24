# Concatenating borrowed views

`<wjr/np/concatenate.hpp>` provides
`concatenate_to(destination, axes<I>, sources...)`. A nonempty list of borrowed
views is copied into a caller-owned destination along the compile-time axis `I`.
Each source has the destination's rank and non-axis extents; its axis extent
contributes to the destination axis length, in argument order. One source is a
disjoint copy. This operation does not add an axis or broadcast inputs.

The destination is writable arithmetic storage. Source element types are the
same type, optionally const; no implicit arithmetic conversion is performed.
Packed, positive-stride and supported padded layouts may be mixed. Layouts
describe logical coordinates: a column-major source copied to a row-major
destination retains the same coordinates. Boolean buffers contain real bool
objects. All views borrow existing objects without retaining owners.

Destination storage is disjoint from every source; read-only sources may overlap
one another. Views and addressed spans satisfy the ordinary mdspan preconditions.
Non-axis shapes agree, and source axis lengths sum exactly to the destination's
axis length. Shape preconditions use level-two assertions with no per-element
checking or runtime alias scan. The diagnostic sum uses remaining length to
avoid overflowing the check itself. Arithmetic extents and offsets remain
representable under the normal view contract.

An empty source accesses no data and forms no pointer from its null handle.
This also holds when a non-concatenated extent is zero. No initialization,
allocation, temporary tensor, or planner is created. Logical elements are copied
once; mapping holes remain untouched. The operation is constexpr and noexcept.

Matching packed layouts concatenated along their outer axis are copied as one
contiguous block per source. Other cases share outer-coordinate traversal in the
destination's layout order and expand the sources at the concatenation axis.
Matching packed inner axes use contiguous block copies, while general strides
use direct loops. Rank-one packed views are contiguous regardless of layout tag.
No temporary slice mapping or copy_to identity scan is needed under the explicit
disjoint contract. Runtime and code-size evidence must still be compared with a
handwritten block-copy implementation under the same alias and shape contract.
Inputs can have different axis lengths, so their inner copy loops remain
separate; a handwritten operation for specifically equal lengths may fuse those
loops further. This is a performance boundary rather than a zero-overhead claim
for every source combination.

See [the example](../../examples/np/concatenate.cpp) and
`benchmarks/src/np_concatenate.cpp` for same-contract packed outer-axis,
inner-axis and positive-stride comparisons.
