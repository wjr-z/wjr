# Performance reports

These reports compare a few important WJR workloads with mature, performance-oriented
libraries. Each report retains the workload contract, hardware and toolchain provenance,
five-sample variation, and the matching workloads where WJR ties or loses.

| Module | Comparison | Report | Status |
| --- | --- | --- | --- |
| Ordered containers | Fixed Abseil B-tree containers | [Legacy B+ tree baseline](btree-showcase.md) | Baseline; layout improvements remain under evaluation |
| Ordered containers | Compact N16 / legacy N16 maps | [Compact storage prototype](btree-compact.md) | Opt-in; lookup and large-value memory regressions retained |
| JSON | Previous minifier and simdjson 3.9.1 | [Whitespace-free blocks](json-minify.md) | Direct SIMD stores; formatted-input tradeoffs retained |
| Integer arithmetic | Original and input-only x86 assembly operands | [128-bit comparison operands](compare128-input-operands.md) | GCC 14 public division caller; unchanged Clang/MSVC paths |

Specialized showcase measurements are opt-in and run only through manual workflow
dispatch. Timing and memory are reported separately. Ratios compare implementations
within one machine/compiler profile; different machines do not isolate OS effects.

[Explicit B-tree hint measurements](btree-hints.md) retain sorted/local-run wins and random-input losses; ordinary range defaults are unchanged.
