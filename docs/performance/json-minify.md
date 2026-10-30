# JSON minification

`json::minify` scans 64-byte blocks, identifies whitespace outside strings, and
compacts the retained bytes. When a complete non-final block has no removable
whitespace, it now stores the original SIMD vectors directly. This avoids the
compression table loads, byte shuffles, and output-offset calculations. Blocks
that remove bytes and the bounded final-block path keep their existing handling.

All source vectors are loaded before output is written, including when earlier
compaction has moved an in-place destination before its source. Input/output
capacity requirements and the returned output end are unchanged. This operation
does not become a JSON or UTF-8 validator.

## Reproduce

The `all` benchmark profile includes `BM_json_minify` and
`BM_json_minify_shuffled`:

```sh
cmake -S . -B build-json -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=all \
  -DWJR_USE_GMP_TESTS=OFF -DWJR_CPU_TARGET=x86-64-v3
cmake --build build-json --target benchmarks --parallel
./build-json/bin/benchmarks --benchmark_filter=BM_json_minify \
  --benchmark_min_time=0.05s --benchmark_repetitions=5
```

`layout:0` uses compact records, `layout:1` uses formatted records, and `layout:2`
deterministically mixes the two. Each record contains a string with spaces,
escaped quotes/backslashes, and a newline escape. Sizes range from one record to
1024 records. The shuffled case mixes 57 documents with different sizes and
layouts. Input construction and output allocation are outside timing; the
reported byte count is input bytes processed.

## Recorded comparison

The [Linux research run](https://github.com/wjr-z/wjr/actions/runs/41058361477)
retains raw samples, compiler commands, CPU details, assembly, symbol sizes, and
the exact source revision. The reference is WJR `0aaf8124`; candidate and reference
are separate externally called functions in the same executable. Five samples
alternate their order and process at least 256 MiB per dataset and implementation.
The production benchmark uses the same synthetic fixtures. The research probe
also uses `test/data/success/twitter.json` and its compacted form.

The runner was Ubuntu 24.04 on an AMD EPYC 7763 virtual machine. Both compilers
used C++20, `-O3 -DNDEBUG -DWJR_DEBUG_LEVEL=0`, with either
`-march=x86-64 -msse4.2 -mpclmul` or `-march=x86-64-v3 -mpclmul`.
Ratios below are median candidate/reference input throughput; higher is faster.

| Compiler / profile | Compact 1024 records | Formatted 1024 records | Mixed 1024 records | Shuffled | Twitter formatted | Twitter compact |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| GCC 14.2 / SSE4.2 | 1.820 | 1.014 | 1.128 | 1.191 | 1.052 | 1.700 |
| GCC 14.2 / AVX2 | 2.593 | 1.003 | 1.163 | 1.278 | 1.067 | 2.490 |
| Clang 18.1.3 / SSE4.2 | 1.850 | 0.992 | 1.130 | 1.177 | 1.022 | 1.773 |
| Clang 18.1.3 / AVX2 | 2.337 | 1.001 | 1.153 | 1.234 | 1.030 | 2.157 |

For the shuffled dataset, the five paired speedup ranges were respectively
1.175–1.194, 1.271–1.286, 1.165–1.183, and 1.231–1.237. The Clang AVX2 compact
synthetic sample had a 12.3% throughput coefficient of variation and a paired
speedup range of 1.730–2.364. Its Twitter compact speedups were 2.144–2.159.
The 0.8% Clang SSE formatted regression is retained as the cost of the additional
branch. Single compact records have no complete non-final block and remain near
parity (median ratios 0.981–1.007).

Function sizes, excluding shared constants, were:

| Compiler / profile | Reference bytes | Candidate bytes |
| --- | ---: | ---: |
| GCC 14.2 / SSE4.2 | 1817 | 1879 |
| GCC 14.2 / AVX2 | 1439 | 1455 |
| Clang 18.1.3 / SSE4.2 | 1594 | 1616 |
| Clang 18.1.3 / AVX2 | 1242 | 1226 |

## Windows comparison

The same probe was compiled with Clang 18.1.8 and the same C++20/optimization/ISA
flags on Windows, using an Intel Core i5-13500HX. The timing process was pinned
to logical processor 2; five samples alternated candidate/reference order and
processed at least 256 MiB per case. The
[individual throughput samples](data/json-minify-windows.csv) retain all sizes,
layouts, and simdjson results. Allocation and input generation remain untimed.

| Profile | Compact 1024 records | Formatted 1024 records | Mixed 1024 records | Shuffled | Twitter formatted | Twitter compact |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| SSE4.2 | 1.634 | 0.994 | 1.088 | 1.150 | 1.043 | 1.627 |
| AVX2 | 2.082 | 1.037 | 1.135 | 1.209 | 1.029 | 2.279 |

Short formatted documents regress: four records give ratios 0.972 (SSE4.2) and
0.937 (AVX2); sixteen formatted records give 1.005 and 0.952. The five AVX2
four-record ratios range from 0.905 to 0.953. Generated AVX2 code retains the
same 264-byte stack frame and nine saved nonvolatile XMM registers, with no new
spills. The additional mask test changes loop layout and adds a jump on the
compression path. Reversing the source `if` or using `continue` produces the
same code. The optimization retains this short-input tradeoff for the larger
compact and mixed-input gains; it adds no input-size heuristic.

The SSE4.2 shuffled speedup ranges from 1.020 to 1.349 and AVX2 from 1.184 to
1.324, with candidate throughput coefficients of variation of 5.7% and 3.0%.
Windows measurements therefore support the direction and tradeoff, with more
noise than the Linux shuffled samples. Windows Clang ASan also passes SSSE3 and
AVX2 differential probes; MSVC 19.42 `/O2 /std:c++20 /arch:AVX2` passes the same
correctness probe, without an MSVC timing claim.

## Mature implementation reference

The same probe compares the existing unmodified simdjson 3.9.1 amalgamation in
`examples/demo/src` ([release source](https://github.com/simdjson/simdjson/tree/v3.9.1),
[Apache-2.0 license](https://github.com/simdjson/simdjson/blob/v3.9.1/LICENSE)).
It explicitly selects `westmere` for SSE4.2 and `haswell` for AVX2. Comparison
inputs are valid JSON, all output bytes are checked against an independent scalar
minifier, and simdjson's error return is checked. This compares successful
minification within their shared contract, without equating malformed-input
diagnostics. No simdjson code is copied into the change.

| Compiler / profile | Compact 1024 records | Formatted 1024 records | Shuffled | Twitter compact |
| --- | ---: | ---: | ---: | ---: |
| GCC 14.2 / SSE4.2 | 1.478 | 0.825 | 0.980 | 1.468 |
| GCC 14.2 / AVX2 | 2.134 | 0.838 | 1.065 | 2.068 |
| Clang 18.1.3 / SSE4.2 | 1.586 | 0.888 | 1.042 | 1.585 |
| Clang 18.1.3 / AVX2 | 2.245 | 0.950 | 1.201 | 1.975 |

These ratios are candidate/simdjson throughput. WJR still trails simdjson for
formatted records; eliminating unnecessary compression does not close that gap.
Hardware branch/cache counters were not collected, and these measurements do
not establish performance on ARM or other processors.

The Linux probe passes ASan and UBSan for both compilers and profiles, with exact
input/output allocations, separate and in-place output, and 16,384 differential
random-byte cases over lengths 0–1023. The regression test additionally covers
successive preserved blocks after compaction across all 128 prefix alignments.
