# Read-only low limbs in x86 128-bit comparisons

The GNU assembly helpers `_asm_less_128` and `_asm_less_equal_128` now describe
their low limb as an input. The previous `+&r` constraint described a value
that was overwritten early, although CMP never writes either operand. When
division uses the original dividend after comparison, that constraint forced
an unnecessary copy and consumed another general-purpose register.

Only the high destination of SBB is read/write. The instruction sequence,
condition-code result, `cc` clobber, architecture guards and portable fallback
remain unchanged. CMP consumes both low operands before SBB overwrites its
high destination. Even when equal inputs share registers, no later instruction
needs a low input after that write. With explicit SETcc instead of a compiler
flag output, SETcc runs after every input has been consumed. These roles follow
the [GCC operand constraint contract](https://gcc.gnu.org/onlinedocs/gcc/Modifiers.html).

The production performance target is the GNU assembly path selected outside
`FAST_INT128_COMPARE`, including GCC 13 and 14. Clang and GCC 8.1 through 12
with native int128 already use the compiler comparison. MSVC retains its
existing fallback. No speedup is claimed for these unchanged paths.

## Measured public caller

The public `div128by128to64_noshift` uses the comparison before optionally
subtracting the normalized divisor and storing the remainder. The same
comparison also appears in `div3by2_divider_noshift::divide` and the biginteger
two-limb division loop. Only the public 128-by-128 caller was timed here;
whole-biginteger or application throughput was not measured.

[Linux research run 41181629185](https://github.com/wjr-z/wjr/actions/runs/41181629185)
used one GitHub `ubuntu-24.04` worker: AMD EPYC 9V74, GCC 14.2.0, C++20,
`-O3 -DNDEBUG -march=x86-64`, Google Benchmark 1.9.0. Both variants ran in the
same process, pinned to the first allowed CPU, with the same input and output
buffers. Separate namespaces avoided mixing old and new template definitions.
Each benchmark processed 256 divisions, with seven real-time repetitions in
random interleaving and a 0.05-second minimum run time. Divisors were normalized;
the quotient was always zero or one. No allocation occurred in the timed loop.

| Input distribution | Original median, ns/batch | Input-only median, ns/batch | Elapsed change |
| --- | ---: | ---: | ---: |
| Deterministic random dividend and divisor | 183.868 | 174.568 | -5.06% |
| Equal high limbs, random low limbs | 190.071 | 182.181 | -4.15% |
| Dividend equals divisor | 202.775 | 197.665 | -2.52% |

GCC's actual caller loop lost one low-limb MOV and used one fewer register;
the arithmetic and branches were unchanged. These are same-host measurements,
not comparisons across CPUs or operating systems. The artifact contains raw
JSON, assembly, compiler version, CPU profile and build/run logs. The exact
[research commit](https://github.com/wjr-z/wjr/commit/3314ad843b20cdf62bc56f5701dba30e944d374c)
contains the standalone harness and temporary workflow; that workflow is not
part of the production change. Hardware branch/cache counters were not collected.

Windows checks used an Intel Core i5-13500HX, Clang 18.1.8 and MSVC 19.42,
C++20 baseline release settings. The original public Clang caller was unchanged;
MSVC public caller/comparison assembly was byte-identical. Windows timings
would not exercise the changed public dispatcher and are therefore omitted.

Correctness checks covered all combinations of seven limb boundary values,
100,000 deterministic random comparisons, equal/overlapping input values and
actual quotient/remainder results. Repository tests additionally keep all four
operands live across both assembly comparisons, including when the public
Clang dispatcher uses native int128, and exercise division borrow boundaries.
