cmake_minimum_required(VERSION 3.20)
project(wjr_consumer LANGUAGES CXX)

set(WJR_PREFIX_PATH_BEFORE "${CMAKE_PREFIX_PATH}")
# Core consumers must not need the former range-v3 dependency.
set(CMAKE_DISABLE_FIND_PACKAGE_range-v3 TRUE)
find_package(wjr CONFIG REQUIRED)
find_package(wjr CONFIG REQUIRED OPTIONAL_COMPONENTS wjr_unavailable)
if(wjr_wjr_unavailable_FOUND)
    message(FATAL_ERROR "An unavailable optional component was reported as found.")
endif()
# Repeated discovery is common when multiple components use the same package.
find_package(wjr CONFIG REQUIRED)
if(NOT CMAKE_PREFIX_PATH STREQUAL WJR_PREFIX_PATH_BEFORE)
    message(FATAL_ERROR "Finding WJR changed the consumer's CMAKE_PREFIX_PATH.")
endif()

get_target_property(WJR_IMPORTED_FEATURES wjr::config INTERFACE_COMPILE_FEATURES)
if(NOT "cxx_std_20" IN_LIST WJR_IMPORTED_FEATURES AND NOT "cxx_std_23" IN_LIST WJR_IMPORTED_FEATURES)
    message(FATAL_ERROR "The installed package must require C++20 or later.")
endif()

if(DEFINED WJR_EXPECT_CXX_STANDARD)
    if(NOT "cxx_std_${WJR_EXPECT_CXX_STANDARD}" IN_LIST WJR_IMPORTED_FEATURES)
        message(FATAL_ERROR "The installed package did not preserve its C++ requirement.")
    endif()
endif()
get_target_property(WJR_IMPORTED_OPTIONS wjr::config INTERFACE_COMPILE_OPTIONS)
if(WJR_IMPORTED_OPTIONS MATCHES "-Wall|-Wextra|/W[0-4]")
    message(FATAL_ERROR "Private WJR warnings leaked into the package interface.")
endif()
if(WJR_IMPORTED_OPTIONS MATCHES "-march=|/arch:")
    message(FATAL_ERROR "ISA options leaked into the common config interface.")
endif()

add_executable(wjr_consumer main.cpp)
target_link_libraries(wjr_consumer PRIVATE wjr::wjr)

add_executable(wjr_module_consumer main.cpp)
target_link_libraries(wjr_module_consumer PRIVATE wjr::json wjr::ring_buffer)

add_executable(wjr_portable_consumer portable.cpp)
target_link_libraries(wjr_portable_consumer PRIVATE wjr::ring_buffer)

file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/portable-options-$<CONFIG>-$<COMPILE_LANGUAGE>.cmake"
    CONTENT "set(WJR_OPTIONS [==[$<TARGET_PROPERTY:wjr_portable_consumer,COMPILE_OPTIONS>]==])
include([==[${CMAKE_CURRENT_SOURCE_DIR}/check_portable.cmake]==])
"
    TARGET wjr_portable_consumer
)

enable_testing()
add_test(NAME wjr_consumer COMMAND wjr_consumer)
add_test(NAME wjr_module_consumer COMMAND wjr_module_consumer)
add_test(NAME wjr_portable_consumer COMMAND wjr_portable_consumer)
add_test(NAME wjr_portable_options
    COMMAND ${CMAKE_COMMAND}
    -P "${CMAKE_CURRENT_BINARY_DIR}/portable-options-$<CONFIG>-CXX.cmake"
)

add_test(NAME wjr_required_unknown_component
    COMMAND ${CMAKE_COMMAND}
    -S "${CMAKE_CURRENT_SOURCE_DIR}/component"
    -B "${CMAKE_CURRENT_BINARY_DIR}/required-component"
    -G "${CMAKE_GENERATOR}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)
set_tests_properties(wjr_required_unknown_component PROPERTIES WILL_FAIL TRUE)

option(WJR_TEST_NP "Validate the installed optional np component." OFF)
if(WJR_TEST_NP)
    if(TARGET wjr::np)
        message(FATAL_ERROR "Ordinary package discovery imported the np component.")
    endif()
    find_package(wjr CONFIG REQUIRED COMPONENTS np)
    find_package(wjr CONFIG REQUIRED COMPONENTS np)
    get_target_property(_WJR_NP_DEPS wjr::np INTERFACE_LINK_LIBRARIES)
    list(FILTER _WJR_NP_DEPS INCLUDE REGEX "^std::mdspan$")
    list(LENGTH _WJR_NP_DEPS _WJR_NP_MDSPAN_COUNT)
    if(NOT _WJR_NP_MDSPAN_COUNT EQUAL 1)
        message(FATAL_ERROR "Repeated discovery duplicated the np backend dependency.")
    endif()
    if(NOT wjr_np_FOUND)
        message(FATAL_ERROR "The installed np component was not found.")
    endif()
    if(NOT CMAKE_PREFIX_PATH STREQUAL WJR_PREFIX_PATH_BEFORE)
        message(FATAL_ERROR "Finding np changed the consumer's CMAKE_PREFIX_PATH.")
    endif()
    add_executable(wjr_np_consumer np.cpp)
    target_link_libraries(wjr_np_consumer PRIVATE wjr::np)
    add_test(NAME wjr_np_consumer COMMAND wjr_np_consumer)

    foreach(_WJR_OPTIONAL IN ITEMS ON OFF)
        add_test(NAME wjr_np_missing_dependency_optional_${_WJR_OPTIONAL}
            COMMAND ${CMAKE_COMMAND}
            -S "${CMAKE_CURRENT_SOURCE_DIR}/component"
            -B "${CMAKE_CURRENT_BINARY_DIR}/np-missing-dependency-${_WJR_OPTIONAL}"
            -G "${CMAKE_GENERATOR}"
            "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            -DWJR_REQUEST_COMPONENT=np
            -DWJR_OPTIONAL_COMPONENT=${_WJR_OPTIONAL}
            -DCMAKE_DISABLE_FIND_PACKAGE_mdspan=TRUE
        )
    endforeach()
    set_tests_properties(wjr_np_missing_dependency_optional_OFF PROPERTIES WILL_FAIL TRUE)
endif()

option(WJR_TEST_SIMD_XSIMD "Validate the installed optional xsimd component." OFF)
if(WJR_TEST_SIMD_XSIMD)
    if(TARGET wjr::simd_xsimd)
        message(FATAL_ERROR "Ordinary package discovery imported the xsimd component.")
    endif()
    find_package(wjr CONFIG REQUIRED COMPONENTS simd_xsimd)
    find_package(wjr CONFIG REQUIRED COMPONENTS simd_xsimd)
    get_target_property(_WJR_SIMD_DEPS wjr::simd_xsimd INTERFACE_LINK_LIBRARIES)
    list(FILTER _WJR_SIMD_DEPS INCLUDE REGEX "^xsimd$")
    list(LENGTH _WJR_SIMD_DEPS _WJR_SIMD_DEP_COUNT)
    if(NOT wjr_simd_xsimd_FOUND OR NOT _WJR_SIMD_DEP_COUNT EQUAL 1)
        message(FATAL_ERROR "The xsimd component must retain exactly one backend dependency.")
    endif()
    if(NOT CMAKE_PREFIX_PATH STREQUAL WJR_PREFIX_PATH_BEFORE)
        message(FATAL_ERROR "Finding xsimd changed the consumer's CMAKE_PREFIX_PATH.")
    endif()
    add_executable(wjr_simd_xsimd_consumer simd_xsimd.cpp)
    target_link_libraries(wjr_simd_xsimd_consumer PRIVATE wjr::simd_xsimd)
    add_test(NAME wjr_simd_xsimd_consumer COMMAND wjr_simd_xsimd_consumer)

    foreach(_WJR_OPTIONAL IN ITEMS ON OFF)
        add_test(NAME wjr_simd_missing_dependency_optional_${_WJR_OPTIONAL}
            COMMAND ${CMAKE_COMMAND}
            -S "${CMAKE_CURRENT_SOURCE_DIR}/component"
            -B "${CMAKE_CURRENT_BINARY_DIR}/simd-missing-dependency-${_WJR_OPTIONAL}"
            -G "${CMAKE_GENERATOR}"
            "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            -DWJR_REQUEST_COMPONENT=simd_xsimd
            -DWJR_OPTIONAL_COMPONENT=${_WJR_OPTIONAL}
            -DCMAKE_DISABLE_FIND_PACKAGE_xsimd=TRUE
        )
    endforeach()
    set_tests_properties(wjr_simd_missing_dependency_optional_OFF PROPERTIES WILL_FAIL TRUE)
endif()
