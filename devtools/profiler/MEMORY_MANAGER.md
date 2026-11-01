# Google Benchmark MemoryManager evidence

`google-benchmark-memory` imports the native MemoryManager fields from verified
**Google Benchmark 1.9.2, JSON schema 1**. This is a separate provider from the
existing 1.9.0 timing importer; no repository C++ dependency is upgraded or added.
The analysis tool needs only Python 3.10's standard library.

```sh
cd devtools/profiler
python -m wjr_profiler import --provider google-benchmark-memory \
  --raw tests/fixtures/memory_manager/raw.json \
  --manifest tests/fixtures/memory_manager/manifest.json --out build/memory
python -m wjr_profiler compare build/memory build/memory --out build/memory-report
```

The fixture is an identity self-check, not an allocator ranking or speed claim.
Its shared development host remains contextual. Input bytes are copied and hashed
as in other providers. Reports retain JSON pointers, raw field values and explicit
missing diagnostics; native aggregates never add samples.

## Units and measurement scope

| Native field | Normalized unit | Meaning |
| --- | --- | --- |
| `allocs_per_iter` | allocations/iteration | Already divided by the **memory pass** iteration count by the producer |
| `max_bytes_used` | bytes/pass | Peak tracked live bytes during that pass |
| `total_allocated_bytes` | bytes/pass | Total tracked allocation bytes during that pass |
| `net_heap_growth` | bytes/pass | Signed tracked allocation-minus-deallocation bytes; no automatic better/worse direction |

The [1.9.2 runner](https://github.com/google/benchmark/blob/v1.9.2/src/benchmark_runner.cc)
runs MemoryManager separately, for `min(16, timing iterations)` on one thread.
Its Start/Stop interval includes manager/setup/teardown work as well as the
benchmark. Whether those allocations are counted depends on the user's hooks;
the importer does not subtract an invented harness constant.

Pass totals and peaks are not divided by the timing iteration count. Peak live
bytes are not RSS, committed pages, pool reservation or total allocation volume.
Missing optional fields and the upstream tombstone stay missing; a measured zero
remains zero. Negative net growth is valid. These are in-process pass repetitions,
not independent process runs or timing measurements.

Each workload must declare `threads: 1`, `clock: "not_applicable"`,
`time_mode: "not_applicable"`, `sample_level: "memory_pass_repetition"`, and:

```json
"instrumentation": {
  "kind": "google-benchmark-memory-manager/1",
  "collection_method": "exact_instrumented",
  "manager_id": "your audited counter implementation/version",
  "coverage": "exactly which allocations/deallocations the hooks see",
  "harness_policy": "included, excluded by hook design, or unknown",
  "pass_iterations": 16
}
```

The explicit pass length must match the fixed-version runner rule in every raw
repetition. Changed coverage/harness policies or pass lengths block comparison;
unknown metadata remains inconclusive. Fewer than five complete raw repetitions
are insufficient. Errors, duplicate/missing repetition indices or invalid values
cannot become valid samples. Other user counters remain
uninterpreted. Timing and memory providers are never treated as interchangeable.

## Why the supported version differs from timing

1.9.0 and 1.9.1 keep pointers into a growing vector of MemoryManager results.
Repetition can invalidate those pointers before JSON reporting. The local native
1.9.0 fixture reproduced corrupted byte fields despite the counter assertions
passing. [Upstream #1939](https://github.com/google/benchmark/issues/1939) describes
this lifetime defect; [#1941](https://github.com/google/benchmark/pull/1941) fixes
the result ownership, and the pinned 1.9.2 source contains that change.

The memory provider rejects unverified versions, including these known affected
ones. It does not label corruptible byte fields as exploratory performance data.
The ordinary timing importer retains its existing 1.9.0 contract.
The rejected native [1.9.0 output](tests/fixtures/memory_manager/rejected-1.9.0.json)
is retained as a negative fixture, with its own provenance and a rejection test.

## Real fixture and reproduction

The independent [producer](tests/fixtures/memory_manager/producer.cpp) was built
against unmodified 1.9.2 commit `afa23b7699c17f1e26c88cbf95257b20d78d6247` on
Windows Clang 18.1.8, C++20 Release. It measures a typed vector allocator, not
global new/malloc, so unrelated framework allocations are outside its hook.
Every MemoryManager Stop checks allocation count, total, peak and zero remaining
live bytes. The process also requires all ten passes to complete.

- Five 16-iteration vector passes: 1 allocation/iteration, 512-byte peak,
  8192-byte total and zero net growth.
- Five empty-control passes: all tracked counts are zero.

[Provenance](tests/fixtures/memory_manager/provenance.json) records the source/raw/
stdout/stderr hashes, exact compiler/link/run commands, provider pin and expected
counters. Rebuild that one standalone source with the pinned benchmark library;
its command is `memory_producer --benchmark_out=raw.json --benchmark_out_format=json
--benchmark_color=false`. The producer is replay material, not a default C++ test
target, installation or global allocator override. Offline fixture tests run in
the existing profiler CI on Linux; native Linux memory collection is not claimed.
