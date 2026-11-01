# Optional comparison charts

Use this for a small number of important, already collected comparisons. It
renders existing evidence; it does not collect measurements or select favorable
results. The CLI remains standard-library-only unless eligible charts are requested.

With Python 3.11+, install the optional pinned renderer:

```sh
python -m pip install -r devtools/profiler/requirements-charts.txt
cd devtools/profiler
python -m wjr_profiler compare build/baseline build/candidate \
  --charts --out build/chart-report
```

`report.md` embeds one SVG per eligible region/metric. `report.json` retains the
full observations, descriptive statistics, raw hashes/locations and comparison
classification, plus producer/host/build metadata and manifest digests.
`charts/index.json` lists emitted and skipped entries, the renderer version,
report digest and individual SVG digests. Existing output directories are never
overwritten. Rendering failures occur before the output directory is created.

## Read the chart

- Bars are medians, dots are observations, and whiskers are **observed min/max**,
  not confidence intervals. All summaries use every observation. To keep output
  bounded, more than 256 dots per side are evenly subsampled by original order;
  the first and last observations remain visible. The footer states this policy.
- Linear axes start at zero. Large or tiny values use an explicitly labeled
  common scale within that chart. Units, scope, time basis, optimization direction
  and sample level remain visible. Zero baselines do not create a speedup ratio.
- CPU and wall time, different metrics and different measurement scopes get
  separate charts. There is no cross-provider composite score or cross-CPU
  frequency correction.
- Only `comparable` and explicitly `contextual` rows render. Contextual charts
  display that qualification and both environments; they are not same-environment
  regression results. Missing/failed/inconclusive evidence stays in the report
  with a skipped-entry reason, even with `--exploratory`.
- LLVM-MCA plots say **MODEL ESTIMATE**, identify the scheduling model/triple and
  label one simulation as `n=1`, rather than treating its iterations as repeats.
  Synthetic schema data are visibly labeled. Neither kind establishes a runtime
  improvement.

A chart is a diagnostic view of producer declarations and existing eligibility
checks, not authenticated evidence or an automatic publication decision. Keep the
bundles and report with exported figures. Public performance showcases still need
same-contract comparisons against leading mainstream libraries, reproducible
Windows/Linux evidence for sensitive shared paths, and explicit limitations.
The separate [Btree showcase](../../docs/performance/btree-showcase.md) retains
its reviewed Abseil baseline and manual-only collection workflow.

## Offline real-fixture smoke check

The existing Benchmark fixture deliberately has an unknown input hash, so its
charts are skipped. Do not invent that hash to make a figure eligible. Instead,
the real LLVM-MCA fixture supports a transparent identity re-import smoke check:

```sh
python -m wjr_profiler import --provider llvm-mca \
  --raw tests/fixtures/llvm_mca/raw.json \
  --manifest tests/fixtures/llvm_mca/baseline.manifest.json --out build/mca-base
python -m wjr_profiler import --provider llvm-mca \
  --raw tests/fixtures/llvm_mca/raw.json \
  --manifest tests/fixtures/llvm_mca/candidate.manifest.json --out build/mca-candidate
python -m wjr_profiler compare build/mca-base build/mca-candidate \
  --charts --out build/mca-charts
```

This demonstrates rendering the same model data, not an optimized candidate.
The tested renderer is Matplotlib 3.11.1, matching the existing manual showcase.
SVG output uses literal text without TeX, fixed element-ID salt and no timestamp;
repeated rendering is byte-stable within the same renderer/font environment.
Different Matplotlib/font versions may produce different layout or SVG bytes.
