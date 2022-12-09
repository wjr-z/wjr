cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "D:/LLVM/bin/clang.exe")
set(CMAKE_CXX_COMPILER "D:/LLVM/bin/clang++.exe")

project(wjr LANGUAGES CXX)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)
set(WJR_SIMD_DIR ${WJR_LIB_DIR}/simd)

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /arch:AVX2")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -Wno-error -mavx2")
endif() 

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://ghproxy.com/https://github.com/google/googletest.git
  GIT_TAG main
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googlebenchmark
        GIT_REPOSITORY https://ghproxy.com/https://github.com/google/benchmark.git
        GIT_TAG main) # need master for benchmark::benchmark

FetchContent_MakeAvailable(googletest googlebenchmark)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(wjr 
    ${PROJECT_SOURCE_DIR}/src/bench_vector.cpp 
    ${PROJECT_SOURCE_DIR}/src/bench_algo.cpp
	${WJR_SRCS}
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ${WJR_SIMD_DIR}
                           ) 

target_link_libraries(
    wjr 
    benchmark::benchmark
)