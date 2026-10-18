from pathlib import Path
import csv
import hashlib
import json
import os
import platform
import shutil
import statistics
import subprocess

source = Path(__file__).resolve().parents[2]
output = source / 'build/minmax-research'
output.mkdir(parents=True, exist_ok=True)
mdspan = output / 'mdspan'
pin = '0e4eba59ad53451784b3839bdf98b25fa2d64931'


def run(args, log):
    process = subprocess.run(list(map(str, args)), cwd=source, capture_output=True)
    (output / log).write_bytes(process.stdout + process.stderr)
    if process.returncode:
        print((process.stdout + process.stderr)[-8000:].decode(errors='replace'))
        raise SystemExit(process.returncode)
    return process


if not (mdspan / '.git').exists():
    run(['git', 'init', mdspan], 'mdspan-init.log')
    run(['git', '-C', mdspan, 'remote', 'add', 'origin', 'https://github.com/kokkos/mdspan.git'],
        'mdspan-remote.log')
run(['git', '-C', mdspan, 'fetch', '--depth=1', 'origin', pin], 'mdspan-fetch.log')
run(['git', '-C', mdspan, 'checkout', '--detach', 'FETCH_HEAD'], 'mdspan-checkout.log')
assert subprocess.check_output(['git', '-C', str(mdspan), 'rev-parse', 'HEAD'], text=True).strip() == pin
run(['lscpu'], 'lscpu.txt')
difference_pin = '9386c922522fa1618d9555585f6edc269404606c'
run(['git', 'fetch', '--depth=1', 'origin', difference_pin], 'difference-fetch.log')
difference_header = subprocess.check_output(
    ['git', 'show', difference_pin + ':include/wjr/np/difference.hpp'], cwd=source)
overlay = output / 'difference-overlay/wjr/np'
overlay.mkdir(parents=True, exist_ok=True)
(overlay / 'difference.hpp').write_bytes(difference_header)
snapshot = output / 'source'
snapshot.mkdir(exist_ok=True)
for filename in ('minmax.cpp', 'minmax_ops.hpp', 'difference_codegen.cpp'):
    shutil.copy2(source / 'benchmarks/research' / filename, snapshot / filename)
flags = ['-std=c++20', '-O3', '-march=x86-64', '-ffp-contract=off', '-fno-fast-math',
         '-DNDEBUG', '-DWJR_DEBUG_LEVEL=0', '-DMDSPAN_USE_PAREN_OPERATOR=1',
         '-DMDSPAN_USE_BRACKET_OPERATOR=0']
includes = ['-I' + str(source / 'include'), '-I' + str(mdspan / 'include')]
cpp = source / 'benchmarks/research/minmax.cpp'
metadata = {'source_sha': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=source, text=True).strip(),
            'mdspan_sha': pin, 'flags': flags, 'platform': platform.platform(),
            'timing_affinity': 'not pinned', 'hardware_counters': 'not collected',
            'difference_source_sha': difference_pin,
            'difference_header_sha256': hashlib.sha256(difference_header).hexdigest(),
            'probe_sha256': hashlib.sha256(cpp.read_bytes()).hexdigest()}
(output / 'metadata.json').write_text(json.dumps(metadata, indent=2), encoding='utf-8')
for name, compiler in [('gcc', 'g++'), ('clang', 'clang++')]:
    run([compiler, '--version'], name + '-version.txt')
    common = [compiler] + flags + includes
    run(common + [cpp, '-o', output / name], name + '-build.log')
    run(common + ['-S', '-masm=intel', cpp, '-o', output / (name + '.s')], name + '-assembly.log')
    # The ordinary binary always validates before any measured kernel calls.
    run([output / name], name + '-oracle.log')
    process = run([output / name, 'bench'], name + '-combined.log')
    (output / (name + '-raw.csv')).write_bytes(process.stdout)
    (output / (name + '-oracle-timing.log')).write_bytes(process.stderr)
    # Sanitizer is a separate correctness-only executable, never used for timing.
    sanitize = [x for x in common if x != '-O3'] + ['-O1', '-g', '-fsanitize=address,undefined',
                                                  '-fno-omit-frame-pointer', cpp, '-o',
                                                  output / (name + '-sanitize')]
    run(sanitize, name + '-sanitize-build.log')
    run([output / (name + '-sanitize')], name + '-sanitize-oracle.log')
    for isa in ('baseline', 'native'):
        difference_flags = [x for x in flags if not x.startswith('-march=')]
        difference_flags += ['-march=' + ('x86-64' if isa == 'baseline' else 'native')]
        run([compiler] + difference_flags + ['-I' + str(output / 'difference-overlay')] + includes +
            ['-S', '-masm=intel', source / 'benchmarks/research/difference_codegen.cpp',
             '-o', output / (name + '-difference-' + isa + '.s')],
            name + '-difference-' + isa + '-assembly.log')

rows = []
for name in ('gcc', 'clang'):
    groups = {}
    with (output / (name + '-raw.csv')).open(newline='') as handle:
        for row in csv.DictReader(handle):
            key = tuple(row[k] for k in ('dtype', 'operation', 'mode', 'count', 'pattern', 'variant'))
            groups.setdefault(key, []).append(float(row['ns_element']))
    medians = {key: statistics.median(values) for key, values in groups.items()}
    for key, median in medians.items():
        base = key[:-1]
        form = key[-1].split('_', 1)[1]
        rows.append(dict(zip(('compiler', 'dtype', 'operation', 'mode', 'count', 'pattern', 'variant'),
                             (name,) + key), ns_element=median,
                         minimum=min(groups[key]), maximum=max(groups[key]),
                         ratio_current_api=median / medians[base + ('api_current',)],
                         ratio_matched_hand=median / medians[base + ('hand_' + form,)]))
with (output / 'summary.csv').open('w', newline='') as handle:
    writer = csv.DictWriter(handle, fieldnames=list(rows[0]))
    writer.writeheader()
    writer.writerows(rows)
print('PASS both Linux compilers: oracle, timing, sanitizer; raw/assembly artifacts recorded')
