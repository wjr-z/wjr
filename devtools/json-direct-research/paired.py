import csv
import io
import os
from pathlib import Path
import subprocess

fixtures = ["fields8-small", "fields8", "fields32", "fields64", "records-short", "records-long", "records-unknown", "numbers4096", "floats4096"]
rows = []
metadata = []
for fixture in fixtures:
    for pair in range(7):
        variants = ["table", "selector"] if pair % 2 == 0 else ["selector", "table"]
        for variant in variants:
            env = dict(os.environ, JSON_FIXTURE=fixture, JSON_REPEATS="1")
            executable = Path("table-build" if variant == "table" else "research-build") / "json_direct_timing"
            result = subprocess.run([str(executable.resolve())], env=env, check=True, capture_output=True, text=True)
            records = list(csv.DictReader(io.StringIO(result.stdout)))
            assert len(records) == 1, records
            rows.append(dict(variant=variant, pair=pair, **records[0]))
            metadata.append(f"{fixture}/{pair}/{variant}: {result.stderr.strip()}")
with Path("paired.csv").open("w", newline="") as output:
    writer = csv.DictWriter(output, fieldnames=rows[0].keys())
    writer.writeheader()
    writer.writerows(rows)
Path("paired-metadata.txt").write_text("\n".join(metadata)+"\n")
