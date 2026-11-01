# Biginteger conversion benchmarks

The `all` benchmark profile includes `wjr_biginteger_to_digits`,
`gmp_biginteger_to_digits`, `wjr_biginteger_from_digits` and
`gmp_biginteger_from_digits`. GMP cases are available when GMP is enabled.
These compare raw base-2/8/10/16 digits: each output or input byte contains a
value from zero to `base - 1`. WJR uses `origin_converter_t` for the same
representation as GMP's low-level `mpn_get_str` and `mpn_set_str` functions.
They do not measure ASCII decoding or encoding.

Each implementation uses a local generator with seed 1, so a matching case
receives the same input regardless of registration order. Formatting inputs
have exactly the requested number of full-width 64-bit limbs. Parsing inputs
have exactly the requested number of digits, including a nonzero leading
digit and values across the complete base's digit range. The formatting
buffer includes GMP's required extra byte; parsing storage includes its
required extra limb.

Both formatting benchmarks preserve the original limb sequence on every
iteration. WJR's converter already preserves it. GMP's decimal converter
destroys its input, so its measured loop copies the original into preallocated
scratch before each call. Power-of-two GMP conversions preserve input and use
it directly. Storage allocation and input generation occur before the measured
loops; the required decimal copy remains part of the measurement. Output
results are kept observable.

The earlier `*_biginteger_*_chars` conversion cases were renamed to `*_digits`
with these corrected contracts. Their historical timings are not comparable:
GMP parsing previously received ASCII bytes instead of raw digits, WJR binary
and octal parsing could receive digits outside the requested base, and GMP
decimal formatting reused already modified limbs. This change corrects the
benchmark workload; it is not a library-throughput optimization.

For example:

```sh
cmake -S . -B build -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=all
cmake --build build --target benchmarks
./build/bin/benchmarks --benchmark_filter='biginteger_(to|from)_digits'
```

Contract reference: [GMP low-level conversion functions](https://gmplib.org/manual/Low_002dlevel-Functions).
