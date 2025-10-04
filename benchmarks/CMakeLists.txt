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

if(EXISTS googletest)
    add_subdirectory(googletest)
else()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW ON
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

if(EXISTS benchmark)
    add_subdirectory(benchmark)
else()
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.0
        GIT_SHALLOW ON
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
    set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Suppressing warnings as errors" FORCE)
    FetchContent_MakeAvailable(benchmark)
endif()

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(
    benchmarks
    ${TEST_SRCS}
)

target_include_directories(benchmarks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(benchmarks PRIVATE ${GMP_INCLUDE_DIRS})

target_link_libraries(benchmarks
    PRIVATE
    wjr::wjr
    benchmark::benchmark
    ${GMP_LIBRARIES}
)