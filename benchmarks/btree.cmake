if(WJR_BENCHMARK_XSIMD)
    message(FATAL_ERROR "WJR_BENCHMARK_XSIMD requires the simd or np benchmark profile")
endif()

set(CMAKE_CXX_STANDARD ${WJR_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Suppressing warnings as errors" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF)
FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(benchmark)

add_executable(benchmarks src/btree_focused.cpp)
add_executable(btree_memory src/btree_memory.cpp)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark)

find_package(Git QUIET)
set(btree_source "unknown")
if(Git_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/.."
        OUTPUT_VARIABLE btree_source OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=normal --ignore-submodules=all
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/.."
        OUTPUT_VARIABLE btree_dirty OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(btree_dirty)
        string(APPEND btree_source "-dirty")
    endif()
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" btree_build_type)
set(btree_flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${btree_build_type}} ${WJR_CXX_FLAGS} ${WJR_BASELINE_FLAGS} C++${WJR_CXX_STANDARD}")
string(REPLACE ";" " " btree_flags "${btree_flags}")
string(REPLACE "\"" "\\\"" btree_flags "${btree_flags}")

foreach(btree_target IN ITEMS benchmarks btree_memory)
    # The general container path needs config/assert, never wjr::isa or GMP.
    target_link_libraries(${btree_target} PRIVATE wjr::assert wjr::iterator)
    target_compile_options(${btree_target} PRIVATE ${WJR_BASELINE_FLAGS})
    target_compile_definitions(${btree_target} PRIVATE
        WJR_BENCHMARK_SOURCE="${btree_source}"
        WJR_BENCHMARK_COMPILER="${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
        WJR_BENCHMARK_FLAGS="${btree_flags}"
        WJR_BENCHMARK_CPU_TARGET="${WJR_CPU_TARGET}"
    )
endforeach()

# No lookup, fetch or link of Abseil occurs while this option is OFF.
if(WJR_BENCHMARK_ABSEIL)
    set(WJR_BENCHMARK_ABSEIL_SOURCE_DIR "" CACHE PATH "Existing reviewed Abseil Git checkout")
    if(NOT Git_FOUND OR NOT EXISTS "${WJR_BENCHMARK_ABSEIL_SOURCE_DIR}/absl/container/btree_map.h")
        message(FATAL_ERROR "Abseil comparison requires Git and WJR_BENCHMARK_ABSEIL_SOURCE_DIR")
    endif()
    execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${WJR_BENCHMARK_ABSEIL_SOURCE_DIR}"
        OUTPUT_VARIABLE btree_absl_revision OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE btree_absl_git_result)
    execute_process(COMMAND "${GIT_EXECUTABLE}" diff --quiet HEAD -- .
        WORKING_DIRECTORY "${WJR_BENCHMARK_ABSEIL_SOURCE_DIR}"
        RESULT_VARIABLE btree_absl_dirty)
    if(NOT btree_absl_git_result EQUAL 0 OR NOT btree_absl_dirty EQUAL 0 OR
       NOT btree_absl_revision STREQUAL "2a2d6aad626f75a32c8b7bb775fbff73d558b8b1")
        message(FATAL_ERROR "Abseil comparison requires an unmodified reviewed commit 2a2d6aad626f75a32c8b7bb775fbff73d558b8b1")
    endif()
    set(ABSL_BUILD_TESTING OFF CACHE BOOL "Disable optional Abseil tests")
    set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "Disable optional Abseil installation")
    set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "Use the selected C++ standard")
    add_subdirectory("${WJR_BENCHMARK_ABSEIL_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/abseil" EXCLUDE_FROM_ALL)
    foreach(btree_target IN ITEMS benchmarks btree_memory)
        target_link_libraries(${btree_target} PRIVATE absl::btree)
        target_compile_definitions(${btree_target} PRIVATE WJR_BENCHMARK_ABSEIL=1)
    endforeach()
endif()
