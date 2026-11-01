# Scoped cooperative CPU batches

Build/install WJR with `-DWJR_ENABLE_EXECUTOR=ON`, then:

```sh
cmake -S examples/executor -B executor-example -DCMAKE_PREFIX_PATH=/path/to/wjr-install
cmake --build executor-example
ctest --test-dir executor-example --output-on-failure
```

Two producer threads submit sixteen stable tasks. One worker computes a checksum
over 65,536 records, yielding after each 128-record chunk. Join precedes reading
results or destroying tasks, and a serial checksum verifies the output. No timing
or parallel speedup is claimed. The vector, task storage and thread setup allocate
before or outside the submission operation.

See the [ownership, stop and shutdown contract](../../docs/modules/executor.md).
