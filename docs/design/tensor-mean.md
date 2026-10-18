# Ordered view means

`<wjr/np/mean.hpp>` provides `mean<Acc = void>(view)` and
`mean_to<Acc = void>(destination, source, axes<Axis>[, keepdims])`.
These are explicit eager view operations. An expression can first be materialized
with `eval` when that boundary is useful; this API does not select a strategy.

Floating input retains its element type as the default accumulator. Integer and
bool input use double, avoiding integer addition overflow and integer division.
An explicit accumulator must be an unqualified floating type. `mean_to` also
requires a writable floating destination: the destination type does not choose
the accumulator, and conversion happens only after division.

Each result sums from positive zero in the selected accumulator, in logical
row-major order for `mean` or increasing selected-axis order for `mean_to`, then
divides once by the element count converted to that accumulator. No reciprocal
multiplication, pairwise regrouping or compensated accumulation is implied.
Like ordinary sum/count, it can overflow even if the mathematical mean fits;
a wider explicit accumulator may help. This is not an overflow-resistant mean.
Fast-math transformations and bitwise agreement across compilers are not promised.

An empty full reduction returns quiet NaN. An empty reduced line writes quiet
NaN; an empty destination performs no accesses. No division by zero is used to
produce the empty result. Nonempty NaN, infinity and signed-zero behavior follows
the ordered arithmetic, without preserving NaN payloads or prescribing floating
exception timing.

The layout/accessor subset and single compile-time axis match view `sum_to`:
default accessor, right/left, reviewed positive strides and padded layouts.
Rank zero is supported by the full reduction. Static shape incompatibilities are
constrained out; dynamic shape agreement is an assertion-level precondition.
Valid views and disjoint logical input/output storage are caller responsibilities.
No extra validation flags, allocation or ownership are added.

The implementation shares the existing sum traversal. In the packed rank-two
axis-zero path, four independent ordered accumulators retain the established
column traversal and divide only at final stores. Other layouts use the existing
axis-line walker. There is no output-sized temporary or separate scaling pass.
The default sum and scan instantiations compile out mean behavior.

See [the runnable example](../../examples/np/mean.cpp) for row centering with a
small explicit keepdims workspace and broadcast expression evaluation.
