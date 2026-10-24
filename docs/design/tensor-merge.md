# Stable merging into borrowed output

`<wjr/np/merge.hpp>` provides `merge_sorted_to(destination, left, right, compare)`.
It combines two sorted rank-one views into caller-owned output without allocation,
source mutation, temporary buffers or ownership bookkeeping. All three views have
the same arithmetic element type; inputs may be const. Positive-stride and padded
layouts are supported, and output padding is untouched.

The default order is ascending with NaNs last. Signed zeros compare equivalent;
equivalent elements from the left precede those from the right. Integer ordering
preserves the original type. A custom strict weak ordering can supply descending
order or other equivalence classes. The input ranges must already be sorted by
that same ordering: custom floating-point predicates must define a coherent NaN
order if NaNs are present.

The output length is exactly the sum of the input lengths and its storage must be
disjoint from both inputs. Read-only input ranges may overlap each other. The
length precondition is asserted at level 2 using subtraction to avoid overflowing
the sum. No input validation pass or release-time overlap detection is performed.
Both empty inputs permit null data pointers.

The comparator is borrowed for the duration of the call, including noncopyable
lvalues and mutable state. It must not modify inputs or access output, and must
maintain its ordering throughout the merge. Invocation uses const element
references. Results with an explicit bool conversion are accepted. The function
is noexcept precisely when invocation and conversion are noexcept. If either
throws, output may contain a partially written prefix; no rollback is attempted.

Packed views use `std::merge` with a reference-capturing predicate adapter. Empty
input tails use `std::copy_n`, without computing null end pointers. Other layouts
use a direct merge loop with strides obtained once. The algorithm performs at
most L+R-1 comparisons when both inputs are nonempty, writes exactly L+R values,
and requires constant auxiliary space. No SIMD or threading policy is imposed.

`examples/np/merge.cpp` shows the API. `benchmarks/src/np_merge.cpp` compares the
public call against standard packed merge and direct strided loops with identical
ordering, output writes and wrapper boundaries. Fixture allocation and sorting
are outside timing. Local Windows measurements are provisional; Linux GCC/Clang
measurements should decide any later kernel specialization.
