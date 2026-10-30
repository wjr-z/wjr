# Performance reports

These reports compare a few important WJR workloads with mature, performance-oriented
libraries. Each report retains the workload contract, hardware and toolchain provenance,
five-sample variation, and the matching workloads where WJR ties or loses.

| Module | Comparison | Report | Status |
| --- | --- | --- | --- |
| Ordered containers | Fixed Abseil B-tree containers | [Legacy B+ tree baseline](btree-showcase.md) | Baseline; layout improvements remain under evaluation |

Specialized showcase measurements are opt-in and run only through manual workflow
dispatch. Timing and memory are reported separately. Ratios compare implementations
within one machine/compiler profile; different machines do not isolate OS effects.
