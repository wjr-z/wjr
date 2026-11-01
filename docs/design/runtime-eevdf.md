# Cooperative runtime and EEVDF policy

Status: implementation proposal, reviewed 2026-11-01 against WJR
`c1039eee3e5b1826f3e0bcfe5a15694a3587c989`. No runtime, coroutine support,
performance result or dependency installation is delivered by this document.

## Purpose and existing code

The first useful workload is a bounded set of independently advancing CPU jobs:
for example, a latency-sensitive batch sharing an owner thread with a larger
background batch. Jobs perform real chunks of work and return to the owner.
Weights control service shares; request sizes control the granularity at which
virtual deadlines are renewed. A deterministic queue can also be embedded in
an application's existing event loop.

`include/wjr/concurrency/scheduler/task.hpp` currently contains only an unused
state enum and a task placeholder; `scheduler.hpp` is empty. Neither defines
submission, execution, ownership or shutdown. Keep these placeholders separate
until an implementation deliberately replaces their unsupported surface.
`wjr::tbb::parallel_for_chunks` is a synchronous, borrowed oneTBB call with
provider-owned partitioning/cancellation. It remains unchanged and is not an
EEVDF runtime or a coroutine continuation scheduler.

This design schedules user code at cooperation points. It cannot interrupt a
running function, unblock a synchronous system call or enforce an OS CPU budget.
Blocking work stops that owner thread and every other job on it. A job that
never yields also prevents cancellation and shutdown from completing. These
limitations are part of the public contract, not implementation details.

## Reviewed sources and dependency decision

These are fixed source-review versions, not claims about all versions or a
completed compiler compatibility campaign. None becomes a required dependency.

| Source | Reviewed revision | License | Relevant finding and decision |
| --- | --- | --- | --- |
| [Original EEVDF report](https://people.eecs.berkeley.edu/~istoica/papers/eevdf-tr-95.pdf) | TR-95-22, revised 1996-01-26 | Research reference; no implementation copied | Eligibility and virtual deadlines allocate proportional service under bounded quanta. Its fairness theorem must not be claimed for arbitrary blocking or unbounded cooperative segments. |
| [Linux fair scheduler](https://github.com/torvalds/linux/blob/adc218676eef25575469234709c2d87185ca223a/kernel/sched/fair.c) | v6.12, `adc218676eef25575469234709c2d87185ca223a` | GPL-2.0-only reference | Review `avg_vruntime`, `vruntime_eligible`, `pick_eevdf`, `update_deadline`, `entity_lag`, `place_entity`; implement mathematics independently, without copying kernel code. |
| [Standalone Asio](https://github.com/chriskohlhoff/asio/tree/8806a6803cde7054c3049d3666d3ec36786568c5) | 1.38.2, `8806a6803cde7054c3049d3666d3ec36786568c5` | Boost Software License 1.0 | Preferred candidate for later cross-platform I/O/timers. `io_context`, `awaitable`, `co_spawn` and cancellation already supply real asynchronous operations. They do not supply this EEVDF policy. |
| [libcoro](https://github.com/jbaldwin/libcoro/tree/0161911f260a7507ac1dd4aa26da2e2c092fb550) | 0.16.0, `0161911f260a7507ac1dd4aa26da2e2c092fb550` | Apache-2.0 | Provides tasks, synchronization and a scheduler. Reviewed scheduler headers use POSIX `unistd.h`/pipe operations, default to a worker pool and own scheduling state. Do not present that scheduler as a drop-in Windows baseline backend. |
| [async_simple](https://github.com/alibaba/async_simple/tree/1a5579d2f2d63a54cd7e1882b310b6a38a670d4b) | 1.4, `1a5579d2f2d63a54cd7e1882b310b6a38a670d4b` | Apache-2.0 | Its explicit `Executor` extension point is relevant. It uses virtual scheduling and `std::function`; successful schedule promises execution. `Lazy::Yield` schedules a continuation without handling rejection, so a shutdown adapter cannot simply reject accepted continuations. |

The reviewed source subsets are Asio `io_context.hpp`, `awaitable.hpp`,
`impl/awaitable.hpp`, `co_spawn.hpp`, `this_coro.hpp`, `cancellation_signal.hpp`;
libcoro `scheduler.hpp`, `task.hpp`, `sync_wait.hpp`, CMake configuration;
async_simple `Executor.h`, `coro/Lazy.h`, `executors/SimpleExecutor.h` and CMake
configuration. Licenses were read at the same revisions. A future adapter must
also validate its transitive source/build requirements and the supported WJR
toolchains; a README or a reviewed task header is not a runtime compatibility test.

Do not import an entire upstream scheduler to obtain a ready queue. Start with
the small policy below, then reuse an established I/O provider when there is an
actual waiting operation. Do not build another socket reactor, thread pool,
general future library, task graph, type-erased backend hierarchy or tracing
daemon. The existing oneTBB acquisition policy remains unchanged.

## Scheduling mathematics

Let `w_i > 0` be an entity's weight, `s_i` its charged service, `v_i` its virtual
runtime, and `r_i > 0` its request size. Service and virtual time have different
units. With normalization constant `K`, a service increment changes virtual
runtime by approximately `K * delta_service / w_i`; weight is not a deadline.

For the currently accounted set, define:

```text
W = sum(w_i)
V = sum(w_i * v_i) / W
L_i = w_i * (V - v_i) / K
eligible(i) iff v_i <= V
deadline_i = request_start_virtual_time_i + ceil(K * r_i / w_i)
```

Select the eligible entity with the smallest virtual deadline. Equal deadlines
use registration order, never pointer addresses, clocks or an unstable sort.
This is neither minimum-vruntime selection alone nor ordinary earliest-deadline
selection without eligibility. Virtual deadlines are service-order keys, not
wall-clock completion promises. [Linux's EEVDF description](https://docs.kernel.org/6.12/scheduler/sched-eevdf.html)
describes the distinction; the pinned implementation also has OS-specific
selection/protection features that are not implicitly part of this proposal.

Use a relative origin `b = min(v_i)` and compute
`S = sum(w_i * (v_i - b))`. Test eligibility as
`S >= (v_i - b) * W`, without dividing and rounding `V` first. Relative
differences avoid multiplying a long-running absolute virtual clock by weights.
For every nonempty valid accounted set at least one entity is eligible.

An early cooperative yield keeps the current request's deadline. Reaching that
deadline in virtual runtime renews it from the charged virtual runtime. Resetting a
deadline at every yield would reward frequent yielding. Overruns are charged
in full; renewal does not erase them or pretend an interrupt occurred.

The weighted-average form equals the ideal fluid virtual clock only under the
appropriate membership/lag conditions. Adding or removing nonzero-lag entities
can move that average. Do not describe arbitrary completion/cancellation as
preserving every remaining entity's exact prior lag. New admissions start at
the current average with zero intended lag, subject to stated integer rounding.
Cancellation is terminal removal, not a sleeper wakeup that earns fresh credit.

## First implementation: bounded deterministic queue

Deliver one coherent implementation with a real chunk-processing example and
tests. The queue is a small-workset policy, not an unbounded production OS run
queue. Its explicit first limits are at most 256 accounted entities, weights
`1..4096`, request/service increments up to `2^20` units, and `K = 2^16`.
These bounds are part of this version's admission contract and must be checked
before an admission or accounting update mutates the queue.

Retain each entity's division remainder when converting charged service to
virtual time. With these limits, every positive service unit advances virtual
runtime, and fractional rounding does not accumulate afresh at every yield.
Request lengths use ceiling division. Document integer decisions rather than
using floating point or different Windows/Linux `long double` precision.

Keep the virtual-runtime span at most `2^40`. Since `W <= 2^20`, every relative
weighted sum and eligibility product is at most `2^60`, within `uint64_t`.
Check intermediate additions, deadline construction and service conversion;
do not rely on signed overflow, unsigned wraparound ordering, silent saturation
or a native-only `__int128` branch. Common-origin rebasing subtracts the same
value from every runtime and deadline and must preserve every selection/tie.
An accounting-limit failure changes no numerical state, faults the dispatch
protocol and permits inspection/detachment, not further fair dispatch on a
silently incomplete charge. Work already performed cannot be rolled back.

The initial queue uses stable borrowed entities and a linear scan, with no
allocation during selection/accounting. One scan can compute the relative
weighted sum while maintaining its origin; a second selects the best eligible
deadline. Registration order supplies deterministic ties. This is `O(n)`, not
Linux's augmented deadline tree. Accept the bounded first implementation only
with explicit operation/storage measurements; consider a tree after measured
workset/latency evidence, without changing the policy oracle.

The intended narrow surface is:

```cpp
// Proposed names and flow; not an available WJR API yet.
wjr::sched::eevdf_queue queue;
wjr::sched::eevdf_entity interactive, background; // stable addresses
queue.enqueue(interactive, {.weight = 1, .request = 32});
queue.enqueue(background,  {.weight = 3, .request = 256});
while (auto* selected = queue.begin_dispatch()) {
    // Actual application state belongs to the caller.
    auto result = process_one_bounded_chunk(*selected);
    queue.complete_dispatch(result.service_units, result.done);
}
```

Recoverable statuses cover invalid parameters, capacity, wrong membership,
busy/current dispatch, closed admission and accounting limits. No additional
checked/unchecked wrapper hierarchy is needed. The exact names are finalized
with implementation. No callback, exception pointer or coroutine handle is
required in the bare policy entity.

`begin_dispatch` retains the selected entity in the accounted set. A successful
`complete_dispatch` charges it before retaining it ready or removing it as
finished. No second dispatch or membership mutation is allowed while a dispatch
is outstanding. A ready requeue must report positive service; an empty finished
job may report zero. Repeated zero-service yields must not create a livelock
hidden by a clock increment invented by the scheduler.

The example processes separate real input buffers in finite chunks and checks
the resulting payloads against direct serial computation. Its charged resource
is explicitly completed work units. This establishes deterministic scheduling
of that resource, not CPU-time fairness when per-unit execution costs differ.
The core does not read RDTSC, sleep, start threads or treat `steady_clock`
elapsed wall time as thread CPU time. A later measured-service adapter must
name its clock, granularity and OS-descheduling/blocking treatment.

## Ownership, cancellation and closing

| Boundary | Contract |
| --- | --- |
| Policy entity | Caller-owned, stable address, not copied/moved while registered. Queue borrows it and never deletes its payload. |
| Queue access | One owner at a time; no concurrent mutation, hidden lock, atomics or automatic worker creation. Debug checks do not make it thread-safe. |
| Running work | The caller owns the active dispatch and must settle it exactly once. No queue operation destroys executing code or unwinds another thread. |
| Ready cancellation | Owner removes the entity once. Its payload remains caller-owned. Cancelling the current dispatch requests cooperation at its boundary; it cannot erase an uncharged segment. |
| Close admission | Seal against new submissions. Ready work can still be explicitly drained, or cancelled/detached by the owner. Close does not secretly run arbitrary callbacks. |
| Queue/borrow destruction | Detach all entities and settle any active dispatch first. Entity and queue destructors do not add ownership retention or blocking joins. |
| Service failure | Report already performed work and settle failed completion. A callback/adapter that cannot report it must fault the epoch; it cannot claim continued accurate accounting. |

The first pump is owner-driven and does not retain callbacks beyond the
registered objects' lifetimes. Borrowed views, callable captures and input/output
buffers must outlive all execution that uses them. No shared ownership is added
to ordinary jobs to conceal a missing lifetime protocol.

Entities represent the trusted scheduling identities whose shares are compared.
Creating more entities creates more aggregate weight; this is not isolation
against an untrusted tenant that can invent identities, reset jobs or lie about
service. Per-tenant grouping or admission control is a separate measured need.

## Coroutine and waiting boundary

A coroutine is a resumable computation, not an asynchronous operation by
itself. The next coroutine slice, if justified by the first workload, may own a
move-only C++20 frame and support only explicit cooperative yield and stop.
Initial suspension transfers ownership before execution; final suspension
retains the completed frame until its owner consumes completion and destroys
it once. The task wrapper must never destroy a running frame. Document frame
allocation and captured-resource lifetime; coroutine allocation elision is not
a guarantee. Prefer composition/static policy to CRTP unless inheritance removes
demonstrated duplication or dispatch cost.

Arbitrary awaitables are not accepted by a yield-only promise. That restriction
prevents a supposedly simple executor from destroying a frame still referenced
by an I/O completion. It is an explicit CPU-cooperation API, not fake async I/O.
Cancellation is observed at documented cooperation/await boundaries. Blocking
calls, an infinite computation or a blocking wait on the owner cannot be made
safe by setting a stop bit.

External waits require a separate complete adapter protocol:

1. Register the operation and its frame lifetime before publishing a wait.
2. Preserve the charged segment and its request state before marking it waiting.
   A waiting entity must not be selected as runnable.
3. Publish completions/cancellation acknowledgements to the owner, using an
   operation generation to reject stale completions and exactly one winning
   completion transition. Worker threads never resume a frame directly.
4. Resume on the documented owner/executor. Preserve affinity when code relies
   on thread-local state. Inline completion must use the same state machine,
   without recursive double-resume or resuming before suspension commits.
5. Stop admission, request cancellation, continue driving acknowledgements,
   settle accepted continuations, then destroy suspended/completed frames.
   No blocking join or `sync_wait` inside the owner callback.

The waiting set, provider work guard and outstanding-operation registry remain
alive through the final acknowledgement. `close_cancel` may take unbounded time
if a provider cannot cancel an operation or work fails to cooperate. Destruction
must not turn this into a use-after-free or silently abandoned accepted callback.
A thread-safe ingress, when needed, has explicit storage/backpressure and
publication ordering; it is not smuggled into the single-owner policy queue.

Sleeping/wakeup also needs a separately tested fairness policy. Simply dequeuing
a negative-lag entity and waking it at zero lag allows gaming. Linux v6.12 uses
deferred dequeue for non-eligible sleepers, lag limits and placement compensation;
its [feature definitions](https://github.com/torvalds/linux/blob/adc218676eef25575469234709c2d87185ca223a/kernel/sched/features.h)
are version-specific. A preserved virtual lag `l` cannot just be subtracted from
the old average on insertion: the average changes. With old total weight `W`
and incoming weight `w`, the placement correction is `l * (W + w) / W`, with
an explicit `W == 0` and rounding policy. Delayed accounting entities must never
be accidentally executed. These rules, weight changes and wait/wake do not
enter the first ready-only queue under an incomplete implementation.

For a future Asio adapter, retain `io_context`/executor ownership, associated
executor work and operation cancellation rather than manually resuming an
`asio::awaitable` handle. For async_simple, honour accepted-schedule execution
and context checkin/checkout during close. A successful adapter must execute a
real controlled localhost I/O/timer scenario with cancellation acknowledgement;
printing a sequence around a synchronous call is insufficient.

## Build, evidence and delivery gates

The implementation adds the baseline `wjr::scheduler` target/alias and public headers
only when they exist. It depends on common config/assertions and necessary
portable utilities, not `wjr::isa`, oneTBB, a network library or NP. Ordinary WJR
does not discover/download a provider. A later optional Asio adapter must use an
explicit system/prebuilt or pinned header acquisition path, validate disabled
configuration/installed consumers and leave the existing Windows matrix intact.

Deliver in this order:

1. This design MR, with accurate proposed status and pinned primary sources.
2. One implementation MR for the deterministic ready-only EEVDF queue, real
   finite chunk-processing pump/example, CMake/install contract, tests and user
   guide. This is one capability, not separate arithmetic/schema/header MRs.
3. An owned coroutine or I/O-provider adapter only after a concrete use case
   supplies its execution/wait requirements. Include its complete cancellation,
   ownership and close protocol in that MR; defer it if those cannot be proved.

The first implementation must verify eligibility-before-deadline, unequal
weights/requests, stable ties, partial yields, full overrun charge, fixed-set
long-run service ratios, all membership changes it actually exposes, ready
cancellation, sealing/draining/detachment and every arithmetic/rebase boundary.
Compare deterministic traces against an independently written bounded integer
reference; ensure the reference is not just the implementation called twice.
Use generated small cases and independent invariant checks as well as examples.
Do not claim a paper's steady-state bound from a single favourable trace.

Record `sizeof`/alignment and allocation counts for queue/entities, setup and
dispatch, including any example/provider/frame allocations. Measure bounded
selection/accounting at small and maximum supported worksets against a competent
same-policy reference. Separately report end-to-end useful work and a direct
serial/FIFO baseline without treating different fairness semantics as a free
speedup. No throughput claim is required for a correctness-first policy API.

Use existing GCC/Clang/MSVC C++20 checks, relevant ASan/UBSan lifetime/arithmetic
tests, and installed/default-off consumers. Deterministic tests use explicit
service units, not sleeps or guessed scheduling timing. Later real-provider
tests use only owned localhost operations and bounded deadlines; hang detection
does not become a fairness measurement. No new broad CI/Windows matrix is
justified by this initial module.
