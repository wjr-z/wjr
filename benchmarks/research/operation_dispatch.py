from pathlib import Path
import difflib
import hashlib
import io
import json
import re
import shutil
import subprocess
import sys
import zipfile

source = Path(__file__).resolve().parents[2]
output = source / 'build/operation-dispatch'
output.mkdir(parents=True, exist_ok=True)
before = '14ea9a1cbcd2e00b87730abb2a647afbb1cb3689'
after = sys.argv[1]
assert re.fullmatch('[0-9a-f]{40}', after)

def run(args, log):
    p = subprocess.run(list(map(str, args)), cwd=source, capture_output=True)
    (output / log).write_bytes(p.stdout + p.stderr)
    if p.returncode:
        raise SystemExit((p.stdout + p.stderr).decode(errors='replace')[-8000:])
    return p.stdout

for name, pin in [('before', before), ('after', after)]:
    run(['git','fetch','--depth=1','origin',pin],name+'-fetch.log')
    archived = run(['git','archive','--format=zip',pin,'include'],name+'-archive.log')
    # Do not retain binary archive as a textual log.
    (output / (name+'-archive.log')).unlink()
    prefix = output / name
    prefix.mkdir(exist_ok=True)
    with zipfile.ZipFile(io.BytesIO(archived)) as archive:
        for item in archive.infolist():
            assert (prefix / item.filename).resolve().is_relative_to(prefix.resolve())
            assert (item.external_attr >> 16) & 0o170000 != 0o120000
        archive.extractall(prefix)

deps = [('mdspan','https://github.com/kokkos/mdspan.git','0e4eba59ad53451784b3839bdf98b25fa2d64931'),
        ('xsimd','https://github.com/xtensor-stack/xsimd.git','e88a72831858123924f7118f345dfe5d70d95991')]
for name, url, pin in deps:
    path = output / name
    run(['git','init',path],name+'-init.log')
    run(['git','-C',path,'remote','add','origin',url],name+'-remote.log')
    run(['git','-C',path,'fetch','--depth=1','origin',pin],name+'-fetch.log')
    run(['git','-C',path,'checkout','--detach','FETCH_HEAD'],name+'-checkout.log')
    assert run(['git','-C',path,'rev-parse','HEAD'],name+'-head.txt').decode().strip() == pin
run(['lscpu'],'lscpu.txt')
probe = source / 'benchmarks/research/operation_dispatch.cpp'
shutil.copy2(probe,output / 'operation_dispatch.cpp')
flags = ['-std=c++20','-O3','-ffp-contract=off','-fno-fast-math','-DNDEBUG',
         '-DWJR_DEBUG_LEVEL=0','-DMDSPAN_USE_PAREN_OPERATOR=1','-DMDSPAN_USE_BRACKET_OPERATOR=0']
metadata = dict(before=before,after=after,flags=flags,dependencies=deps,
    probe_sha256=hashlib.sha256(probe.read_bytes()).hexdigest(),
    runtime='not run; code generation only')
(output / 'metadata.json').write_text(json.dumps(metadata,indent=2))

def functions(path):
    text = path.read_text()
    labels = {}
    for match in re.finditer(r'^(\.L[^:\n]+):[^\n]*\n(.*?)(?=^\S[^\n]*:|\Z)',text,re.M|re.S):
        values = re.findall(r'^\s*\.(long|quad|byte|zero|short|word|value|float|double)\s+([^#\n]+)',match[2],re.M)
        if values:
            labels[match[1]] = 'C' + hashlib.sha256(repr(values).encode()).hexdigest()[:16]
    result = {}
    names = re.findall(r'^\s*\.type\s+([^,\s]+),\s*@function',text,re.M)
    for name in names:
        match = re.search(r'^'+re.escape(name)+r':[^\n]*\n(.*?)^\s*\.size\s+'+re.escape(name)+r'\b',text,re.M|re.S)
        assert match, name
        body = match[1]
        blocks = {label:'B'+str(i) for i,label in enumerate(re.findall(r'^(\.L[^:\n]+):',body,re.M))}
        normalized = []
        for line in body.splitlines():
            line = line.split('#',1)[0].strip()
            if line.endswith(':') and line[:-1] in blocks:
                normalized.append(blocks[line[:-1]] + ':')
                continue
            if not line or line.startswith('.') or line.endswith(':'):
                continue
            line = re.sub(r'\.L[A-Za-z0-9_.$]+',lambda m:blocks.get(m[0],labels.get(m[0],m[0])),line)
            normalized.append(re.sub(r'\s+',' ',line))
        result[name] = normalized
    return result

summary = []
for label, compiler in [('gcc','g++'),('clang','clang++')]:
    run([compiler,'--version'],label+'-version.txt')
    for isa in ('baseline','native'):
        march = '-march=' + ('x86-64' if isa == 'baseline' else 'native')
        command = [compiler]+flags+[march,'-S','-masm=intel']
        run([compiler,march,'-###','-x','c++','-c','/dev/null'],label+'-'+isa+'-resolved-flags.log')
        compared = {}
        for version in ('before','after'):
            name = label+'-'+isa+'-'+version
            includes = ['-I'+str(output/version/'include')]
            includes += ['-I'+str(output/n/'include') for n,_,_ in deps]
            asm = output / (name+'.s')
            run(command+includes+[probe,'-o',asm],name+'-compile.log')
            compared[version] = functions(asm)
            (output/(name+'-normalized.json')).write_text(json.dumps(compared[version],indent=2))
        assert set(compared['before']) == set(compared['after']), 'Changed function set'
        for function, old in compared['before'].items():
            new = compared['after'][function]
            equal = old == new
            summary.append(dict(compiler=label,isa=isa,function=function,equal=equal,
                                before_instructions=sum(not s.endswith(':') for s in old),
                                after_instructions=sum(not s.endswith(':') for s in new)))
            if not equal:
                diff = '\n'.join(difflib.unified_diff(old,new,fromfile='before',tofile='after'))
                filename = label+'-'+isa+'-diff-'+hashlib.sha256(function.encode()).hexdigest()[:12]+'.txt'
                (output/filename).write_text(function+'\n'+diff)
(output/'comparison.json').write_text(json.dumps(summary,indent=2))
print('Compared',len(summary),'function/profile pairs;',sum(r['equal'] for r in summary),
      'exact normalized instruction sequences; differences retained individually')
