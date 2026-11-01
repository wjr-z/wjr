<!-- markdownlint-disable MD043 -->
<!-- This provider guide describes the live producer API and offline diagnosis. -->

# Python live-block diagnosis with tracemalloc (P06b)

`tracemalloc-json` imports `wjr.tracemalloc-json/1` on the existing Python 3.10+
standard-library analyzer. The producer API is verified on **CPython 3.12.7** and
exports the caller's own `tracemalloc.Snapshot` objects to JSON. It does not run a
program, attach to a process, start/stop tracing or load native snapshot files.
No optional dependency is added.

```sh
cd devtools/profiler
python -m wjr_profiler import --provider tracemalloc-json \
  --raw tests/fixtures/tracemalloc/full.json \
  --manifest tests/fixtures/tracemalloc/full.manifest.json --out build/python-memory
python -m wjr_profiler tracemalloc-summary build/python-memory \
  --out build/python-memory-report --limit 20
```

The JSON/Markdown summary shows baseline/ending live bytes and block counts,
signed retained differences, separate domain totals, positive/negative traceback
changes, and an optional global checkpoint. Markdown limits its site tables;
`report.json` retains every site, full recorded frame sequence, original depth
and raw JSON pointers. Frame paths are displayed as data; source files are not
opened or executed. New output paths are required.

## Capture an owned workload

On the matching CPython 3.12.7 producer, after importing the application/tooling:

```python
import tracemalloc
from wjr_profiler.exporters.tracemalloc_json import export_snapshots, memory_checkpoint

tracemalloc.start(8)
before = tracemalloc.take_snapshot()
tracemalloc.reset_peak()

# Run the owned workload. Keep intended survivors alive through the ending snapshot.
retained = [bytearray(64 * 1024)]
temporary = bytearray(512 * 1024)
del temporary

checkpoint = memory_checkpoint(
    peak_window="since reset_peak immediately after baseline snapshot",
    sampling_point="immediately before ending take_snapshot",
)
after = tracemalloc.take_snapshot()
tracemalloc.stop()

export_snapshots(
    before, after, "build/memory.json", manifest_output="build/memory.manifest.json",
    coverage="unfiltered snapshots; all domains",
    harness_policy="baseline Snapshot and checkpoint helper included; no subtraction",
    epoch_policy="same tracing epoch; no clear/stop between endpoints; peak reset after baseline",
    checkpoint=checkpoint,
)
```

Export after collection; serialization is outside these snapshots/checkpoint.
Tracing changes execution and memory use. Imports/allocations before tracing are
outside its coverage. The producer owns the same-epoch and identical selection
contract: do not stop/clear tracing between snapshots or silently change filters.
Snapshot objects do not encode that history, so the exporter cannot infer it.
They also provide no reliable timestamps to verify endpoint order. Verified
producer support describes the format/API, not the truth of these declarations.
Declare any selection, collection objects, GC policy and other harness work.
Missing coverage/epoch/harness declarations remain explicitly exploratory.

The optional checkpoint reads current/peak tracked bytes and tracing-metadata
storage in that order. The producer declares its sampling point and peak window;
the helper never resets the peak itself. Sampling before the ending snapshot
avoids including that snapshot's construction in this checkpoint. Earlier
Snapshot/helper allocations still belong to the declared collection policy.
The three counters are not one atomic sample; no subtraction reconciles them.

`checkpoint=None` is valid. Snapshot endpoints cannot recover a transient peak,
so its counters stay missing. A real zero remains zero. Native
`Snapshot.dump()` files are pickle: neither this API nor the offline importer
calls `Snapshot.load`, `pickle.load` or `pickle.loads`.

## Meaning and boundaries

| Evidence | Meaning | Does not establish |
| --- | --- | --- |
| Baseline/ending live bytes and blocks | All records in each supplied snapshot, grouped by domain and traceback | All allocations ever performed |
| Signed retained difference | Ending minus baseline at the supplied endpoints | Gross allocation/free counts, allocation volume or a leak verdict |
| Global current/peak bytes | All tracer domains at the separately declared checkpoint; peak since tracing start or latest reset | Per-frame peak, sum of site peaks, or the ending snapshot size |
| Tracer overhead bytes | Storage reported by `get_tracemalloc_memory()` at the checkpoint | RSS, total process heap or traced live-byte payload |

Net changes may be negative. A temporary allocation freed before both endpoints
has no retained site, even if it dominates the global peak. Larger live storage
can represent useful retained work; observations have no automatic better/worse
direction. Memory metrics have no duration/latency conversion.

Every pair is **n=1**, labeled `in_process_snapshot_pair`. Blocks, frames and
same-process checkpoints are not independent repetitions. Net changes are
`derived` observations; endpoint/checkpoint sizes are instrumented measurements.
The existing comparison command cannot promote these into a regression gate.
Manifest clock/time-mode compatibility fields are `not_applicable`; no timer is
collected. Unknown provenance remains unknown rather than being filled from the
analysis machine.

The exporter walks each Snapshot trace and preserves `(domain, recorded frames,
total_nframe)`. Frames retain oldest-to-newest order, literal file/line identity,
and `<unknown>`/empty source names. Original depth may be null; a larger depth
than the recorded sequence is explicitly truncated. Missing callers are never
guessed. Domain 0 is Python tracing; extensions can register other domains whose
resources must be interpreted by their declared coverage. Global counters cover
all tracked domains even when supplied snapshots have been filtered.

The pinned [Python wrapper](https://github.com/python/cpython/blob/v3.12.7/Lib/tracemalloc.py)
shows that ordinary `statistics('traceback')` grouping drops domain/original-depth
axes and native snapshot loading uses pickle. This exporter retains those axes
directly. The [C implementation](https://github.com/python/cpython/blob/v3.12.7/Python/tracemalloc.c)
maintains global current/peak counters across domains and separate trace-table
storage. The [standard-library API](https://docs.python.org/3.12/library/tracemalloc.html)
documents reset/coverage, frame ordering and truncated depths. These sources do
not make traced sizes equivalent to resident memory or native allocation volume.

## Real fixtures and validation

The owned [program](tests/fixtures/tracemalloc/program.py) runs twice on CPython
3.12.7: `--frames 5` retains caller paths, and `--frames 1` records truncation.
It keeps 64/96 KiB payloads from different callers of the same leaf, releases an
earlier 128 KiB payload, and creates/frees a 768 KiB temporary. Payload sizes
exclude object and harness overhead. Assertions verify survivors and a global
peak at least 700 KiB above checkpoint current storage. The temporary has no
surviving traceback; its peak is not assigned to any caller by the report.

[Provenance](tests/fixtures/tracemalloc/provenance.json) records producer/build,
base revision, source and raw hashes, exact commands, and separate stdout/stderr.
Raw JSON is complete, unfiltered output; the manifest leaves unestablished host,
input and correctness fields unknown. These samples diagnose the owned example,
not an application memory ranking or a performance improvement.

```sh
python tests/fixtures/tracemalloc/program.py --frames 5 \
  --output build/own-full.json --manifest build/own-full.manifest.json
python tests/fixtures/tracemalloc/program.py --frames 1 \
  --output build/own-truncated.json --manifest build/own-truncated.manifest.json
```

From the repository root, the existing profiler suite checks JSON/CLI integrity,
malformed/duplicate records, signed/zero/missing values, domains, unknown depths,
scope mismatches, n=1, exclusive outputs and refusal to load pickle:

```sh
python -m unittest discover -s devtools/profiler/tests -v
```

Live producer tests run only on the verified CPython 3.12.7 interpreter. Offline
fixture tests run in the existing Python 3.10 CI without a collector or new
dependencies. Other producer versions can be inspected as exploratory JSON.
