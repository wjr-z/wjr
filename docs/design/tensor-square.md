# Single-child square expression

`<wjr/np/expression_math.hpp>` provides `np::square(value)` for a scalar,
ordinary view or expression. It captures one child and computes its value once
per output point, then returns `value * value`. This is a unary expression, not
an implicit materialization, a shared subexpression or a runtime cache.

Use `np::square(x + offset)` when the intermediate value is needed twice in
one multiplication. `(x + offset) * (x + offset)` instead captures two subtrees;
compiler common-subexpression elimination is not guaranteed across binding or
non-inlined boundaries. A child map is called once by square; its ordinary
capture, exception and side-effect rules still apply. Squaring does not grant
an unknown child permission for SIMD or interleaving.

The result follows C++ multiplication promotions: float stays float, narrow
integers and bool promote as in `value * value`, and unsigned multiplication
wraps in the promoted arithmetic type. Signed multiplication must be
representable; there are no per-element overflow checks or saturation.
An accumulator or destination conversion happens after the multiplication.
This is neither `std::pow(value, 2)` nor a stable sum-of-squares algorithm.
For extreme magnitudes use the existing stable `norm_l2` contract instead.

The node preserves its child's shape/layout and adds no state. Existing
broadcast, positive-stride, exact in-place evaluation, ordered and eligible
pairwise reductions work through the existing unary traversal. Eligible
homogeneous float/double xsimd expressions use one batch read followed by a
batch multiply; mixed/map/cast/stride admission remains unchanged. No new
executor path or global compiler flag is introduced.

The [pairwise example](../../examples/np/expression_pairwise.cpp) computes
`pairwise_sum<double>(square(x))`: child multiplication remains float while
the subsequent accumulation uses double. Representation and generated-code
checks must distinguish a single capture from a universal throughput claim;
the compiler may already eliminate duplicated reads in particular `x*x` loops.
