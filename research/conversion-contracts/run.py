from pathlib import Path
import subprocess as sp,json,os,resource,platform

root=Path.cwd()
out=root/'research-output';out.mkdir(exist_ok=True)
source=root/'research/conversion-contracts'
build=root/'research-build'
resource.setrlimit(resource.RLIMIT_CORE,(0,0))
old=out/'old_biginteger.cpp'
old.write_bytes(sp.check_output(['git','show','84132724:benchmarks/src/biginteger.cpp']))
def run(command,name,expect=True):
    result=sp.run([str(x) for x in command],cwd=root,capture_output=True)
    (out/name).write_bytes(result.stdout+result.stderr)
    if expect and result.returncode:
        print(result.stdout.decode(errors='replace')[-5000:]);print(result.stderr.decode(errors='replace')[-5000:]);raise RuntimeError((name,result.returncode))
    return result
run(['cmake','-S',source,'-B',build,'-GNinja','-DCMAKE_BUILD_TYPE=Release','-DCMAKE_CXX_COMPILER=g++-13','-DCMAKE_C_COMPILER=gcc-13',f'-DWJR_SOURCE={root}',f'-DOLD_SOURCE={old}','-DWJR_CXX_STANDARD=20','-DWJR_CPU_TARGET=baseline','-DWJR_ENABLE_ASSEMBLY=OFF','-DWJR_BUILD_BENCHMARKS=ON','-DWJR_BENCHMARK_PROFILE=all','-DWJR_USE_GMP_TESTS=ON',f'-DFETCHCONTENT_SOURCE_DIR_BENCHMARK={os.environ["RUNNER_TEMP"]}/benchmark-src'],'configure.log')
run(['cmake','--build',build,'--target','benchmarks','contract_bench','old_contract_bench','--parallel','2'],'build.log')
actual,contract,old_contract=(Path(line) for line in (build/'paths.txt').read_text().splitlines())
select='(wjr|gmp)_biginteger_(to|from)_digits/(2|8|10|16)/(1|2|64|1024)$'
for name,executable in [('actual',actual),('contract',contract)]:
    result=run([executable,'--benchmark_filter='+select,'--benchmark_min_time=2x','--benchmark_out='+str(out/(name+'.json')),'--benchmark_out_format=json'],name+'.log')
    data=json.loads((out/(name+'.json')).read_text())
    assert len(data['benchmarks'])==64,len(data['benchmarks'])
    assert all(row['iterations']==2 for row in data['benchmarks'])
    if name=='contract':assert b'get_calls=32 set_calls=32 format_cases=16 parse_cases=16' in result.stderr
for name,filter,markers in [('old-invalid-digits','gmp_biginteger_from_chars/2/1$',['raw digit outside base']),('old-destructive-input','gmp_biginteger_to_chars/10/64$',['format input changed','format input is not normalized'])]:
    result=run([old_contract,'--benchmark_filter='+filter,'--benchmark_min_time=2x'],name+'.log',expect=False)
    assert result.returncode!=0
    assert any(marker.encode() in result.stderr for marker in markers),result.stderr[-1000:]
metadata={'source':sp.check_output(['git','rev-parse','HEAD'],text=True).strip(),'candidate':'ecee4f44117822f1fd8281322ae32c634028b1b7','old_source':'84132724','compiler':sp.check_output(['g++-13','--version'],text=True).splitlines()[0],'platform':platform.platform(),'result':'all-profile GMP target and 64 cases x 2 iterations; wrapper cross-checks WJR/GMP outputs and input contracts; old-source contract failures reproduced before invalid calls','timings_are_diagnostic_only':True}
(out/'metadata.json').write_text(json.dumps(metadata,indent=2))
print(json.dumps(metadata,indent=2))
