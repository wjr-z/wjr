from pathlib import Path
import subprocess as sp, json, os, platform

root=Path(__file__).resolve().parents[2]
source=Path(__file__).resolve().parent
out=root/'research-output'; out.mkdir(exist_ok=True)
cpu=min(os.sched_getaffinity(0)); os.sched_setaffinity(0,{cpu})
report={'platform':platform.platform(),'affinity_cpu':cpu,'compilers':{},'candidate':'81abb6aaf287a597cecf8b911dda559f7b06e49f'}
report['cpu']=next(x for x in Path('/proc/cpuinfo').read_text().splitlines() if x.startswith('model name'))
def run(cmd,name,expect=0):
    result=sp.run([str(x) for x in cmd],stdout=sp.PIPE,stderr=sp.STDOUT,cwd=root)
    (out/name).write_bytes(result.stdout)
    if result.returncode!=expect:
        print(result.stdout.decode(errors='replace')[-6000:]); raise RuntimeError((name,result.returncode))
    return result

for compiler in ['clang++-18','g++-13']:
    tag=compiler.replace('+','p')
    report['compilers'][compiler]=sp.check_output([compiler,'--version'],text=True).splitlines()[0]
    build=root/'research-build'/tag
    run(['cmake','-S',source,'-B',build,'-GNinja','-DCMAKE_BUILD_TYPE=Release',f'-DCMAKE_CXX_COMPILER={compiler}',f'-DWJR_SOURCE={root}','-DWJR_BUILD_TESTS=ON','-DWJR_USE_GMP_TESTS=OFF','-DWJR_CPU_TARGET=baseline','-DWJR_ENABLE_ASSEMBLY=OFF'],tag+'-configure.log')
    if compiler.startswith('clang'):
        path=root/'src/biginteger/detail/convert.cpp'; candidate=path.read_bytes()
        assert b'rest <= 0' in candidate
        try:
            path.write_bytes(candidate.replace(b'rest <= 0',b'rest == 0'))
            run(['cmake','--build',build,'--target','prefix_probe','--parallel','2'],tag+'-old-build.log')
            run([build/'prefix_probe'],tag+'-old-prefix.log',expect=1)
        finally: path.write_bytes(candidate)
    run(['cmake','--build',build,'--target','prefix_probe','scratch_probe','timing_probe','output_tests','--parallel','2'],tag+'-build.log')
    for binary in ['prefix_probe','scratch_probe','timing_probe','output_tests']:
        run([build/binary],tag+'-'+binary+'.log')
    if compiler.startswith('clang'):
        asan=root/'research-build/asan'
        run(['cmake','-S',source,'-B',asan,'-GNinja','-DCMAKE_BUILD_TYPE=Release',f'-DCMAKE_CXX_COMPILER={compiler}',f'-DWJR_SOURCE={root}','-DWJR_BUILD_TESTS=ON','-DWJR_USE_GMP_TESTS=OFF','-DWJR_CPU_TARGET=baseline','-DWJR_ENABLE_ASSEMBLY=OFF','-DWJR_ENABLE_ASAN=ON','-DCMAKE_CXX_FLAGS=-fsanitize=undefined -fno-sanitize-recover=all'],tag+'-asan-configure.log')
        run(['cmake','--build',asan,'--target','output_tests','--parallel','2'],tag+'-asan-build.log')
        run([asan/'output_tests'],tag+'-asan-tests.log')
(out/'metadata.json').write_text(json.dumps(report,indent=2))
