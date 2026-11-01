# Explicit mimalloc allocators

This optional component supplies an empty standard allocator and a PMR resource
using the maintained [mimalloc](https://github.com/microsoft/mimalloc) C API.
Ordinary WJR containers and scratch allocation keep their existing defaults.
Enabling the component does not replace global `new`/`delete`, set the default
PMR resource, or link the provider into `wjr::wjr`.

## Build and use

Provide an installed **shared** mimalloc 3.6 package, then configure WJR with
`-DWJR_ENABLE_MIMALLOC=ON`; `mimalloc_DIR` can identify its config directory.
WJR never downloads or builds this dependency. The base package remains usable
without it; installed clients request the component explicitly:

```cmake
find_package(wjr CONFIG REQUIRED COMPONENTS mimalloc)
target_link_libraries(app PRIVATE wjr::wjr wjr::mimalloc)
```

```cpp
#include <memory_resource>
#include <wjr/memory/mimalloc.hpp>
#include <wjr/vector.hpp>

wjr::vector<int, wjr::mimalloc_allocator<int>> selected;
wjr::mimalloc_memory_resource resource;
std::pmr::vector<int> runtime_selected(&resource);
```

`mimalloc_allocator<T>` selects storage for that container. Nested members retain
their own allocators; use allocator-aware types for recursive selection. The
resource also works with `json::pmr_document::parse_with_allocator`, using the
existing [JSON PMR contract](../design/json-allocator-resources.md).

Build the provider with `MI_OVERRIDE=OFF` (and `MI_WIN_REDIRECT=OFF` on Windows)
for this explicit selection model. Provider binaries configured to interpose
allocation have their own process-wide effects. The adapter does not include
`mimalloc-new-delete.h`. Deploy the shared provider alongside clients as required
by the platform loader, and retain its MIT license.

## Lifetime, failure and cost

- Typed allocators have no state or construct/destroy hooks, are trivially
  copyable, and compare equal within the same provider runtime. Existing
  allocator-traits relocation paths remain available. Storage includes correct
  over-alignment; count overflow and allocation failure report `std::bad_alloc`.
- PMR uses the standard virtual interface and resource pointer. The adapter adds
  no state beyond the base resource; equality is resource identity. Reuse one
  instance for allocator-equal container transfers. It outlives every owner
  borrowing it, including moved-out subdocuments.
- Destruction of a resource does not bulk-free storage. Owners deallocate their
  blocks individually; mimalloc controls its own caching and reclamation. The
  resource supports concurrent allocation/free and cross-thread deallocation.
- `WJR_DISABLE_EXCEPTIONS` uses WJR's failure policy. Allocation-failure handling
  is shared out of line and marked cold where supported; typed allocation stays
  in the header. No default-library path gains a runtime dispatch or dependency.

## Measured scope

Validation uses mimalloc **3.6.1**, commit
`56dc5eb80145f9a3ba7ca23d47c6a248b9d8586c`, baseline ISA, shared provider,
overrides/statistics/profiling disabled. Windows Clang 18 and MSVC 19.42 clients,
Linux Clang 18/GCC 13 clients, and a Clang ASan/UBSan client with an ASan-tracked
provider pass allocation, alignment, lifetime, failure, cross-thread and JSON
integration checks. Installed C++20/23 consumers and disabled/missing-provider
configurations are checked separately.

The owning PMR JSON test includes parsing and destruction; input loading and
resource initialization are outside timing. It rotates three resources over
seven repetitions with the same iteration count and pins one CPU. For the
repository's Twitter fixture, median times are:

| Client / machine | new-delete resource | WJR mimalloc resource | unsynchronized pool |
| --- | ---: | ---: | ---: |
| Windows Clang 18 / i5-13500HX | 3.233 ms | 1.349 ms | 1.595 ms |
| Linux Clang 18 / Xeon 8573C | 3.092 ms | 2.186 ms | 2.773 ms |
| Linux GCC 13 / same Xeon | 3.048 ms | 2.098 ms | 2.673 ms |

Compare resources within each row; different machines do not isolate OS effects.
Small/wide records and every raw sample are included in the evidence. These
results cover steady-state PMR DOM ownership, not allocator startup, RSS, or a
general allocator ranking. Fixed-size, variable-size and over-aligned typed
allocation probes match competent handwritten C-API wrappers instruction for
instruction on the measured Windows Clang profile.

[Metadata and reproducible workload sources](../performance/data/mimalloc-adapters-20261101.json)
and [raw samples](../performance/data/mimalloc-adapters-20261101.csv) record compiler
flags, the frozen feature patch, provider options and validation results. The
[Linux worker](https://github.com/wjr-z/wjr/actions/runs/41288577793) contains the
full build/sanitizer logs. Research workflows are separate from production CI.
