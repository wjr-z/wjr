from pathlib import Path
import hashlib, io, json, re, shutil, subprocess, zipfile

repo=Path(__file__).resolve().parents[2]
out=repo/'build/pairwise-research';out.mkdir(parents=True,exist_ok=True)
before='0565e732c8ff35fca6f25645fe18d20e8573a7cb'
after='bc1967d82b517985e0ed11f272adb843b05ed6a2'
pin='0e4eba59ad53451784b3839bdf98b25fa2d64931'
def run(args,log):
 p=subprocess.run(list(map(str,args)),cwd=repo,capture_output=True)
 (out/log).write_bytes(p.stdout+p.stderr)
 if p.returncode:raise SystemExit((p.stdout+p.stderr).decode(errors='replace')[-8000:])
 return p.stdout
for name,sha in [('before',before),('after',after)]:
 run(['git','fetch','--depth=1','origin',sha],name+'-fetch.log')
 data=subprocess.check_output(['git','archive','--format=zip',sha,'include'],cwd=repo)
 folder=out/name;folder.mkdir(exist_ok=True)
 with zipfile.ZipFile(io.BytesIO(data)) as archive:
  for f in archive.infolist():
   assert (folder/f.filename).resolve().is_relative_to(folder.resolve())
   assert (f.external_attr>>16)&0o170000!=0o120000
  archive.extractall(folder)
mdspan=out/'mdspan'
run(['git','init',mdspan],'mdspan-init.log')
run(['git','-C',mdspan,'remote','add','origin','https://github.com/kokkos/mdspan.git'],'mdspan-remote.log')
run(['git','-C',mdspan,'fetch','--depth=1','origin',pin],'mdspan-fetch.log')
run(['git','-C',mdspan,'checkout','--detach','FETCH_HEAD'],'mdspan-checkout.log')
run(['lscpu'],'lscpu.txt')
probe=repo/'benchmarks/research/pairwise_probe.cpp';shutil.copy2(probe,out/'pairwise_probe.cpp')
flags=['-std=c++20','-O3','-ffp-contract=off','-fno-fast-math','-DNDEBUG','-DWJR_DEBUG_LEVEL=0',
 '-DMDSPAN_USE_PAREN_OPERATOR=1','-DMDSPAN_USE_BRACKET_OPERATOR=0']
(out/'metadata.json').write_text(json.dumps(dict(before=before,after=after,mdspan=pin,flags=flags,
 probe_sha256=hashlib.sha256(probe.read_bytes()).hexdigest(),timing='not measured'),indent=2))

def parse(path):
 text=path.read_text();constants={};result={}
 for m in re.finditer(r'^(\.L[^:\n]+):[^\n]*\n(.*?)(?=^\S[^\n]*:|\Z)',text,re.M|re.S):
  values=re.findall(r'^\s*\.(long|quad|byte|zero|short|word|value)\s+([^#\n]+)',m[2],re.M)
  if values:constants[m[1]]='C'+hashlib.sha256(repr(values).encode()).hexdigest()[:16]
 for name in re.findall(r'^\s*\.type\s+([^,\s]+),\s*@function',text,re.M):
  m=re.search(r'^'+re.escape(name)+r':[^\n]*\n(.*?)^\s*\.size\s+'+re.escape(name)+r'\b',text,re.M|re.S)
  if not m:
   alias=re.search(r'^\s*\.set\s+'+re.escape(name)+r',\s*(\S+)',text,re.M)
   assert alias,name
   result[name]=['ALIAS '+alias[1]]
   continue
  blocks={b:'B'+str(i) for i,b in enumerate(re.findall(r'^(\.L[^:\n]+):',m[1],re.M))};body=[]
  for line in m[1].splitlines():
   line=line.split('#',1)[0].strip()
   if line.endswith(':') and line[:-1] in blocks:body.append(blocks[line[:-1]]+':');continue
   if not line or line.startswith('.') or line.endswith(':'):continue
   line=re.sub(r'\.L[A-Za-z0-9_.$]+',lambda x:blocks.get(x[0],constants.get(x[0],x[0])),line)
   body.append(re.sub(r'\s+',' ',line))
  result[name]=body
 return result

def graph_compare(old,new,root):
 # Template Input spelling changes private symbols. Pair targets and verify
 # their complete bodies recursively; external calls must keep exact identity.
 queue=[(root,root)];mapping={root:root};visited=set();differences=[]
 while queue:
  a,b=queue.pop(0)
  if (a,b) in visited:continue
  visited.add((a,b));left=old[a];right=new[b]
  if len(left)!=len(right):differences.append(dict(before=a,after=b,reason='sequence length'));continue
  for index,(x,y) in enumerate(zip(left,right)):
   c=re.fullmatch(r'(call|jmp|ALIAS) (\S+?)(@PLT)?',x);d=re.fullmatch(r'(call|jmp|ALIAS) (\S+?)(@PLT)?',y)
   if c and d and c[2] in old and d[2] in new:
    if c[1]!=d[1] or c[3]!=d[3] or (c[2] in mapping and mapping[c[2]]!=d[2]):
     differences.append(dict(before=a,after=b,index=index,left=x,right=y));continue
    mapping[c[2]]=d[2];queue.append((c[2],d[2]))
   elif x!=y:differences.append(dict(before=a,after=b,index=index,left=x,right=y))
 return dict(equal=not differences,helpers=list(visited),differences=differences)

summary=[]
for name,compiler in [('gcc','g++'),('clang','clang++')]:
 run([compiler,'--version'],name+'-version.txt')
 for isa,march in [('baseline','x86-64'),('native','native')]:
  common=[compiler]+flags+['-march='+march,'-I'+str(mdspan/'include')]
  run([compiler,'-march='+march,'-###','-x','c++','-c','/dev/null'],name+'-'+isa+'-resolved-flags.log')
  modules={}
  for version in ('before','after'):
   label=name+'-'+isa+'-'+version
   run(common+['-I'+str(out/version/'include'),'-S','-masm=intel',probe,'-o',out/(label+'.s')],label+'-compile.log')
   modules[version]=parse(out/(label+'.s'))
   (out/(label+'-normalized.json')).write_text(json.dumps(modules[version],indent=2))
  roots=[r for r in modules['before'] if r.startswith('view_')]
  assert len(roots)==10
  compared={r:graph_compare(modules['before'],modules['after'],r) for r in roots}
  summary.append(dict(compiler=name,isa=isa,views=compared))
  label=name+'-'+isa+'-fusion'
  run(common+['-I'+str(out/'after/include'),'-DPAIRWISE_EXPRESSIONS','-S','-masm=intel',probe,'-o',out/(label+'.s')],label+'-compile.log')
  functions=parse(out/(label+'.s'))
  (out/(label+'-normalized.json')).write_text(json.dumps(functions,indent=2))
  if isa=='baseline':
   executable=out/(name+'-oracle')
   run(common+['-I'+str(out/'after/include'),'-DPAIRWISE_EXPRESSIONS','-DPAIRWISE_RUNTIME',probe,'-o',executable],name+'-oracle-build.log')
   run([executable],name+'-oracle.log')
(out/'view-comparison.json').write_text(json.dumps(summary,indent=2))
print('View roots unchanged:',sum(v['equal'] for r in summary for v in r['views'].values()),'/40; each referenced recursive helper compared too')
print('PASS GCC/Clang actual fused/hand numerical oracles; baseline/native fusion assembly retained, no throughput measured')
