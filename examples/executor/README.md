# Scoped cooperative CPU batches

Build/install WJR with `-DWJR_ENABLE_EXECUTOR=ON`, then:

```sh
cmake -S examples/executor -B executor-example -DCMAKE_PREFIX_PATH=/path/to/wjr-install
cmake --build executor-example
ctest --test-dir executor-example --output-on-failure
```

Two producer threads each submit one eight-task batch from disjoint spans of a
stable sixteen-task array. Each batch is admitted under one lock or rejected in
full; no individual task nodes are allocated. One worker computes a checksum
over 65,536 records, yielding after each 128-record chunk. Join precedes reading
results or destroying tasks, and a serial checksum verifies the output. No timing
or parallel speedup is claimed. The input vector and thread setup allocate before
or outside submission; task and output arrays use the caller's stack.

See the [ownership, stop and shutdown contract](../../docs/modules/executor.md).
