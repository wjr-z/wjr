# Explicit resources for owning JSON documents

Include `<wjr/json/pmr.hpp>` and use `pmr_document::parse_with_allocator(reader,
pmr_document::allocator_type(&resource))`. The resource is borrowed and must
outlive all nested and moved-out values. The [snapshot example](../../examples/json/README.md)
uses the standard `unsynchronized_pool_resource` across successive documents.
No custom arena or thread-local allocator is introduced.

The explicit domain uses 24-byte values, PMR strings/maps/vectors, and recursively
propagates the resource into payload metadata, character buffers, map nodes,
keys, and child values. The existing 16-byte document and its allocation policy
remain unchanged. Copy construction selects the default resource unless an
allocator is supplied; assignment retains the destination resource. Unequal
resource moves rebuild and may throw. Swap requires equal allocators. Resource
equality permits storage exchange but does not combine resource lifetimes.
Parse retains the existing terminating allocation-failure policy; ordinary
allocator-aware constructors and assignments preserve their throwing contracts.
Serialization output has a separate allocator. The [design](../design/json-allocator-resources.md)
records the propagation, relocation, exception, and ownership proofs and compares
pinned simdjson 3.9.1 and yyjson 0.10.0 ownership models.

## Correctness and default-path checks

The focused suite contains nine PMR cases and eight existing JSON/minify cases.
It checks recursive resource identity and backing allocation ownership, input
independence, Unicode and duplicate keys, registered serializers, copy selection,
equal/unequal-resource moves, moved-child lifetime, swap, malformed-input cleanup,
and failure injection at every allocation in copy and registered construction.
Windows Clang 18 C++20/C++23 and MSVC 19.42 C++20 Release each passed all 17 cases.
Windows Clang 18 ASan passed the 16 cases excluding throwing-allocation rollback;
its exception runtime corrupts a caught integer even in an independent no-WJR
reproducer. That local sanitizer exception case is not claimed as passed.

Linux GCC 14 and Clang 18, each C++20 and C++23 Release, passed all 17 cases.
GCC 14 C++20 and Clang 18 C++23 ASan+UBSan executions also returned success;
these initial executions did not set fatal UBSan recovery, so this result alone
is not a claim that no recoverable UBSan diagnostic occurred.

Windows Clang 18 `-O3 -march=x86-64-v3` assembly, compared against fixed pre-change
revision `c59764bd`, is text-identical after removing only the `.file` directive:
2,889 lines of default value/string/copy/move/swap/access probes and 8,381 lines of
the default parser translation unit. No PMR state is added to the default value.

## Same-domain measurements

These are preliminary short eager parse-and-destroy measurements, not a universal
speedup claim. Input token storage is warmed and reused. Every backend checks
canonical output equality before timing and parses at least 4 MiB per fixture
(minimum eight iterations). Backends alternate order across repeats. Timing uses
the normal allocator without allocation counters. Warm pool construction is
outside the timed loop; cold pool construction and destruction are inside each
iteration. Every document dies before its pool. The root-string case is 64 KiB;
string arrays contain 512 values, object fixtures contain 128 nested records,
and the numeric array contains 4,096 values. Twitter is the repository fixture.

Windows uses Clang 18.1.8 with the MSVC 19.42 STL on i5-13500HX, pinned to CPU 2,
three repeats per standard. Linux uses GCC 14 or Clang 18 with libstdc++ on an
AMD EPYC 7763 VM, five repeats per standard. Linux source, workflow, compiler/CPU
metadata and raw CSV are retained in the [research run](https://github.com/wjr-z/wjr/actions/runs/41080777915)
and research branch `research/json-pmr-20261030` (source `084454dc`).
The later scalar-conversion noexcept constraint does not change these primitive
fixture operations. The tables report paired throughput ratios to the default
document, median [minimum, maximum]; values below 1 are slower.

Windows pool reuse helps string/object snapshots. Numeric arrays are slower,
cold pools can be substantially worse, and Linux often loses to the existing
default allocator. PMR/new-delete exposes the representation/dispatch overhead
without pool reuse. Choose this explicit capability for resource ownership and
measured workload benefit; it does not replace the default document.

### windows-cxx20

| Fixture | PMR new/delete | Warm pool | Cold pool |
| --- | ---: | ---: | ---: |
| small_config | 0.841 [0.820, 0.856] | 1.491 [1.477, 1.507] | 0.889 [0.875, 0.891] |
| strings_8 | 0.813 [0.797, 0.824] | 1.965 [1.913, 1.981] | 0.453 [0.453, 0.472] |
| strings_64 | 0.865 [0.862, 0.878] | 2.461 [2.391, 2.485] | 0.940 [0.710, 0.968] |
| strings_256 | 0.974 [0.934, 0.980] | 2.296 [2.175, 2.348] | 1.037 [0.941, 1.150] |
| objects_128 | 0.880 [0.732, 0.897] | 2.149 [2.145, 2.179] | 1.561 [1.544, 1.603] |
| numbers_4096 | 0.556 [0.547, 0.586] | 0.601 [0.595, 0.606] | 0.441 [0.428, 0.508] |
| root_string_65536 | 0.904 [0.869, 1.052] | 0.951 [0.940, 1.134] | 0.873 [0.812, 1.074] |
| twitter | 0.910 [0.781, 0.941] | 1.929 [1.903, 2.171] | 1.376 [1.226, 1.611] |

### windows-cxx23

| Fixture | PMR new/delete | Warm pool | Cold pool |
| --- | ---: | ---: | ---: |
| small_config | 0.850 [0.806, 0.858] | 1.483 [1.362, 1.578] | 0.898 [0.827, 0.938] |
| strings_8 | 0.802 [0.774, 0.846] | 2.068 [2.067, 2.089] | 0.465 [0.426, 0.474] |
| strings_64 | 0.880 [0.864, 0.925] | 2.731 [2.485, 2.788] | 0.969 [0.769, 1.049] |
| strings_256 | 0.948 [0.868, 0.958] | 2.174 [2.090, 2.406] | 0.921 [0.703, 1.024] |
| objects_128 | 0.893 [0.881, 0.900] | 2.300 [2.288, 2.315] | 1.656 [1.649, 1.697] |
| numbers_4096 | 0.545 [0.545, 0.562] | 0.603 [0.590, 0.622] | 0.505 [0.460, 0.517] |
| root_string_65536 | 0.947 [0.926, 0.956] | 1.059 [1.032, 1.085] | 0.964 [0.904, 0.992] |
| twitter | 0.891 [0.790, 1.018] | 1.935 [1.912, 2.197] | 1.478 [1.198, 1.626] |

### g++-14-cxx20

| Fixture | PMR new/delete | Warm pool | Cold pool |
| --- | ---: | ---: | ---: |
| small_config | 0.744 [0.722, 0.782] | 0.687 [0.679, 0.720] | 0.442 [0.431, 0.464] |
| strings_8 | 0.738 [0.721, 0.751] | 0.941 [0.923, 0.960] | 0.877 [0.860, 0.887] |
| strings_64 | 0.793 [0.779, 0.805] | 1.027 [1.009, 1.054] | 0.952 [0.938, 0.968] |
| strings_256 | 0.827 [0.824, 0.837] | 1.078 [1.069, 1.081] | 1.007 [0.977, 1.020] |
| objects_128 | 0.761 [0.754, 0.781] | 0.960 [0.929, 0.983] | 0.896 [0.888, 0.921] |
| numbers_4096 | 0.855 [0.850, 0.867] | 0.852 [0.844, 0.865] | 0.841 [0.833, 0.853] |
| root_string_65536 | 0.875 [0.792, 0.881] | 0.889 [0.876, 0.897] | 0.844 [0.822, 0.858] |
| twitter | 0.835 [0.785, 0.842] | 0.967 [0.919, 0.998] | 0.954 [0.893, 0.987] |

### g++-14-cxx23

| Fixture | PMR new/delete | Warm pool | Cold pool |
| --- | ---: | ---: | ---: |
| small_config | 0.827 [0.816, 0.837] | 0.747 [0.741, 0.761] | 0.467 [0.458, 0.477] |
| strings_8 | 0.780 [0.739, 0.794] | 0.915 [0.888, 0.937] | 0.900 [0.877, 0.932] |
| strings_64 | 0.818 [0.799, 0.827] | 1.043 [1.018, 1.058] | 0.967 [0.960, 0.978] |
| strings_256 | 0.853 [0.830, 0.858] | 1.108 [1.079, 1.117] | 1.024 [1.000, 1.032] |
| objects_128 | 0.829 [0.817, 0.834] | 1.038 [1.004, 1.078] | 0.984 [0.944, 1.003] |
| numbers_4096 | 0.861 [0.855, 0.871] | 0.861 [0.846, 0.870] | 0.843 [0.841, 0.856] |
| root_string_65536 | 0.999 [0.966, 1.019] | 0.995 [0.968, 1.019] | 0.945 [0.777, 0.964] |
| twitter | 0.855 [0.838, 0.922] | 1.020 [0.965, 1.127] | 0.995 [0.958, 1.099] |

### clang++-18-cxx20

| Fixture | PMR new/delete | Warm pool | Cold pool |
| --- | ---: | ---: | ---: |
| small_config | 0.816 [0.813, 0.838] | 0.786 [0.777, 0.787] | 0.514 [0.508, 0.518] |
| strings_8 | 0.735 [0.727, 0.843] | 0.938 [0.931, 1.084] | 0.867 [0.859, 1.000] |
| strings_64 | 0.749 [0.739, 0.758] | 1.021 [1.014, 1.028] | 0.933 [0.927, 0.937] |
| strings_256 | 0.766 [0.758, 0.787] | 1.049 [1.045, 1.067] | 0.962 [0.938, 0.993] |
| objects_128 | 0.815 [0.799, 0.825] | 1.024 [1.011, 1.054] | 0.963 [0.951, 0.997] |
| numbers_4096 | 0.772 [0.769, 0.773] | 0.769 [0.760, 0.770] | 0.756 [0.754, 0.758] |
| root_string_65536 | 0.874 [0.869, 0.903] | 0.888 [0.884, 0.889] | 0.846 [0.791, 0.879] |
| twitter | 0.846 [0.810, 0.852] | 0.971 [0.957, 1.025] | 0.952 [0.906, 1.006] |

### clang++-18-cxx23

| Fixture | PMR new/delete | Warm pool | Cold pool |
| --- | ---: | ---: | ---: |
| small_config | 0.854 [0.834, 0.861] | 0.807 [0.791, 0.815] | 0.527 [0.517, 0.531] |
| strings_8 | 0.889 [0.887, 0.965] | 1.052 [1.043, 1.146] | 1.050 [1.037, 1.144] |
| strings_64 | 0.877 [0.871, 0.881] | 1.171 [1.135, 1.203] | 1.063 [1.045, 1.102] |
| strings_256 | 0.884 [0.869, 0.889] | 1.179 [1.174, 1.203] | 1.096 [1.076, 1.109] |
| objects_128 | 0.825 [0.815, 0.840] | 1.056 [1.050, 1.070] | 0.995 [0.976, 1.013] |
| numbers_4096 | 0.772 [0.707, 0.798] | 0.770 [0.765, 0.794] | 0.755 [0.754, 0.783] |
| root_string_65536 | 0.979 [0.975, 0.994] | 0.994 [0.967, 1.032] | 0.955 [0.925, 0.991] |
| twitter | 0.868 [0.857, 0.880] | 1.025 [1.009, 1.047] | 0.984 [0.968, 1.028] |
