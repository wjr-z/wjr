# Explicit partial pairwise sums

`<wjr/np/pairwise.hpp>` provides `pairwise_sum<Accumulator = void>(view)`.
It is a direct view kernel, with no expression wrapper, allocation or temporary
payload. Default `sum` keeps its existing ordered fold; grouping changes only
when the caller explicitly chooses this entry point.

The separate [expression overload](tensor-expression-pairwise.md) fuses a
restricted pure, nothrow, equally shaped packed tree into this same grouping;
it does not broaden the view overload or change the default sum policy.

```cpp
const auto result = np::pairwise_sum(samples);
const auto wider = np::pairwise_sum<double>(samples);
```

The accumulator must be an unqualified floating-point type. Its default is
the input type's C++ `T + T` result; integral input therefore needs an explicit
floating-point accumulator. Every input is converted before it is added.
Only the default accessor and nonvolatile arithmetic elements are accepted.

Packed `layout_right` views of any rank use logical row-major flattening.
Rank-zero and rank-one views also accept the other reviewed positive-stride
layouts. Rank greater than one with `layout_left`, `layout_stride` or padding
is rejected, so the API never silently substitutes a different traversal or
grouping. Views, logical sizes, addressed spans and positive strides must meet
the ordinary trusted-input preconditions. Empty input returns before reading
data or stride metadata; rank zero reads exactly one object.

## Fixed grouping

The implementation adapts the real-valued partial pairwise scheme in
[NumPy 2.2.0](https://github.com/numpy/numpy/blob/v2.2.0/numpy/_core/src/umath/loops_utils.h.src).
The cutoff and grouping are numerical semantics, rather than tuning settings:

1. Empty input returns positive zero.
2. A nonempty segment shorter than eight folds from negative zero in index order.
3. For lengths 8–128, initialize eight accumulators from the first eight inputs.
   Accumulator `j` then receives indices `8+j, 16+j, ...` before the final tail.
   Merge as `((a0+a1)+(a2+a3))+((a4+a5)+(a6+a7))`, then add remaining inputs
   sequentially in index order.
4. Longer segments split at `floor(length/16)*8`. Evaluate the left segment,
   evaluate the right segment, and add the two results.

Packed and rank-one strided views of the same sequence have the same grouping.
The contiguous recursive specialization passes only pointer and count; the
strided specialization carries one additional step. The balanced recursion
uses `O(log length)` stack and no heap. Its depth is bounded by the bit width
of `size_t`; exact frame sizes and register spills remain compiler-dependent.
There is no runtime layout test or division in a per-element strided load.
Small leaves remain available for inlining. The recursive tree is kept out of
line to bound recursive code expansion; calls, recursion frames and the entry
cutoff branch remain real costs. This is not a claim of instruction-for-instruction
equivalence to every hand-written caller or a throughput improvement.

Under ordinary IEEE round-to-nearest settings, a nonempty all-negative-zero
sequence retains negative zero; a mixed-zero sequence produces positive zero.
NaN and infinities follow the chosen ordinary arithmetic operations. NaN
payloads, signaling behavior, trapping and strict floating-point environment
sequencing are not standardized by this interface. Compiler settings that
reassociate additions can change the stated grouping and its results.

This is not a NumPy dtype-promotion, dispatch or bit-identical-result promise.
It does not request fast math, approximate operations or overflow protection.
Pairwise summation often reduces rounding error, but can still overflow, lose
small values or be less accurate than another grouping. It is not compensated
or correctly rounded summation. A wider accumulator is an independent choice.

## Evidence and reuse

Tests compare the exact grouping against an independent oracle across leaf,
tail, split and stride boundaries. Exact integer totals provide cancellation
and magnitude references without assuming that `long double` is wider than
`double` on the target. The [example](../../examples/np/pairwise.cpp) demonstrates
this distinction. No universal throughput or accuracy improvement is claimed.

The adapted algorithm's copyright and BSD-3-Clause terms are retained in
[numpy-pairwise.txt](../licenses/numpy-pairwise.txt), also installed with the
optional NP component under `share/licenses/wjr`.
