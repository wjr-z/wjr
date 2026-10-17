cmake_minimum_required(VERSION 3.22)
project(wjr_np_examples LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED COMPONENTS np)

add_executable(wjr_np_elementwise elementwise.cpp)
target_link_libraries(wjr_np_elementwise PRIVATE wjr::np)

add_executable(wjr_np_expression expression.cpp)
target_link_libraries(wjr_np_expression PRIVATE wjr::np)

add_executable(wjr_np_row_centering row_centering.cpp)
target_link_libraries(wjr_np_row_centering PRIVATE wjr::np)

option(WJR_NP_EXAMPLES_XSIMD "Build the explicit SIMD expression example" OFF)
if(WJR_NP_EXAMPLES_XSIMD)
    find_package(wjr CONFIG REQUIRED COMPONENTS simd_xsimd)
    add_executable(wjr_np_expression_simd expression_simd.cpp)
    target_link_libraries(wjr_np_expression_simd PRIVATE wjr::np wjr::simd_xsimd)
endif()

include(CTest)
if(BUILD_TESTING)
    add_test(NAME wjr_np_elementwise COMMAND wjr_np_elementwise)
    add_test(NAME wjr_np_expression COMMAND wjr_np_expression)
    add_test(NAME wjr_np_row_centering COMMAND wjr_np_row_centering)
    if(WJR_NP_EXAMPLES_XSIMD)
        add_test(NAME wjr_np_expression_simd COMMAND wjr_np_expression_simd)
    endif()
endif()
