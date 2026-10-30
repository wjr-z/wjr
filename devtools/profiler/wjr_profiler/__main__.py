"""python -m wjr_profiler: explicit local files in, new local directories out."""

import argparse
import sys

from .adapters import cprofile_json, google_benchmark, llvm_mca, curl_json, perf_stat
from .bundle import import_bundle, load_bundle
from .compare import compare
from .cprofile_report import write_summary
from .exporters.cprofile_json import export_profile
from .report import write_report
from .curl_report import write_summary as write_curl_summary
from .perf_report import write_summary as write_perf_summary

PROVIDERS = {"google-benchmark": google_benchmark.import_artifact,
             "cprofile-json": cprofile_json.import_artifact,
             "llvm-mca": llvm_mca.import_artifact,
             "curl-json": curl_json.import_artifact,
             "perf-stat": perf_stat.import_artifact}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    importing = commands.add_parser("import", help="copy and normalize explicitly selected raw evidence")
    importing.add_argument("--provider", choices=PROVIDERS, required=True)
    importing.add_argument("--raw", required=True)
    importing.add_argument("--manifest", required=True)
    importing.add_argument("--out", required=True)
    importing.add_argument("--stderr", help="explicit perf stat diagnostic file, copied and hashed into the bundle")
    comparing = commands.add_parser("compare", help="check eligibility and render descriptive changes")
    comparing.add_argument("baseline")
    comparing.add_argument("candidate")
    comparing.add_argument("--out", required=True)
    comparing.add_argument("--context-axis", action="append", default=[],
                           help="explicit environmental comparison axis, e.g. build.compiler")
    comparing.add_argument("--exploratory", action="store_true",
                           help="show explicitly exploratory deltas despite incomplete provenance/samples; never bypass contracts or units")
    exporting = commands.add_parser("export-cprofile", help="convert your own local .prof on its matching CPython 3.12.7 producer")
    exporting.add_argument("--input", required=True)
    exporting.add_argument("--output", required=True)
    exporting.add_argument("--producer-version", required=True,
                           help="explicit original interpreter version; .prof does not embed it")
    exporting.add_argument("--timer", default="unknown")
    exporting.add_argument("--time-basis", choices=("wall", "cpu", "unknown"), default="unknown")
    exporting.add_argument("--thread-coverage", default="unknown")
    exporting.add_argument("--manifest-out", help="write an editable manifest with unknown metadata and explicit function mappings")
    hotspots = commands.add_parser("cprofile-summary", help="rank self-time hotspots from one imported profile; no regression claim")
    hotspots.add_argument("bundle")
    hotspots.add_argument("--out", required=True)
    requests = commands.add_parser("curl-summary", help="show request phases from an offline curl JSONL bundle")
    requests.add_argument("bundle")
    requests.add_argument("--out", required=True)
    counters = commands.add_parser("perf-summary", help="describe one offline whole-process perf stat run")
    counters.add_argument("bundle")
    counters.add_argument("--out", required=True)
    args = parser.parse_args(argv)
    try:
        if args.command == "import":
            decoders = {"curl-json": curl_json.decode_raw, "perf-stat": perf_stat.decode_raw}
            import_bundle(args.provider, PROVIDERS[args.provider], args.raw, args.manifest, args.out,
                          decoder=decoders.get(args.provider), stderr_path=args.stderr)
        elif args.command == "perf-summary":
            write_perf_summary(load_bundle(args.bundle), args.out)
        elif args.command == "curl-summary":
            write_curl_summary(load_bundle(args.bundle), args.out)
        elif args.command == "export-cprofile":
            export_profile(args.input, args.output, args.producer_version, args.timer,
                           args.time_basis, args.thread_coverage, args.manifest_out)
        elif args.command == "cprofile-summary":
            write_summary(load_bundle(args.bundle), args.out)
        else:
            report = compare(load_bundle(args.baseline), load_bundle(args.candidate),
                             args.context_axis, args.exploratory)
            write_report(report, args.out)
    except (ValueError, OSError, UnicodeError, OverflowError) as error:
        print("error: " + str(error), file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
