# Expression extrema

`<wjr/np/expression_extrema.hpp>` provides the terminal value operations
`min(expr)` and `max(expr)`, and caller-output axis operations
`min_to(dst, expr, axes<I>[, keepdims])` and `max_to(...)`. The same axis
operations accept a `tensor_view` directly. They evaluate the expression at the
call site; they do not create a lazy reduction node, payload temporary or
allocation. See the runnable
[expression extrema example](../../examples/np/expression_extrema.cpp).

The full reduction uses the expression's natural right-aligned broadcast
domain and returns its value type without promotion. Its trusted input domain
must be nonempty; there is no identity for an empty minimum or maximum. An
empty output view performs no expression reads. A nonempty output with an
empty reduced axis violates the trusted input precondition.

Both full and axis reductions visit values in logical row-major order. Along a
reduced axis, coordinates increase from zero. Equal values retain the first
value, including the sign of the first equal zero. For floating values, the
first NaN is returned and ends evaluation of that full fold or output line;
remaining map calls are therefore not guaranteed. This is not `fmin`/`fmax`
ignore-NaN behavior. NaN payload identity, signaling behavior and strict
floating-point-environment sequencing are not promised.

Axis output shape is the natural broadcast shape with the selected axis
removed, or retained with extent one when `keepdims` is passed. The output and
every expression input leaf must be logically disjoint. Each line is reduced
into a local value and written only when the line completes. If an expression
read throws, earlier output lines stay committed and the failing line remains
unchanged. A first NaN completes that line under the same short-circuit rule
as full-view extrema. The caller owns the expression and all borrowed views
through the synchronous call. The destination element type must accept
assignment from the expression value type; conversion occurs once at the
completed line store.

The implementation binds expression data and callable references once. Packed
compatible expressions use a linear walk; other layouts bind leaf strides and
use the existing logical traversal. Broadcast leaves contribute zero stride,
so they are read once per logical result value as specified by the expression.
Repeated subtrees are evaluated at each occurrence; no CSE or caching is
provided. Mapping validity, positive strides, representable offsets and
shape compatibility remain trusted descriptor preconditions.
