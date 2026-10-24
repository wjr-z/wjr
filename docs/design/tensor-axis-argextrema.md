# Axis indices of extrema

`<wjr/np/extrema_axis.hpp>` provides:

```cpp
argmin_to(destination, source, axes<I>);
argmax_to(destination, source, axes<I>);
argmin_to(destination, source, axes<I>, keepdims);
argmax_to(destination, source, axes<I>, keepdims);
```

For a shape `(2, 3)` input, axis 1 produces two column indices, while axis 0
produces three row indices. `keepdims` leaves the reduced extent as one, so the
corresponding output shapes are `(2, 1)` and `(1, 3)`. Reducing a rank-one input
without `keepdims` writes a scalar rank-zero view. Results are positions within
the selected axis, rather than flattened positions in the complete input.

## Numerical behavior

Strict comparisons preserve the first equal value, including signed zeros.
The first NaN on each line wins for both operations, matching the existing
whole-view `argmin`/`argmax` behavior. Otherwise infinities follow ordinary
ordering. Integer and boolean values are compared in their original dtype;
there is no floating-point conversion, so full-width integer differences remain
visible. Each line is traversed in increasing logical axis position.

## View and storage contract

- Source is an arithmetic, nonvolatile view. Destination has a writable integral
  dtype excluding `bool`; plain character integer types are supported too.
- Source and output can have packed, padded, or positive-stride layouts. Shapes
  follow the existing reduction rules, checked statically when known and with
  level-two assertions otherwise. Ordinary custom accessors are not added.
- An empty output performs no source element access, including when the selected
  source axis is also empty. A nonempty output requires a nonempty selected axis.
- The output dtype must represent `source.extent(I)-1`; `uint8_t` can therefore
  index a selected axis of length 256. Representability is a single level-two
  boundary precondition. The destination limit is not narrowed to `size_t`.
- Logical destination storage is disjoint from source storage. Source values
  remain stable during the call. There is no alias scan or defensive copy.
- The function fully writes every logical output once and preserves output holes.
  It performs no allocation, output initialization pass, retained ownership, or
  per-element validation. All overloads are `constexpr` and `noexcept`.

Implementation reuses `compatible_sum_shape`, `axis_stride`, and
`walk_axis_lines` from existing reductions. One private comparison-direction
kernel serves both operations. Line callbacks preserve static counts and known
unit strides explicitly, avoiding new line views or mapping construction.
First-NaN handling can end a line early; ordinary nonempty lines need at most
`axis_length-1` subsequent element reads and comparisons.

This first API accepts views. Expression evaluation, multi-axis flattened
indices, ignored-NaN variants, parallel traversal and SIMD specializations are
separate capabilities, not implicit behavior of this interface.

`examples/np/extrema_axis.cpp` demonstrates both axes and `keepdims`.
`benchmarks/src/np_extrema_axis.cpp` compares complete public calls to equivalent
handwritten loops, with identical NaN/tie behavior and output/alias contracts.
Measurements and generated-code observations must be distinguished: using the
existing walker does not by itself prove equivalence to the best loop on every
axis or compiler.
