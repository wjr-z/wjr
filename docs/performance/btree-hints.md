# Explicit B-tree hints: sorted wins and random costs

The final API keeps ordinary range insertion, range construction and list assignment on their existing paths. The automatic-hint prototype improved sorted imports but regressed random imports, so it was **not adopted as the generic default**. There is no runtime sorting heuristic or new threshold.

Callers that know their input contains ordered/local runs can choose `insert_with_hint(first, last)`, or use the standard-style single-element hint family. The explicit bulk importer accepts unsorted input too and falls back correctly; its validation work can cost time on random input. See the [API and lifetime contract](../design/btree-hints.md).

## Hinted import against the existing emplace loop

Ratios are median hinted-import / ordinary per-element emplace wall time; below 1 is faster. All WJR containers use N16, uint64 keys and uint64 mapped values. Sorted input follows the comparator. `blocks32` shuffles disjoint blocks while retaining ascending order inside each block; random shuffles all keys. Build and destruction are timed, input generation/oracle checks are excluded. Every group retains five samples in [the source data](data/btree-hints.json).

| Profile | Container | Elements | Sorted | Blocks of 32 | Random |
| --- | --- | ---: | ---: | ---: | ---: |
| Windows Clang | set/wjr16 | 4096 | 0.282 | 0.355 | 1.030 |
| Windows Clang | set/wjr16 | 32768 | 0.238 | 0.335 | 1.181 |
| Windows Clang | map/wjr16 | 4096 | 0.756 | 0.692 | 1.101 |
| Windows Clang | map/wjr16 | 32768 | 0.774 | 0.776 | 0.985 |
| Windows Clang | map/compact16 | 4096 | 0.336 | 0.385 | 1.067 |
| Windows Clang | map/compact16 | 32768 | 0.298 | 0.356 | 1.135 |
| Linux GCC | set/wjr16 | 4096 | 0.489 | 0.556 | 1.156 |
| Linux GCC | set/wjr16 | 32768 | 0.404 | 0.482 | 1.106 |
| Linux GCC | map/wjr16 | 4096 | 0.525 | 0.558 | 1.140 |
| Linux GCC | map/wjr16 | 32768 | 0.439 | 0.574 | 1.096 |
| Linux GCC | map/compact16 | 4096 | 0.533 | 0.510 | 1.140 |
| Linux GCC | map/compact16 | 32768 | 0.444 | 0.464 | 1.094 |
| Linux Clang | set/wjr16 | 4096 | 0.209 | 0.270 | 1.065 |
| Linux Clang | set/wjr16 | 32768 | 0.188 | 0.278 | 1.051 |
| Linux Clang | map/wjr16 | 4096 | 0.425 | 0.456 | 1.075 |
| Linux Clang | map/wjr16 | 32768 | 0.520 | 0.513 | 1.052 |
| Linux Clang | map/compact16 | 4096 | 0.219 | 0.270 | 1.069 |
| Linux Clang | map/compact16 | 32768 | 0.207 | 0.288 | 1.034 |

Sorted and local-run imports benefit in all three profiles. Random input does not: its ratios span 0.985?1.181. In particular, the Windows 32768-element set and compact map regress by about 18.1% and 13.5%. Near-one results are not evidence of improvement. These losses are why callers must explicitly select the hinted importer; ordinary range APIs retain the original implementation.

## Standard-container context

At the preselected larger size, 32768, the following ratios compare WJR hinted import with the standard container's ordinary range insertion. The standard library uses its own tree representation rather than an artificial N16 setting. This is context for the selected successful-operation contract, not a universal ranking.

| Profile | Container | Sorted | Blocks of 32 | Random |
| --- | --- | ---: | ---: | ---: |
| Windows Clang | set/wjr16 | 0.235 | 0.151 | 0.520 |
| Windows Clang | map/wjr16 | 1.237 | 0.706 | 0.845 |
| Windows Clang | map/compact16 | 0.264 | 0.175 | 0.496 |
| Linux GCC | set/wjr16 | 0.734 | 0.421 | 0.661 |
| Linux GCC | map/wjr16 | 0.397 | 0.589 | 0.904 |
| Linux GCC | map/compact16 | 0.256 | 0.341 | 0.642 |
| Linux Clang | set/wjr16 | 0.326 | 0.195 | 0.599 |
| Linux Clang | map/wjr16 | 0.359 | 0.441 | 0.813 |
| Linux Clang | map/compact16 | 0.095 | 0.176 | 0.575 |

The Windows legacy map's sorted import still loses to std::map (1.237), despite benefiting relative to its own ordinary emplace loop. Ties, losses and every small-size sample remain in the source data. No iterator/reference survives mutation in these workloads. Different invalidation/exception guarantees and element-address mechanisms remain relevant when selecting a container.

## Measurement and final entry-point equivalence

- C++20 Release, baseline x86-64, Google Benchmark 1.9.0; std::allocator with no counters in timing. Five raw repetitions, minimum 0.03 seconds each. Seed 1656023321; 4096/32768 distinct keys, mapped values `key * 7 + 1`.
- Windows 11 22631, Intel Core i5-13500HX, process pinned to CPU 0; Clang 18.1.8 GNU frontend, MSVC STL 143.202408, `-O3 -DNDEBUG -march=x86-64`.
- Ubuntu 24.04, Xeon Platinum 8370C shared VM, unpinned. GCC 13.3.0 and Clang 18.1.3 ran sequentially on the same worker, with `-O3 -DNDEBUG -march=x86-64`. Each JSON retains its exact libstdc++ stamp and compiler/flags/runtime context.
- Permutations are identical for candidates within each profile. `std::shuffle` can differ across STLs. Different CPUs do not isolate an OS effect; compare ratios inside profiles rather than absolute durations across machines.
- [Linux run 41071711766](https://github.com/wjr-z/wjr/actions/runs/41071711766) contains both compiler outputs, compile commands, CPU/OS and source records. Windows was collected locally on 2026-10-30. The checked-in compact source retains all 720 raw normalized samples and original-file hashes.
- Timed source was 5b22bdf111915172d0ef0ec319d18bdd5135cdd4. After rejecting automatic adoption, its bulk function body was moved unchanged to the explicit `insert_with_hint` entry. That body is byte-identical. A Windows Clang baseline assembly check of the full benchmark before/after API exposure retains the same 16088 instructions after normalizing only the renamed API symbol. The benchmark now selects the explicit entry; the measured retained algorithm is unchanged. Generic range/list/constructor bodies were restored to main's originals.
- No hardware counters or requested-memory campaign was run for this API change, and no memory win is claimed. The benchmark consumes tree size and applies a memory barrier; its oracle checks complete final contents before timing and is destroyed before the measured loop. Both input buffers and validation are outside timing.

## Reproduce

The `btree_hint` target is excluded from the default build. It compares the unchanged emplace loop, WJR explicit hinted import, and standard-container range insertion, and retains sorted/blocks32/random groups:

```sh
cmake -S . -B build-hint -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree \
  -DWJR_CPU_TARGET=baseline -DWJR_CXX_STANDARD=20
cmake --build build-hint --target btree_hint --parallel
./build-hint/bin/btree_hint --benchmark_min_time=0.03s \
  --benchmark_repetitions=5 --benchmark_report_aggregates_only=false \
  --benchmark_out=hint-timing.json
```

Raw WJR policy labels use `range` for the retained hinted-import body; std policy `range` is its normal insertion API. The final WJR generic range is deliberately not that candidate. Reproduction records source, compiler, STL, flags, CPU target and trace seed in JSON.
