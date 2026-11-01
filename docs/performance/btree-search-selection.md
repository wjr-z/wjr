# B-tree search selection: fewer templates, the same algorithms

The core searcher previously defined three capacity-specific class specializations,
three copies of its public `search` contract, and separate hybrid/binary member
templates. A single `void` specialization now chooses those same three algorithms
with `if constexpr`. This removes two class definitions, four member-template
definitions and the three capacity `enable_if` expressions; the header loses 83 lines.

The public `basic_btree_searcher_impl<N, Enable=void>` form stays compatible,
including explicit `void`; non-void arguments remain incomplete. Small/medium
search keeps level-two assertion/assumption macros; large search keeps level one.
The N32 integer-set specialization and the midpoint/linear/binary algorithms are
unchanged. This does not include the rejected known-pivot-bound optimization.

The inline-storage specialization uses the named `complete_btree_key` concept
around **the existing `is_complete_v` customization**. It does not substitute a
standard/`sizeof` completeness detector. Recursive users' explicit false traits,
including pairs containing those types, remain honored before and after the
recursive type definition. The storage threshold, value type and object layout
are unchanged.

## Actual cost and code-generation evidence

[Compact evidence](data/btree-search-selection.json) includes the complete probe,
source SHA-256, flags, assembly hashes and trace counts. Windows Clang 18.1.8,
MSVCSTL 143.202408, C++20:

| Probe result | Before | After |
|---|---:|---:|
| Search-member function instantiations |29|16|
| Private hybrid/binary helper instantiations |13|0|
| Searcher class instantiations |11|11|
| All function instantiations in this TU |572|559|

These are actual Clang `InstantiateFunction`/`InstantiateClass` trace events at
granularity 0, from one representative TU. They are **not** a whole-project build
speedup or a wall-time measurement. Standard-header and other container work
remains; removing 13 helper instantiations does not mean compilation becomes 45%
faster.

Complete optimized assembly is identical before/after under both
`-march=x86-64` and the local i5-13500HX's `-march=native`, after removing comments
and blank lines only. The probe contains direct capacities 4/5/8/9/16/17/32/64
searches, real map lower/upper bounds, heterogeneous string-view bounds, the N32
integer-set path, compact N64 lookup and a table of object/iterator sizes and
alignments. No throughput or cross-platform performance gain is claimed.

To reproduce, extract `probe_source` from the JSON into `codegen.cpp`, initialize
the pinned fmt submodule, and use the normal compiler/SDK environment. For the
reference, place the base commit's `include/wjr/container/btree.hpp` in a separate
`old/wjr/container/` include overlay. Compile both versions with the JSON's flags,
adding `-Iold` only for the reference before `-Iinclude -Ithird-party/fmt/include`.
For trace counts, use `-ftime-trace=<file> -ftime-trace-granularity=0` and count the
named events; do not infer counts from duration thresholds.

MSVC focused tests pass for all capacities 1–64, every partial size and insertion
position, explicit-void/non-void admission, recursive completeness customization,
heterogeneous queries and unique/duplicate set mutation. The full clang-format
hook passes. Normal Linux/Windows PR CI verifies their supported compilers and
standards; this local assembly proof is scoped to the stated Windows compiler.
