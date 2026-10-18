# Full-view extrema

`<wjr/np/extrema.hpp>` provides `min(view)`, `max(view)`, `argmin(view)` and
`argmax(view)`. These are direct terminal view kernels: no allocation,
expression wrapper, temporary payload, optional result or lifetime retention.
Axis reductions and expression inputs are separate future capabilities.

The input must be a valid **nonempty** borrowed view. The entry uses
`WJR_ASSERT_L2` for this trusted-input precondition; empty input has no defined
identity or result. Callers needing a recoverable empty case inspect
`view.empty()` before calling. Valid extents, positive strides, representable
logical size and addressed span remain descriptor preconditions.

Default-accessor, nonvolatile arithmetic elements are supported, including
real `bool` storage. Values retain their unqualified input dtype: there is no
integer-to-floating conversion or accumulator promotion. Reviewed packed-left,
packed-right, positive-stride and padded layouts are accepted. Rank zero
returns its only value and index zero.

Every operation follows logical row-major order, independently of physical
layout. Equal values retain their first occurrence; `+0` and `-0` compare equal,
so the selected sign is the first one's sign. The first NaN propagates to the
value result and its logical index is returned by both argument reductions.
The kernel may return immediately on that NaN. This is not `fmin`/`fmax`'s
ignore-NaN behavior. NaN payload identity, signaling behavior and strict
floating-point-environment sequencing are not promised. Fast-math assumptions
that discard NaNs or signed zero are incompatible with these guarantees.

`argmin` and `argmax` return `size_t` logical flattened indices, rather than
physical offsets or multidimensional coordinates. For shape `[rows, cols]`,
recover coordinates as `{index / cols, index % cols}`. A tie follows the same
rule for all layouts, including transposed and padded views.

The packed-right and one-dimensional paths use direct loops; generic layouts
bind extents and strides once and traverse nested loops. Value-only
instantiations do not update an index counter. Early-NaN handling and stable
selection are part of the computation, and can limit vectorization relative to
reductions with different NaN or tie semantics. No throughput advantage or
universal minimum-overhead claim is made.

The function declarations tolerate pre-existing Windows `min`/`max` macros.
At a call site affected by those macros, spell `(wjr::np::min)(view)` and
`(wjr::np::max)(view)`, or use the application's existing `NOMINMAX` convention.

See the [padded-window example](../../examples/np/extrema.cpp).
