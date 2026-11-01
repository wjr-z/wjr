<!-- markdownlint-disable MD043 -->
<!-- This tool guide uses CLI-oriented sections, not the fixed document template. -->

# WJR offline profiler

Python 3.10+, standard library only. Import and report generation are offline;
the analysis machine needs no compiler, benchmark executable, LLVM, network
connection or collector. This page is the current provider/version index;
design contracts are in the [design record](../../docs/design/profiler.md),
and remaining acceptance work is in the [implementation status](../../docs/design/profiler-implementation.md).

| Provider | Verified input/version | Available operation | Limit |
| --- | --- | --- | --- |
| Google Benchmark | v1.9.0, `context.json_schema_version=1` | Import and compare benchmark JSON | The bundled B+ tree fixture has an unknown input-data hash; its default comparison is inconclusive |
| LLVM-MCA | 18.1.8 JSON | Import and compare model estimates | Fixture is a Windows-produced simulation with an explicit Linux target; other versions are exploratory, not Linux runtime evidence |
| cProfile | `wjr.cprofile-json/1`; exporter verified on CPython 3.12.7 | Import JSON, explicitly export a local `.prof` on its matching producer, summarize one profile | `.prof` is version-specific; exporter support outside the verified producer is untested |
| Linux perf stat | JSON source-checked against Linux v6.8.12 | Import and summarize whole-process counters or unavailable diagnostics | The fixture worker denied both hardware and tool-event collection; no positive PMU capture is claimed. Other perf versions are exploratory |
| curl | 8.11.1 native `%{json}` JSONL | Import and summarize independent HTTP/1.1 request processes | Verified with an owned localhost server; unknown curl versions are exploratory |

Provider guides document exact contracts, fixtures, and reproduction steps:
[Google Benchmark](#supported-input-and-provenance), [LLVM-MCA](LLVM_MCA.md),
[cProfile](CPROFILE.md), [perf stat](PERF_STAT.md), and [curl](CURL.md).
Collection services, py-spy/Scalene/Tracy, iperf3/OTel, ETW/WPA and performance
regression gates are not implemented.

[Optional comparison charts](CHARTS.md) add offline SVG bars, sample dots and
observed ranges with `compare --charts`. Only this renderer needs Matplotlib and
Python 3.11+; ordinary import/compare remains standard-library-only on Python 3.10+.

From the repository root:

```sh
python -m unittest discover -s devtools/profiler/tests -v
cd devtools/profiler
python -m wjr_profiler import --provider google-benchmark \
  --raw tests/fixtures/btree-v1.9.0.json \
  --manifest tests/fixtures/baseline.manifest.json --out build/baseline
python -m wjr_profiler import --provider google-benchmark \
  --raw tests/fixtures/btree-v1.9.0.json \
  --manifest tests/fixtures/candidate.manifest.json --out build/candidate
python -m wjr_profiler compare build/baseline build/candidate --out build/report
python -m wjr_profiler compare build/baseline build/candidate \
  --exploratory --out build/exploratory-report
```

Read `report.json` and `report.md` in each report directory. The real fixture has
an explicitly unknown input-data hash, so its default report is **inconclusive**.
The final command shows a clearly labeled exploratory delta while retaining that
classification. It does not turn the fixture into a regression gate. Output
directories must be new; raw input is copied byte-for-byte and never overwritten.
Exit 0 means a report/import succeeded, including inconclusive or failed evidence;
exit 2 means bad arguments, schema, hash, JSON, paths or output IO. No speed threshold
changes the exit code. Exit 3 is reserved for future explicit collectors.

## Supported input and provenance

Google Benchmark **v1.9.0**, `context.json_schema_version=1`, native JSON only.
Unknown tool versions or a missing schema declaration remain exploratory;
unsupported schema major versions are rejected. `real_time` and `cpu_time` stay
separate, normalized to ns/iteration from ns/us/ms/s. Benchmark names are exact
manifest selectors, never inferred workload contracts or work-item counts.

`tests/fixtures/btree-v1.9.0.json` is real, untrimmed output from the existing
focused Btree benchmark, with legacy/compact variants and five raw repetitions
plus mean/median/stddev/cv aggregates. Its provenance JSON records the exact
argv, source revision, CPU, platform, exit status and raw SHA-256; stdout and
stderr are separate attachments. Local `.gitattributes` preserves these raw
bytes, including native line endings. Other JSON files use LF. Synthetic edge
cases live in tests and are never presented as measured performance.

Only iteration rows become samples. Aggregates remain metadata, never additional
samples. Internal `iterations` is work inside one sample, not independent n.
This native-file adapter labels repetitions `in_process_repetition`; it cannot
promote them into independent process runs through a manifest declaration.
Errors remain failed observations. Missing/negative/nonfinite durations have
explicit missing reasons; unknown counters retain their raw names and values
for display without unit/Counter-flag guesses. NaN/Infinity are preserved only in
raw bytes and become null or missing diagnostics in strict normalized/report JSON.

## Explicit manifest

The two fixture manifests are complete examples. The v1 shape is:

- `schema_version`, `bundle_id`, `producer: {provider, version}`, `created_at`,
  `revision`, `dirty_patch_sha256` and `raw_source: {sha256}`.
- `host` and `build`: actual machine/core/OS/ABI/CPU, compiler, flags, standard,
  sanitizer/LTO/PGO and a build-configuration fingerprint. Revision/implementation
  source hashes may differ; a configuration fingerprint should exclude those
  identities when comparing separately built implementations.
- `workloads`: an id-indexed dictionary with version, parameters, input hash,
  scope, threads, instrumentation, clock, time mode and sample level.
- `semantic_contracts`: an id-indexed dictionary with version, semantics and
  correctness evidence. Regions explicitly map variants to this common contract.
- `regions`: a list with unique `id`, `variant`, `workload_id`, `contract_id`,
  optional source hash and exact Google Benchmark `benchmark` run_name.

Use `unknown` for unavailable metadata; no host properties are inferred from
the analysis machine. Optional extension fields are retained in the manifest/raw.
Raw, normalized data and the manifest are bound by hashes; changing a manifest
after import requires reimport into a new directory. This is provenance/integrity,
not authenticated evidence or a guarantee that a producer's declarations are true.
Paths inside bundles cannot escape their bundle; artifact commands/URLs are never
executed or fetched.

## Comparison boundaries

Eligibility is checked before delta computation. Contract, workload/input,
units, timing scope/basis, threads, sample level and instrumentation must match.
Known environment changes require explicit `--context-axis host.cpu` (repeat for
every changed field, e.g. `host.id`, `build.compiler`, `build.flags`,
`build.fingerprint`); their deltas remain **contextual**. Shared/development hosts
are contextual even when the remaining fields match. Missing critical metadata
is **inconclusive**, never presumed equal. `--exploratory` can show a labeled
delta with incomplete provenance or too few samples, but cannot bypass known
contract/workload/unit/environment mismatches or failed/missing observations.

Reports show median, min/max, MAD and sample standard deviation/variance (n≥2),
source SHA-256 and JSON pointers. Baseline zero has no relative ratio. The basic
sample threshold is five; at least seven independent process runs are recommended.
There is no confidence interval, automatic significance test, composite score,
cross-CPU frequency calibration or zero-overhead certification. CPU and wall
time are never added together; arbitrary counters never enter regression decisions.

The adapter interface is an internal function
`import_artifact(raw: dict, manifest: dict) -> (observations, diagnostics)`.
Providers are explicitly listed in `__main__.py`; there is no plugin framework.
