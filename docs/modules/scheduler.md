# Deterministic cooperative EEVDF queue

Include `<wjr/concurrency/scheduler/eevdf.hpp>` and link `wjr::scheduler`.
The module uses baseline C++20 and has no thread, coroutine, network or oneTBB
dependency. It selects ready work; the caller executes the selected work.
The [runtime design](../design/runtime-eevdf.md) separates later frame ownership,
external waiting, worker placement and shutdown protocols from this policy.

An `eevdf_entity` is caller-owned and must remain at a stable address until
detached. A queue borrows entities, allocates nothing and never destroys their
payloads. Neither queue nor entity may be copied/moved. All access is by one
owner at a time; there are no locks, atomics, worker threads or wakeups.
Detach all entities and settle any dispatch before destroying either object.
Debug assertions diagnose those lifetime preconditions without making them safe
under concurrent access.

```cpp
wjr::sched::eevdf_entity short_job, bulk_job;
wjr::sched::eevdf_queue queue;
queue.enqueue(short_job, {.weight = 1, .request = 32});
queue.enqueue(bulk_job,  {.weight = 3, .request = 256});
queue.close(); // seals admission; registered jobs may still drain
while (!queue.empty()) {
    auto dispatch = queue.begin_dispatch();
    // Check dispatch.status(), then perform a finite chunk for dispatch.entity().
    auto result = process_chunk(*dispatch.entity());
    // Check the result; charge actual work, not the requested amount.
    queue.complete_dispatch(result.service_units, result.finished);
}
```

The complete [batch example](../../examples/eevdf) checks statuses, processes
real independent buffers, settles every dispatch and verifies checksums against
serial computation. Its service unit is one completed record. This is fairness
in charged work units, not CPU time when records have different execution costs.
The queue does not read clocks, interrupt running work or make blocking calls
asynchronous. A callback that blocks or never returns stops the owner.

## Selection and accounting

Weight controls relative service share; request controls virtual deadline
renewal. Both are positive integers. For virtual runtime `v_i`, total weight
`W` and weighted average `V`, an entity is eligible when `v_i <= V`. Select the
smallest deadline among eligible entities, with registration order breaking
ties. An earlier deadline cannot bypass eligibility.

The implementation compares relative weighted products exactly, avoiding
floating-point differences across Windows/Linux. Each service charge is
normalized with scale `2^16`, retaining its division remainder across calls.
An early yield preserves the old deadline. Once virtual runtime reaches it,
renew from the fully charged runtime with ceiling-normalized request length.
An overrun is charged in full; it is not clipped to a request or time slice.
Yield is a scheduling opportunity and need not select a different job.

New admissions start at the floor of the current virtual average. Removing
nonzero-lag work changes that average; no dynamic-membership fluid-clock bound
is claimed. Cancelling and resubmitting is fresh work, not a sleep/wake credit
preservation protocol. Trusted scheduling identities matter: creating more
entities creates more aggregate weight.

## Protocol and limits

`begin_dispatch()` returns a status and entity pointer. A successful selection
stays in the accounted set until exactly one successful `complete_dispatch`.
While it is current, another selection, membership change or ordinary clear
returns `busy`. A ready requeue must charge positive service; a finished empty
job may charge zero. Invalid zero-service requeue leaves dispatch outstanding
so the caller can settle it explicitly.

`cancel(entity)` detaches ready work without executing or destroying it.
`close()` permanently seals admission while permitting drain/cancel/clear.
An unclosed queue can be cleared and reused; a closed queue is not reopened.
Registered accounting observers (`weight`, `request`, `virtual_runtime`,
`virtual_deadline`) are valid only while the entity is registered.

The current small-workset limits are 256 **entities per queue**, weights
`1..4096`, request sizes and service increments `1..2^20` (zero is allowed for
finished work), and virtual-runtime span at most `2^40`. This entity bound is
independent of any future thread-pool worker limit. No worker pool exists here.
The relative weighted sum/product is bounded by `2^60`. Common-origin rebasing
at `2^48` preserves every relative runtime/deadline and tie; observer values
may decrease together, so they are not absolute timestamps.

Invalid admission/capacity/membership operations return a status without
changing membership. An accounting limit leaves numerical fields unchanged,
faults/seals the queue and retains the unsettled entity. After its work has
returned, `clear()` detaches the faulted dispatch and all ready entities.
The faulted queue cannot resume accurate fair dispatch; create a new queue for
a new epoch. Work already performed cannot be rolled back or silently ignored.
No sleeping/wakeup, reweighting, cross-thread ingress or arbitrary coroutine
handle acceptance is part of this entry.

## Costs and validation

Selection uses two linear passes over intrusive nodes; accounting is constant
time using the selected dispatch's saved origin. Completion/cancellation unlink
in constant time. Admission computes the current average; rare common-origin
rebase and clear are linear. No per-job callback allocation, stored callable,
shared ownership, frame allocation or payload copying is hidden in the queue.

Windows Clang 18.1.8 x64 baseline release measured an entity as 56 bytes and a
queue as 48 bytes, alignment 8. These are recorded ABI observations, not portable
size promises. A standalone allocation counter observed zero allocations for
setup/registration and dispatch/accounting with caller stack storage.

A bounded Intel i5-13500HX CPU-0 probe used 100,000 charge/select cycles, five
alternating samples and explicit service 1. Median ns/cycle were:

| Entities | Borrowed intrusive queue | Packed arithmetic reference |
| ---: | ---: | ---: |
| 2 | 5.312 | 3.984 |
| 16 | 41.909 | 19.583 |
| 64 | 261.705 | 123.890 |
| 256 | 707.816 | 278.760 |

Decision hashes matched. The reference owns packed accounting records and omits
the public membership/close/fault protocol; it is a useful locality/arithmetic
cost boundary, not the same complete borrowing API. The pointer traversal and
relative-origin work have a measured cost, especially at the upper bound.
This first implementation provides deterministic policy correctness and does
not claim a throughput win, O(1) selection or zero overhead against flat storage.
Use sufficiently substantial chunks; a large-workset tree/array redesign needs
its own measured selection/membership tradeoff. No cross-machine timing claim
or hardware-counter result is attached to this probe.

Tests use an independent pairwise-lag integer oracle and cumulative-service
conversion, stable ties, eligibility-before-deadline, fractional yields,
overruns, fixed-set weighted service, late admission/removal, capacity and
numeric boundaries, faults, close/drain and common-origin rebasing. The installed
consumer links only `wjr::scheduler` and checks that no ISA options are inherited.
