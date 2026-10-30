# Bounded SIMD byte stores

Build and install WJR, then build this standalone example against the installed
package. It links the public `wjr::json` alias, which supplies the matching ISA
requirements and the minifier used by the example.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DWJR_CPU_TARGET=baseline \
  -DWJR_ENABLE_ASSEMBLY=OFF -DCMAKE_INSTALL_PREFIX="$PWD/wjr-install"
cmake --build build --parallel
cmake --install build
cmake -S examples/arch -B arch-example-build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PWD/wjr-install"
cmake --build arch-example-build --parallel
ctest --test-dir arch-example-build --output-on-failure
```

`wjr_bounded_store [count]` writes exactly `count` bytes of a vector to an
allocation of that size, prints a fixed seven-byte prefix, and minifies a short
JSON document. The default count is seven. A baseline x86 build accepts 0–16;
an AVX build accepts 0–32. There is no runtime ISA dispatch.

`sse::storeu_bytes(dst, value, count)` and `avx::storeu_bytes` accept byte counts
up to the vector width. The destination needs only that many writable bytes;
bytes outside the range are untouched. Count zero permits a null destination.
The count bound is a trusted precondition checked by the existing assertion
levels. `storeu_bytes<N>(dst, value)` provides the same operation for a constant
count and diagnoses an excessive count at compile time.

The API does not validate arbitrary pointers or promise a speedup over a caller's
existing tail strategy. Both overloads use bounded scalar pieces and reuse SSE
halves for AVX. The fixed-count SSE stores may overlap each other, but every
written byte stays inside the requested prefix.

These operations suit callers that already hold a vector and have an exact-size
output allocation. The JSON minifier retains its temporary-buffer tail strategy:
a direct bounded-store candidate improved short inputs in a focused GCC/Clang
Linux comparison, but grew the caller and showed a mixed-tail regression on
Windows AVX2. Fewer copies alone did not establish a general improvement.
