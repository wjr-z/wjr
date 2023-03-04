cmake_minimum_required(VERSION 3.14)

project(wjr LANGUAGES CXX)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)
set(WJR_SIMD_DIR ${WJR_LIB_DIR}/simd)
set(WJR_ASM_DIR ${WJR_LIB_DIR}/asm)
set(WJR_ALGO_DIR ${WJR_LIB_DIR}/algo)
set(WJR_MP_DIR ${WJR_LIB_DIR}/mp)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /arch:AVX2")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wno-error -mavx2")
endif() 

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

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

FetchContent_Declare(
    cpu_features
        GIT_REPOSITORY https://ghproxy.com/https://github.com/google/cpu_features.git
        GIT_TAG v0.7.0)

FetchContent_MakeAvailable(googletest googlebenchmark cpu_features)

# define BENCHMARK_ENABLE_TESTING=off
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

file(GLOB_RECURSE 
    WJR_SRCS 
    ${WJR_LIB_DIR}/*.cpp
)

add_library(
    wjr_library STATIC
	${WJR_SRCS}
)

target_link_libraries(
    wjr_library
    cpu_features
)

target_include_directories(wjr_library PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ${WJR_SIMD_DIR}
                           ${WJR_ASM_DIR}
                           ${WJR_ALGO_DIR}
                           ${WJR_MP_DIR}
                           ) 

add_executable(
    wjr 
    ${PROJECT_SOURCE_DIR}/src/bench_algo.cpp
)

target_link_libraries(
    wjr 
    benchmark::benchmark
    wjr_library
)