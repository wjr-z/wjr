cmake_minimum_required(VERSION 3.14)
project(benchmarks LANGUAGES CXX)

set(WJR_USE_GMP_DEFAULT ON)

if(WJR_USE_GMP)
    set(WJR_USE_GMP_DEFAULT ${WJR_USE_GMP})
else()
    find_package(GMP QUIET)

    if(NOT GMP_FOUND)
        find_package(PkgConfig)
        pkg_check_modules(GMP QUIET gmp)

        if(NOT GMP_FOUND)
            set(WJR_USE_GMP_DEFAULT OFF)
        else()
            set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIRS})
            set(GMP_LIBRARIES ${GMP_LIBRARIES})
        endif()
    else()
        set(GMP_INCLUDE_DIRS)
        set(GMP_LIBRARIES gmp libgmp)
    endif()
endif()

option(WJR_USE_GMP_TESTS "Use GMP library for tests" ${WJR_USE_GMP_DEFAULT})

if(WJR_USE_GMP_TESTS)
    add_compile_definitions(WJR_USE_GMP)
else()
    message(STATUS "GMP library not found, tests will be built without GMP support.")
endif()

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Suppressing warnings as errors" FORCE)

if(EXISTS benchmark)
    add_subdirectory(benchmark)
else()
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.0
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(benchmark)
endif()

set(WJR_BENCHMARK_PROFILE "core" CACHE STRING "Benchmark profile to build")
set_property(CACHE WJR_BENCHMARK_PROFILE PROPERTY STRINGS core all)

set(BENCHMARK_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/division.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/math.cpp
)

set(BENCHMARK_ALL_SRCS
    ${BENCHMARK_CORE_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/biginteger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/btree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/container.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory.cpp
)

if(WJR_BENCHMARK_PROFILE STREQUAL "core")
    set(BENCHMARK_SRCS ${BENCHMARK_CORE_SRCS})
elseif(WJR_BENCHMARK_PROFILE STREQUAL "all")
    set(BENCHMARK_SRCS ${BENCHMARK_ALL_SRCS})
else()
    message(FATAL_ERROR "WJR_BENCHMARK_PROFILE must be 'core' or 'all'")
endif()

add_executable(
    benchmarks
    ${BENCHMARK_SRCS}
)

if(WJR_BENCHMARK_PROFILE STREQUAL "core")
    target_compile_definitions(benchmarks PRIVATE WJR_BENCHMARK_QUICK)
endif()

target_include_directories(benchmarks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(benchmarks PRIVATE ${GMP_INCLUDE_DIRS})

target_link_libraries(benchmarks
    PRIVATE
    wjr::wjr
    benchmark::benchmark
    ${GMP_LIBRARIES}
)