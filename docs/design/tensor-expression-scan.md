# Fused expression prefix sums

`<wjr/np/expression_scan.hpp>` adds
`cumsum_to<Accumulator = void>(output, expression, axes<Axis>)`.
It evaluates each expression value directly into an inclusive prefix sum;
there is no intermediate tensor, allocation, lazy reducer or callable copy
during evaluation. The view overload in `<wjr/np/scan.hpp>` remains available.

```cpp
const auto residual = np::as_expr(samples) - np::as_expr(row_baseline);
np::cumsum_to(prefix_energy, residual * residual, np::axes<1>);
```

The destination must have exactly the expression's natural broadcast shape
and rank. This operation adds no assignment broadcast. Select one compile-time
axis; rank-zero expressions have no scan axis. Static shape mismatches are
rejected by constraints; dynamic compatibility is an assertion-level caller
precondition, resolved once before data binding. Empty domains perform no
reads, callable invocations or pointer arithmetic.

Every line starts with `Accumulator{}` and adds values in increasing axis
order, without reassociation. The default accumulator is the expression
value type's C++ `T + T` result, just as for view scans. Each node keeps its
own dtype before conversion to the accumulator. Each completed prefix is
converted to the output type; a narrow output never becomes the state for the
next prefix. Signed overflow and out-of-range floating-to-integer conversions
retain ordinary C++ preconditions. An explicit wide accumulator does not widen
arithmetic inside the expression; use `cast` at the required node when needed.

Each input leaf must be logically disjoint from the written region or map
every expression coordinate to exactly the corresponding output object.
Broadcast leaves must be disjoint from that region. Exact pointwise in-place
scans read the current input expression before overwriting that coordinate;
partial overlap is a precondition violation. No temporary or runtime alias
search is introduced.

Throwing reads propagate normally and commit only completed prefixes. They
finish one line before starting another, in logical row-major order of the
remaining axes. The current failing prefix and later prefixes remain untouched.
The public `noexcept` follows the expression's actual read operation. This is
a partial-commit guarantee, rather than transaction rollback.

Packed compatible expressions scanning a contiguous axis use direct linear
leaf reads. Other layouts and broadcasts bind each leaf stride once and use
integer offset increments. Padded and positive-stride views are supported,
including scalar map/select/math fallback paths. There is no per-element
shape validation. Generated code and measured throughput still depend on
layout, expression structure and compiler; fusion alone is not a universal
zero-overhead or speedup guarantee.

Rank-two axis-zero scans can interleave four independent columns when reads
are nonthrowing, each input column step is zero or one, and the output column
step is one. Only built-in arithmetic and contractually pure map trees qualify.
Math, unknown operations and throwing reads retain sequential lines even when
their signatures happen to be `noexcept`. Each column still has its own ordered
accumulator; this changes neither dtype nor the fold within a column.

Baseline x86-64 paired probes with `-O3 -ffp-contract=off` found the same
instruction counts as handwritten rank-one/last-axis loops on Clang 18; GCC 12
retained one additional instruction. A local alternating-order axis-zero
microbenchmark covered four shapes from 8x9 to 1024x300 and compared a competent
four-column loop. Medians were within approximately 17%, while the earlier
single-column traversal had been 1.4–4.4 times slower on the larger shapes.
The full axis-zero function still has larger code size because it contains
unit/splat and generic alternatives plus metadata setup. This is a measured
tradeoff, not a claim of minimal code size or performance equivalence for other
expressions, compilers, data distributions or machines. Hardware counters and
Linux timing were not collected for this scan experiment.

The runnable [example](../../examples/np/expression_scan.cpp) builds prefix
squared residuals directly and uses prefix differences for window totals.
