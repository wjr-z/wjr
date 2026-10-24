# Bincount into caller-owned storage

`<wjr/np/bincount.hpp>` provides two rank-one scatter reductions:

```cpp
np::bincount_to(counts, indices);
np::bincount_to(totals, indices, weights);
```

The destination length determines the number of bins. Both overloads first zero
every logical destination element, including bins absent from the input, then
process indices in their original order. Empty input therefore still clears the
destination. Empty input with zero bins and null data handles is valid.

## Types and contracts

- Unweighted output is a writable non-boolean integral type. Every resulting
  count must be representable in that type, including unsigned output; wrapping
  counts are outside the contract.
- Indices are non-boolean integral values, nonnegative and strictly less than
  the number of bins. No negative-index normalization is performed.
- Weighted output is writable floating point; weights may have any arithmetic
  type, including bool. Indices and weights have equal lengths.
- Each weight is converted to the destination type before addition. Accumulation
  takes place in that type, in input order within each bin. Ordinary arithmetic
  propagates NaNs and infinities. No compensated sum, NaN filtering, atomic update
  or reassociation is implied.
- Destination logical storage is disjoint from indices and weights; the two
  read-only inputs may share storage. Input elements remain stable during the call.
- Volatile elements are excluded. Packed, positive-stride and padded kernel
  mappings are accepted. Padding and stride holes are untouched.

Index bounds and dynamic shape compatibility use assertion level 2. Trusted
release calls do not validate inputs or perform a preliminary validation pass.
These are ordinary views, without a checked-view wrapper or retained ownership.

## Cost model

For N input elements and B bins, required work is B zero stores followed by N
index loads and N read-modify-write updates, plus N weight loads for weighted
calls. The implementation performs those operations directly, with O(1) extra
storage, no allocation, no initialization of temporary arrays and no workspace.
Mapping strides are obtained once at the API boundary. A compile-time choice
selects weighted versus unweighted updates; it adds no runtime mode branch.

The destination pointer carries the existing compiler-portable restrict annotation
under the disjoint-storage contract. Packed mapping strides are compile-time one.
This baseline deliberately does not replicate histograms, sort indices or add SIMD
scatter: repeated bins introduce dependencies, and such alternatives need workload
evidence before trading extra passes or memory for throughput.

`benchmarks/src/np_bincount.cpp` compares the actual view-based call with a competent
handwritten zero-and-scatter loop under the same noinline boundary, dtypes,
strides, input order and alias assumptions. Reset work is timed for both paths;
allocation and fixture setup are outside both. Cases include empty and small
inputs, large inputs, few/many bins and repeated/random indices. These benchmarks
are a verification mechanism, not a claim of equal speed on every compiler.
