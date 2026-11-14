from pathlib import Path
from datetime import datetime
import csv
import hashlib
import json
import statistics
import subprocess

output = Path('tail-results')
output.mkdir(exist_ok=True)
rows = []
manifest = []
for repetition in range(3):
    order = ['baseline', 'growing'] if repetition % 2 == 0 else ['growing', 'baseline']
    for mode in order:
        executable = Path('build-' + mode) / 'tail_timing'
        started = datetime.now().astimezone().isoformat()
        result = subprocess.run([str(executable)], capture_output=True, text=True, check=True)
        ended = datetime.now().astimezone().isoformat()
        (output / f'r{repetition+1}-{mode}.csv').write_text(result.stdout)
        (output / f'r{repetition+1}-{mode}.metadata.txt').write_text(result.stderr)
        manifest.append(dict(mode=mode, round=repetition+1, started=started, ended=ended,
                             executable_sha256=hashlib.sha256(executable.read_bytes()).hexdigest()))
        for row in csv.DictReader(result.stdout.splitlines()):
            rows.append(dict(mode=mode, round=repetition+1, **row))
summary = []
for fixture in sorted({row['fixture'] for row in rows}):
    medians = {mode: statistics.median(float(row['ns_per_call']) for row in rows
                                      if row['fixture'] == fixture and row['mode'] == mode)
               for mode in ['baseline', 'growing']}
    pairs = []
    for repetition in range(1,4):
        pair = {mode: statistics.median(float(row['ns_per_call']) for row in rows
                                        if row['fixture'] == fixture and row['mode'] == mode
                                        and row['round'] == repetition)
                for mode in ['baseline', 'growing']}
        pairs.append(pair['growing'] / pair['baseline'])
    summary.append(dict(fixture=fixture, **medians,
                        growing_over_baseline=medians['growing']/medians['baseline'],
                        round_ratios=pairs))
(output / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
(output / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
print(json.dumps(summary, indent=2))
