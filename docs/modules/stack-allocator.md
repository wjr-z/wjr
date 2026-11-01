# Stack allocation across library boundaries

`unique_stack_allocator` is a thread-bound, nested scratch allocator. Its default
arena is thread-local, and independent libraries can have different default
arenas. Constructing an unrelated default allocator in a plugin does not create
a scope in the caller's arena.

Use an explicit owner when a nested scratch operation crosses a module boundary:

```cpp
void plugin_operation(wjr::unique_stack_allocator &owner) {
    auto scratch = owner.make_scope();
    auto *temporary = scratch.allocate(1024);
    // Consume temporary before scratch is destroyed.
}
```

`make_scope()` returns a nonmovable owner by guaranteed copy elision. It captures
the existing arena and creates the usual stack context, with no heap allocation
or lookup of the plugin's default arena. `weak_stack_allocator<T>` can also use
this child owner.

Scopes sharing an arena must stay on one thread and be destroyed in reverse
construction order. The parent and the module defining its arena's pool provider
must outlive the child. Small allocations through **either** owner after child
construction are rewound when the child ends; those pointers must not escape the
child. Large allocations stay on the allocating owner's separate cleanup list.
Allocation/free across modules requires a compatible allocation ABI and runtime;
this API does not bridge incompatible Windows CRT heaps or retain loaded modules.

## Refill ownership

Each arena stores one constant-initialized pointer to its originating module's
pool provider. The out-of-line growth path calls it only when allocating a new
slab or the initial slab table. Moving between already cached slabs, bump
allocation, and context rewind do not read it.

This matters when foreign code refills an existing arena. Previously the refill
could link a retained slab into the foreign module's thread-local pool. Destroying
that pool freed memory still cached in the original arena. The provider routes
refill storage back to the arena's pool, so teardown of an unrelated pool cannot
invalidate it. Default arenas remain module-local where the linker chooses that
model; no process-global TLS indirection is added.

The pool now owns the slab-pointer table as well. The old final table was allocated
separately and was never released by the arena's intentionally trivial destructor.
Growing the table releases its previous allocation; thread exit releases the
latest table together with slabs. This adds one pool header to each live table
(16 bytes on the measured x64 builds), not to each scratch allocation.

## Validation and costs

The cross-module consumer test compiles the memory implementation independently
into an executable and a real shared library, with hidden module internals on
ELF. It checks distinct default arenas, explicit child rewind, outer allocation
preservation, and reuse after clearing the foreign pool. The latter simulates pool
teardown without unloading code that might still be executing.

The arena retains its trivial destructor and the old hot-field offsets. Measured
`sizeof` values are:

| x64 ABI | arena before / after | unique owner before / after |
| --- | ---: | ---: |
| Windows Clang 18 / MSVC ABI | 48 / 64 | 32 / 32 |
| Linux Clang 18 / GCC 13 | 64 / 64 | 32 / 32 |

The pointer uses existing tail padding on the measured Linux ABI; Windows adds
16 bytes after alignment. Consumers exchanging WJR objects across modules must
rebuild those modules together when updating this layout.

For Windows Clang 18, baseline C++20, `-O2`, `NDEBUG`, release debug level 0, the
complete warm `owner.allocate(n)` probe has the same 35 instructions before and
after; construction/allocation/destruction has the same 82. This is a codegen
check, not a claim about all compilers or cold allocation throughput.

The bounded timing control uses identical source and flags per old/new pair,
prewarms TLS, pins both processes to the same CPU, and alternates execution order
for seven samples. Each round constructs an owner, performs the stated number of
allocations and observable byte writes, and destroys it. It covers a warm bump
round (32 x 64 bytes), cached slab traversal (40 x 1024), growth/trim (80 x 1024),
and direct large allocation (1 x 32768). Compare revisions **within** each machine;
the Windows and Linux machines have different processors.

Median nanoseconds per round, old / new:

| Round | Windows Clang 18 | Linux Clang 18 | Linux GCC 13 |
| --- | ---: | ---: | ---: |
| Warm bump | 29.47 / 28.80 | 14.80 / 15.32 | 18.24 / 17.83 |
| Cached refill | 52.54 / 51.49 | 24.39 / 23.16 | 28.34 / 27.44 |
| Grow and trim | 319.80 / 318.73 | 71.78 / 61.03 | 76.66 / 78.48 |
| Large | 86.62 / 85.37 | 19.07 / 18.99 | 18.06 / 19.00 |

Windows uses an Intel Core i5-13500HX; Linux uses an AMD EPYC 9V45 runner.
These limited controls include both increases and decreases, and do not establish
a throughput improvement or universal nonregression. Cold growth now resolves a
module-bound provider; the measured Windows hot instructions are unchanged.
[Raw samples, compiler flags, source identities and research drivers](../performance/data/stack-domain-20261101.json)
record the comparison without attributing different machines' results to the OS.

The old implementation produces a reproducible `heap-use-after-free` when the
foreign pool is cleared and the retained slab is reused. For this heap-lifetime
control, `ASAN_OPTIONS=allow_user_poisoning=0` keeps the arena's unpoison calls from
erasing ASan's freed-heap shadow. Both user poison and unpoison check this option
in [LLVM 18's implementation](https://github.com/llvm/llvm-project/blob/llvmorg-18.1.8/compiler-rt/lib/asan/asan_poisoning.cpp).
The normal ASan consumer test still runs with the usual arena annotations.
An isolated worker-thread probe also reports the old final table as a 256-byte
LSan leak after its TLS roots disappear; the fixed probe exits without a report.
The ASan unit suite includes this worker-exit case.

To run the permanent regression tests:

```sh
cmake -S . -B build -DWJR_BUILD_TESTS=ON -DWJR_ENABLE_ASAN=ON
cmake --build build --target wjr_stack_domain
ctest --test-dir build -R '^wjr_stack_domain' --output-on-failure
```

The implementation and tests are frozen independently from the research workflow.
The research branch is not part of the production CI configuration.
