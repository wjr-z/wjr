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
set(BENCHMARK_ENABLE_INSTALL OFF)

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
set_property(CACHE WJR_BENCHMARK_PROFILE PROPERTY STRINGS core all simd np)
option(WJR_BENCHMARK_XSIMD "Compare SIMD kernels with pinned xsimd" OFF)

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
elseif(WJR_BENCHMARK_PROFILE STREQUAL "simd")
    set(BENCHMARK_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/simd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/simd_scan.cpp
    )
elseif(WJR_BENCHMARK_PROFILE STREQUAL "np")
    if(NOT WJR_ENABLE_NP)
        message(FATAL_ERROR "The np benchmark profile requires WJR_ENABLE_NP=ON")
    endif()
    set(BENCHMARK_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_owner.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_compact.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_partition.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_searchsorted.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_rank.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_statistics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_segmented_reduce.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_bincount.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/np_expression_reduce.cpp
    )
    set_source_files_properties(src/np_expression_reduce.cpp PROPERTIES
        COMPILE_OPTIONS "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-ffp-contract=off>"
    )
else()
    message(FATAL_ERROR "WJR_BENCHMARK_PROFILE must be 'core', 'all', 'simd', or 'np'")
endif()

if(WJR_BENCHMARK_XSIMD AND NOT WJR_BENCHMARK_PROFILE MATCHES "^(simd|np)$")
    message(FATAL_ERROR "WJR_BENCHMARK_XSIMD requires the simd or np benchmark profile")
endif()
if(WJR_BENCHMARK_XSIMD AND WJR_BENCHMARK_PROFILE STREQUAL "np" AND NOT WJR_ENABLE_XSIMD)
    message(FATAL_ERROR "The np SIMD benchmarks require WJR_ENABLE_XSIMD=ON")
endif()

add_executable(
    benchmarks
    ${BENCHMARK_SRCS}
)

target_compile_definitions(benchmarks PRIVATE
    WJR_BENCHMARK_PROFILE="${WJR_BENCHMARK_PROFILE}"
    WJR_BENCHMARK_COMPILER="${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
    WJR_BENCHMARK_CXX_STANDARD="${WJR_CXX_STANDARD}"
    WJR_BENCHMARK_CPU_TARGET="${WJR_CPU_TARGET}"
)

if(WJR_BENCHMARK_PROFILE STREQUAL "np")
    target_link_libraries(benchmarks PRIVATE wjr::np)
    if(WJR_BENCHMARK_XSIMD)
        target_link_libraries(benchmarks PRIVATE wjr::simd_xsimd)
    endif()
endif()

if(WJR_BENCHMARK_XSIMD)
    include(wjrXsimd)
    wjr_load_xsimd(FALSE)
    target_link_libraries(benchmarks PRIVATE xsimd)
    target_compile_features(benchmarks PRIVATE cxx_std_20)
    target_compile_definitions(benchmarks PRIVATE WJR_BENCHMARK_XSIMD=1)
endif()

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