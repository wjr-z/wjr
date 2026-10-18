# Minimum, maximum, and clipping expressions

`<wjr/np/expression_math.hpp>` provides `minimum(a, b)`, `maximum(a, b)`, and
`clip(value, lower, upper)`. Each call constructs the existing static expression
tree. Operands may be arithmetic scalars, views, or compatible expressions;
right-aligned broadcasting and shape checks follow the ordinary binary
expression rules. Constructing a node does not read data or allocate a tensor.

For `minimum` and `maximum`, the two child value types are converted to their
`std::common_type_t` before comparison. This is C++ common-type behavior, not a
NumPy dtype promotion promise. `clip` computes one common type from all three
child value types before composing `minimum(maximum(value, lower), upper)`.
Operands already having that type are captured directly; only differing child
types need the existing `cast` node. A single common type matters for signed
and unsigned mixtures: `clip(-1, 0u, 1.)` converts all operands to `double`, so
the result is `0.` rather than the result of converting `-1` to `unsigned` in
an intermediate node.

Floating-point NaNs propagate: one NaN operand makes the result NaN. When both
are NaN, the left operand is returned. For non-NaN equal values, including
`+0` and `-0`, the left operand is returned, so operand order determines the
zero sign. NaN payload selection, signaling-NaN exception order, and exact
floating-point-environment behavior are not promised. Integral common types
use comparisons only and have no NaN test. No saturation is applied to integer
casts or arithmetic.

Clipping uses the stated nested min/max rule and does not validate that
`lower <= upper`; for finite values and reversed bounds it returns `upper`.
Because every child is read as the expression tree is evaluated, `clip` is not
a lazy branch and does not guard invalid child computations. As with other
expressions, exact in-place evaluation is available where `eval_to`'s aliasing
contract permits it; partial overlap is not made safe by these operations.

The operation tags are admitted to the existing sum-column interleaving
whitelist. That is an ordering contract for these pure comparisons, not a
claim that this expression adds no instructions. Homogeneous `clip` composes
only those min/max nodes and remains eligible; mixed-type `clip` with conversion
nodes remains outside the whitelist because ordinary casts are not globally
declared safe to reorder. The explicit xsimd policy has no specialized min/max
batch implementation yet and therefore uses the scalar fallback. Performance
claims require comparison with a hand-written loop using the same NaN and tie
rules.

See [the standalone example](../../examples/np/expression_minmax.cpp) and
`test/np/expression_minmax.cpp` for mixed promotion, broadcasting, NaN, signed
zero, empty-domain, scalar, and in-place coverage.
