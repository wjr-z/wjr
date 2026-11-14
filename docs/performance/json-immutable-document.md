# Compact immutable JSON snapshots

`immutable_document` owns decoded strings and one exact-sized packed allocation.
Its intended consumers retain snapshots and repeatedly read fields. The
[API guide](../modules/json-immutable-document.md) defines exact scalar getters,
borrowing lifetimes and checked parsing. The [installed-package consumer](../../examples/json/immutable_snapshots.cpp)
projects 100 Twitter statuses into owning records after its input and reader die.

## Complete consumers

Each timed iteration reads the input, constructs a fresh output, recursively
checksums its values and decoded strings, then destroys the output. The
read-many case additionally performs 64 query passes before destruction. Twitter
passes read each status text, user screen name and an absent field. Fixed-width
objects query first, middle, last and absent keys. Query-only trials retain a
completed owner outside the timing window.

The simdjson 3.9.1 DOM reference uses `parse_into_document` with an external
document. Its decoded output survives parser and padded-input destruction.
Reader/parser scratch is warmed and reused for timing. Allocation experiments
create fresh readers/parsers, include their scratch in peak usage and destroy
them before measuring retained output. Input preparation is outside both windows.

The following entries are median elapsed-time ratios over five alternating
trials. Values below one mean the immutable consumer finished sooner.

| Platform | Consumer | Immutable / mutable | Immutable / simdjson DOM |
| --- | --- | ---: | ---: |
| Windows Clang | Twitter parse + scan + destroy | 0.531 | 2.609 |
| Linux Clang | Twitter parse + scan + destroy | 0.595 | 3.340 |
| Linux GCC | Twitter parse + scan + destroy | 0.640 | 2.960 |
| Windows Clang | Twitter parse + scan + query64 + destroy | 0.624 | 1.529 |
| Linux Clang | Twitter parse + scan + query64 + destroy | 0.637 | 1.385 |
| Linux GCC | Twitter parse + scan + query64 + destroy | 0.813 | 1.431 |
| Windows Clang | 40 fields, parse + scan + query64 + destroy | 0.570 | 0.467 |
| Linux Clang | 40 fields, parse + scan + query64 + destroy | 0.500 | 0.574 |
| Linux GCC | 40 fields, parse + scan + query64 + destroy | 0.955 | 0.500 |
| Windows Clang | 128 fields, parse + scan + query64 + destroy | 0.391 | 0.521 |
| Linux Clang | 128 fields, parse + scan + query64 + destroy | 0.460 | 0.342 |
| Linux GCC | 128 fields, parse + scan + query64 + destroy | 0.857 | 0.310 |

For 40-field complete consumers, absolute immutable times are 12.43/11.97/13.13
microseconds on Windows Clang/Linux Clang/Linux GCC. Corresponding simdjson
times are 26.64/20.85/26.23 microseconds. These measurements support repeated
lookup over retained compact objects. simdjson constructs ordinary snapshots
substantially faster, including the Twitter consumer above.

## Retained and transient memory

These are bytes requested from intercepted global allocation functions, excluding
the instrumentation header, input buffers, owner objects on the stack and allocator
bookkeeping. RSS was unmeasured. The final immutable allocation contains 16-byte
nodes, 12-byte object members and decoded bytes. Owner/view sizes are 24/16 bytes
on these x64 toolchains.

| Twitter backend | Platform | Allocation calls | Cumulative bytes | Peak live bytes | Output after reader/parser destruction |
| --- | --- | ---: | ---: | ---: | ---: |
| Immutable | Windows | 69 | 3,308,763 | 1,783,385 | 750,681 |
| Mutable | Windows | 28,559 | 1,736,039 | 1,731,351 | 1,714,928 |
| simdjson DOM | Windows | 6 | 8,640,572 | 8,640,572 | 6,105,024 |
| Immutable | Linux | 49 | 2,695,496 | 1,732,185 | 750,681 |
| Mutable | Linux | 27,295 | 1,657,431 | 1,652,743 | 1,636,359 |
| simdjson DOM | Linux | 6 | 8,640,572 | 8,640,572 | 6,105,024 |

The simdjson DOM tape and decoded strings logically use 711,892 bytes for this
input. Its allocated capacity remains 6,105,024 bytes after the parser dies.
The immutable representation uses about 5.4% more logical storage and retains
about 87.7% fewer allocated bytes. The smaller retained capacity accompanies
additional construction work and a builder-plus-packed peak.

## Controls and limits

- The 4,096-integer array retains 65,552 immutable bytes. Mutable storage is
  65,599 bytes on Windows and 65,560 on Linux. Immutable parse + scan + destroy
  takes 1.34/1.97/1.51 times mutable time across the three platforms. Its peak
  is 168,334 bytes on Windows and 213,008 on Linux, versus about 115 KB mutable.
- Forty overwritten 4 KB strings compact to 49 output bytes. The immutable peak
  remains about 410 KB, versus about 21 KB mutable. simdjson preserves duplicate
  members and therefore performs different work for this control. Its duplicate
  timing is excluded from the cross-backend claims.
- The small configuration takes 0.97/1.14/1.26 times mutable construction-consumer
  time. The small-object case provides limited benefit from the packed layout.
- Object lookup uses sorted decoded keys and binary search. Arrays retain input
  order and support forward traversal, with linear indexed access. These choices
  add construction and packing work for consumers that read once.
- The mutable reference retains its older UTF-8/control/token-boundary validation
  behavior. All timed inputs belong to the common valid domain. The immutable
  entry and simdjson perform their full checks.

## Reproduction and validation

Production runtime code was frozen at `a57491a5d96b61677a5ffd61239dbc1154660fb1`,
with test/usage corrections at `4b43b7d78719c78724442f1b4c0dd7f62b00fed4`.
The base is direct-parser revision `971b0dfc7e3d0c58b9b056476ed0584179c771b5`.
The [driver and CMake project](https://github.com/wjr-z/wjr/tree/9a390c7974ce35420aa54757fba7f8d186736e56/devtools/json-immutable-research)
build normal WJR modules and a separate reference executable. The
[Linux research run](https://github.com/wjr-z/wjr/actions/runs/43618211356)
contains raw CSV, machine metadata, compile commands, allocation-failure logs
and source hashes for both compilers.

Windows uses Clang 18.1.8 with MSVC 19.42 STL on an i5-13500HX, CPU2 affinity.
Linux Clang 18.1.3 runs on Xeon Platinum 8573C, and GCC 14.2 runs on AMD EPYC7763.
Each Linux process pins its first allowed CPU. Builds use C++20 Release and
native ISA flags on the architecture-dependent modules. simdjson selects haswell
on Windows and the GCC runner, and icelake on the Clang runner. WJR's x86 lexer
uses its AVX2 implementation. This is a best-native product comparison.

The 631,514-byte Twitter fixture has SHA-256
`68f2ed1261eeccb70ac34d8cab3c3b8bc7b7b510b6bd3a97ac5636e27e872d3c`.
The immutable header/source hashes are
`e486f85c394c9011bdb3b23ba3abb88ff48a4c738c56793e48315c9015b65db9` and
`43be622529c5c1581cd025b1204ba70936ead78ed155c238feff409f8dd18d94`.

Normal Clang CMake ran 926 unit tests successfully. MSVC's normal JSON modules
and nine focused tests passed. Each Linux compiler passed nine focused tests,
eight ASan/UBSan tests and exhaustive allocation-failure injection over the ten
benchmark fixtures. The installed-package snapshot consumer also passed.

An earlier test/benchmark traversal dereferenced temporary getter results in
C++20 range-for initializers. GCC ASan identified the resulting dangling range.
All earlier timings were discarded. The reported trials retain named checked
results throughout traversal, and the guide and example demonstrate this lifetime.
