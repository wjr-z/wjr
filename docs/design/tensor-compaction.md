# Ordered compaction into caller-owned storage

`<wjr/np/compact.hpp>` provides allocation-free selection primitives:

```cpp
size_t written = np::compress_to(destination, source, mask);
size_t found = np::flatnonzero_to(positions, source);
```

Both return the number of written elements. They preserve logical input order,
leave the unused destination suffix untouched, and require destination storage
disjoint from the input storage. No ownership is retained.

## Masked selection

`compress_to` accepts rank-one arithmetic destination, source and mask views.
Source and mask have equal lengths; the destination's length is its capacity,
which must accommodate the selected elements. All three accept the existing
packed, positive-stride and padded kernel layouts. Volatile elements and const
destinations are excluded.

A mask element selects its source element when `mask != 0`. Negative values,
infinities and NaNs select; either signed zero does not. Selected source elements
are converted directly to the destination type, with the ordinary C++ conversion
contract. Unselected source elements are not read or converted, so an excluded NaN
does not cause a floating-to-integer conversion. A zero-capacity/null destination
is valid when no elements are selected. Stride holes and padding are untouched.

## Nonzero positions

`flatnonzero_to` accepts an arithmetic source with any fixed rank, including zero.
It returns **row-major logical flat positions**, independent of physical mapping.
The destination is a packed rank-one writable integral, non-boolean view. Its
element type must represent `source.size() - 1` when the source is nonempty; a
uint8 destination therefore supports up to 256 logical source positions.
Destination capacity must accommodate all results. Nonzero uses the same
`!= 0` semantics, without converting full-width source integers to floating point.

A nonzero rank-zero scalar yields index 0; a zero scalar yields no indices. An
empty source returns immediately without reading data or strides. A nonempty
all-zero source may use a zero-capacity/null destination.

## Cost and contracts

There is no count prepass, allocation, workspace, zero initialization, atomic
update or runtime selection mode. A single ordered scan writes selected results
directly. Packed calls use pointer loops; rank-one strides are loaded once.
Multidimensional non-row-major input reuses the existing row-major kernel walker
and needs only O(rank) stack metadata. Scalar input takes the linear path.

Shape compatibility, output-index representability and write capacity are level-2
assertions. Capacity is checked only when a write is selected; release calls do
not add a check or validate the entire input before doing useful work. These are
caller preconditions, not a separate recoverable error API.

`benchmarks/src/np_compact.cpp` compares packed public calls with equivalent
handwritten loops under the same noinline boundary and alias assumptions. Both
consume the written count and preserve output tails. Allocation and initialization
are outside the timed region, with no output reset for either implementation.
Cases cover empty, small and large inputs and none/all/alternating/random selection.
Hardware and compiler evidence remains necessary before claiming zero overhead;
branch prediction and selected-store dependencies differ by distribution.
