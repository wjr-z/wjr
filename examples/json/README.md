# Pool-backed owning JSON snapshots

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
