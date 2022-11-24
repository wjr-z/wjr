cmake_minimum_required(VERSION 3.10)

project(wjr LANGUAGES CXX)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17")
endif() 

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://ghproxy.com/https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googlebenchmark
        GIT_REPOSITORY https://ghproxy.com/https://github.com/google/benchmark.git
        GIT_TAG main) # need master for benchmark::benchmark

FetchContent_MakeAvailable(googletest googlebenchmark)

add_executable(wjr 
    ${PROJECT_SOURCE_DIR}/src/benchmarks.cpp 
    ${WJR_LIB_DIR}/random.cpp 
    ${WJR_LIB_DIR}/allocator.cpp
	${WJR_LIB_DIR}/type_traits.cpp
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ) 

target_link_libraries(wjr 
  benchmark::benchmark
)