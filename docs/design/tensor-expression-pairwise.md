# Explicit fused pairwise expression sums

`<wjr/np/expression_pairwise.hpp>` adds `pairwise_sum<Accumulator = void>(expr)`
for a restricted subset of expressions. It fuses point computation into the
existing [view pairwise kernel](tensor-pairwise-sum.md), without an intermediate
payload, allocation, shared ownership, or callable copy. Ordinary `sum(expr)`
retains its ordered fold and wider set of supported expressions.

```cpp
const auto x = np::as_expr(samples);
const auto energy = np::pairwise_sum<double>(x * x);
const auto scaled = np::map([](float value, float factor) noexcept {
    return value * factor;
}, x, 0.5f);
const auto total = np::pairwise_sum<double>(scaled);
```

The default accumulator follows the same C++ `value_type + value_type` rule as
the view overload and must be an unqualified floating-point type. Float and
double expressions therefore retain their types; integer and bool expressions
require an explicit floating-point accumulator. A child operation evaluates in
its own C++ dtype **before** its value is converted to the accumulator. For
example, an integer division remains integer division with a double accumulator,
and `pairwise_sum<double>(float_expr + 1.f)` does not promote the addition itself.

## First supported subset

Every view leaf must have the expression's full rank and identical complete
shape, with `layout_right`; rank-zero/rank-one leaves also accept packed
`layout_left`. Arithmetic scalar operands are allowed. Different view ranks,
statically known broadcast dimensions, multidimensional left layouts and all
`layout_stride` leaves are rejected by constraints. Dynamic extents must satisfy
the same complete-shape precondition: a level-2 assertion checks linearity at
the boundary, and there is no broadcast or stride fallback in this overload.
Empty domains must meet that precondition too.

This is a trusted-input entry point. Views, addressed spans and the domain's
logical element count must already be valid and representable. It is not a
checked factory and does not extend any source or callable lifetime.

The accepted tree consists of the recognized pure arithmetic operations,
minimum/maximum, and pure `map` nodes whose reads are `noexcept`. Scalar and view
leaves are accepted directly. Math/cast/mask nodes, unknown operations and maps
that may throw are rejected in this first overload. Use ordinary `sum(expr)`
when its broader traversal or exception contract is needed. Nothrow alone does
not infer an unknown operation's purity; map retains its existing pure-callable
contract. Evaluation binds a const pointer to the real callable, without copying
or moving it; capturing state still has its ordinary read costs.

## Grouping and costs

Empty input returns positive zero; rank zero reads one point. The leaf cutoff
128, eight accumulators, pairwise merge and multiple-of-eight recursive split
are identical to the view overload, including its signed-zero, infinity and NaN
rules. Each point is evaluated once in increasing logical row-major index order;
this does not promise an order between separate children of the same expression
or common-subexpression elimination. It requests no fast math or reassociation.
The grouping is explicit numerical policy, not an automatic strategy selection.

The domain and existing value/callable bindings are built once. A two-word
cursor holds the binding address and an index offset for the shared kernel.
Leaf reads use direct indices, without per-point shape checks, division, stride
projection or metadata planning. Recursion still uses logarithmic stack space
and real calls; binding/offset handling and captured-state loads are costs to
assess against an equally fused handwritten pairwise loop. Fusion is not a
promise of universal speed or accuracy improvement over materialization.

The shared algorithm retains its NumPy 2.2.0 BSD attribution and installed
[license](../licenses/numpy-pairwise.txt). Tests use an independent grouping
oracle, dtype cases, recursion boundaries and move-only callables. The standalone
[example](../../examples/np/expression_pairwise.cpp) demonstrates grouping and
accumulator precision without allocating a transformed tensor.
