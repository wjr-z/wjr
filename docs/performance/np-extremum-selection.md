# Floating expression minimum and maximum

The floating-point selection in `minimum`, `maximum`, and their composed `clip`
nodes combines two predicates: keep the left operand when it is NaN or when
the ordered inclusive comparison selects it. Otherwise keep the right operand.
The boolean bitwise OR deliberately evaluates both predicates. This preserves
NaN propagation and left-biased ties, including signed zeros, while giving the
compiler a smaller selection mask than separate NaN and equality branches.
Integral selection, dtype promotion, expression storage, and evaluation remain
unchanged. Floating-point-environment and NaN-payload guarantees remain those
in [the API contract](../design/tensor-expression-minmax.md).

## Measured Linux comparison

[Run 39095167130](https://github.com/wjr-z/wjr/actions/runs/39095167130) records
the fixed research source `0648d11e0f0b51eaa945bbe848b097fb039535bb` and its raw
CSV, assembly, compiler versions, CPU description, source snapshots, and oracle
logs. It used Ubuntu 24.04, AMD EPYC 7763, GCC 13.3 and Clang 18.1.3 with
`-std=c++20 -O3 -march=x86-64 -ffp-contract=off -fno-fast-math`, debug level 0.
This is a baseline ISA measurement; native min/max timing was not collected.

Each candidate uses the actual expression evaluator with the corresponding
static operation and a matched direct loop, with identical dtype and NaN/tie
rules. No `restrict` or fast-math assumptions are added. The 112 cases per
compiler cover float/double, minimum/maximum, lengths 16/300/8192 and a mixed
sequence, and seven distributions: finite, ordered, equal-heavy, signed-zero,
sparse left/right NaNs, and mixed NaNs. Five rounds rotate candidate order;
the table compares per-case medians and aggregates their ratios geometrically.

| Candidate / previous actual expression | GCC | Clang |
| --- | ---: | ---: |
| Separate left/right NaN checks, strict comparison | 0.855 | 0.642 |
| Left NaN check, explicit tie, inclusive comparison | 0.653 | 0.485 |
| Combined boolean mask, selected implementation | 0.618 | 0.382 |
| Left NaN check, inclusive comparison | 0.660 | 0.399 |

The selected expression's per-case ratios to the previous expression ranged
from 0.546–0.830 with GCC and 0.252–0.578 with Clang. Its geometric mean ratio
to its matched hand loop was 0.938/0.989; the largest individual ratios were
1.097/1.074. These are observations from this runner, not a promise of universal
speedup or exact equivalence to every hand loop. Timing affinity was not pinned
and hardware counters were not collected.

GCC emits packed comparisons and mask selection with fewer comparisons in the
main loop. Clang's previous double loop remained scalar; the selected loop
vectorizes. Both selected actual interfaces have no per-element function calls.
Total instruction counts alone are not used as a throughput claim.

The ordinary and separate ASan/UBSan binaries each pass 2,840 kernel oracle
calls per compiler, including empty/tail counts, infinities, subnormals, NaNs,
and both zero signs. Non-NaNs are compared bitwise and NaNs by classification.
The committed regression also covers the actual public interface with long
double and mixed floating types. MSVC uses the same selected implementation;
the inclusive-only candidate's observed MSVC signed-zero failure is avoided.

To reproduce the research run, check out the fixed research commit and dispatch
its existing `benchmarks.yml` workflow with `profile=np`, or run
`python3 benchmarks/research/run_minmax.py` on an x86 Linux system with GCC and
Clang. The research workflow and probe are isolated from the production change;
the driver pins Kokkos mdspan and records the precise flags and environment.
