# Cursor return ABI experiment

Reference: `7cc9216382f516bb1374e8bc72252650cd9b063e` (the accepted single-pass
copy). Candidate: `e9220c588a132d8b68d06190a5deb77f14d3d5a8`. Research-only
delivery: `233e4dc7563b7ece32e37b23b417ade11744ee6b`.

The candidate returns the next retired leaf in RAX instead of updating a
`list_node_type *&`. Since the reference already returns the copied subtree's
minimum key in RAX, the candidate repairs separator minima through initialized
parent/position links at completed leaves and fresh branches. It remains a
single traversal, copies every value and executes the same allocator hooks.
No public signature, storage policy, exception domain or layout changes.

## Actual ABI evidence

Windows Clang 18 COFF confirms that the reference has a cursor-address reload
and an indirect store after each reused leaf (`movq %rax,(%rdi)`, offset 0x29f
in the representative inner function). Returning a pointer removes that store.
Both versions still save eight GPRs and reserve 0x78 stack bytes. The candidate
adds a parent/position traversal for minimum repair; the representative inner
function contains 206 disassembled instruction lines instead of 202, including
alignment padding. No new aggregate return is introduced in the hot recursion.

Linux Clang 18 SysV reserves 0x48 instead of 0x58 stack bytes, but the inner
function has 221 instead of 205 disassembled instruction lines. GCC 13 keeps
0x48 stack bytes. Its candidate has a stack-canary sequence around the local
cursor passed by reference to the existing cold clone helper; the reference
does not. These observations describe generated code, not a proof that one
instruction or the canary explains the complete timing difference.

## Same-profile measurements

Each cell is candidate/reference median elapsed time, below 1 is faster.
Each engine has ten raw samples, from RCCR engine order with five repetitions
per run and a 0.005-second minimum wall time. N16 and 4096 elements are fixed.
All 20 cases, ranges and coefficients of variation are in [the raw data](data/btree-copy-cursor-abi.json).
All candidate/reference min/max envelopes cross 1; the small differences do
not establish a reliable speedup. This is one bounded screen, not an expanded
search for a favorable platform or fixture.

| Profile / mapped size | match 0% | 25% | 50% early | 75% | 100% | 50% late | random shape | size mismatch |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Windows Clang / uint64 | .918 | 1.007 | 1.017 | .949 | .995 | 1.021 | 1.054 | 1.062 |
| Windows Clang / 64B | .972 | .973 | 1.021 | 1.000 | 1.012 | 1.045 | .961 | 1.032 |
| Linux GCC / uint64 | 1.069 | 1.087 | 1.090 | 1.097 | 1.063 | 1.061 | 1.032 | 1.118 |
| Linux GCC / 64B | 1.054 | 1.053 | 1.039 | 1.066 | 1.063 | 1.037 | 1.002 | 1.069 |
| Linux Clang / uint64 | .982 | .970 | .994 | .974 | .977 | .994 | .973 | .967 |
| Linux Clang / 64B | .990 | .988 | 1.002 | 1.005 | 1.001 | 1.005 | 1.009 | 1.003 |

Matching means the pre-existing occupancy fixture's structural/slot
compatibility, never value equality or permission to skip user copying.

Windows uses an i5-13500HX, Clang 18.1.8 and MSVC STL, fixed CPU0. Linux GCC
13.3 and Clang 18.1.3 run on the same EPYC 9V74 worker, using libstdc++, with
C++20 `-O3 -DNDEBUG -march=x86-64`. Each comparison is within its own profile;
absolute Windows/Linux times and this worker's times versus the earlier #281
EPYC 7763 worker must not be compared as an ABI/OS effect. Compiler commands,
CPU details, overlay header hashes, binaries and both platform disassemblies
are retained alongside the raw results.

## Correctness and decision

All 14 MSVC Release, 14 Windows ASan/UBSan and 14 Linux ASan/UBSan tests pass.
The tests cover SSO/heap transitions, self-copy, POCCA, PMR resources,
over-alignment, every value hook and graph/leaf ownership. Candidate and
reference memory-accounting CSVs are byte-for-byte identical on all three
profiles: no allocation, peak or retained-memory advantage.

The source-level concern was real: the reference does an indirect write.
Returning a pointer also has a cost because another scalar result still has
to reach the parent. This particular legal implementation has no demonstrated
stable overall benefit and has unfavorable GCC medians. Keep the reference
protocol. No production ABI/source change and no additional MR are warranted
by this initial experiment alone. The GCC result contains a localized avoidable
interference: the clone helper takes an immutable insertion marker by reference.
A fair control passes that marker by value in both reference and candidate;
the follow-up ELF gate and any conditional GCC timing are recorded separately.

Linux run: https://github.com/wjr-z/wjr/actions/runs/41325553258 (success).

## Fair read-only-marker control and final decision

`_clone_copy_branch` never assigns its insertion marker. Its parameter is now
passed by value in both engines: reference `3e563acb` and candidate `76e0299c`.
The reference overlay is byte-for-byte equal to that reference commit's header
(SHA-256 `38e03ac7f68aca6ef20c74e2c8cd0fe3d6f6fbe728f38d157002a8348a36ffc9`).
The candidate also removes the now-unnecessary local `clone_cursor`.

This removes the candidate's GCC canary. Actual GCC ELF disassembly has no
canary in either inner function, 326 reference versus 312 candidate instruction
lines (without raw-byte continuation lines), and 0x48 versus 0x38 stack bytes.
This passed the predeclared code-generation gate, so one GCC-only RCCR control
was run on the same EPYC 7763 worker. Windows and Clang were not rerun.

| Mapped size | match 0% | 25% | 50% early | 75% | 100% | 50% late | random shape | size mismatch |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| uint64 | 1.076 | 1.112 | 1.059 | 1.087 | 1.117 | 1.160 | 1.008 | 1.067 |
| 64B | 1.093 | 1.074 | 1.062 | 1.073 | 1.083 | 1.081 | 1.024 | 1.086 |

The fair-control ranges are wide and all cross 1, so these medians do not
establish a precise slowdown. They also give no stable benefit despite fewer
instructions and a smaller frame. Allocation/peak ledgers remain exactly equal.
The complete initial and fair-control raw samples, envelopes and dispersion
are in the accompanying data; no unfavorable case has been dropped.

Final decision: **do not adopt the return-cursor protocol**. Keep the existing
single-pass scalar-key result, and retain only the independently justified
read-only marker parameter cleanup. Initial 14-test ownership/sanitizer results
apply to the return-cursor protocol before the parameter-only correction;
the corrected engines were compiled and executed in the GCC timing/memory
passes. No extra Windows/Clang matrix or further tuning variant was run.

Fair control: https://github.com/wjr-z/wjr/actions/runs/41326456506 (success).
The accidental old-source dispatch `41326383996` was cancelled before being
used and is excluded from every result.
