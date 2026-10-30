from pathlib import Path
import os,subprocess
source=Path.cwd()
root=Path(os.environ['RUNNER_TEMP'])/'readonly'
root.mkdir(exist_ok=True)
out=source/'json-readonly-results'
out.mkdir(exist_ok=True)
deps=root/'cpu_features'
subprocess.run(['git','clone','--depth','1','--branch','v0.10.1','https://github.com/google/cpu_features.git',str(deps)],check=True)
project=root/'project'
project.mkdir(exist_ok=True)
(project/'CMakeLists.txt').write_text(f'''cmake_minimum_required(VERSION 3.20)
project(json_readonly_research LANGUAGES CXX)
set(WJR_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_ASSEMBLY OFF CACHE BOOL "" FORCE)
set(WJR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FETCHCONTENT_SOURCE_DIR_CPU_FEATURES {deps} CACHE PATH "" FORCE)
add_subdirectory({source/'third-party/fmt'} fmt EXCLUDE_FROM_ALL)
add_subdirectory({source} wjr EXCLUDE_FROM_ALL)
add_executable(readonly_bench {source/'.github/research/json-readonly.cpp'})
target_link_libraries(readonly_bench PRIVATE wjr::json)
add_executable(readonly_count {source/'.github/research/json-readonly.cpp'})
target_link_libraries(readonly_count PRIVATE wjr::json)
target_compile_definitions(readonly_count PRIVATE WJR_READONLY_COUNT=1)
''')
build=root/'release'
subprocess.run(['cmake','-S',str(project),'-B',str(build),'-G','Ninja','-DCMAKE_CXX_COMPILER=clang++-18','-DCMAKE_C_COMPILER=clang-18','-DCMAKE_BUILD_TYPE=Release','-DWJR_CPU_TARGET=x86-64-v3','-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'],check=True)
subprocess.run(['cmake','--build',str(build),'--target','readonly_bench','readonly_count','--parallel','4'],check=True)
(out/'compile_commands.json').write_bytes((build/'compile_commands.json').read_bytes())
subprocess.run([str(build/'readonly_bench')],check=True)
with (out/'counts.csv').open('w') as output:
    subprocess.run([str(build/'readonly_count'),'count'],stdout=output,check=True)
for repeat in range(2):
    with (out/('run'+str(repeat)+'.csv')).open('w') as output:
        subprocess.run([str(build/'readonly_bench'),'bench']+(['reverse'] if repeat else []),stdout=output,check=True)
san=root/'sanitize'
subprocess.run(['cmake','-S',str(project),'-B',str(san),'-G','Ninja','-DCMAKE_CXX_COMPILER=clang++-18','-DCMAKE_C_COMPILER=clang-18','-DCMAKE_BUILD_TYPE=Release','-DWJR_CPU_TARGET=x86-64-v3','-DWJR_ENABLE_ASAN=ON','-DCMAKE_CXX_FLAGS=-fsanitize=undefined -fno-sanitize=builtin'],check=True)
subprocess.run(['cmake','--build',str(san),'--target','readonly_bench','--parallel','4'],check=True)
environment=os.environ.copy()
environment['ASAN_OPTIONS']='detect_leaks=1:halt_on_error=1'
environment['UBSAN_OPTIONS']='halt_on_error=1:print_stacktrace=1'
result=subprocess.run([str(san/'readonly_bench')],env=environment,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
(out/'sanitizer.log').write_text(result.stdout)
result.check_returncode()
