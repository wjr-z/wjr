# Tensor cost comparisons

The `np` benchmark profile measures the actual public mdspan expression API.
It is separate from examples and ordinary correctness CI. No timing threshold
is a portable pass/fail guarantee.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=np \
  -DWJR_ENABLE_NP=ON -DWJR_ENABLE_XSIMD=ON -DWJR_BENCHMARK_XSIMD=ON \
  -DWJR_CPU_TARGET=baseline -DWJR_USE_GMP_TESTS=OFF
cmake --build build --target benchmarks --parallel
./build/bin/benchmarks --benchmark_filter='np_' \
  --benchmark_repetitions=7 --benchmark_min_time=0.05 \
  --benchmark_out=tensor-results.json --benchmark_out_format=json
```

Disable `WJR_BENCHMARK_XSIMD` and `WJR_ENABLE_XSIMD` to build only the portable
scalar comparisons. The existing `native` or `x86-64-v3` CPU profiles can be
selected deliberately; record the resulting compiler flags and CPU. Explicit
SIMD must be compared against the handwritten SIMD backend at the same ISA.

The manually triggered Benchmarks workflow also accepts `profile=np` and
uses the existing JSON artifacts and comparison summary. The weekly default
remains `core`. No extra Windows or per-PR benchmark job is added.

## Cases

`np_fixed<Backend, Square, Reuse>` includes the following independent choices:

| Parameter | Meaning |
| --- | --- |
| `manual` | Ordinary C++ scalar loop, allowing compiler vectorization |
| `expression` | Public `as_expr` and scalar `eval_to` |
| `manual_simd` | Explicit xsimd batches and a bounded scalar tail |
| `expression_simd` | Public expression with explicit xsimd policy |
| `Square=false` | `alpha*x + beta*y` |
| `Square=true` | `(x+y)*(x+y)`, with a local reused subtree description |
| `Reuse=false` | Construct and evaluate across the same noinline pointer/scalar boundary |
| `Reuse=true` | Reuse an expression built outside the loop; evaluator binding remains inside |

The fixed-size cases include empty, tiny, daily-universe-sized, cache-resident,
and streaming arrays. `np_mixed<Backend, Alternating>` tests alternating empty
and long lengths, or a deterministic shuffled schedule of eight short/medium
lengths. Both change the branch context compared with repeating one length.
The shuffle is seeded; its exact permutation may differ between STL versions.

Output buffers are preallocated for the pointwise cases. Each backend is
checked against the same ordinary scalar formula before timing. A memory
clobber keeps output stores observable; printing and input generation are
outside the measured loop. Inputs and coefficients are dyadic values chosen
to make this correctness smoke independent of minor contraction differences.
The public operation's full floating-point semantics still need unit tests.

`np_materialize<Storage>` includes allocation, initialization, evaluation and
destruction in each iteration. It compares default-initialized raw scalar
arrays, value-initialized `std::vector`, `wjr::vector(default_construct)`, and
the default tensor factory, and actual owning `np::eval`. This exposes costs
that preallocated `eval_to`
cannot measure. The ordinary tensor factory's value initialization is part of
its contract; it should not be confused with an uninitialized output factory.
The raw/container/factory cases share the same pointwise evaluator to compare
storage costs. The owning `eval` case additionally includes its natural-shape
resolution and error contract; its total cost cannot be attributed solely to
initialization.
The JSON also records descriptor/expression sizes and owning-object sizes.
These `sizeof` values exclude allocator-internal metadata, backing allocations,
and capacity slack; they are representation evidence, not total resident memory.

## Interpreting evidence

Zero library allocations in a preallocated loop do not establish zero
overhead. Inspect setup and loop code separately on GCC, Clang and MSVC:

- Initialization writes before a full overwrite, including retained memset.
- Descriptor and closure copies that survive optimization.
- Metadata reloads, runtime shape branches, alias guards and function calls.
- Vectorization, tail handling, stack spills and security-cookie setup.
- Object size, code size, and construction across non-inline boundaries.

Use stable CPU affinity/frequency when possible and retain repeated samples,
compiler/standard/CPU-target context, actual CPU model and SIMD lane labels.
Check cache and branch counters when available. This suite does not collect
hardware counters or automatically control machine scheduling. A noisy
GitHub-hosted runner or a shorter instruction listing is insufficient to
claim throughput equivalence or a regression.
