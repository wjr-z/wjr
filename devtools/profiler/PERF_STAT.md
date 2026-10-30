<!-- markdownlint-disable MD043 -->

# perf stat offline process counters (P04)

Import your own Linux perf stat JSONL on a Python 3.10+ analysis machine.
The importer and reports use only the standard library; they never start
perf, execute an artifact command, fetch URLs or attach to a process.

The format contract is checked against **Linux v6.8.12** producer source and
its official JSON-output lint. This is schema evidence, **not successful
hardware collection**. The existing authorized GitHub worker had
`perf_event_paranoid=4`: both hardware and tool-event probes returned 255
with empty output. Their real diagnostics are included. No permission
settings were changed and no elevated collection was attempted.

## Try the real unavailable capture

From `devtools/profiler`, create new output directories:

```sh
python -m wjr_profiler import --provider perf-stat \
  --raw tests/fixtures/perf_stat/hardware.json \
  --manifest tests/fixtures/perf_stat/hardware.manifest.json \
  --stderr tests/fixtures/perf_stat/hardware.stderr --out build/perf-bundle
python -m wjr_profiler perf-summary build/perf-bundle --out build/perf-report
```

The report shows unavailable counters with n=0, CPU/core information,
producer exit status and the original permission diagnostic. Empty output
does not become zero cycles, and importing failed evidence returns success
for the import operation without claiming the collection succeeded.

`--stderr` is an optional, explicitly selected perf diagnostic file. It is
copied byte-for-byte to `raw/stderr.txt`, hashed and checked on bundle load.
Its UTF-8 diagnostic view uses replacement characters for invalid bytes;
the attachment preserves the originals. All existing providers retain their
interfaces; `--stderr` is rejected for other provider imports.

## Import a file from your own producer

For a command you are authorized to measure, an explicit producer command
can be recorded outside this offline tool:

```sh
LC_ALL=C perf stat --json-output --no-big-num \
  -e cycles:u,instructions:u,branches:u,branch-misses:u \
  -o own-stat.jsonl -- ./owned-program > own.stdout 2> own.stderr
```

Save the actual exit status, binary/version, argv, CPU, kernel, PMU,
affinity/core type, privilege filters, inheritance and scaling policy.
Use a fresh invocation/output file for each independent process run.
If permission is denied, preserve that result; this tool does not request
elevation or change host settings. The example is a producer instruction,
never automatically executed by import or report commands.

Start from `hardware.manifest.json`, update the raw SHA-256, provenance,
environment, workload and exact event selectors, and set `exit_status` to
the real result. `producer.evidence` may be `user_supplied`; the included
`synthetic` example is deliberately marked synthetic and cannot establish
performance changes. Unknown environment/collection fields stay `unknown`.
Do not reuse the fixture's worker identity for a different machine.

`producer.perf_stat` declares one `owned_command`, `whole_process`,
`global` aggregation, `repeat=1` and `interval=false`. `inherit` is an
explicit boolean. `scaling` is `perf_scaled` for normal perf output or
`raw` with `--no-scale`. The single region's `events` are exact native
names, including modifiers such as `:u`. No benchmark-name or PMU alias
mapping is inferred. Scope must remain `whole_process`, with
`independent_process_run` samples.

## Counter interpretation

- Native JSONL uses one object per event line. Physical line locators
  `/lines/N` refer to that raw object. Blank lines retain their numbering.
  JSON arrays are not the source-checked producer format.
- Counter values retain their printed units, including `msec`. Empty
  native units represent counts. Integer-valued decimal strings preserve
  precision beyond 2^53. There is no CPU-frequency conversion.
- `counter-value` is retained as printed: normal perf output may already
  be multiplex-scaled. The importer never divides it by `pcnt-running`.
  `--no-scale` output stays raw; it is not promoted to full-coverage counts.
- `event-runtime` and rounded `pcnt-running` are separate metadata.
  Hardware runtime uses ns; the v6.8.12 `user_time`/`system_time` tool
  bookkeeping uses microseconds. Values and runtime units remain separate.
  `time_enabled` is not exported directly by this JSON format and remains
  unknown; it is not reconstructed from the rounded percentage.
- A zero value from an event that ran is a real zero. `<not supported>`,
  `<not counted>`, absent events, invalid numbers and failed producer exits
  have distinct unavailable diagnostics and no fabricated zero sample.
- One file is one process-run sample per selected event. Events, internal
  iterations and perf `-r` aggregates are not independent repetitions.
  A single run has variance=null. Repeated/append output, interval/per-CPU
  layouts and aggregate rows are rejected rather than pooled.
- Perf's extra derived metric rows remain in raw evidence with a diagnostic;
  their formulas or units are not guessed. Selected counter events can
  still be imported when these extra rows are present.

The summary lists every event's native unit, coverage, multiplexing,
availability and source. Counts cover process startup/teardown and inherited
children according to the declared policy. They cannot be attributed to a
kernel or divided into per-call counts without separate instrumentation.

Comparison is conservative: known matching host id, CPU and core plus
matching PMU, kernel, filters, inheritance, affinity, core type and scaling
are required even with `--exploratory`. Context axes cannot bypass those
counter contracts. Multiplexed counters and synthetic schema examples
remain diagnostic evidence without deltas. Other perf versions are
exploratory. A matching version identifies the source-checked schema; it
does not assert this repository collected the caller's hardware counters.

## Evidence and validation

`tests/fixtures/perf_stat/provenance.json` records the real denied run,
commands, source URLs/hashes and the upstream lint invocation. The two empty
raw captures and original stdout/stderr are byte-preserved. The fixture
`synthetic.schema.jsonl` is handwritten, not measured; it exercises numeric
counts, native units, partial coverage and unavailable markers. It passes
the official v6.8.12 JSON lint. The upstream tool is not bundled or needed
by the offline suite:

```sh
python -m unittest discover -s devtools/profiler/tests -v
```

Primary format references are the pinned
[producer](https://github.com/gregkh/linux/blob/v6.8.12/tools/perf/util/stat-display.c),
[JSON lint](https://github.com/gregkh/linux/blob/v6.8.12/tools/perf/tests/shell/lib/perf_json_output_lint.py),
[native test](https://github.com/gregkh/linux/blob/v6.8.12/tools/perf/tests/shell/stat%2Bjson_output.sh)
and [manual](https://github.com/gregkh/linux/blob/v6.8.12/tools/perf/Documentation/perf-stat.txt).
No perf source or binary is redistributed. Positive PMU smoke remains
uncompleted on the permission-restricted worker and must not be inferred
from the schema tests.
