# Ordered expression column sums

`sum_to(out, expression, axes<0>)` can fuse eligible two-dimensional arithmetic
and pure map inputs into four independent ordered column accumulators. This is
an internal execution choice; the public API and per-column grouping do not change.

The maintained `NPExpressionColumns` benchmarks compare the actual API with a
handwritten four-column loop. Both include the same non-inlined pointer/scalar
call boundary, use the same child and accumulator types, and disable floating-point
contraction in this benchmark translation unit. Source data, coefficients and
shape are runtime snapshots; neither backend receives `restrict` or fast-math.
The cases cover packed square-plus-scalar, captured map plus column bias, padded
map input, float child with double accumulation, column splats and shuffled shapes.
Both implementations check their results before timing. No payload allocation
occurs inside either kernel.

## Observed Linux results

Three-repetition median wall-time ratios below are **expression / handwritten**;
smaller values are better. These are individual shared-runner observations.

| Case | GCC 13.3 | Clang 18.1.3 |
| --- | ---: | ---: |
| double square, 2048 × 300 | 1.00 | 1.00 |
| map plus bias, 2048 × 300 | 1.02 | 1.00 |
| padded map, 2048 × 300 | 0.96 | 0.99 |
| float child, double accumulator, 2048 × 300 | 0.97 | 0.92 |
| column splat, 2048 × 300 | 1.22 | 1.00 |
| shuffled square dimensions | 0.97 | 1.00 |
| shuffled padded map dimensions | 0.97 | 0.99 |
| shuffled column splat dimensions | 1.20 | 1.00 |

[GCC run 39088397940](https://github.com/wjr-z/wjr/actions/runs/39088397940)
used the final reader fix; its JSON and SysV assembly are attached as artifacts.
[Clang run 39087551294, attempt 2](https://github.com/wjr-z/wjr/actions/runs/39087551294/attempts/2)
used the same column implementation before that reader annotation. Both used
C++20, Release, native, `-ffp-contract=off` and the existing NP benchmark job.
The final annotation leaves the paired Windows Clang arithmetic kernels unchanged;
Clang/SysV was not rerun solely for the annotation and its assembly was not retained.

The GCC runner reported four logical CPUs, L2 1.25 MiB and L3 48 MiB; the Clang
runner reported four logical CPUs, L2 512 KiB and L3 32 MiB. Exact CPU model was
not recorded for the final GCC run. The machines differ, so their absolute times
and measurements from different runs must not be used as controlled speedup ratios.

The initial single-kernel research compared the previous linewise API with a
four-column candidate in the same executable. It showed opportunity on GCC and
Clang, including shuffled dimensions, but is not a direct measurement of every
final API path against the previous release. Source and raw results remain in
[GCC research](https://github.com/wjr-z/wjr/actions/runs/39086622817) and
[Clang research](https://github.com/wjr-z/wjr/actions/runs/39086932773).

## Generated code and limits

The first two-mode draft exposed a GCC 13 case where the inner mapped-reader
lambda was outlined into per-element calls. Applying the existing guarded
`WJR_FORCEINLINE_LAMBDA` to the two bound-reader lambdas preserves the outer
reader's inlining intent. The final GCC/SysV map and padded-map four-column loops
have no reader calls or per-row stack metadata reloads. Cold stack-check failure
calls and setup remain. Windows GCC 12/Clang 18 ordinary arithmetic kernels have
identical normalized instructions before and after this annotation.

The remaining GCC column-splat difference is about 20% in these observations.
Small inputs still pay setup and dispatch; code size and compiler decisions matter.
These tests do not establish universal zero overhead, an optimal width for every
tree, or performance on other CPUs. No hardware branch/cache counters were collected.
Throwing callbacks, math and unknown operations retain the existing linewise path;
their ordering/error-state contract is not exchanged for this optimization.

## Internal block width

The column driver now takes a positive compile-time width:
`sum_expression_columns_to<N, Unit, E, Accumulator>`. Its block helper keeps
independent states and expands lanes at compile time; the outer step is `N`.
Widths above four use four-column blocks before the final scalar tail, so
`N=8` with seven remaining columns processes `4+1+1+1`.

The public evaluator still selects four. The original four-scalar block is
retained because a uniform array implementation changed Clang's register
allocation; packed/map/padded/splat probes then kept identical GCC 12/Clang 18
instructions before and after this refactor. Widths one and eight have
correctness coverage, rather than a new performance recommendation. Future
width-specific code can use `if constexpr` or tag overloads in the block helper;
there is no runtime width policy or ISA-dependent portable definition.
