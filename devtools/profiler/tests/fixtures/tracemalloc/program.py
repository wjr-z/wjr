"""Owned producer: retained/released blocks, temporary-only peak and two leaf callers."""

import argparse
import gc
from pathlib import Path
import sys
import tracemalloc

sys.path.insert(0, str(Path(__file__).resolve().parents[3]))

from wjr_profiler.exporters.tracemalloc_json import export_snapshots, memory_checkpoint


def leaf(size):
    return bytearray(size)


def left():
    return leaf(64 * 1024)


def right():
    return leaf(96 * 1024)


def temporary():
    value = leaf(768 * 1024)
    if len(value) != 768 * 1024:
        raise RuntimeError("temporary payload has the wrong size")


def main(output, manifest, frames):
    gc.collect()
    tracemalloc.start(frames)
    released = leaf(128 * 1024)
    before = tracemalloc.take_snapshot()
    tracemalloc.reset_peak()
    retained = [left(), right()]
    del released
    temporary()
    gc.collect()
    checkpoint = memory_checkpoint(peak_window="since reset_peak immediately after baseline snapshot",
                                   sampling_point="immediately before ending take_snapshot")
    after = tracemalloc.take_snapshot()
    tracemalloc.stop()
    if [len(value) for value in retained] != [64 * 1024, 96 * 1024]:
        raise RuntimeError("retained payload sizes differ")
    if checkpoint["peak_bytes"] - checkpoint["current_bytes"] < 700 * 1024:
        raise RuntimeError("temporary allocation did not establish the expected global peak")
    export_snapshots(before, after, output,
        coverage="unfiltered owned snapshots; all supplied domains (fixture has domain 0)",
        harness_policy="baseline Snapshot and checkpoint helper allocations included; GC explicit; no subtraction",
        epoch_policy="same tracing epoch; no clear/stop between endpoints; peak reset after baseline",
        checkpoint=checkpoint, manifest_output=manifest)
    print("retained payload=163840 bytes; released payload=131072 bytes; temporary payload=786432 bytes")
    print("Payload bytes exclude object/harness overhead; peak is global, with no frame attribution.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True)
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--frames", type=int, default=5)
    args = parser.parse_args()
    main(args.output, args.manifest, args.frames)
