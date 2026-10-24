# Adjacent run boundaries

`<wjr/np/run_offsets.hpp>` provides `run_offsets_to(destination, keys, equal)`.
It emits `0`, each adjacent run's starting position, and the final source length,
then returns the number of runs. The default callable is `std::equal_to<>`.

For keys `[1, 1, 2, 2, 1]`, offsets are `[0, 2, 4, 5]` and the return value is
three. Runs describe adjacency: repeated nonadjacent keys remain separate.
No sorting, hashing, allocation, or source mutation is performed.

## Storage and callable contracts

- Keys are a rank-one arithmetic view with packed, positive-stride, or padded
  layout. Volatile elements are excluded.
- Output is a writable packed rank-one integral view excluding `bool`. Its type
  must represent the source length, including the final sentinel. For example,
  `uint8_t` supports length 255, rather than 256.
- The destination is disjoint from the source and has capacity for the actual
  number of runs plus one. The unused suffix remains untouched. Empty input
  still writes one zero, so needs one output element; a null empty source is valid.
- The callable is borrowed for the call, including noncopyable and stateful
  lvalues. It receives const references to adjacent keys, in source order,
  exactly `N-1` times for nonempty input. Its result is explicitly converted to
  `bool`; `noexcept` reflects both invocation and this conversion.
- Keys remain stable during the call. The callable must not modify key storage
  or access destination storage; its external bookkeeping is otherwise allowed.
- Default equality groups signed zeros, but each NaN starts a new run. A custom
  equality callable may group NaNs or implement another adjacent relation.
- If comparison throws, previously emitted starts remain written. There is no
  rollback and no completed final-sentinel guarantee.

Capacity and representability are trusted caller preconditions using existing
level-two assertions, rather than an always-on validation pass. The implementation
performs `N-1` comparisons and `R+1` offset writes, with no temporary owner or
initialization pass. Packed keys use a compile-time unit stride; other supported
layouts supply their existing stride. There is no automatic SIMD or unroll policy.

The emitted span directly feeds `segmented_sum_to`, `segmented_mean_to`, or
`segmented_reduce_to`. See `examples/np/run_offsets.cpp`; callers own both the
boundary buffer and any reduction output. The benchmark compares the full public
call to the equivalent handwritten boundary loop, including empty-input behavior,
and consumes the returned run count. Performance evidence is recorded with the MR.
