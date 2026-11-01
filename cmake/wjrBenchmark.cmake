include_guard(GLOBAL)
include(FetchContent)

# Share dependency selection across the regular and focused benchmark profiles.
function(wjr_load_benchmark)
    if(TARGET benchmark::benchmark)
        return()
    endif()

    # Explicit source choices take precedence over automatic package discovery.
    if(NOT FETCHCONTENT_SOURCE_DIR_BENCHMARK AND
       NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt")
        find_package(benchmark 1.9.0 EXACT QUIET CONFIG)
        if(TARGET benchmark::benchmark)
            return()
        endif()
    endif()

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppress benchmark tests" FORCE)
    set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Suppress benchmark warnings as errors" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt")
        add_subdirectory(benchmark)
    else()
        # Discovery above already enforced the reviewed version.
        set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
        FetchContent_Declare(benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.9.0
            GIT_SHALLOW ON
        )
        FetchContent_MakeAvailable(benchmark)
    endif()
endfunction()
