# Explicit SIMD expression evaluation

`<wjr/np/expression_xsimd.hpp>` adds an explicit backend for packed expressions
and statically eligible broadcast rows. Link `wjr::np` and `wjr::simd_xsimd`; the latter retains
the project's reviewed xsimd version and ISA configuration.

```cpp
auto expression = 2.0 * np::as_expr(x) - 0.5 * np::as_expr(y);
np::eval_to(output, expression, np::xsimd_policy<>{});
```

The scalar overload remains available without including xsimd. Including the
optional header does not replace its implementation or change existing node
layouts. An explicit architecture can be selected through
`np::xsimd_policy<xsimd::sse2>{}` when supported by the deployment target.
This interface does not provide runtime CPU dispatch.

Private operation adapters live in `detail/xsimd_operation.hpp`. Each
`xsimd_operation<Op>` specialization keeps its accepted child/result kinds and
`static apply` together; unknown operations default to unsupported. Packed
and projected-row readers use the same adapters, including selection. The
evaluator separately preserves the numeric intermediate dtype and leaf/layout
requirements. Adding an adapter does not automatically admit mixed dtypes.

Scalar reduction ordering properties live in `detail/expression_operation.hpp`.
That explicit operation trait is independent of batch support: a backend
implementation alone does not establish permission to interleave outputs.
Node, layout and rank dispatch remain compile-time structural decisions;
these traits add neither expression state nor runtime operation dispatch.

The backend vectorizes built-in arithmetic trees when the output and every
numeric leaf and intermediate result have the same `float` or `double`
type. Numeric scalars must have that same type too: `2.0f * float_expression` qualifies,
whereas `2.0 * float_expression` retains the scalar evaluator's intermediate
double precision. Mixed integer division also stays scalar, so SIMD cannot
turn `int(3) / int(2) + float(0)` into `1.5` in some lanes.

Homogeneous `minimum`, `maximum`, and `clip` also use numeric batches. They
select with comparison masks to preserve NaN propagation and left-hand ties,
including the sign of zero; hardware min/max alone do not meet those rules.

These numeric trees can also contain six comparisons, named `logical_not`,
`logical_and`, `logical_or`, and eager `select`. Comparisons produce the numeric
batch's matching `batch_bool<T, Arch>`, rather than `batch<bool>`. Bool scalars
use predicate splats, and an inner select may choose predicates. The final
output must still be numeric. All children are read before selection; a mask
cannot protect invalid arithmetic or an invalid address in either branch.

```cpp
const auto a = np::as_expr(input);
np::eval_to(output, np::select(a < 0., 0., np::select(a > 1., 1., a)),
            np::xsimd_policy<>{});
```

Bool views and bool outputs retain scalar evaluation. Casts, maps, standard
math operations, unknown operation tags, or mixed numeric types also select
the scalar path for the whole tree before instantiating a batch reader.
This preserves each node's C++ dtype rules, including integer division inside
a floating-point select. No temporary mask array, callable slot, runtime
operation dispatch or additional expression field is introduced.

The shape, lifetime, and alias preconditions are identical to
scalar `eval_to`. Output must be logically disjoint from each input or exactly
correspond to that input at every coordinate. Complete batches load before
their output store, making exact pointwise in-place evaluation valid.
Partial overlap is a caller precondition violation; no temporary is allocated.
The boundary first checks linear compatibility: array leaves with the output's
actual shape and packed traversal order use the existing packed batch loop.
Other expressions can use row batches when the output is `layout_right` and
every leaf's innermost step is statically proven to be one or zero:

- `layout_right` views with a static innermost extent greater than one use unit loads.
- Static singleton innermost extents, rank-zero views and scalars use splats.
- Missing leading axes retain the ordinary right-aligned outer projection.

For example, `[dynamic_rows, 300]` plus `[300]`, or plus `[dynamic_rows, 1]`,
qualifies. Broadcasts containing dynamic innermost extents retain the scalar
coordinate traversal, since a dynamic extent can be either one or the full row.
Mixed layouts, padded slices and positive-stride inputs/outputs retain that traversal as well.
There is no combinatorial runtime dispatch over leaf strides and no shape or
load-kind branch inside the batch loop.

```cpp
using matrix_shape = np::extents<size_t, np::dynamic_extent, 300>;
using row_shape = np::extents<size_t, 300>;
auto x = np::as_expr(np::tensor_view<const double, matrix_shape>(prices, days));
auto b = np::as_expr(np::tensor_view<const double, row_shape>(bias));
np::eval_to(output, x + 0.5 * b, np::xsimd_policy<>{});
```

The same projection binding and outer walker serve both scalar and SIMD rows.
An internal executor type selects the row loop; it adds no field to expression
nodes and does not change the default scalar definition depending on which
headers a translation unit includes. Per-row splat values are loaded once,
cached as batches, and reused by the tail. Static row lengths survive the
callback boundary, allowing the compiler to discard an empty tail.

Loads and stores use unaligned operations. Full batches never cross the
logical allocation end, and remaining elements keep the tree's scalar operations
and intermediate types.
Zero-size views do not offset or dereference null handles. Shapes are checked
once through the existing assertion level at the evaluation boundary.

The default contract follows ordinary scalar arithmetic under the same
compiler floating-point options. The backend does not request reassociation,
approximate division, or fused operations. Compiler flags such as fast math
can change both scalar and SIMD behavior; trapping floating-point exceptions
and strict floating-point environment sequencing are outside this interface.

Explicit SIMD avoids relying on a compiler's automatic vectorization of
nested expression objects. It does not establish universal zero overhead:
construction, generated loops, tails, code size, and representative throughput
must still be compared with an equivalent handwritten SIMD implementation.
The row implementation still inherits generic projection setup and temporary
metadata; MSVC can retain a one-time outlined walker call and security-cookie
work. Prototype comparisons found packed row loops on GCC, Clang and MSVC,
while these setup differences remained. No throughput-equivalence claim follows
from vectorization or the absence of per-element function calls.

For the typed-predicate extension, paired `-O3 -ffp-contract=off` probes on
Clang 18 and GCC 12 compared packed clipping, a range predicate and a bool
scalar gate with handwritten xsimd loops, at baseline x86-64 and native ISA.
The expression loops contained no residual calls or intermediate arrays;
whole-function instruction counts still differed because of setup and scalar
tail organization. Existing packed arithmetic and broadcast-row probes kept
their instruction counts; Clang's instructions were unchanged, while GCC
occasionally reversed equivalent compare operands. These are code-generation
observations, not measured throughput or branch/cache-counter results.
