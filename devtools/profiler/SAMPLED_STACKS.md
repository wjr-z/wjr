# Sampled Python stack hotspots

The offline provider `py-spy-speedscope` imports the sampled-seconds Speedscope
export from py-spy 0.4.1. It ranks leaf-frame weights and inclusive frame weights,
retains recursive stacks, and keeps each selected thread profile separate. This
supports hotspot diagnosis without deterministic cProfile instrumentation.

The importer uses the Python standard library. py-spy is needed only for an
explicit capture on the producer machine. Reading a bundle never attaches to a
process or executes a collector. The verified fixture was captured on Windows
with py-spy 0.4.1 and CPython 3.12.7. Linux native collection and native C/C++
unwinding remain unverified.

## Capture and inspect

From `devtools/profiler`, explicitly capture a program you own.

```sh
py-spy record --duration 2 --rate 100 --format speedscope --full-filenames \
  --output capture.json -- python tests/fixtures/speedscope/program.py
python -m wjr_profiler speedscope-manifest --raw capture.json --out capture.manifest.json
```

Edit the generated manifest to record the actual revision, input, host, collector
policy and correctness evidence. Unknown metadata stays explicit. Regions select
both the raw `profile_index` and its exact `profile_name`. Thread IDs may change
between captures, so check that mapping again for each raw file.

```sh
python -m wjr_profiler import --provider py-spy-speedscope \
  --raw capture.json --manifest capture.manifest.json --out build/sample-bundle
python -m wjr_profiler speedscope-summary build/sample-bundle --out build/hotspots --limit 20
```

`report.md` contains the leading frames per thread. `report.json` retains every
observed frame, raw frame indices, source pointers, empty-stack weight and declared
sampling policy. Frame identities retain file and line information. Identically
named functions in different files stay separate. An inclusive weight counts a
frame once per stack even when recursion repeats that frame. Inclusive weights
overlap across frames and must not be summed. Fractions use all recorded weights,
including samples whose stacks are empty.

## Interpretation

py-spy 0.4.1 assigns each retained stack the reciprocal of the requested sample
rate and writes their sum as the profile end value. Those values can differ from
the capture duration. The importer preserves them as recorded weights. They
provide sampled attribution, without establishing measured CPU seconds, wall
duration, exact calls or between-run variance. Thread weights remain separate.

Record `producer.sampling_policy`, including `rate_hz`, `gil_only`, `include_idle`,
`native`, `nonblocking` and `errors`. The raw Speedscope export does not embed all
these settings, and the analysis machine cannot reconstruct them. GIL/idle
filters and dropped or failed samples affect what the hotspots represent.

One capture supplies one observation per selected profile. Thousands of stacks
still provide n=1 for independent-run statistics. This provider supplies hotspot
summaries. Generic comparisons retain an inconclusive classification and withhold
deltas for these derived weights. Throughput improvements require matching runtime
measurements. Captures
with zero positive weight are marked missing. Unknown exporter versions are
exploratory. Evented profiles and other weight units are rejected by this provider.

The real fixture contains 199 retained samples and zero collector-reported errors.
Its producer command, program hash, platform and raw SHA-256 are in
[`provenance.json`](tests/fixtures/speedscope/provenance.json). Synthetic tests
cover recursive attribution, empty stacks, multiple threads, malformed indices,
weights, schema versions and offline CLI round-trips.

The format and weighting rules were checked against the
[Speedscope schema](https://www.speedscope.app/file-format-schema.json) and the
[py-spy 0.4.1 exporter](https://github.com/benfred/py-spy/blob/v0.4.1/src/speedscope.rs).
