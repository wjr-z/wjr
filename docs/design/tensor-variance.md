# Two-pass view variance

`<wjr/np/variance.hpp>` adds `variance<Acc = void>(view, ddof = 0)`.
This is an eager scalar terminal for borrowed arithmetic views, with no allocation
or intermediate tensor. It computes the ordered mean, then the ordered sum of
squared deviations, divided by `N - ddof`. Both passes use logical row-major
order. `ddof` is a nonnegative `size_t`; zero is population variance, one gives
the usual sample variance. If `N <= ddof`, the result is quiet NaN without reads.

The default accumulator is the input floating type, or double for integer/bool.
An explicit accumulator must be an unqualified floating type. Conversion occurs
before subtraction and multiplication. The mean, deviations, products, sum and
division all use that type. Rank zero contains one observation; its finite
population variance is zero. Positive strides, left/right and reviewed padded
layouts use the same view subset as mean.

The formulation follows the familiar centered two-pass definition described by
[NumPy variance](https://numpy.org/doc/stable/reference/generated/numpy.var.html).
WJR uses its own ordered folds, supports only nonnegative integral ddof, and
does not claim NumPy bitwise compatibility. No complex, masked, NaN-skipping,
streaming or overflow-resistant algorithm is implied. Nonfinite data and
overflow follow the ordinary arithmetic; even a constant finite input can suffer
mean overflow. A wider accumulator may help but is not an accuracy guarantee.
Fast-math, NaN payloads and floating exception timing are outside this contract.

The second pass composes the existing expression reader with ordered sum rather
than adding a parallel reducer or materializing deviations. The view is read
twice; shared ownership and input mutation are absent. A matched handwritten
two-loop baseline checks that this composition does not introduce hidden work.
Preliminary Windows Clang 18 baseline/native instruction sequences match that
baseline; GCC 12 has no additional calls and slightly fewer instructions in the
tested dynamic rank-one case. These are bounded code-generation observations,
not throughput claims for all layouts or platforms.

See [variance.cpp](../../examples/np/variance.cpp) for population and sample use.
