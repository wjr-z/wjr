#!/usr/bin/env python3
"""Build an HTML dashboard and repository-relative LCOV from llvm-cov."""

from __future__ import annotations

import argparse
from datetime import datetime
from html import escape
from pathlib import Path
import re
import subprocess


DEFAULT_IGNORE_REGEX = "simd.*(sse|avx|simd_cast)"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--llvm-cov", required=True, help="Path to llvm-cov")
    parser.add_argument("--binary", required=True, help="Instrumented test binary")
    parser.add_argument("--profile", required=True, help="Merged llvm profile")
    parser.add_argument("--output-dir", required=True, help="Coverage output directory")
    parser.add_argument("--details-dir", default="details", help="HTML detail subdirectory")
    parser.add_argument("--generate-html", action="store_true", help="Generate file-level HTML")
    parser.add_argument(
        "--source-root",
        type=Path,
        default=Path(__file__).resolve().parents[1],
        help="Repository root containing the include and src coverage directories",
    )
    parser.add_argument(
      "--ignore-regex",
      help="Custom llvm-cov filename ignore regex (default excludes legacy SIMD files)",
    )
    return parser.parse_args()


def default_ignore_regex(source_root: Path) -> str:
    # Match separators from both native Windows and POSIX coverage mappings.
    prefix = re.escape(source_root.resolve().as_posix()).replace("/", r"[/\\]")
    return "^" + prefix + r"[/\\].*" + DEFAULT_IGNORE_REGEX


def coverage_command(args: argparse.Namespace, action: str, *options: str) -> list[str]:
    source_root = args.source_root.resolve()
    ignore_regex = args.ignore_regex
    if ignore_regex is None:
        ignore_regex = default_ignore_regex(source_root)
    return [
        args.llvm_cov,
        action,
        args.binary,
        f"-instr-profile={args.profile}",
        f"-ignore-filename-regex={ignore_regex}",
        *options,
        str(source_root / "include"),
        str(source_root / "src"),
    ]


def write_html(args: argparse.Namespace) -> None:
    # The local caller selects llvm-cov; coverage_command returns argv, never shell text.
    subprocess.run(  # nosemgrep: python.lang.security.audit.dangerous-subprocess-use-audit.dangerous-subprocess-use-audit
        coverage_command(
            args,
            "show",
            "-show-line-counts-or-regions",
            "-use-color",
            "-show-instantiation-summary",
            "-show-branches=count",
            "-format=html",
            "-output-dir=" + str(Path(args.output_dir) / args.details_dir),
        ),
        check=True,
    )


def run_report(args: argparse.Namespace) -> str:
    command = coverage_command(
        args,
        "report",
        "-show-region-summary=true",
        "-show-branch-summary=true",
    )
    result = subprocess.run(command, check=True, capture_output=True, text=True)
    return result.stdout


def normalize_lcov(report: str, source_root: Path) -> str:
    source_root = source_root.resolve()
    records = []
    for record in report.split("end_of_record"):
        lines = record.strip().splitlines()
        source = next((line[3:] for line in lines if line.startswith("SF:")), None)
        if source is None:
            continue
        path = Path(source)
        if not path.is_absolute():
            path = source_root / path
        try:
            relative = path.resolve().relative_to(source_root)
        except ValueError:
            # System headers and external dependencies are not repository files.
            continue
        records.append("\n".join(
            "SF:" + relative.as_posix() if line.startswith("SF:") else line
            for line in lines
        ) + "\nend_of_record\n")
    if not records:
        raise ValueError("llvm-cov export did not contain repository source files")
    return "".join(records)


def write_lcov(args: argparse.Namespace) -> None:
    command = coverage_command(args, "export", "-format=lcov")
    result = subprocess.run(command, check=True, capture_output=True, text=True)
    report = normalize_lcov(result.stdout, args.source_root)
    (Path(args.output_dir) / "lcov.info").write_text(
        report, encoding="utf-8", newline="\n"
    )


def total_metrics(report: str) -> list[tuple[str, int, int, str]]:
    total_line = next(
        (line for line in report.splitlines() if line.strip().startswith("TOTAL")),
        "",
    )
    values = re.findall(r"(\d+)\s+(\d+)\s+(\d+(?:\.\d+)?)%", total_line)
    labels = ["Regions", "Functions", "Lines", "Branches"]
    return [
        (label, int(total), int(missed), f"{coverage}%")
        for label, (total, missed, coverage) in zip(labels, values)
    ]


def metric_card(label: str, total: int, missed: int, coverage: str) -> str:
    percentage = float(coverage.rstrip("%"))
    tone = "good" if percentage >= 80 else "warn" if percentage >= 60 else "low"
    covered = total - missed
    return f"""
      <article class="metric {tone}">
        <div class="metric-label">{escape(label)}</div>
        <div class="metric-value">{escape(coverage)}</div>
        <div class="metric-detail">{covered:,} covered / {total:,} total</div>
      </article>
    """


def write_badge(output_dir: Path, metrics: list[tuple[str, int, int, str]]) -> None:
    lines = next((metric for metric in metrics if metric[0] == "Lines"), None)
    if lines is None:
        raise ValueError("llvm-cov report did not contain line coverage")

    coverage = lines[3]
    percentage = float(coverage.rstrip("%"))
    color = "#167c80" if percentage >= 80 else "#ae6a16" if percentage >= 60 else "#b24646"
    label_width = 74
    value_width = max(58, len(coverage) * 8 + 18)
    total_width = label_width + value_width
    svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="{total_width}" height="20" role="img" aria-label="coverage: {coverage}">
  <title>coverage: {coverage}</title>
  <linearGradient id="s" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <clipPath id="r"><rect width="{total_width}" height="20" rx="3"/></clipPath>
  <g clip-path="url(#r)">
    <rect width="{label_width}" height="20" fill="#555"/>
    <rect x="{label_width}" width="{value_width}" height="20" fill="{color}"/>
    <rect width="{total_width}" height="20" fill="url(#s)"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif" font-size="11">
    <text x="{label_width / 2}" y="15" fill="#010101" fill-opacity=".3">coverage</text>
    <text x="{label_width / 2}" y="14">coverage</text>
    <text x="{label_width + value_width / 2}" y="15" fill="#010101" fill-opacity=".3">{coverage}</text>
    <text x="{label_width + value_width / 2}" y="14">{coverage}</text>
  </g>
</svg>
'''
    (output_dir / "coverage.svg").write_text(svg, encoding="utf-8")


def write_dashboard(args: argparse.Namespace, report: str) -> None:
    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    details_url = f"{args.details_dir}/index.html"
    metrics = total_metrics(report)
    cards = "".join(metric_card(*metric) for metric in metrics)
    generated = datetime.now().astimezone().strftime("%Y-%m-%d %H:%M %Z")

    html = f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>WJR Coverage Overview</title>
  <style>
    :root {{
      color-scheme: light;
      --ink: #17212b;
      --muted: #617080;
      --paper: #f4f7f8;
      --panel: #ffffff;
      --line: #dbe3e7;
      --good: #167c80;
      --warn: #ae6a16;
      --low: #b24646;
    }}
    * {{ box-sizing: border-box; }}
    body {{
      margin: 0;
      color: var(--ink);
      background: linear-gradient(135deg, #eef5f3 0%, var(--paper) 48%, #f7f4ee 100%);
      font: 15px/1.5 ui-sans-serif, system-ui, -apple-system, "Segoe UI", sans-serif;
    }}
    main {{ max-width: 1120px; margin: 0 auto; padding: 52px 28px 72px; }}
    header {{ display: flex; justify-content: space-between; gap: 24px; align-items: end; }}
    h1 {{ margin: 0; font-size: clamp(2rem, 4vw, 3.6rem); letter-spacing: -0.03em; }}
    .kicker {{ margin: 0 0 8px; color: var(--good); font-weight: 700; text-transform: uppercase; letter-spacing: .12em; font-size: .75rem; }}
    .generated {{ color: var(--muted); white-space: nowrap; }}
    .metrics {{ display: grid; grid-template-columns: repeat(4, minmax(0, 1fr)); gap: 14px; margin: 34px 0; }}
    .metric {{ padding: 22px; background: var(--panel); border: 1px solid var(--line); border-top: 4px solid var(--muted); box-shadow: 0 12px 30px rgba(23, 33, 43, .06); }}
    .metric.good {{ border-top-color: var(--good); }}
    .metric.warn {{ border-top-color: var(--warn); }}
    .metric.low {{ border-top-color: var(--low); }}
    .metric-label {{ color: var(--muted); font-weight: 700; }}
    .metric-value {{ margin-top: 8px; font-size: 2.2rem; font-weight: 800; letter-spacing: -0.04em; }}
    .metric-detail {{ color: var(--muted); font-size: .9rem; }}
    .panel {{ padding: 24px; background: rgba(255,255,255,.82); border: 1px solid var(--line); }}
    .panel h2 {{ margin: 0 0 10px; font-size: 1.15rem; }}
    a {{ color: var(--good); font-weight: 700; }}
    pre {{ overflow: auto; margin: 18px 0 0; padding: 18px; background: #17212b; color: #e7f0ef; font: 12px/1.55 ui-monospace, SFMono-Regular, Consolas, monospace; }}
    @media (max-width: 760px) {{
      main {{ padding: 32px 18px 48px; }}
      header {{ display: block; }}
      .generated {{ margin-top: 10px; }}
      .metrics {{ grid-template-columns: repeat(2, minmax(0, 1fr)); }}
    }}
  </style>
</head>
<body>
  <main>
    <header>
      <div><p class="kicker">LLVM source-based coverage</p><h1>WJR test coverage</h1></div>
      <div class="generated">Generated {escape(generated)}</div>
    </header>
    <section class="metrics" aria-label="Coverage totals">{cards}</section>
    <section class="panel">
      <h2>Explore the source details</h2>
      <p><a href="{escape(details_url)}">Open the file-level HTML report</a> to inspect uncovered lines and branches.</p>
      <details>
        <summary>Show raw llvm-cov report</summary>
        <pre>{escape(report)}</pre>
      </details>
    </section>
  </main>
</body>
</html>
"""
    (output_dir / "index.html").write_text(html, encoding="utf-8")
    (output_dir / "summary.txt").write_text(report, encoding="utf-8")
    write_badge(output_dir, metrics)


def main() -> None:
    args = parse_args()
    if args.generate_html:
        write_html(args)
    report = run_report(args)
    write_dashboard(args, report)
    write_lcov(args)
    print(report, end="")


if __name__ == "__main__":
    main()