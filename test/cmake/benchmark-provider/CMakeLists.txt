cmake_minimum_required(VERSION 3.20)
project(wjr_benchmark_provider_test LANGUAGES CXX)
include(CMakePackageConfigHelpers)
include(FetchContent)
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/wjrBenchmark.cmake")

if(NOT MODE MATCHES "^(installed|preexisting|disabled|incompatible|source)$")
    message(FATAL_ERROR "Unknown benchmark provider mode: ${MODE}")
endif()

unset(WJR_TEST_BENCHMARK_PACKAGE_USED CACHE)
unset(WJR_TEST_BENCHMARK_SOURCE_USED CACHE)
set(_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/fake-package")
file(MAKE_DIRECTORY "${_PACKAGE_DIR}")
file(WRITE "${_PACKAGE_DIR}/benchmarkConfig.cmake"
    "add_library(benchmark::benchmark INTERFACE IMPORTED GLOBAL)\n"
    "set(WJR_TEST_BENCHMARK_PACKAGE_USED TRUE CACHE INTERNAL \"test marker\")\n")
set(_VERSION 1.9.0)
if(MODE STREQUAL "incompatible")
    set(_VERSION 0.1.0)
endif()
write_basic_package_version_file("${_PACKAGE_DIR}/benchmarkConfigVersion.cmake"
    VERSION ${_VERSION} COMPATIBILITY SameMajorVersion)
set(benchmark_DIR "${_PACKAGE_DIR}" CACHE PATH "Test benchmark package" FORCE)

# A parent declaration supplies a local fallback: these fixtures never download.
set(_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/fake-source")
file(MAKE_DIRECTORY "${_SOURCE_DIR}")
file(WRITE "${_SOURCE_DIR}/CMakeLists.txt"
    "add_library(benchmark INTERFACE)\n"
    "add_library(benchmark::benchmark ALIAS benchmark)\n"
    "set(WJR_TEST_BENCHMARK_SOURCE_USED TRUE CACHE INTERNAL \"test marker\")\n")
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
FetchContent_Declare(benchmark SOURCE_DIR "${_SOURCE_DIR}")

if(MODE STREQUAL "preexisting")
    add_library(benchmark::benchmark INTERFACE IMPORTED GLOBAL)
endif()
if(MODE MATCHES "^(preexisting|disabled)$")
    set(CMAKE_DISABLE_FIND_PACKAGE_benchmark TRUE)
endif()
if(MODE STREQUAL "source")
    set(FETCHCONTENT_SOURCE_DIR_BENCHMARK "${_SOURCE_DIR}" CACHE PATH "Explicit source" FORCE)
endif()

wjr_load_benchmark()
if(NOT TARGET benchmark::benchmark)
    message(FATAL_ERROR "Provider must expose benchmark::benchmark")
endif()
if(MODE STREQUAL "installed")
    if(NOT WJR_TEST_BENCHMARK_PACKAGE_USED OR WJR_TEST_BENCHMARK_SOURCE_USED)
        message(FATAL_ERROR "Compatible installed package was not reused")
    endif()
elseif(MODE STREQUAL "preexisting")
    if(WJR_TEST_BENCHMARK_PACKAGE_USED OR WJR_TEST_BENCHMARK_SOURCE_USED)
        message(FATAL_ERROR "Parent target should bypass package/source selection")
    endif()
elseif(WJR_TEST_BENCHMARK_PACKAGE_USED OR NOT WJR_TEST_BENCHMARK_SOURCE_USED)
    message(FATAL_ERROR "Disabled/incompatible package or explicit source did not use fallback")
endif()
