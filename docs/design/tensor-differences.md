# First differences into a borrowed output

`<wjr/np/difference.hpp>` provides
`diff_to(destination, source, axes<Axis>)`. It computes adjacent differences
along one compile-time axis, directly into caller-provided storage:

```cpp
np::diff_to(changes, samples, np::axes<1>);
```

The output has the same rank as the source. Its selected extent is
`max(source.extent(Axis) - 1, 0)`; every other extent is unchanged. Statically
incompatible shapes or invalid axes are constrained out. Dynamic shape
agreement uses the existing assertion level, without always-on checks.
Rank zero has no selectable axis. Empty outputs, including singleton reduced
axes, return before slicing, data access or pointer arithmetic; a valid null
empty view is supported.

For bool input the result is inequality, identifying transitions. Other
arithmetic input computes `next - previous` using ordinary C++ promotions,
then converts to the destination element type. For example, uint8 input can
produce negative differences in an int destination. The destination type does
not widen intermediate subtraction; signed overflow and invalid conversions
remain caller preconditions. Unsigned arithmetic, NaNs, infinity and signed
zero follow the corresponding C++ operation and compiler FP settings.

The operation supports default accessors, nonvolatile arithmetic input,
writable arithmetic output and the existing packed, positive-stride and
padded view layouts. Logical input and output storage must be disjoint;
no alias scan or overlap temporary is provided.

The adjacent input address is obtained from the selected axis stride. Matching
packed layouts use contiguous blocks with the existing compiler-guarded
`WJR_RESTRICT` convention, justified by their disjoint complete spans. Other
layouts reuse the existing
nested stride walker. Static mappings keep their compile-time strides. No
slice descriptors, difference array, expression tree, allocator or shared
ownership are introduced. The boundary establishes shape and nonempty
preconditions once; the inner kernel does not repeat them. Packed loop sizes
come from the source shape, without loading redundant destination extents in
release builds.

Six targeted tests cover constexpr, both packed orders, mixed layouts, generic
strides and padding, empty/singleton axes, interior rank-three axes, narrow
indices, bool transitions and ordinary arithmetic conversions. Local Clang 18
ASan, MSVC 19.42 and GCC 12 actual test bodies pass. Windows GCC/Clang baseline
and native assembly checks use the same disjoint contract in the handwritten
baseline, including restrict: packed row loops match on Clang, while GCC keeps
small setup/scheduling differences; no per-element metadata or extra calls are
introduced. This is preliminary code-generation evidence, not Linux throughput
or a universal zero-overhead claim.

See the [runnable example](../../examples/np/difference.cpp) for two signals
differenced along their sample axis. The output is fully overwritten without
a redundant zero-initialization pass.
