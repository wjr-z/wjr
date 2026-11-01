# Pool-backed owning JSON snapshots

For a separate readonly native-engine pipeline, see
[yyjson statuses](yyjson/statuses.cpp) and its
[ownership/component guide](../../docs/modules/json-yyjson.md). Build the installed
consumer with `cmake -S examples/json/yyjson -B build-yyjson
-DCMAKE_PREFIX_PATH="/path/to/wjr/install;/path/to/yyjson/install"`, then run
`yyjson_statuses test/data/success/twitter.json`. It copies selected fields into
owning application records and releases both input and DOM before consuming the
records. The executable also has a small built-in fixture when no path is given.

`pool_snapshots.cpp` parses successive configuration snapshots with one borrowed
`std::pmr::unsynchronized_pool_resource`, replaces the active owning document,
and mutates a string field after parsing. The standard pool can reuse released
storage across snapshots; the example adds no allocator implementation.

After installing WJR, build it as a normal package consumer:

```sh
cmake -S examples/json -B build-json-example -GNinja \
  -DCMAKE_PREFIX_PATH=/path/to/wjr/install
cmake --build build-json-example
./build-json-example/json_pool_snapshots
```

Include `<wjr/json/pmr.hpp>` explicitly and link `wjr::json`. The regular
`json::document` remains a 16-byte value with its existing allocation behavior;
the opt-in PMR value is 24 bytes on the measured x64 toolchains, and its string
accessor returns `std::pmr::string&`.

The pool must outlive every value, moved-out child, string, and container that
still refers to it. Equal resources can exchange storage without replacing all
nested allocator objects, so equality does not merge resource lifetimes. Do not
call `release()` with live clients. This unsynchronized resource is reused
sequentially here.

`parse_with_allocator` returns ordinary JSON errors and keeps the existing
terminating allocation-failure policy. Copy construction selects the default
resource unless an allocator is supplied. Assignment retains the destination
resource; unequal-resource moves may allocate and throw. Swap requires equal
allocators. Returned serialization output still uses its own allocator; use
`dump_impl` with an explicitly constructed PMR string when that is desired.

Resource propagation, default-path code generation, and performance tradeoffs
are described in the [allocator design](../../docs/design/json-allocator-resources.md).

## Fixed-size coordinate projection

`json_fixed_coordinates` uses `<wjr/json/array.hpp>` to compare typed coordinate
storage for a single two-dimensional Polygon feature. It counts allocation
requests/bytes in the projected containers and verifies equal point counts and
coordinate checksums. It does not measure parse throughput or RSS.

```sh
curl -L https://raw.githubusercontent.com/simdjson/simdjson-data/4197c425e857f0ec38e89822fdd0bd9ea21f4daf/jsonexamples/canada.json -o canada.json
./build-json-example/json_fixed_coordinates canada.json
```

The 2,251,051-byte fixture has SHA-256
`f83b3b354030d5dd58740c68ac4fecef64cb730a0d12a90362a7f23077f50d78`.
The same build commands above build this second executable. The example retains
one live DOM for both projections, excludes its allocations from the counters,
and releases each typed result before constructing the other. No dataset or
third-party implementation is copied into WJR. Exact-shape, error, move and
lifetime contracts are in the [array adapter guide](../../docs/modules/json-fixed-array-conversion.md).
