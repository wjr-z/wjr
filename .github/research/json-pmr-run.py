from pathlib import Path
import os,subprocess,json
source=Path.cwd()
temporary=Path(os.environ['RUNNER_TEMP'])/'json-pmr'
temporary.mkdir(exist_ok=True)
out=source/'json-pmr-results'
out.mkdir(exist_ok=True)
deps=temporary/'deps'
deps.mkdir(exist_ok=True)
for name,url,tag in [('googletest','https://github.com/google/googletest.git','v1.17.0'),('cpu_features','https://github.com/google/cpu_features.git','v0.10.1')]:
    subprocess.run(['git','clone','--depth','1','--branch',tag,url,str(deps/name)],check=True)
wrapper=temporary/'project'
wrapper.mkdir(exist_ok=True)
(wrapper/'CMakeLists.txt').write_text(f"""cmake_minimum_required(VERSION 3.20)
project(json_pmr_research LANGUAGES CXX)
set(WJR_BUILD_TESTS ON CACHE BOOL "" FORCE)
set(WJR_USE_GMP_TESTS OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(WJR_ENABLE_ASSEMBLY OFF CACHE BOOL "" FORCE)
set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST {deps/'googletest'} CACHE PATH "" FORCE)
set(FETCHCONTENT_SOURCE_DIR_CPU_FEATURES {deps/'cpu_features'} CACHE PATH "" FORCE)
add_subdirectory({source/'third-party/fmt'} fmt EXCLUDE_FROM_ALL)
add_subdirectory({source} wjr EXCLUDE_FROM_ALL)
add_executable(pmr_tests {source/'test/json/pmr.cpp'} {source/'test/json/json.cpp'} {source/'test/json/minify.cpp'})
target_include_directories(pmr_tests PRIVATE {source/'test'})
target_link_libraries(pmr_tests PRIVATE wjr::json gtest_main)
add_executable(pmr_measure {source/'.github/research/json-pmr-measure.cpp'})
target_link_libraries(pmr_measure PRIVATE wjr::json)
enable_testing()
add_test(NAME focused_json COMMAND pmr_tests)
""")
for compiler,cc in [('g++-14','gcc-14'),('clang++-18','clang-18')]:
    for standard in [20,23]:
        label=compiler+'-cxx'+str(standard)
        build=temporary/label
        subprocess.run(['cmake','-S',str(wrapper),'-B',str(build),'-G','Ninja','-DCMAKE_CXX_COMPILER='+compiler,'-DCMAKE_C_COMPILER='+cc,'-DCMAKE_BUILD_TYPE=Release','-DWJR_CPU_TARGET=x86-64-v3','-DWJR_CXX_STANDARD='+str(standard),'-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'],check=True)
        subprocess.run(['cmake','--build',str(build),'--target','pmr_tests','pmr_measure','--parallel','4'],check=True)
        subprocess.run(['ctest','--test-dir',str(build),'-R','^focused_json$','--output-on-failure'],check=True)
        shutil_target=out/(label+'-compile-commands.json')
        shutil_target.write_bytes((build/'compile_commands.json').read_bytes())
        for repeat in range(5):
            with (out/(label+'-run'+str(repeat)+'.csv')).open('w') as output:
                subprocess.run([str(build/'pmr_measure'),'bench']+(['reverse']if repeat%2 else []),stdout=output,check=True)
    standard=20 if compiler.startswith('g') else 23
    label=compiler+'-asan-cxx'+str(standard)
    build=temporary/label
    subprocess.run(['cmake','-S',str(wrapper),'-B',str(build),'-G','Ninja','-DCMAKE_CXX_COMPILER='+compiler,'-DCMAKE_C_COMPILER='+cc,'-DCMAKE_BUILD_TYPE=Release','-DWJR_CPU_TARGET=x86-64-v3','-DWJR_CXX_STANDARD='+str(standard),'-DWJR_ENABLE_ASAN=ON','-DCMAKE_CXX_FLAGS=-fsanitize=undefined'],check=True)
    subprocess.run(['cmake','--build',str(build),'--target','pmr_tests','--parallel','4'],check=True)
    subprocess.run(['ctest','--test-dir',str(build),'-R','^focused_json$','--output-on-failure'],check=True)
