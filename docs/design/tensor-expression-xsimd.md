# Explicit SIMD expression evaluation

`<wjr/np/expression_xsimd.hpp>` adds an explicit backend to the packed
expression evaluator. Link `wjr::np` and `wjr::simd_xsimd`; the latter retains
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

The first backend vectorizes built-in arithmetic trees only when the output,
every leaf, and every intermediate result have the same `float` or `double`
type. Scalars must have that same type too: `2.0f * float_expression` qualifies,
whereas `2.0 * float_expression` retains the scalar evaluator's intermediate
double precision. Mixed integer division also stays scalar, so SIMD cannot
turn `int(3) / int(2) + float(0)` into `1.5` in some lanes.

The shape, lifetime, and alias preconditions are identical to
scalar `eval_to`. Output must be logically disjoint from each input or exactly
correspond to that input at every coordinate. Complete batches load before
their output store, making exact pointwise in-place evaluation valid.
Partial overlap is a caller precondition violation; no temporary is allocated.
The boundary checks linear compatibility once: all array leaves must have the
output's actual shape and packed traversal order. Broadcast, mixed-layout, and
positive-stride expressions use the shared scalar coordinate traversal instead.
No batch loop checks shape or silently loads a broadcast leaf as a full batch.

Loads and stores use unaligned operations. Full batches never cross the
logical allocation end, and remaining elements use the original scalar tree.
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
