# Owning record parsing

The native typed parser builds registered records, owning strings and nested vectors
without an intermediate document. Measured implementation is `01388b366312fd3db4b5980a56da325544d9f136`.
The [API and error contract](../modules/json-direct-parser.md) describes its supported
domain and strict validation.

## Complete-path comparison

Each iteration scans the input, constructs all owned output, computes a checksum of
every field, and destroys the output. Reader/parser buffers and input are prepared
before timing. The document reference parses a native document and move-projects it
through the existing registered serializer. Canonical output equality is checked
before timing. Input bytes and hashes match across Windows and Linux.

The hand-written WJR visitor uses the same grammar, raw UTF-8 validation, string
control checks, four-character JSON whitespace and immediate token-boundary checks.
The optional simdjson 3.9.1 reference manually builds the same owned records, checks
required fields and every duplicate, and recursively validates unknown values.
It is compiled from the repository demo for this experiment. It adds no library
dependency. These references cover the shared valid record corpus. Their failure
reporting APIs differ from the typed factory's error code and byte offset.

The legacy document/check paths retain validation gaps documented with the API.
The typed path performs additional validation on this common valid-input domain.
Its throughput relative to document projection therefore includes that work.

Windows uses Clang 18.1.8, MSVC 19.42 STL, native Release on i5-13500HX, CPU 2.
Linux uses GCC 14 or Clang 18, libstdc++, native Release, and one allowed CPU per
runner. Seven pairs alternate execution order. Windows has visibly noisier raw
samples. Values below are median throughput ratios. Values above one favor direct
typed parsing.

| Profile | Records | Direct / document | Direct / WJR hand | Direct / simdjson hand |
| --- | --- | ---: | ---: | ---: |
| Windows Clang | Short strings | 3.235 | 1.141 | 0.622 |
| Windows Clang | Long strings | 2.623 | 1.148 | 0.700 |
| Windows Clang | Unknown nested fields | 3.484 | 1.166 | 0.606 |
| Linux Clang 18 | Short strings | 2.488 | 1.079 | 0.567 |
| Linux Clang 18 | Long strings | 1.695 | 1.068 | 0.686 |
| Linux Clang 18 | Unknown nested fields | 2.219 | 1.075 | 0.674 |
| Linux GCC 14 | Short strings | 1.804 | 1.103 | 0.358 |
| Linux GCC 14 | Long strings | 1.278 | 1.073 | 0.532 |
| Linux GCC 14 | Unknown nested fields | 1.518 | 1.134 | 0.358 |

Short batches contain 128 records, long and unknown batches contain 64. Each record
has eight fields including a nested point, integer samples and a vector of points.
The Linux simdjson native backend is verified as icelake. A separately compiled
Haswell control gives direct/simdjson ratios of 0.577/0.721/0.699 on Clang and
0.379/0.542/0.379 on GCC for the three record cases. simdjson's hand-written owned
path has higher throughput in these comparisons.

The 4,096-value integer/float arrays give direct/document ratios of 0.793/0.977 on
Linux Clang, 0.721/0.738 on Linux GCC, and 1.517/1.438 on Windows Clang. This capability
is primarily useful for registered owning records and nested values.

## Selector and allocation costs

The strict table-selection control is `c6b66e6f7ed95b53c882f1b6f74549ffd3ce22f7`.
Its tree differs only in `inplace_parser.hpp`. The compiled selector reduces record
time by 5.1–7.2% on Windows and 3.7–6.4% on Linux. Clang's 32/64-field cases improve
by 48.9/64.9%, while GCC's corresponding cases take 7.0/6.4% more time. This tradeoff
is retained for the record improvements and Clang's wider-schema benefit.

Allocation counters use separate executables. Linux short/long/unknown batches
request 776/455/391 allocations through the direct path and 3722/1929/2697 through
document projection. The corresponding requested bytes are 54144/43968/27008 and
247816/140872/171592. All measured outputs release their counted allocations.
Bounded short escaped strings decode before choosing their owned capacity, which
preserves SSO for raw 16-byte names that decode to 12 bytes.

The [frozen research source](https://github.com/wjr-z/wjr/tree/2bf6ea5621253da939a4693a0b4444f49bdb1069/devtools/json-direct-research)
and [Linux run](https://github.com/wjr-z/wjr/actions/runs/43581779286) retain the
consumer, paired driver, raw CSV, CPU/backend metadata, actual compile commands,
corpus hashes and sanitizer logs. The 64-field fixture spells out the equivalent
static descriptors and single-object document projection because the existing
registration macro's argument domain is smaller.

## Validation

Normal CMake entry points pass 917 Clang unit tests and 39 MSVC JSON consumer tests.
The Linux consumer passes the same 39 tests on both compilers and 21 focused
ASan plus fatal-UBSan tests. The UTF-8 validator also has exhaustive two-byte and
boundary tests. The all-profile benchmark target builds and its ten new cases run.
The full clang-format hook passes. The visitor observer-only change produces
byte-identical optimized document/check explicit-instantiation assembly to main
under Clang 18. The shared string-capacity change is measured separately.
