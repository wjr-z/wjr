include_guard(GLOBAL)
include(FetchContent)

function(wjr_load_gtest)
    if(TARGET GTest::gtest_main)
        return()
    endif()
    if(TARGET gtest_main)
        get_target_property(_wjr_gtest_main gtest_main ALIASED_TARGET)
        if(NOT _wjr_gtest_main)
            set(_wjr_gtest_main gtest_main)
        endif()
        add_library(GTest::gtest_main ALIAS ${_wjr_gtest_main})
        return()
    endif()

    # Instrumented and debug builds need the same source configuration as WJR.
    string(TOUPPER "${CMAKE_BUILD_TYPE}" _wjr_gtest_build_type)
    if(NOT WJR_ENABLE_ASAN AND NOT WJR_ENABLE_COVERAGE AND
       NOT CMAKE_CONFIGURATION_TYPES AND
       _wjr_gtest_build_type MATCHES "^(RELEASE|RELWITHDEBINFO|MINSIZEREL)$" AND
       NOT FETCHCONTENT_SOURCE_DIR_GOOGLETEST AND
       NOT FETCHCONTENT_TRY_FIND_PACKAGE_MODE STREQUAL "NEVER" AND
       NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
        # Keep rejected imported targets out of the source fallback's scope.
        add_subdirectory("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gtest-provider"
                         "${CMAKE_CURRENT_BINARY_DIR}/gtest-provider")
        if(TARGET GTest::gtest_main)
            return()
        endif()
    endif()

    if(WJR_ENABLE_ASAN AND WIN32)
        # ASan uses the static CRT on Windows.
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    else()
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
        add_subdirectory(googletest)
    else()
        set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
        FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    if(WJR_ENABLE_ASAN)
        # Match annotate_string and the sanitizer runtime used by the tests.
        foreach(_wjr_gtest_target gtest gtest_main)
            target_compile_options(${_wjr_gtest_target} PRIVATE -fsanitize=address)
        endforeach()
    endif()
endfunction()
