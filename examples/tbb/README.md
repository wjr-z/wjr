# Borrowed synchronous batches

Install a system or official prebuilt oneTBB **2021.11 or later** config package,
then build/install WJR with `-DWJR_ENABLE_TBB=ON`. WJR does not download or build
oneTBB. Add its installation to `CMAKE_PREFIX_PATH` if it is not a system package.
On Windows, make the prebuilt package's runtime DLL directory available on
`PATH` when running the example or tests.

```sh
cmake -S . -B build -DWJR_ENABLE_TBB=ON -DWJR_CPU_TARGET=baseline \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/wjr-install"
cmake --build build --parallel
cmake --install build
cmake -S examples/tbb -B tbb-example-build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PWD/wjr-install"
cmake --build tbb-example-build --parallel
ctest --test-dir tbb-example-build --output-on-failure
```

`batches.cpp` computes independent row squared norms into preallocated storage,
using the same two-slot `oneapi::tbb::task_arena` twice. Chunks borrow one const
callable; each owns its output rows. The adapter has no NP dependency and no
per-call arena, global thread setting or heap-owned callable. oneTBB still has
real scheduler/task allocation and startup costs; this example does not time
them or promise a speedup for these small inputs.

The [API contract](../../docs/design/parallel-tbb.md) explains inherited
cancellation, exceptions, borrowing and concurrent-callback responsibilities.
