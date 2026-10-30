"""python -m wjr_profiler: explicit local files in, new local directories out."""

import argparse
import sys

from .adapters import google_benchmark
from .bundle import import_bundle, load_bundle
from .compare import compare
from .report import write_report

PROVIDERS = {"google-benchmark": google_benchmark.import_artifact}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    importing = commands.add_parser("import", help="copy and normalize explicitly selected raw evidence")
    importing.add_argument("--provider", choices=PROVIDERS, required=True)
    importing.add_argument("--raw", required=True)
    importing.add_argument("--manifest", required=True)
    importing.add_argument("--out", required=True)
    comparing = commands.add_parser("compare", help="check eligibility and render descriptive changes")
    comparing.add_argument("baseline")
    comparing.add_argument("candidate")
    comparing.add_argument("--out", required=True)
    comparing.add_argument("--context-axis", action="append", default=[],
                           help="explicit environmental comparison axis, e.g. build.compiler")
    comparing.add_argument("--exploratory", action="store_true",
                           help="show explicitly exploratory deltas despite incomplete provenance/samples; never bypass contracts or units")
    args = parser.parse_args(argv)
    try:
        if args.command == "import":
            import_bundle(args.provider, PROVIDERS[args.provider], args.raw, args.manifest, args.out)
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
