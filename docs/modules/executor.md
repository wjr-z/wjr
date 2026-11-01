# Cooperative scoped CPU executor

Enable `WJR_ENABLE_EXECUTOR=ON` and link `wjr::executor`. Installed consumers use
`find_package(wjr CONFIG REQUIRED COMPONENTS executor)`. The optional compiled
module uses C++20, the standard threading library and WJR's intrusive list. Core
WJR discovery does not import this target or discover Threads. It carries no ISA
requirement, external runtime, coroutine engine or EEVDF dependency.

This first executor runs **one** background worker. Independent producer threads
submit CPU tasks; each invocation returns `task_step::finished` or cooperatively
returns `again` to rotate behind other ready tasks. It cannot preempt a callback.
Blocking a callback blocks all tasks on that executor, including cancellation and
shutdown progress. First-inbox order follows successful admission under the
publication mutex, rather than producer wall-clock arrival. Up to 64 callback
steps run before newly submitted work is appended to the owner's ready ring.
This bounds ingestion by steps, not by time; a callback has no duration bound.

The [producer example](../../examples/executor) submits sixteen chunked checksum
tasks from two producers and checks all 65,536 records against serial arithmetic.
This is offloaded cooperative CPU work, not parallel speedup on a one-worker CPU
executor. There is no automatic network access, detached execution or hidden
blocking-I/O thread pool.

## Storage, admission and cancellation

`cooperative_task<F>` owns `F` inline and borrows any resources captured by
reference. `F&` must be nothrow-invocable with `task_context&`, returning
`task_step`. Move-only callables work. Tasks are address-stable, noncopyable,
nonmovable and one-shot: an accepted task can never be resubmitted. Rejected tasks
remain caller-owned and unsubmitted. Serialize attempts to submit the same task,
including attempts through different executors; different tasks may be submitted
concurrently using the same cached `submitter`.

Construct a `task_scope(executor, maximum_tasks)` and cache `make_submitter()` on
each producer. The scope's explicit acceptance budget counts total successful
admissions, including completed tasks. Zero rejects all work. The bound is
`size_t`, independent of the future pool's 256-worker limit and the separate
bounded EEVDF policy's fair-entity limit. Admission rejects closed scopes/domains,
already accepted tasks and exhausted budgets. Successful admission makes the
task's storage borrowed through scope join, even if its callback finishes early.

`task.request_stop()` affects one task. `scope.request_stop()` seals admission and
requests stop for all accepted tasks. Both are cooperative: a not-yet-invoked
task is skipped; an active callback polls `context.stop_requested()` and should
return promptly. Returning `again` after a stop request becomes cancelled.
Returning `finished` publishes completed even if stop was concurrently requested;
stop does not roll back user effects. Flags use relaxed atomic accesses because
they publish no additional user data. Cancellation is not a memory fence for
arbitrary captured state.

## Join and shutdown

`scope.join()` seals admission, waits for every accepted task to terminate, then
unlinks its registry and releases all task borrows. Observe `task.outcome()` and
non-atomic callback results only before submission or after successful join.
Destroying accepted task storage before join violates the contract even when its
callback appears finished. The scope destructor joins. Keep borrowed resources
alive through the scope destructor, or explicitly join before releasing them.

Cached submitters retain the closed scope/domain state once per handle and may
outlive both public facades. Subsequent submission safely reports closed; they do
not keep the worker running after executor close. Do not destroy or move a handle
concurrently with an operation on that same handle.

`executor.close_drain()` atomically seals admission and joins the worker after all
accepted tasks finish. `close_cancel()` also seals/stops every scope, then joins.
The executor destructor drains. A scope may outlive the executor facade and still
join safely. Executor control/destruction has one controlling thread; concurrent
scope joins and producer calls remain supported. An ordinary scope also has one
lifetime owner. Join/close called from this executor's worker returns
`would_deadlock` without changing state. Destroying either owning facade on its
worker violates the contract and terminates, since its destructor cannot return
that error. Self-destruction inside a task is not supported.

Domain/scope construction, callable construction and native synchronization can
throw standard exceptions. The worker invokes only `noexcept` callbacks; exceptions
escaping such a callback terminate according to the C++ contract. Shutdown does
not forcibly unwind user code, and an endless `again` task makes drain endless.

## Synchronization and cost

This is a mutex/condition-variable baseline, not a lock-free mailbox. No new RCU,
hazard-pointer or tagged wake algorithm is needed for one durable worker. The
single publication mutex protects admission, scope seal, inbox and termination.
User callbacks run outside it. Ready continuations are owner-local. Each task has
two intrusive hooks: the queue hook transfers from inbox to ready; the registry
hook stays in its scope until join. Neither path allocates task nodes.

| Path | Required work and contention |
| --- | --- |
| Scope/handle creation | Shared state allocation per scope; shared ownership increment per cached handle. Standard thread/domain resources are setup costs. |
| Successful submit | One mutex critical section; admission checks, two intrusive appends, two counters and an owner store. No shared-pointer copy, task allocation or callback copy. All producers contend on this one mutex. |
| Local `again` | Two relaxed stop loads before invocation, one indirect call, conditional stop polling after it, one local intrusive append; no mutex or reference-count update. |
| Inbox ingestion | One mutex critical section, whole-ring splice in constant pointer work; every 64 steps or when ready empties. |
| Terminal publication | One mutex critical section, outcome store and pending decrement; notification if last. Completion contends with producers. |
| Idle/park | Predicate and sleeping hint under the same mutex; standard CV releases it while waiting. No spin budget or claim of low wake latency. |
| Wake | Notify under publication lock before clearing sleeping hint and unlocking. Later producers cannot publish past a notifier stalled while holding that lock. |
| Scope join | Seal under mutex, CV wait for zero pending, then O(accepted tasks) registry unlink. |
| Close/cancel | Seal under mutex; cancellation scans registered scopes, not each task; notify and native join. |

Only admission or global close wakes an idle worker: each makes its wait predicate
true. An empty scope's stop request does not notify or clear the sleeping hint;
doing so without changing that predicate could leave it false while CV wait
internally resumes waiting. Spurious wakes keep the hint armed under the predicate
wait. This uses the standard [atomic unlock-and-wait contract](https://eel.is/c++draft/thread.condition.condvar).

The same mutex orders task initialization before worker ingestion and callback
stores before join. Terminal notification happens **before** the completion lock
is released; after that unlock the worker never accesses that task or its scope.
Join acquires this last-access barrier before storage reclamation. Publishing a
terminal flag followed by an unprotected notification would not provide this
lifetime guarantee. The worker-running marker uses release/acquire only for cold
self-join detection, avoiding false detection if an exited thread ID is reused.

The table describes source operations, not measured cache-line transfers or a
bounded mutex retry count. Standard mutex/CV fairness, allocator internals, OS
parking latency and physical cache placement are implementation-dependent.
On Windows x64, Clang 18.1.8 with MSVC STL 14.42, C++20 and `-march=x86-64`, an
empty-callable task measured 64 bytes, each public scope/submitter/executor facade
16 bytes, the domain 208 bytes and scope state 160 bytes. Shared control blocks,
allocator overhead and native thread/CV resources are additional; these are not
portable ABI size guarantees or measured cache-line transfers.
Preconstructed submission and continuation contain no WJR allocation; callables,
scope setup and OS resources can allocate. This baseline makes no throughput,
global fairness, NUMA or near-O(1) placement claim for a future multi-worker pool.
The retained-worker-slot, parking, resize and hierarchical-accounting review must
be validated separately before adding those contracts.

Tests cover four producers and 1,028 tasks, deterministic round-robin ordering,
budget/rejection, move-only storage, individual/scope/global cancellation,
callback self-join rejection, close/admission races, repeated idle publication,
and immediate task reclamation after join. These are executable lifecycle tests,
not a formal model checker or a proof of a new concurrent algorithm.
