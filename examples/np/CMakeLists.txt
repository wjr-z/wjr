cmake_minimum_required(VERSION 3.22)
project(wjr_np_examples LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED COMPONENTS np)

add_executable(wjr_np_elementwise elementwise.cpp)
target_link_libraries(wjr_np_elementwise PRIVATE wjr::np)

add_executable(wjr_np_expression expression.cpp)
target_link_libraries(wjr_np_expression PRIVATE wjr::np)

add_executable(wjr_np_row_centering row_centering.cpp)
target_link_libraries(wjr_np_row_centering PRIVATE wjr::np)

add_executable(wjr_np_prefix_sums prefix_sums.cpp)
target_link_libraries(wjr_np_prefix_sums PRIVATE wjr::np)

add_executable(wjr_np_expression_scan expression_scan.cpp)
target_link_libraries(wjr_np_expression_scan PRIVATE wjr::np)

option(WJR_NP_EXAMPLES_XSIMD "Build the explicit SIMD expression example" OFF)
if(WJR_NP_EXAMPLES_XSIMD)
    find_package(wjr CONFIG REQUIRED COMPONENTS simd_xsimd)
    add_executable(wjr_np_expression_simd expression_simd.cpp)
    target_link_libraries(wjr_np_expression_simd PRIVATE wjr::np wjr::simd_xsimd)
endif()

add_executable(wjr_np_evaluate evaluate.cpp)
target_link_libraries(wjr_np_evaluate PRIVATE wjr::np)

add_executable(wjr_np_expression_reduce expression_reduce.cpp)
target_link_libraries(wjr_np_expression_reduce PRIVATE wjr::np)

add_executable(wjr_np_expression_cast expression_cast.cpp)
target_link_libraries(wjr_np_expression_cast PRIVATE wjr::np)

add_executable(wjr_np_expression_map expression_map.cpp)
target_link_libraries(wjr_np_expression_map PRIVATE wjr::np)

add_executable(wjr_np_expression_mask expression_mask.cpp)
target_link_libraries(wjr_np_expression_mask PRIVATE wjr::np)

add_executable(wjr_np_strided_tensor strided_tensor.cpp)
target_link_libraries(wjr_np_strided_tensor PRIVATE wjr::np)

add_executable(wjr_np_expression_math expression_math.cpp)
target_link_libraries(wjr_np_expression_math PRIVATE wjr::np)

add_executable(wjr_np_slice slice.cpp)
target_link_libraries(wjr_np_slice PRIVATE wjr::np)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME wjr_np_elementwise COMMAND wjr_np_elementwise)
    add_test(NAME wjr_np_expression COMMAND wjr_np_expression)
    add_test(NAME wjr_np_row_centering COMMAND wjr_np_row_centering)
    add_test(NAME wjr_np_prefix_sums COMMAND wjr_np_prefix_sums)
    add_test(NAME wjr_np_expression_scan COMMAND wjr_np_expression_scan)
    if(WJR_NP_EXAMPLES_XSIMD)
        add_test(NAME wjr_np_expression_simd COMMAND wjr_np_expression_simd)
    endif()
    add_test(NAME wjr_np_evaluate COMMAND wjr_np_evaluate)
    add_test(NAME wjr_np_expression_reduce COMMAND wjr_np_expression_reduce)
    add_test(NAME wjr_np_expression_cast COMMAND wjr_np_expression_cast)
    add_test(NAME wjr_np_expression_map COMMAND wjr_np_expression_map)
    add_test(NAME wjr_np_expression_mask COMMAND wjr_np_expression_mask)


    add_test(NAME wjr_np_strided_tensor COMMAND wjr_np_strided_tensor)
    add_test(NAME wjr_np_expression_math COMMAND wjr_np_expression_math)

    add_test(NAME wjr_np_slice COMMAND wjr_np_slice)
endif()
