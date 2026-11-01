"""One frozen API adapter against three ordinary resources; no production CI changes."""
from pathlib import Path
import subprocess as sp, os, json, platform, csv, statistics
root=Path(__file__).resolve().parent
out=root/'evidence';out.mkdir(exist_ok=True)
provider=root/'_provider'
cpu_source=root/'_cpu_features'
def run(command,name,expect=0,env=None):
    command=[str(x) for x in command]
    result=sp.run(command,cwd=root,stdout=sp.PIPE,stderr=sp.STDOUT,env=env)
    (out/(name+'.log')).write_bytes(result.stdout)
    if result.returncode!=expect:
        print(result.stdout.decode(errors='replace')[-6500:]);raise RuntimeError((name,result.returncode,command))
    return result
report={'platform':platform.platform(),'cpu':Path('/proc/cpuinfo').read_text().split('model name')[1].split('\n')[0],'source':'6261a5641f6feee519f6508197e7beac5f9b7e15','provider':'56dc5eb80145f9a3ba7ca23d47c6a248b9d8586c','profiles':[]}
run(['git','clone','--depth','1','--branch','v0.10.1','https://github.com/google/cpu_features.git',cpu_source],'cpu-source')
for name,compiler,ccompiler,asan in [('clang18','clang++-18','clang-18',False),('gcc13','g++-13','gcc-13',False),('clang18-asan','clang++-18','clang-18',True)]:
    prefix=root/('_mi-'+('asan' if asan else 'release'))
    mi_build=root/('_mi-build-'+('asan' if asan else 'release'))
    if name!='gcc13':
        command=['cmake','-S',provider,'-B',mi_build,'-GNinja','-DCMAKE_BUILD_TYPE=Release','-DCMAKE_C_COMPILER=clang-18','-DCMAKE_CXX_COMPILER=clang++-18','-DCMAKE_INSTALL_PREFIX='+str(prefix),'-DMI_BUILD_SHARED=ON','-DMI_BUILD_STATIC=OFF','-DMI_BUILD_OBJECT=OFF','-DMI_BUILD_TESTS=OFF','-DMI_OVERRIDE=OFF','-DMI_OPT_ARCH=OFF','-DMI_USE_CXX=ON','-DMI_STATS=OFF','-DMI_PROFILE=OFF','-DMI_SECURE=OFF','-DMI_GUARDED=OFF']
        if asan:command+=['-DMI_TRACK=ASAN','-DCMAKE_CXX_FLAGS=-fsanitize=address -fno-omit-frame-pointer']
        run(command,name+'-provider-configure')
        run(['cmake','--build',mi_build,'--parallel','2'],name+'-provider-build')
        run(['cmake','--install',mi_build],name+'-provider-install')
    harness=root/('_harness-'+name);harness.mkdir(exist_ok=True)
    harness.joinpath('CMakeLists.txt').write_text('''cmake_minimum_required(VERSION 3.20)
project(mimalloc_research LANGUAGES CXX)
enable_testing()
set(WJR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_ASSEMBLY OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_MIMALLOC ON CACHE BOOL "" FORCE)
set(WJR_CPU_TARGET baseline CACHE STRING "" FORCE)
add_subdirectory("${WJR_SOURCE}" wjr)
add_subdirectory("${WJR_SOURCE}/test/consumer/mimalloc" consumer)
add_executable(mimalloc_study "${WJR_SOURCE}/mimalloc_study.cpp")
target_link_libraries(mimalloc_study PRIVATE wjr::json wjr::mimalloc)
''')
    build=root/('_client-'+name)
    command=['cmake','-S',harness,'-B',build,'-GNinja','-DCMAKE_BUILD_TYPE=Release','-DCMAKE_C_COMPILER='+ccompiler,'-DCMAKE_CXX_COMPILER='+compiler,'-DWJR_SOURCE='+str(root),'-DFETCHCONTENT_SOURCE_DIR_CPU_FEATURES='+str(cpu_source),'-Dmimalloc_DIR='+str(prefix/'lib/cmake/mimalloc-3.6'),'-DCMAKE_EXPORT_COMPILE_COMMANDS=ON']
    if asan:command+=['-DWJR_ENABLE_ASAN=ON','-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer']
    run(command,name+'-configure')
    run(['cmake','--build',build,'--target','wjr_mimalloc_consumer']+([] if asan else ['mimalloc_study'])+['--parallel','2'],name+'-build')
    run(['ctest','--test-dir',build,'--output-on-failure'],name+'-consumer')
    item={'name':name,'compiler':sp.check_output([compiler,'--version'],text=True).splitlines()[0],'consumer_passed':True,'provider_tracking':'ASAN' if asan else 'OFF','compile_commands':json.loads((build/'compile_commands.json').read_text())}
    if not asan:
        cpu=str(min(os.sched_getaffinity(0)))
        result=run(['taskset','-c',cpu,build/'mimalloc_study',root/'test/data/success/twitter.json'],name+'-timings')
        rows=list(csv.DictReader(result.stdout.decode().splitlines()))
        assert len(rows)==63,len(rows)
        (out/(name+'-timings.csv')).write_bytes(result.stdout)
        item['affinity_cpu']=cpu;item['rows']=rows
        item['medians']={case:{provider:statistics.median(float(x['ns_per_document']) for x in rows if x['case']==case and x['resource']==provider) for provider in ['std_new_delete','mimalloc','std_unsynchronized_pool']} for case in ['small','wide','twitter']}
    report['profiles'].append(item)
(out/'result.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps([{k:v for k,v in x.items() if k not in ['rows','compile_commands']} for x in report['profiles']],indent=2))
