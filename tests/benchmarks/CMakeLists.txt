cmake_minimum_required(VERSION 3.14)
project(wjr LANGUAGES CXX)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_definitions(WJR_USE_GMP)

set(BUILD_TESTING OFF)

set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS c;cc;cxx;cpp;h;hpp)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -m64 -DNDEBUG -Wall -Wextra -DWJR_DEBUG_LEVEL=0 -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" -lpthread")

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/../../src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(googletest)
add_subdirectory(benchmark)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(
    wjr
    src/main.cpp
    src/math.cpp
    ${WJR_SRCS}
)

set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")

link_directories(${GMP_LIBS})
target_link_libraries(wjr
    ${GMP_LIBS}/libgmp.dll.a
    ${GMP_LIBS}/libgmpxx.dll.a
)

target_include_directories(wjr PUBLIC
    ${WJR_INCLUDE_DIR}
)

target_link_libraries(
    wjr
    benchmark::benchmark
)