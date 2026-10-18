from pathlib import Path
import hashlib, io, json, re, shutil, subprocess, zipfile

repo = Path(__file__).resolve().parents[2]
out = repo / 'build/axis-norm-research'
out.mkdir(parents=True, exist_ok=True)
before = 'ca8c0e12ff844bb40fb4ed5a3fbd66b31a92a94e'
after = '15dae1be6477f90560d34a4163e9c076746cd0ef'
pin = '0e4eba59ad53451784b3839bdf98b25fa2d64931'

def run(args, log):
    p = subprocess.run(list(map(str, args)), cwd=repo, capture_output=True)
    (out / log).write_bytes(p.stdout + p.stderr)
    if p.returncode:
        raise SystemExit((p.stdout + p.stderr).decode(errors='replace')[-8000:])
    return p.stdout

for name, sha in [('before', before), ('after', after)]:
    run(['git', 'fetch', '--depth=1', 'origin', sha], name + '-fetch.log')
    data = subprocess.check_output(['git', 'archive', '--format=zip', sha, 'include'], cwd=repo)
    folder = out / name
    folder.mkdir(exist_ok=True)
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        for f in archive.infolist():
            assert (folder / f.filename).resolve().is_relative_to(folder.resolve())
            assert (f.external_attr >> 16) & 0o170000 != 0o120000
        archive.extractall(folder)
mdspan = out / 'mdspan'
run(['git', 'init', mdspan], 'mdspan-init.log')
run(['git', '-C', mdspan, 'remote', 'add', 'origin', 'https://github.com/kokkos/mdspan.git'], 'mdspan-remote.log')
run(['git', '-C', mdspan, 'fetch', '--depth=1', 'origin', pin], 'mdspan-fetch.log')
run(['git', '-C', mdspan, 'checkout', '--detach', 'FETCH_HEAD'], 'mdspan-checkout.log')
run(['lscpu'], 'lscpu.txt')
probe = repo / 'benchmarks/research/axis_norm_probe.cpp'
shutil.copy2(probe, out / probe.name)
flags = ['-std=c++20', '-O3', '-ffp-contract=off', '-fno-fast-math', '-DNDEBUG', '-DWJR_DEBUG_LEVEL=0',
         '-DMDSPAN_USE_PAREN_OPERATOR=1', '-DMDSPAN_USE_BRACKET_OPERATOR=0']
(out / 'metadata.json').write_text(json.dumps(dict(before=before, after=after, mdspan=pin, flags=flags,
    probe_sha256=hashlib.sha256(probe.read_bytes()).hexdigest(), timing='not measured'), indent=2))

def parse(path):
    text = path.read_text()
    constants, result = {}, {}
    for m in re.finditer(r'^(\.L[^:\n]+):[^\n]*\n(.*?)(?=^\S[^\n]*:|\Z)', text, re.M | re.S):
        values = re.findall(r'^\s*\.(long|quad|byte|zero|short|word|value)\s+([^#\n]+)', m[2], re.M)
        if values:
            constants[m[1]] = 'C' + hashlib.sha256(repr(values).encode()).hexdigest()[:16]
    for name in re.findall(r'^\s*\.type\s+([^,\s]+),\s*@function', text, re.M):
        m = re.search(r'^' + re.escape(name) + r':[^\n]*\n(.*?)^\s*\.size\s+' + re.escape(name) + r'\b', text, re.M | re.S)
        if not m:
            alias = re.search(r'^\s*\.set\s+' + re.escape(name) + r',\s*(\S+)', text, re.M)
            assert alias, name
            result[name] = ['ALIAS ' + alias[1]]
            continue
        blocks = {b: 'B' + str(i) for i, b in enumerate(re.findall(r'^(\.L[^:\n]+):', m[1], re.M))}
        body = []
        for line in m[1].splitlines():
            line = line.split('#', 1)[0].strip()
            if line.endswith(':') and line[:-1] in blocks:
                body.append(blocks[line[:-1]] + ':')
                continue
            if not line or line.startswith('.') or line.endswith(':'):
                continue
            line = re.sub(r'\.L[A-Za-z0-9_.$]+', lambda x: blocks.get(x[0], constants.get(x[0], x[0])), line)
            body.append(re.sub(r'\s+', ' ', line))
        result[name] = body
    return result

def compare(old, new, left_root, right_root):
    queue = [(left_root, right_root)]
    mapping, visited, differences = {left_root: right_root}, set(), []
    while queue:
        a, b = queue.pop(0)
        if (a, b) in visited:
            continue
        visited.add((a, b))
        left, right = old[a], new[b]
        if len(left) != len(right):
            differences.append(dict(before=a, after=b, reason='sequence length', lengths=[len(left),len(right)]))
            continue
        for index, (x, y) in enumerate(zip(left, right)):
            c = re.fullmatch(r'(call|j[a-z]+|ALIAS) (\S+?)(@PLT)?', x)
            d = re.fullmatch(r'(call|j[a-z]+|ALIAS) (\S+?)(@PLT)?', y)
            if c and d and c[2] in old and d[2] in new:
                if c[1] != d[1] or c[3] != d[3] or (c[2] in mapping and mapping[c[2]] != d[2]):
                    differences.append(dict(before=a,after=b,index=index,left=x,right=y))
                    continue
                mapping[c[2]]=d[2]
                queue.append((c[2], d[2]))
            elif x != y:
                differences.append(dict(before=a,after=b,index=index,left=x,right=y))
    return dict(equal=not differences, helpers=list(visited), differences=differences)

summary = []
for name, compiler in [('gcc', 'g++'), ('clang', 'clang++')]:
    run([compiler, '--version'], name + '-version.txt')
    for isa, march in [('baseline','x86-64'), ('native','native')]:
        common = [compiler] + flags + ['-march='+march, '-I'+str(mdspan/'include')]
        run([compiler, '-march='+march, '-###', '-x', 'c++', '-c', '/dev/null'], name+'-'+isa+'-resolved-flags.log')
        modules = {}
        for version in ('before','after'):
            label = name+'-'+isa+'-'+version
            run(common+['-I'+str(out/version/'include'), '-DONLY_FULL_NORM', '-S', '-masm=intel', probe,
                        '-o', out/(label+'.s')], label+'-compile.log')
            modules[version] = parse(out/(label+'.s'))
        old = {r: compare(modules['before'],modules['after'],r,r) for r in ('full_norm','stride_norm')}
        label = name+'-'+isa+'-axis'
        run(common+['-I'+str(out/'after/include'), '-S','-masm=intel',probe,'-o',out/(label+'.s')],label+'-compile.log')
        funcs = parse(out/(label+'.s'))
        (out/(label+'-normalized.json')).write_text(json.dumps(funcs,indent=2))
        pairs = {k: compare(funcs,funcs,'axis_'+k,'hand_'+k) for k in ('rows','cols','float','wide','padded','static')}
        summary.append(dict(compiler=name,isa=isa,unchanged=old,axis_pairs=pairs))
        if isa == 'baseline':
            exe = out/(name+'-oracle')
            run(common+['-I'+str(out/'after/include'),'-DNORM_RUNTIME',probe,'-o',exe],name+'-oracle-build.log')
            run([exe],name+'-oracle.log')
(out/'comparison.json').write_text(json.dumps(summary,indent=2))
assert all(v['equal'] for r in summary for v in r['unchanged'].values())
print('PASS full/strided norm unchanged 8/8 roots incl helpers; axis/API paired assembly retained; both runtime oracles passed')
