from pathlib import Path
import subprocess as sp, json, os, time, sys, collections, platform, statistics

root=Path.cwd()
out=root/'research-output';out.mkdir(exist_ok=True)
compiler=sys.argv[1]
build=root/'build'
def run(cmd,name):
    result=sp.run([str(x) for x in cmd],stdout=sp.PIPE,stderr=sp.STDOUT,cwd=root)
    (out/name).write_bytes(result.stdout)
    if result.returncode:
        print(result.stdout.decode(errors='replace')[-6000:]);raise RuntimeError((name,result.returncode))
    return result
run(['cmake','-S',root,'-B',build,'-GNinja','-DCMAKE_BUILD_TYPE=Release','-DWJR_BUILD_TESTS=ON','-DWJR_DEBUG_LEVEL_RELEASE=3','-DWJR_ENABLE_ASAN=ON','-DWJR_USE_GMP_TESTS=ON','-DWJR_CPU_TARGET=native',f'-DCMAKE_CXX_COMPILER={compiler}',f'-DFETCHCONTENT_BASE_DIR={os.environ["RUNNER_TEMP"]}/wjr-fetchcontent',f'-DCMAKE_C_COMPILER_LAUNCHER=python3;{root}/.github/scripts/ccache_launcher.py',f'-DCMAKE_CXX_COMPILER_LAUNCHER=python3;{root}/.github/scripts/ccache_launcher.py'],'configure.log')
run(['cmake','--build',build,'--target','tests','--parallel','2'],'build.log')
env=dict(os.environ,ASAN_OPTIONS='detect_leaks=1',LSAN_OPTIONS='suppressions='+str(root/'.github/lsan_suppressions.txt'),OPENBLAS_NUM_THREADS='1')
for key in ['GTEST_TOTAL_SHARDS','GTEST_SHARD_INDEX','GTEST_SHARD_STATUS_FILE','GTEST_FILTER','GTEST_OUTPUT']:
    env.pop(key,None)
cases=None;results=[]
for iteration,count in enumerate([1,2,2,1,1,2]):
    processes=[];files=[];paths=[];statuses=[];peak=0
    start=time.perf_counter()
    for index in range(count):
        path=out/f'run-{iteration}-n{count}-s{index}.json';paths.append(path)
        status=out/f'run-{iteration}-n{count}-s{index}.status';statuses.append(status)
        child_env=dict(env)
        if count>1:child_env.update(GTEST_TOTAL_SHARDS=str(count),GTEST_SHARD_INDEX=str(index),GTEST_SHARD_STATUS_FILE=str(status))
        stream=path.with_suffix('.log').open('wb');files.append(stream)
        processes.append(sp.Popen([str(build/'bin/tests'),'--gtest_color=no','--gtest_output=json:'+str(path)],cwd=build/'test',env=child_env,stdout=stream,stderr=sp.STDOUT))
    while True:
        total_rss=0
        for process in processes:
            try:
                status=Path(f'/proc/{process.pid}/status').read_text()
                total_rss+=int(next(line.split()[1] for line in status.splitlines() if line.startswith('VmRSS:')))*1024
            except (FileNotFoundError,ProcessLookupError,StopIteration):pass
        peak=max(peak,total_rss)
        if all(process.poll() is not None for process in processes):break
        time.sleep(.01)
    wall=time.perf_counter()-start
    for stream in files:stream.close()
    assert all(p.returncode==0 for p in processes),[p.returncode for p in processes]
    if count>1:assert all(p.exists() for p in statuses)
    selected=[];per_shard=[]
    for path in paths:
        data=json.loads(path.read_text())
        assert data['failures']==0
        names=[suite['name']+'.'+test['name'] for suite in data['testsuites'] for test in suite['testsuite'] if test['status']=='RUN']
        selected.extend(names);per_shard.append(len(names))
    assert all(n==1 for n in collections.Counter(selected).values())
    if cases is None:cases=set(selected)
    else:assert cases==set(selected)
    row={'iteration':iteration,'shards':count,'wall_seconds':wall,'sampled_aggregate_peak_rss_bytes':peak,'executed_cases':len(selected),'per_shard_cases':per_shard}
    results.append(row);print(json.dumps(row),flush=True)
report={'source':sp.check_output(['git','rev-parse','HEAD'],text=True).strip(),'compiler':sp.check_output([compiler,'--version'],text=True).splitlines()[0],'cpu':next(line for line in Path('/proc/cpuinfo').read_text().splitlines() if line.startswith('model name')),'platform':platform.platform(),'profile':'Release C++20 native WJR_DEBUG_LEVEL_RELEASE=3 ASan+GMP, same compiled executable','sample_interval_seconds':.01,'results':results,'summary':{}}
for count in [1,2]:
    rows=[r for r in results if r['shards']==count]
    report['summary'][count]={'median_wall_seconds':statistics.median(r['wall_seconds'] for r in rows),'min_wall_seconds':min(r['wall_seconds'] for r in rows),'max_wall_seconds':max(r['wall_seconds'] for r in rows),'max_sampled_aggregate_rss_bytes':max(r['sampled_aggregate_peak_rss_bytes'] for r in rows),'all_cases':len(cases)}
(out/'results.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report['summary'],indent=2))
