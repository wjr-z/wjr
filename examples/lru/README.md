<!-- markdownlint-disable MD043 -->
<!-- Standalone installed-package example. -->

# Fixed-capacity memoization

After installing WJR, build the example as an ordinary package consumer:

```sh
cmake -S examples/lru -B build-lru -GNinja -DCMAKE_PREFIX_PATH=/path/to/wjr/install
cmake --build build-lru
./build-lru/lru_memoize
```

The example caches rendered route descriptions. It returns an owning string to
its caller; the pointer from `get` is used only while the cache entry is alive.
The sequence orders/users/get orders/health keeps orders and health, evicts users,
and performs only three computations. Transparent hash/equality allow borrowed
`string_view` lookup without first constructing an owning key.

A second cache uses a borrowed standard PMR pool. Its node allocation and its
allocator-aware string payload use that resource; the resource is declared first
and outlives the cache. Ordinary `std::string` does not become a PMR string merely
because the outer allocator changes. This unsynchronized resource and both caches
are used by one thread.

The complete [API and ownership contract](../../docs/modules/lru-cache.md)
describes capacity zero, temporary capacity+1 admission, exceptions and borrowed
pointer invalidation. No timing or universal speedup is claimed by this example.
