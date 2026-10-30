<!-- markdownlint-disable MD043 -->
<!-- This provider guide uses task-oriented headings. -->

# cProfile JSON and single-run hotspots (P03)

The offline analyzer reads `wjr.cprofile-json/1` on Python 3.10+. It never loads
marshal or executes a profiled program during import. The explicit producer
exporter is verified only on **CPython 3.12.7**. Other interpreter versions can
analyze the fixed JSON fixture; they cannot export `.prof` with this adapter.

From `devtools/profiler`, no collector is needed for this example:

```sh
python -m wjr_profiler import --provider cprofile-json \
  --raw tests/fixtures/cprofile/profile.json \
  --manifest tests/fixtures/cprofile/profile.manifest.json \
  --out build/cprofile-fixture
python -m wjr_profiler cprofile-summary build/cprofile-fixture \
  --out build/cprofile-hotspots
```

The JSON/Markdown report ranks self seconds and keeps primitive/total call
counts, cumulative seconds, raw hash, function locations and caller edges.
There is no sum of cumulative times, no between-run variance, no native-internal
attribution and no Python/C++ speed ranking. One profile remains **n=1** even
when its functions were called many times. The comparison command consequently
does not treat it as a repeated-run performance gate.

## Profile actual offline analysis

After the P01 example has created the baseline/candidate bundles, run this on
the matching CPython 3.12.7 producer:

```sh
python -m cProfile -o build/own.prof -m wjr_profiler compare \
  build/baseline build/candidate --exploratory --out build/profiled-report
python -m wjr_profiler export-cprofile --input build/own.prof \
  --output build/own.json --producer-version 3.12.7 \
  --timer "cProfile default real-time timer" --time-basis wall \
  --thread-coverage "current thread" --manifest-out build/own.manifest.json
python -m wjr_profiler import --provider cprofile-json \
  --raw build/own.json --manifest build/own.manifest.json --out build/own.bundle
python -m wjr_profiler cprofile-summary build/own.bundle \
  --out build/own-hotspots
```

Export only **your own, single, unmerged local cProfile run** on its matching
producer. `.prof` uses version-related marshal data and does not embed enough
metadata to recover its original interpreter, timer or thread coverage. The
version is therefore an explicit declaration, checked against the verified
current interpreter. Unknown timer/coverage stays exploratory. The exporter
does not guess it, accept arbitrary foreign `.prof`, merge runs or attach to
another process. The analyzer receives JSON only.

Both output files must be new. The optional manifest contains explicit mappings
for every function and honest unknown environment/workload/correctness fields;
fill those before import if known. It is immediately usable for single-run
hotspot diagnosis without inventing controlled benchmark provenance.

## Identity and measured limits

Function identity is `(file, line, name)`, not just name. Across revisions,
edit the manifest to map a common region id to each exact function identity;
the adapter never guesses associations from names or nearby line numbers.
Native and built-in functions retain their reported identities.

The exporter reads `pstats.Stats.stats` directly, preserving unrounded numbers.
CPython 3.12.7 function tuples are primitive/total/self/cumulative/callers;
caller tuples use **total/primitive/self/cumulative**, a different order.
Recursive semantics and caller edges retain that distinction. It never uses
`get_stats_profile()`, which groups by name and rounds timing values.

`tests/fixtures/cprofile/` contains a real run of the owned `program.py`,
`left.py` and `right.py`: duplicate names in two files, recursion and tiny timing.
`provenance.json` records the interpreter/build, source hashes, command, raw
JSON hash, local `.prof` hash, exit status and separate stdout/stderr. Only JSON is
distributed; the native marshal artifact stays on its producer. Synthetic
malformed JSON in tests checks errors without representing measured results.

Run all offline tests from the repository root:

```sh
python -m unittest discover -s devtools/profiler/tests -v
```

On unverified producers only the native-export smoke is skipped; fixture import,
identity, recursion, precision, schema/error, CLI and hotspot tests still run.
