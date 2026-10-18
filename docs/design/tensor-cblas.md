# Optional CBLAS view boundary

`<wjr/np/cblas.hpp>` provides `np::cblas::dot(x, y)` and
`np::cblas::gemm_to(c, a, b, alpha, beta, op_a, op_b)`. The latter writes
`C = alpha * op(A) * op(B) + beta * C`; coefficients default to one and zero,
and `transpose::{none, transposed}` default to `none`.
This is an explicit eager kernel boundary. It does not build an expression,
materialize an input, choose a provider or change provider thread settings.

Enable `WJR_ENABLE_NP=ON` and `WJR_ENABLE_NP_CBLAS=ON`, install OpenBLAS
development files and pkg-config, then link `wjr::np_cblas`. Installed consumers
use `find_package(wjr CONFIG REQUIRED COMPONENTS np_cblas)`. Both options are
off by default; ordinary `wjr` and `np` discovery neither searches for nor links
OpenBLAS. The adapter does not acquire `wjr::isa` or native compiler flags.

## Provider and ABI

The first version supports native OpenBLAS **LP64** only: BLAS integer arguments
are 32-bit `int`, including on a 64-bit host. A pkg-config `openblas` package
must supply matching public headers and libraries. CMake compiles, links and
runs a C++ probe that checks `blasint`, rejects `USE64BITINT` in
`openblas_get_config()` before passing integer arguments, and exercises all
four float/double dot/GEMM symbols with tiny exact cases. Installed component
discovery repeats this validation in its consumer environment. Cross-compiling
is not supported by this initial native probe.

This deliberately does not rely on `FindBLAS` alone: its
[documented interface](https://cmake.org/cmake/help/latest/module/FindBLAS.html)
discovers BLAS, rather than proving a matching CBLAS header and LP64 ABI.
OpenBLAS [packaging guidance](https://www.openmathlib.org/OpenBLAS/docs/distributing/)
documents the integer-interface and library-naming differences. The runtime
marker comes from the reviewed
[OpenBLAS configuration implementation](https://github.com/OpenMathLib/OpenBLAS/blob/v0.3.26/driver/others/openblas_get_config.c).
The WJR dependency has no pinned binary release: the locally selected OpenBLAS
must pass that capability/ABI probe. Changing pkg-config search paths selects
a fresh discovery key; the ABI probe reruns on each native configure.

## Borrowing and geometry

Only matching `float` or matching `double` elements, the default accessor and
the existing positive-stride layout subset are accepted. Input elements may
be const; output elements must be mutable. Volatile elements and dtype
conversion are excluded.

- `dot` accepts rank-one views of equal length, including positive nonunit
  increments. It returns the input element type. It is a BLAS dot product,
  without a WJR ordered-fold guarantee.
- `gemm_to` accepts rank-two views. All three physical matrices share one
  row-major or column-major convention. Packed and padded layouts retain their
  known order; a `layout_stride` view must have unit inner stride and a positive
  leading dimension at least the physical inner extent. Each transpose flag
  changes the mathematical operand, not its physical major convention.
- Nonempty provider-call dimensions, increments and leading dimensions fit
  `int`. Shapes match the selected operation. These trusted-input preconditions
  use `WJR_ASSERT_L2` at entry, without per-element checks or recoverable checked
  descriptors. A matching valid mapping and sufficient live storage remain
  the caller's responsibility.
- GEMM output logical elements are disjoint from both inputs. The two inputs
  may overlap each other. There is no alias search or scratch copy.

Empty dot returns positive zero. Empty GEMM output does nothing. Both return
before stride access, pointer offsetting or integer narrowing. For nonempty
output, `K == 0` or `alpha == 0` explicitly computes only `beta*C` without reading
either input. This includes nonfinite alpha when K is zero; it is a WJR boundary
choice, rather than a promise to reproduce every provider's nonfinite corner.

`beta == 0` never reads C. Its objects must already be alive, but their numeric
values need not be initialized. In the zero-product path WJR writes positive
zero; otherwise it passes beta zero directly to GEMM, **without pre-clearing C**.
For nonzero beta, C contains initialized values; beta one in the zero-product
path leaves those values unchanged. Padding is never part of the logical
output. These rules permit a `default_construct` owner to be filled once by
normal GEMM. Netlib's
[GEMM contract and implementation](https://github.com/Reference-LAPACK/lapack/blob/v3.12.0/BLAS/SRC/dgemm.f)
provide the beta-zero reference.

Provider reduction order, FMA, FP environment, nonfinite results during a
normal call and threading follow OpenBLAS. There is no ordered-sum or
bit-identical scalar-loop promise. WJR allocates no intermediate arrays;
OpenBLAS may pack matrices, allocate workspace and start threads. Configure
those through the provider/user boundary, for example the documented
[runtime environment](https://www.openmathlib.org/OpenBLAS/docs/runtime_variables/),
not a WJR global thread setter.

## Validation boundary

The dedicated tests cover float/double dot, positive increments, both physical
orders and all transpose pairs, padded windows, beta-zero default-initialized
output, zero-product scaling and empty views. One existing Linux NP CI job
installs `libopenblas-dev`, runs these numerical tests, checks installed
component discovery and executes the standalone example. It sets
`OPENBLAS_NUM_THREADS=1` only in its test environment.

Local Windows validation uses a recording-only C API fixture for argument
forwarding and no-preclear behavior; it does not substitute for a numerical
OpenBLAS provider. Separate package checks exercise exports, repeated discovery,
missing optional/required dependencies and rejection after a pkg-config path
change. Generated-code comparisons use identical direct CBLAS calls, rather
than attributing provider throughput to the wrapper. No matrix-multiplication
speedup is claimed by this adapter.
