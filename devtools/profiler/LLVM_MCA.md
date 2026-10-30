<!-- markdownlint-disable MD043 -->
<!-- This provider guide uses task-oriented headings. -->

# LLVM-MCA scheduling estimates (P02)

The offline importer reads the verified LLVM-MCA **18.1.8** JSON subset. No LLVM
installation is needed on the analysis machine. Google Benchmark and cProfile
commands remain available. From `devtools/profiler`:

```sh
python -m wjr_profiler import --provider llvm-mca \
  --raw tests/fixtures/llvm_mca/raw.json \
  --manifest tests/fixtures/llvm_mca/baseline.manifest.json --out build/mca-base
python -m wjr_profiler import --provider llvm-mca \
  --raw tests/fixtures/llvm_mca/raw.json \
  --manifest tests/fixtures/llvm_mca/candidate.manifest.json --out build/mca-copy
python -m wjr_profiler compare build/mca-base build/mca-copy \
  --out build/mca-report
```

This example reimports identical evidence to check normalization and comparison;
it claims no optimization. The two input regions have distinct semantics and
contracts and are **not compared to one another**. A candidate implementation
needs an explicit common contract and correctness evidence, as in P01.

## Model identity, not hardware timing

Observations retain `kind=estimate`, `time_basis=model`, and
`sample_level=simulation`. A simulation is one result, regardless of how many
iterations were simulated. The one-value min/median/max describes that result;
sample variance is null. It is not a runtime repetition or a five-sample gate.
Derived values retain their estimate origin and input pointers.

For comparison, the analyzer version, model CPU, target triple, simulation
parameters, iterations, dispatch width, resource names/order, successful exit
status and **all analyzer options** must be known and equal. Record semantic
options in `producer.options`, excluding only input/output paths. Missing options
do not establish that defaults were used. The actual analysis host CPU, OS and
compiler do not define the selected scheduling model and do not gate these
estimates. Runtime and model observations never compare as if they shared units.

Unknown version/model/options, failed or partial producer output, call-containing
regions and explicit ignored-instruction modes remain unqualified. Unknown
fields can be imported as exploratory evidence, but `--exploratory` cannot bypass
the MCA model gate to produce a delta. Missing views stay missing, never zero.

MCA estimates do not simulate real branch prediction, cache misses, OS behavior
or end-to-end latency. There is no cycles-to-nanoseconds conversion, comparison
to runtime speed, or performance-regression exit code.

## Actual views and source locations

Manifest regions bind `raw_region_index` to `CodeRegions` positions; display
names may be empty or repeated. Instruction order and optional instruction
details remain in metadata. Resource pressure stays sparse: absent cells do not
become zero. `InstructionIndex == len(Instructions)` is the block summary,
not an additional instruction. Block totals and per-instruction cells are separate.

The fixture has two static instructions per region, 100 simulation iterations
and 200 simulated instructions. For its dependent region, BlockRThroughput is
0.5 cycles/iteration, while TotalCycles/Iterations is 1.04; these are separate
metrics. Reports show model identity and instruction details, resource-pressure
rows, raw hashes and JSON pointers. The raw JSON is preserved byte-for-byte,
including its native CRLF, through a local `.gitattributes` exception.

`tests/fixtures/llvm_mca/` contains real Windows LLVM-MCA 18.1.8 output for the
project-authored `input.s`, with an explicit Linux triple and Skylake model.
`provenance.json` records command, source/raw hashes and successful verification.
Re-executing that command returned exit 0 and byte-identical raw JSON. This is a
scheduling simulation, **not Linux runtime evidence**. Extracting real GCC/Clang
caller regions and collecting further native-platform evidence remain explicit
producer tasks; they are not claimed by this offline importer.

```sh
llvm-mca -json -iterations=100 -mcpu=skylake \
  -mtriple=x86_64-unknown-linux-gnu input.s -o raw.json
```

No collection service, text dependency-graph guesser, plugin framework or automatic
semantic-region extractor is implemented. Run the shared standard-library tests
with `python -m unittest discover -s devtools/profiler/tests -v` from the repo root.
