# Expression design experiment

This is an executable **design sketch**, not an installed `wjr::np` expression API.
It demonstrates value closures, arithmetic syntax and evaluation into an existing
buffer. The prototype is deliberately limited to double-valued, already bound
iteration domains. It does not implement general shape deduction, broadcast
validation, alias planning, reductions or an XLA graph.

```cpp
auto a = ex::as_expr(std::span{x});
auto b = ex::as_expr(std::span{y});
auto difference = a - b;
auto expr = 0.5 * difference * (a + b);
ex::eval_to(std::span{output}, expr);
```

`main.cpp` is the short runnable example. `detail/expression.hpp` contains the
isolated prototype. `validation.cpp` compares it with direct loops and optionally
uses Google Benchmark. These files are not included by production WJR targets.
See [the expression architecture](../../../docs/design/tensor-expressions.md)
for the intended public contract and the remaining acceptance gates.

```sh
cmake -S examples/np/expression_design -B build/expression-design -G Ninja \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/expression-design --parallel
ctest --test-dir build/expression-design --output-on-failure
./build/expression-design/expression_example
```

These commands require Ninja and use its single-configuration output paths.
Expected example output: `-7.5 -2.5 2.5 7.5`.
On Windows, use a configured compiler/SDK environment; MSVC receives the existing
preprocessor mode required by WJR headers. C++23 can be selected explicitly.

The checks exercise returned local expression nodes, empty inputs, small/static
arrays, repeated subexpressions, row binding and buffers ending at the logical
tail. The allocation counter covers expression construction/evaluation, excluding
the test's explicit buffer allocation and printing. It does not prove that an
arbitrary future functor cannot allocate.
The allocation override is disabled under ASan, whose Windows runtime provides
its own operators; allocation counting and sanitizer checks are separate runs.

For the optional SIMD experiment, install the reviewed xsimd **14.3.0** package
and add `-DWJR_EXPR_XSIMD=ON`. Ordinary builds neither download nor require it.
The explicit batch evaluator has a scalar tail; both paths need separate
floating-point and code-generation review. This is not runtime ISA dispatch.

For timing, install Google Benchmark (tested with 1.9.0) and also configure
`-DWJR_EXPR_BENCHMARK=ON`:

```sh
./build/expression-design/expression_benchmark \
  --benchmark_min_time=0.1s --benchmark_repetitions=7 \
  --benchmark_enable_random_interleaving=true \
  --benchmark_out=expression-comparison.json
```

The comparison includes sizes 4, 300 and 262144, plus an alternating-size workload.
It uses real time because short Windows CPU-time measurements have limited
resolution. Record compiler, CPU, ISA flags, SIMD version and measurement noise;
the prototype has not passed the complete zero-overhead acceptance criteria.
In particular, the measured scalar MSVC expression loop does not autovectorize
like its direct counterpart. No hardware branch/cache counters were collected.
