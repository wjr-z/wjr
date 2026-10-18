# Synchronous oneTBB chunks

`<wjr/parallel/tbb.hpp>` provides one entry in namespace `wjr::tbb`:

```cpp
template<class F>
requires std::invocable<const F&, std::size_t, std::size_t>
void parallel_for_chunks(std::size_t first, std::size_t last,
                         std::size_t grain, const F& function);
```

The callable receives `[begin,end)` chunks of `[first,last)`. Without
cancellation, they are nonempty, nonoverlapping and cover every index exactly
once. Chunk sizes, order, thread identity and actual parallel execution are
unspecified. `grain` controls splitting; it is not a maximum chunk size.
`first <= last` and `grain > 0` are trusted preconditions checked with
`WJR_ASSERT_L2`. Empty input invokes nothing and returns before calling oneTBB.

## Borrowing, concurrency and completion

The adapter synchronously borrows **the same const F object**; it does not copy
or move F. A move-only temporary with a const call operator is valid for the
duration of the call. Return values are discarded. Only the small wrapper
holding a reference is copied by oneTBB. There is no heap-owned callable,
future, scheduler object or hidden output allocation in WJR.

F and all captured resources stay alive and support concurrent calls until
return or propagated failure. `const` invocation proves neither purity nor
race freedom: captured references and synchronized side effects remain the
caller's responsibility. Avoid cross-chunk alias hazards and do not require
every callback to run simultaneously. Work independently enqueued or retained
by a callback is outside this synchronous lifetime contract.

oneTBB waits for dispatched callbacks before this call returns or propagates
an exception. Callback exceptions use its cancellation/propagation mechanism;
already completed writes remain, with no ordered-prefix or rollback guarantee.
If several callbacks fail, the selected exception is unspecified. Even a
`noexcept` callback does not make the adapter `noexcept`: provider setup and
allocation may fail.

**Normal return alone is not a completeness signal under external
cancellation.** Nested execution inherits oneTBB's parent cancellation. A
cancelled parent can suppress child callbacks without a child exception. This
entry does not isolate the context or invent a completion status; do not let
an unwritten `default_construct` result escape on an assumption of coverage.

Callers can construct and reuse their own `oneapi::tbb::task_arena`, then invoke
the adapter from `arena.execute`. Nested loops use that arena/scheduler; WJR
does not create an arena per loop, use `global_control`, change global thread
limits or force nested execution to be serial. While waiting, oneTBB may run
other tasks on the calling thread. Code depending on thread-local state must
account for that reentrancy; explicit isolation remains a caller choice.

## Optional dependency

Enable `WJR_ENABLE_TBB=ON` and link `wjr::tbb`. WJR requires a system or
prebuilt `find_package(TBB 2021.11 CONFIG COMPONENTS tbb)` package; it never
fetches, downloads or builds oneTBB sources. Installed consumers use
`find_package(wjr CONFIG REQUIRED COMPONENTS tbb)`. Both ordinary WJR and NP
remain independent of TBB when the option/component is unused. This module
depends only on WJR common configuration/assertions and `TBB::tbb`, with no NP
or `wjr::isa` dependency and no native compiler flags.

2021.11 is the supported version floor, not an EXACT binary pin. The reviewed
source is [oneTBB v2021.11.0](https://github.com/oneapi-src/oneTBB/tree/v2021.11.0),
commit `8b829acc65569019edb896c5150d427f288e8aba`; the adapter uses the
[`parallel_for` range/body overload](https://github.com/oneapi-src/oneTBB/blob/v2021.11.0/include/oneapi/tbb/parallel_for.h)
with `blocked_range<size_t>` and `auto_partitioner`. Cancellation and arena
semantics follow that provider, rather than a new WJR scheduler contract.
oneTBB is Apache-2.0 licensed; WJR does not redistribute its implementation.

Exception-disabled consumers cannot throw from callbacks. Normal execution is
verified separately against the prebuilt runtime; this does not promise
recoverable provider failures in a consumer compiled without exception support
or rebuild oneTBB to change its failure behavior.

## Evidence and cost boundary

The dedicated tests cover empty/single/uneven/large-index ranges, a grain larger
than the range, callable identity with zero copies/moves, move-only temporaries,
bounded reusable arenas, nested chunks, inherited cancellation and exception
quiescence. They never assume exact partition sizes, thread counts or a barrier
requiring simultaneous workers. The standalone [batch example](../../examples/tbb)
writes independent row results with a reused caller-owned arena.

Local Windows checks use official oneTBB 2021.11.0 precompiled x64 Release
libraries with Clang/MSVC, without compiling provider sources. One existing
Linux CI job uses `libtbb-dev` for runtime tests, installed consumers and the
example; a direct GCC consumer checks the same system provider including its
exception-disabled header subset. The existing two Windows CI entries are
unchanged and do not install TBB.

The cost reference is the equivalent direct oneTBB call with the same borrowed
callable, range and partitioner. Provider startup, task allocation, scheduling
and synchronization are real costs: a thin wrapper does not make them free or
establish that parallel work beats a serial loop. Choose sufficient independent
work and reuse caller scheduling resources; no throughput claim is attached
to this initial adapter.

A local Clang 18 Windows x64 probe inlines the WJR adapter but makes a different
decision about outlining oneTBB's `start_for::run` setup from the direct call.
Forcing only the WJR wrapper inline did not remove that difference, so no such
attribute is added. GCC 12 MinGW has the same entry instruction count in that
probe; this is compile-only evidence, not a claim that its ABI can use the MSVC
prebuilt runtime. Bounded warm direct/wrapped runtime samples cover cheap and
more expensive chunks plus mixed lengths; their variation does not establish
instruction identity, general zero overhead or a speedup over serial execution.
