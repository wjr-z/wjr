# Legacy B+ tree baseline against Abseil

This report compares WJR's existing N16 B+ tree with Abseil, a mature high-performance
ordered-container implementation. It is a baseline for further layout work. Abseil
is a strong practical comparison, not a claim that one library is fastest for every
ordered-container workload.

The fixed selection is 4096 elements, integer set, integer map and a map with a
64-byte mapped value. Every selected group includes build + destroy, lower bound,
mixed updates and key scan; ties and losses remain visible beside wins. Byte-budget,
pool and compact candidates are outside this initial comparison.

## Wall-clock time

![Median WJR N16 / Abseil wall-time ratios, with observed five-sample envelopes](assets/btree/btree-time.svg)

A ratio below 1 means WJR took less wall-clock time in that profile. Each bar is
`median(WJR) / median(Abseil)`. Whiskers span `min(WJR) / max(Abseil)` through
`max(WJR) / min(Abseil)`: an observed envelope from five samples, **not a confidence
interval**. The [full numeric table](assets/btree/btree-samples.md) retains each
backend's median and min/max. Near-equal results are not evidence of an improvement.

The initial Windows profile shows fast integer lookups and set construction, while
integer map build + destroy costs about 1.25 times Abseil and small-value key scans
are effectively tied. This is workload- and compiler-specific evidence, not a
universal ranking or a reason to change legacy defaults.

## Requested memory

![Live allocator requests after the mixed trace, from a separate counting pass](assets/btree/btree-memory.svg)

These are live requested bytes after the mixed trace in a **separate counting
allocator pass**. The [numeric table](assets/btree/btree-samples.md) also reports
peak requests and allocation counts. Counts cover build and mixed mutation;
clear/destruction add deallocations. Every selected pass balances allocations and
deallocations and returns requested live bytes to zero after destruction.

The chart excludes the container object's own size, allocator metadata, size-class
rounding, RSS and committed pages. Object/iterator sizes remain in the compact
source JSON. All selected keys are integers and mapped values own no external
buffers. There are no pools; reserved pool memory is `NA`. This chart must not be
read as process memory consumption or timing instrumentation overhead.

## Contract and provenance

- `set<uint64_t>`, `map<uint64_t,uint64_t>` and
  `map<uint64_t,array<byte,64>>`, with value-initialized mapped values.
- WJR N16 and public Abseil B-tree containers use `std::less<>` and correctly
  rebound `std::allocator` during timing. Memory counters are absent from timing.
- Shuffled insertion order and queries are generated before timing. The same
  seed, data and trace are used **within a compiler/STL profile**. `std::shuffle`
  may differ across STLs; cross-platform traces are not asserted identical.
- Build includes empty-container construction, insertion and destruction. Lookup
  uses `lower_bound`: 50% of query **keys exist**, which does not mean 50% return
  `end()`. Before-first and after-last misses are included.
- Mixed replays contain 90% lookup, 5% insertion and 5% iterator deletion, including
  the search for that iterator and the shared validity branch. Each replay starts
  from fresh state; its setup and destruction are excluded from the timed region.
- Scan consumes keys and a digest; it does **not read mapped payloads**. The
  64-byte case measures the effect of storage layout on key traversal, not mapped
  payload throughput. No reference/iterator survives mutation, accommodating the
  different invalidation guarantees. Successful operations are compared.
- CPU, OS, compiler, STL, flags, allocator, WJR source, Abseil source and trace-source
  hash are in the compact input. Hardware performance counters were not collected.

| Profile | CPU | Compiler / STL | Configuration | Evidence |
| --- | --- | --- | --- | --- |
| Windows Clang 18 | Intel Core i5-13500HX | Clang 18.1.8 / MSVC STL 143.202408 | Release, C++20, baseline, `/O2 /Ob2 /DNDEBUG` | [Five selected raw samples and memory rows](data/btree-windows-clang18.json) |

The Windows run uses WJR source `d9c4db33bbdc235fae0b296d472476d8f9edca41` and
Abseil `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`. Original local timing/memory files
are identified by SHA-256 in the compact JSON; the selected raw samples are retained
there for public chart verification. The complete historical raw files are not
copied into the repository. Linux Abseil evidence is produced by the manual flow
below and will be added after its first validated run.

## Reproduce

The specialized flow is disabled by default and has no push/PR/schedule trigger:

```sh
gh workflow run benchmarks.yml --ref <reviewed-branch> -f showcase=true
```

The explicit manual entry calls `btree-showcase.yml`: one Ubuntu 24.04 worker
measures GCC 13 and Clang 18 sequentially, checks out the reviewed Abseil commit,
runs five repetitions and a separate memory pass, then uploads `btree-showcase`.
No vendor source is published in that artifact. The regular scheduled `core`
profile and existing Windows matrix retain their previous behavior.

To regenerate the checked-in charts from the compact evidence:

```sh
python -m pip install matplotlib==3.11.1
python benchmarks/tools/btree_showcase.py render \
  docs/performance/data/btree-windows-clang18.json \
  --output-dir docs/performance/assets/btree
python -m unittest discover -s benchmarks/tools -p test_btree_showcase.py
```

The script also accepts `--format png`. Its `collect` command extracts only this
fixed selection from the benchmark JSON and memory CSV; it rejects missing Abseil,
incomplete repetition sets, incompatible compiler/STL/flags and unbalanced memory
passes. `render` consumes those collected compact inputs; it is not a general
benchmark-data validation interface. See the workflow for the exact collection/build commands and
[the baseline methodology](btree-baseline.md) for the executable's larger input set.
