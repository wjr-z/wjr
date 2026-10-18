# Full-view NaN-skipping reductions

`<wjr/np/nan_reduce.hpp>` provides `nansum(view)` and `nanmean(view)` for one
existing arithmetic view. These are full-view scalar reductions; they do not
allocate an intermediate array or add axis, mask, planner, or SIMD policies.
Views use the same supported kernel layouts and default accessor as `sum` and
`mean`.

The accumulator type follows the matching ordinary reduction. `nansum` uses
`sum`'s default `T + T` result and accepts the same explicit accumulator types.
`nanmean` uses `mean`'s type rule: floating inputs keep their type, while
integer and `bool` inputs default to `double`; an explicit `nanmean` accumulator
must be floating point. Integer inputs compile directly to the existing
`sum`/`mean` functions, with no NaN test or valid-item count.

For floating inputs, each source value is tested for NaN before conversion to
the accumulator, then non-NaN values are folded in logical row-major order.
This is important for `nansum<int>(floating_view)`: a NaN is skipped before
floating-to-integer conversion. Finite values must still be representable in
an explicit integer accumulator. With an explicit integer accumulator, the
caller must ensure every retained value, including infinities, can be legally
converted and that accumulation does not overflow a signed accumulator.
`nanmean` divides once by the number of non-NaN values. `nansum` does not
maintain a count.

Empty and all-NaN `nansum` results are positive zero. Empty and all-NaN
`nanmean` results are `quiet_NaN` in its accumulator type. Infinities are not
filtered; they follow ordinary ordered arithmetic, so opposite infinities may
produce NaN. Skipping NaNs preserves the order of all remaining values, but
does not promise identical floating-point exception flags to a plain `sum`.

See [the example](../../examples/np/nan_reduce.cpp) and
`test/np/nan_reduce.cpp` for constexpr, dtype, empty, rank-zero, stride, padding,
layout, signed-zero, infinity, and NaN coverage.
