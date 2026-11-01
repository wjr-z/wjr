"""Archived two-module lifetime regression and same-machine performance controls."""
from pathlib import Path
import subprocess as sp, json, os, platform, statistics
root=Path(__file__).resolve().parent
out=root/'research-output'; out.mkdir(exist_ok=True)
old=out/'old'; (old/'wjr/memory').mkdir(parents=True,exist_ok=True)
base='e961ba6d'
def snapshot(path): return sp.check_output(['git','show',base+':'+path],cwd=root)
(old/'wjr/memory/stack_allocator.hpp').write_bytes(snapshot('include/wjr/memory/stack_allocator.hpp'))
original=snapshot('src/memory/stack_allocator.cpp')
(old/'host.cpp').write_bytes(original)
(old/'foreign_memory.cpp').write_bytes(original+b'\nnamespace wjr { extern "C" __attribute__((visibility("default"))) void wjr_foreign_clear_pool() noexcept { _stack_free_pool.clear(); } }\n')
foreign=(root/'test/consumer/stack_domain/foreign.cpp').read_text()
foreign=foreign[:foreign.index('extern "C" void wjr_foreign_clear_pool()')].replace('auto local = owner.make_scope();','wjr::unique_stack_allocator local;')
(old/'foreign.cpp').write_text(foreign)
bench=out/'bench.cpp'
bench.write_text(r'''
#include <wjr/memory/stack_allocator.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
__attribute__((noinline)) void run_round(size_t count, size_t bytes) {
    wjr::unique_stack_allocator owner;
    for (size_t i=0;i<count;++i)
        *static_cast<volatile unsigned char*>(owner.allocate(bytes))=static_cast<unsigned char>(i);
}
int main(int, char** argv) {
    size_t count=std::strtoull(argv[1],nullptr,10), bytes=std::strtoull(argv[2],nullptr,10);
    size_t rounds=std::strtoull(argv[3],nullptr,10);
    for(size_t i=0;i<2000;++i) run_round(count,bytes);
    auto begin=std::chrono::steady_clock::now();
    for(size_t i=0;i<rounds;++i) run_round(count,bytes);
    double ns=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-begin).count()/rounds;
    std::printf("%.9f\n",ns);
    std::fprintf(stderr,"arena=%zu owner=%zu\n",sizeof(wjr::stack_allocator_object),sizeof(wjr::unique_stack_allocator));
}
''')
report={'platform':platform.platform(),'base':base,'candidate':'e7c32ece99f375e8e2d558cc66c7e7b956d8f640','cpu':Path('/proc/cpuinfo').read_text().split('model name')[1].split('\n')[0],'compilers':{},'checks':[],'timings':[]}
def run(cmd,name,expect=0,env=None):
    result=sp.run([str(x) for x in cmd],cwd=root,stdout=sp.PIPE,stderr=sp.STDOUT,env=env)
    (out/(name+'.log')).write_bytes(result.stdout)
    if expect is not None and result.returncode!=expect:
        print(result.stdout.decode(errors='replace')[-6000:]); raise RuntimeError((name,result.returncode))
    return result
for cc in ['clang++-18','g++-13']:
    report['compilers'][cc]=sp.check_output([cc,'--version'],text=True).splitlines()[0]
    for asan in ([False,True] if cc.startswith('clang') else [False]):
        tag=cc+('-asan' if asan else '-release')
        for version in ['old','new']:
            build=out/(tag+'-'+version); build.mkdir(exist_ok=True)
            flags=[cc,'-std=c++20','-O2','-g','-DNDEBUG','-DWJR_DEBUG_LEVEL_RELEASE=0','-DFMT_HEADER_ONLY','-fPIC','-fvisibility=hidden','-fvisibility-inlines-hidden']
            if asan: flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer']
            if version=='old': flags+=['-I'+str(old)]
            flags+=['-I'+str(root/'include'),'-I'+str(root/'third-party/fmt/include'),'-I'+str(root/'test/consumer/stack_domain')]
            mem=old/'foreign_memory.cpp' if version=='old' else root/'src/memory/stack_allocator.cpp'
            bridge=old/'foreign.cpp' if version=='old' else root/'test/consumer/stack_domain/foreign.cpp'
            assertion=root/'src/utility/assert.cpp'
            run(flags+['-shared','-DWJR_STACK_DOMAIN_EXPORTS=1',bridge,mem,assertion,'-o',build/'libforeign.so'],tag+'-'+version+'-dll-build')
            host_mem=old/'host.cpp' if version=='old' else mem
            run(flags+[root/'test/consumer/stack_domain/main.cpp',host_mem,assertion,'-L'+str(build),'-lforeign','-Wl,-rpath,$ORIGIN','-o',build/'domain'],tag+'-'+version+'-host-build')
            if version=='new':
                result=run([build/'domain'],tag+'-domain'); report['checks'].append({'name':tag+'-domain','result':result.returncode})
                if asan:
                    result=run([build/'domain','--pool-only'],tag+'-domain-heap-shadow',env=dict(os.environ,ASAN_OPTIONS='allow_user_poisoning=0:detect_leaks=1'))
                    report['checks'].append({'name':tag+'-domain-heap-shadow','result':result.returncode})
            elif asan:
                result=run([build/'domain','--pool-only'],tag+'-old-uaf',None,dict(os.environ,ASAN_OPTIONS='allow_user_poisoning=0:detect_leaks=0:halt_on_error=1'))
                assert result.returncode!=0 and b'heap-use-after-free' in result.stdout
                report['checks'].append({'name':tag+'-old-uaf','result':result.returncode,'heap_use_after_free':True})
            if asan:
                thread_probe=out/'thread_probe.cpp'
                thread_probe.write_text('#include <wjr/memory/stack_allocator.hpp>\n#include <thread>\nint main() {std::thread([] {wjr::unique_stack_allocator owner; *static_cast<volatile unsigned char*>(owner.allocate(1024))=1;}).join();}\n')
                run(flags+[thread_probe,host_mem,assertion,'-pthread','-o',build/'thread_probe'],tag+'-'+version+'-thread-build')
                result=run([build/'thread_probe'],tag+'-'+version+'-thread-leaks',None,dict(os.environ,ASAN_OPTIONS='detect_leaks=1'))
                assert (result.returncode!=0 and b'LeakSanitizer: detected memory leaks' in result.stdout) if version=='old' else result.returncode==0
                report['checks'].append({'name':tag+'-'+version+'-thread-leaks','result':result.returncode,'expected':1 if version=='old' else 0})
            if not asan:
                run(flags+[bench,host_mem,assertion,'-o',build/'bench'],tag+'-'+version+'-bench-build')
                (build/'flags.json').write_text(json.dumps(flags))
        if not asan:
            cpu=str(min(os.sched_getaffinity(0)))
            for case,n,b,rounds in [('bump',32,64,1000000),('cached_refill',40,1024,250000),('grow_trim',80,1024,100000),('large',1,32768,200000)]:
                samples={'old':[],'new':[]}
                for sample in range(7):
                    for version in (['old','new'] if sample%2==0 else ['new','old']):
                        result=run(['taskset','-c',cpu,out/(tag+'-'+version)/'bench',n,b,rounds],tag+f'-{case}-{sample}-{version}')
                        samples[version].append(float(next(line for line in result.stdout.decode().splitlines() if line[:1].isdigit())))
                medians={k:statistics.median(v) for k,v in samples.items()}
                report['timings'].append({'compiler':cc,'case':case,'samples_ns':samples,'median_ns':medians,'new_over_old':medians['new']/medians['old'],'cpu':cpu})
(out/'result.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
