# Positive-stride slices

`<wjr/np/slice.hpp>` provides `slice_view(source, slices...)`. It returns the
ordinary borrowed `tensor_view` selected by the pinned Kokkos `submdspan`;
there is no owning wrapper, allocation, reference count or error-state object.
The source buffer must remain alive. A const descriptor still allows writes
when its element type is mutable; use `tensor_view<const T,...>` for read-only
elements. An owner's `view()` cannot be obtained from a temporary owner.

## Specifiers

Provide one specifier per source axis:

| Specifier | Result |
| --- | --- |
| integer | Select one coordinate and remove the axis |
| `np::full_extent` | Keep the complete axis and its static extent |
| `std::pair{begin,end}` | Keep the half-open interval `[begin,end)` |
| `np::strided_slice{offset,extent,step}` | Keep every `step`th element in the source interval `[offset,offset+extent)` |

For `strided_slice`, `extent` is the covered source interval length, **not**
the output element count or end coordinate. Its result length is zero for
zero extent and otherwise `1 + (extent-1)/step`. For example, `{1,5,2}`
selects source coordinates `1,3,5`. This differs from Python's `start:stop:step`.
Integer and range endpoints can use `std::integral_constant` to preserve
compile-time extents. There is no negative step, new-axis or ellipsis syntax.

```cpp
auto row = np::slice_view(matrix, 2, np::full_extent);
auto window = np::slice_view(matrix, std::pair{1,4}, std::pair{2,6});
auto alternating = np::slice_view(matrix, np::full_extent,
                                  np::strided_slice{0,6,2});
np::eval_to(alternating, np::as_expr(alternating) * 2.0);
```

## Trusted-input contract

Construction follows the backend's preconditions without repeating metadata
validation. The source must already be a valid CPU view. Integer indices must
be in range; pairs need `0 <= begin <= end <= extent`; strided intervals must
fit within the source axis, with nonnegative offset/extent and positive step.
All conversions, products of source strides and steps, mapping offsets and
result spans must be representable in the relevant source index type and
address arithmetic. In particular, an otherwise harmless singleton or empty
slice does not authorize overflowing its stored stride product.

The initial API accepts default accessors and the three existing layouts,
plus the pinned backend's padded layouts returned by earlier slices. Source
index types must have at least `int`'s value bits: the backend's submapping
stride construction currently rejects narrower types through narrowing
list-initialization. Custom accessors and broader backend-specific slice
types are outside this interface's documented subset.

For a `strided_slice` whose extent and step are both integral constants,
the extent must be divisible by the step in this backend snapshot. Otherwise
the backend disagrees with its own static extent when constructing the result;
WJR rejects this combination at compile time. Use runtime extent/step values
for nondivisible intervals; these correctly use ceiling division and retain
static information from the other axes. Static pairs and full axes are
unaffected.

Empty ranges and zero-extent source axes are supported, including null data
for empty sources and `[extent,extent)` boundary ranges. An empty result's
data handle follows the backend offset policy; it must not be dereferenced
or assumed to identify a logical element. Selecting all axes by integer
produces a rank-zero view with one element; slicing a rank-zero source takes
no specifiers and preserves that element.

There is one known backend restriction: a `layout_stride` result must retain
strictly positive strides even when empty. For example, left-layout shape
`(0,6)` with `(full_extent, strided_slice{0,6,2})` would produce strides `(1,0)`;
right-layout shape `(4,0)` with `(strided_slice{0,4,2}, full_extent)` would
produce `(0,1)`. These subsets are excluded. Fully static packed sources are
rejected at compile time when this condition can be established. For dynamic
sources it remains a caller precondition: a valid source does not itself
guarantee that every backend empty slicing operation produces a legal
mapping. WJR does not construct and repair an invalid result or add a runtime
normalization branch. Other valid empty slices remain supported.

## Layout and evaluation

Static extents and packed left/right layouts survive when the backend proves
they apply. An ordinary matrix window may instead retain its leading
dimension in a backend padded-layout tag; stepped selections can use
`layout_stride`. WJR retains these types instead of discarding their static
information into a dynamic descriptor. Their experimental backend type names
are not a separate public WJR owner API.

Copy, fill, transforms, expression evaluation, sum and scan operate on logical
elements and leave padding untouched. Padded descriptors never enter a dense
linear loop merely because their layout differs from `layout_stride`.
`eval(expression)` materializes a packed owner, preserving left-layout
preference for left-padded input; it does not create a padded owner. The
optional xsimd policy currently falls back to scalar evaluation for padded
leaves/destinations. Existing shape and alias preconditions still apply:
exact pointwise in-place evaluation is supported; shifted overlapping
windows require a separate materialized result.

The adapter introduces no operations beyond direct backend slicing in the
paired Clang/GCC code-generation probes. This does not establish equivalence
to a handwritten address calculation in every case: the pinned backend can
retain offset normalization for empty boundary ranges, and GCC can retain a
boundary branch even for integer row selection. Constant row/step probes
match the handwritten calculation with Clang; dynamic windows retain extra
instructions. These are known backend/compiler costs, not a measured
throughput claim or a reason to add another slice implementation.

The [installed example](../../examples/np/slice.cpp) borrows alternate columns
of a padded owner and evaluates directly into the selected elements.
