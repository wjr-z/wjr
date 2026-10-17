cmake_minimum_required(VERSION 3.20)
project(wjr_consumer LANGUAGES CXX)

set(WJR_PREFIX_PATH_BEFORE "${CMAKE_PREFIX_PATH}")
find_package(wjr CONFIG REQUIRED)
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
