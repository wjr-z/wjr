from pathlib import Path
import subprocess as sp,json,os,sys,shlex,re,resource

root=Path.cwd();source=root/'research/checked-parse'
out=root/'research-output';out.mkdir(exist_ok=True)
compiler=sys.argv[1]
resource.setrlimit(resource.RLIMIT_CORE,(0,0))
def run(command,name,check=True,cwd=root,env=None):
    result=sp.run([str(x) for x in command],cwd=cwd,capture_output=True,env=env)
    (out/name).write_bytes(result.stdout+result.stderr)
    if check and result.returncode:
        print(result.stdout.decode(errors='replace')[-5000:]);print(result.stderr.decode(errors='replace')[-5000:]);raise RuntimeError((name,result.returncode))
    return result
cc=compiler.replace('clang++','clang').replace('g++','gcc')
common=['-GNinja','-DCMAKE_BUILD_TYPE=Release',f'-DCMAKE_CXX_COMPILER={compiler}',f'-DCMAKE_C_COMPILER={cc}',f'-DWJR_SOURCE={root}','-DWJR_CXX_STANDARD=20','-DWJR_CPU_TARGET=baseline','-DWJR_BUILD_TESTS=ON','-DWJR_USE_GMP_TESTS=OFF','-DWJR_ENABLE_ASSEMBLY=OFF']
release=root/'research-build/release'
run(['cmake','-S',source,'-B',release,*common],'release-configure.log')
run(['cmake','--build',release,'--target','range_tests','parse_perf','--parallel','2'],'release-build.log')
run([release/'range_tests'],'release-tests.log',cwd=release/'wjr/test')
allowed=os.sched_getaffinity(0);cpu=min(allowed)
os.sched_setaffinity(0,{cpu})
run([release/'parse_perf'],'performance.csv',cwd=release/'wjr/test')
os.sched_setaffinity(0,allowed)
object=Path('wjr/src/biginteger/CMakeFiles/wjr-biginteger.dir/biginteger.cpp.o')
command=shlex.split(sp.check_output(['ninja','-C',str(release),'-t','commands',str(object)],text=True).splitlines()[-1])
old_source=out/'old-biginteger.cpp'
old_source.write_bytes(sp.check_output(['git','show','84132724:src/biginteger/biginteger.cpp']))
old_object=out/'old-biginteger.o'
command[command.index('-o')+1]=str(old_object)
command[command.index('-c')+1]=str(old_source)
if '-MF' in command:command[command.index('-MF')+1]=str(out/'old-biginteger.d')
run(command,'old-release-build.log',cwd=release)
code={};sizes={}
for name,path in [('old',old_object),('new',release/object)]:
    symbols=sp.check_output(['nm','-S','--defined-only',str(path)],text=True)
    line=next(line for line in symbols.splitlines() if '_from_chars_implILb0EE' in line)
    fields=line.split();symbol=fields[-1];sizes[name]=int(fields[1],16)
    asm=sp.check_output(['objdump','-d','--disassemble='+symbol,str(path)],text=True)
    (out/('unchecked-'+name+'.s')).write_text(asm)
    code[name]=bytes(int(byte,16) for line in asm.splitlines() if re.match(r'^\s*[0-9a-f]+:',line) for byte in re.match(r'^\s*[0-9a-f]+:\s*((?:[0-9a-f]{2}\s)+)',line).group(1).split())
metadata={'compiler':sp.check_output([compiler,'--version'],text=True).splitlines()[0],'candidate':'b823dfd2b07b67fc8708f66f4a60c615cd3c7ecd','source':sp.check_output(['git','rev-parse','HEAD'],text=True).strip(),'cpu':next(line for line in Path('/proc/cpuinfo').read_text().splitlines() if line.startswith('model name')),'affinity_cpu':cpu,'unchecked_code_bytes':sizes,'unchecked_machine_code_identical':code['old']==code['new'],'profile':'Release C++20 baseline debug0, same process old/current valid null-terminated strings, separate TUs and matching attributes'}
assert metadata['unchecked_machine_code_identical']
if 'clang' in compiler:
    asan=root/'research-build/asan'
    run(['cmake','-S',source,'-B',asan,*common,'-DWJR_ENABLE_ASAN=ON','-DWJR_DEBUG_LEVEL_RELEASE=3'],'asan-configure.log')
    run(['cmake','--build',asan,'--target','range_tests','old_guard','--parallel','2'],'asan-build.log')
    env=dict(os.environ,ASAN_OPTIONS='detect_leaks=1',LSAN_OPTIONS='suppressions='+str(root/'.github/lsan_suppressions.txt'))
    run([asan/'range_tests'],'asan-tests.log',cwd=asan/'wjr/test',env=env)
    old=run([asan/'old_guard'],'old-asan-guard.log',check=False,cwd=asan/'wjr/test',env=env)
    assert old.returncode!=0
    assert b'AddressSanitizer: heap-buffer-overflow' in old.stderr
    metadata['old_guard_asan_overread_reproduced']=True
(out/'metadata.json').write_text(json.dumps(metadata,indent=2))
print(json.dumps(metadata,indent=2))
