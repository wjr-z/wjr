cmake_minimum_required(VERSION 3.14)

project(wjr LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(BUILD_TESTING OFF)

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /Od")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /O2 /arch:AVX2")
	
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -mavx2")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -mavx2")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(${CMAKE_CXX_FLAGS_DEBUG})
message(${CMAKE_CXX_FLAGS_RELEASE})

add_subdirectory(googletest)
add_subdirectory(benchmark)
add_subdirectory(cpu_features)

# define BENCHMARK_ENABLE_TESTING=off
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_executable(
    wjr 
    ${PROJECT_SOURCE_DIR}/src/bench_algo.cpp
    ${WJR_LIB_DIR}/cpuinfo.cpp
    ${WJR_LIB_DIR}/allocator.cpp
    ${WJR_LIB_DIR}/network/thread_pool.cpp
)

target_include_directories(wjr PUBLIC ${WJR_INCLUDE_DIR} ${CPU_FEATURES_INCLUDE_DIR}) 

target_link_libraries(
    wjr 
    benchmark::benchmark
    cpu_features
)