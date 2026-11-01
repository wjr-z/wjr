# Cooperative weighted batches

This example performs real checksum work on two independent input buffers.
An owner-driven EEVDF queue interleaves finite chunks. Its service units are
completed records, not CPU time or elapsed nanoseconds. A record with a more
expensive computation would consume more CPU without being charged more records.
No thread, asynchronous I/O or OS-preemption claim is made.

After installing WJR:

```sh
cmake -S examples/eevdf -B build-eevdf -DCMAKE_PREFIX_PATH=/path/to/wjr
cmake --build build-eevdf
ctest --test-dir build-eevdf --output-on-failure
```

The example links only `wjr::scheduler`; its usage requirements are baseline
C++20. Input buffers and stable-address jobs outlive the queue. All setup
allocations belong to the two input vectors and the application; selection and
accounting do not allocate. `close()` seals admission and lets the existing jobs
drain. Every selected dispatch is charged before finishing or becoming ready
again, and final checksums are verified against direct serial computation.
