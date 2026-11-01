"""Optional offline SVG diagnostics; eligibility and measurements stay in compare.py."""

import hashlib
import io
import math
import textwrap

from .model import field


def _chart_reason(row):
    if row["classification"] not in ("comparable", "contextual") or row["exploratory_delta"]:
        return "comparison is " + row["classification"] + "; no eligible chart"
    for side in ("baseline", "candidate"):
        stats = row[side]["statistics"]
        values = [stats[key] for key in ("median", "minimum", "maximum")]
        if not stats["n"] or any(value is None or not math.isfinite(value) or value < 0
                                 for value in values):
            return "chart requires finite nonnegative observations on both sides"
    return None


def _plotter():
    try:
        import matplotlib
        from matplotlib.figure import Figure
    except ImportError as error:
        raise ValueError("charts require the optional matplotlib dependency; "
                         "install devtools/profiler/requirements-charts.txt with Python 3.11+") from error
    return matplotlib, Figure


def _label(text, width=76):
    return textwrap.fill(str(text), width=width, replace_whitespace=True)


def _dots(samples):
    if len(samples) <= 256:
        return samples
    return [samples[index * (len(samples) - 1) // 255] for index in range(256)]


def _caption(report, row):
    if row["time_basis"] == "model":
        metadata = row["baseline"]["observation"]["metadata"]
        return (f"MODEL ESTIMATE: {metadata.get('cpu_model', 'unknown')}; "
                f"{metadata.get('target_triple', 'unknown')}; not measured runtime")
    parts = []
    for side in ("baseline", "candidate"):
        source = report["provenance"][side]
        parts.append(side + ": " + "; ".join(str(field(source, path) or "unknown")
                     for path in ("host.id", "host.os", "host.cpu", "build.compiler")))
    return "\n".join(_label(part, 108) for part in parts)


def _svg(report, row, matplotlib, Figure):
    # Literal labels: no TeX process, math interpretation, external images or links.
    with matplotlib.rc_context({"text.usetex": False, "text.parse_math": False,
                                "svg.fonttype": "none", "svg.hashsalt": "wjr-profiler",
                                "font.family": "DejaVu Sans", "font.size": 10}):
        figure = Figure(figsize=(11, 6.2), facecolor="white")
        axis = figure.add_subplot()
        figure.subplots_adjust(left=0.30, right=0.91, top=0.72, bottom=0.40)
        colors = ("#3568a8", "#db8435")
        largest = max(row[side]["statistics"]["maximum"] for side in ("baseline", "candidate"))
        scale = largest if 0 < largest < 0.001 or largest > 1e6 else 1
        axis.set_xlim(0, largest / scale * 1.30 if largest > 0 else 1)
        for index, side in enumerate(("baseline", "candidate")):
            stats = row[side]["statistics"]
            median = stats["median"]
            axis.barh(index, median / scale, height=0.46, color=colors[index], alpha=0.85)
            axis.errorbar(median / scale, index,
                          xerr=[[(median - stats["minimum"]) / scale],
                                [(stats["maximum"] - median) / scale]],
                          fmt="none", ecolor="#222222", capsize=5)
            samples = [value / scale for value in _dots(row[side]["observation"]["samples"])]
            axis.scatter(samples, [index] * len(samples), s=13, color="#222222", zorder=3)
            axis.text(stats["maximum"] / scale + axis.get_xlim()[1] * 0.025, index,
                      f"{median:.6g}", va="center", fontsize=10)
        axis.set_yticks([0, 1], labels=[_label(side.title() + ": " + str(row[side]["variant"]), 30)
                                      for side in ("baseline", "candidate")])
        axis.set_ylim(1.6, -0.6)
        xlabel = row["unit"] + (f" (axis multiplied by {scale:.6g})" if scale != 1 else "")
        if row["direction"] in ("lower", "higher"):
            xlabel += "; " + row["direction"] + " is better"
        axis.set_xlabel(xlabel)
        axis.grid(axis="x", alpha=0.2)
        axis.set_axisbelow(True)
        for spine in ("top", "right", "left"):
            axis.spines[spine].set_visible(False)
        synthetic = any(source.get("synthetic") is True
                        for source in report["provenance"].values())
        status = row["classification"].upper()
        if row["time_basis"] == "model":
            status = "MODEL ESTIMATE | " + status
        if synthetic:
            status += " | SYNTHETIC SCHEMA DATA"
        if row["classification"] == "contextual":
            status += " | contextual delta; no same-environment regression claim"
        title = _label(row["region_id"] + " / " + row["metric"], 90)
        figure.text(0.04, 0.96, title, fontsize=13, va="top", weight="bold")
        figure.text(0.04, 0.86, _label(status, 110), color="#844400", va="top")
        figure.text(0.04, 0.79, _label(f"{row['kind']}; {row['time_basis']}; scope: {row['scope']}", 115),
                    va="top", fontsize=9)
        levels = "; ".join(f"{side}: n={row[side]['statistics']['n']} ({row[side]['observation']['sample_level']})"
                           for side in ("baseline", "candidate"))
        footer = "Bars: medians. Whiskers: observed min/max, NOT confidence intervals.\n"
        footer += "Dots: at most 256 observations/side, evenly selected by order; summaries use ALL observations.\n"
        footer += _label(levels, 120) + "\n" + _caption(report, row)
        footer += "\nDescriptive evidence only. Full provenance, hashes and raw locations: report.json."
        figure.text(0.04, 0.31, footer, va="top", fontsize=8)
        output = io.BytesIO()
        figure.savefig(output, format="svg", metadata={"Date": None, "Creator": "WJR offline profiler",
                       "Title": title, "Description": status + "; " + footer})
        figure.clear()
        return output.getvalue()


def render(report):
    """Prepare all outputs before the caller creates its exclusive destination."""
    index = dict(schema_version=1, renderer="matplotlib SVG", renderer_version=None,
                 baseline_id=report["baseline_id"], candidate_id=report["candidate_id"],
                 contextual_axes=report["contextual_axes"], entries=[])
    assets = {}
    plotter = None
    for position, row in enumerate(report["comparisons"]):
        reason = _chart_reason(row)
        entry = dict(comparison_index=position, region_id=row["region_id"], metric=row["metric"],
                     classification=row["classification"], reason=reason, file=None)
        if reason is None:
            if plotter is None:
                plotter = _plotter()
                index["renderer_version"] = plotter[0].__version__
            name = f"comparison-{position + 1:04d}.svg"
            content = _svg(report, row, *plotter)
            assets[name] = content
            entry.update(file=name, sha256=hashlib.sha256(content).hexdigest())
        index["entries"].append(entry)
    return index, assets
