# Ordered tensor axis kernels

For a packed row-major matrix, `sum_to(..., axes<0>)` and
`cumsum_to(..., axes<0>)` keep four adjacent columns in independent scalar
accumulators. Each accumulator still executes `0 + x0 + x1 + ...` in axis order.
The compiler may vectorize across columns without a horizontal reduction or
floating-point reassociation. Remaining columns use one accumulator each.

This path is selected at compile time for rank-two, axis-zero, packed right
input and contiguous output. Reduction also accepts its rank-one left output
or right-layout keepdims output. Other axes, ranks and layouts retain the
existing traversal. There is no runtime layout dispatch, extra validation,
allocation, hidden temporary array, or SIMD dependency. Scan reads the block's
inputs before its writes and supports exact pointwise in-place storage;
reduction still requires logically disjoint storage.

Accumulator dtype remains independent of output dtype. Scan converts each
prefix to output after updating the accumulator, retaining the original
accumulator for the next input. It never reloads rounded output as state.
Empty output returns without accessing storage; an empty reduced axis writes
zero. The ordinary scalar sum's logical row-major fold is unchanged.

Stride queries use constant evaluation when all contributing extents are
static: suffix dimensions for right layout, prefix dimensions for left layout.
Otherwise they retain the original mapping query. This removes runtime stride
product loops that remained in the reviewed MSVC code for `[dynamic_rows,300]`.

## Evidence and reproduction

Paired probes compared actual public `tensor_view`, `sum_to` and `cumsum_to`
against four-column handwritten loops at the same pointer/scalar call boundary.
Both sides used baseline x86-64, C++20, DEBUG0, ordinary floating-point options,
and no restrict or GS configuration changes. Clang 18.1 and GCC 12.1 used `-O3`;
MSVC 19.42 used `/O2 /Ob2 /EHsc`.

Clang and GCC generate packed addition across independent columns; MSVC keeps
four independent scalar accumulation chains. None of these inspected column
paths retains per-element descriptor loads, guard state or calls. Using a
four-element local array instead of four scalar variables introduced an MSVC
security-cookie path even after scalar replacement, so that variant was
rejected. This is not a claim that every dtype, compiler or surrounding caller
has identical code to handwritten loops. Boundary and unroll choices remain.

To inspect a consumer, add paired `WJR_NOINLINE` wrappers around the public
axis operation and the following reference, compile through `wjr::np`, and
request assembly (`-O3 -S` or `/O2 /Ob2 /FA`). Use the same CPU/debug/FP flags
for both wrappers. For each block of four columns, the reference maintains
four scalar accumulators; for each increasing row it adds the four original
inputs, writes four prefixes for scan, or writes four final values after the
row loop for reduction. Include the scalar column remainder, the zero-axis
result and the same disjoint/exact-inplace contract. A naive one-column loop
is a useful historical comparison, but does not represent the best practical
baseline for this operation.

Targeted tests cover columns 0/1/3/4/5/7/300, zero/singleton/multiple rows,
keepdims, explicit wide accumulation, narrowed output, exact in-place scan,
constexpr and order-sensitive cancellation. Clang, MSVC and Clang ASan pass;
standalone same-function codegen/correctness probes also pass GCC.
No throughput, branch/cache counter or stable-clock measurements were collected.
Presence of SIMD, fewer assembly lines, and the absence of allocation alone
do not establish an end-to-end speedup or universal zero overhead.
