cmake_minimum_required(VERSION 3.22)
project(wjr_np_examples LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED COMPONENTS np)

add_executable(wjr_np_elementwise elementwise.cpp)
target_link_libraries(wjr_np_elementwise PRIVATE wjr::np)

add_executable(wjr_np_expression expression.cpp)
target_link_libraries(wjr_np_expression PRIVATE wjr::np)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME wjr_np_elementwise COMMAND wjr_np_elementwise)
    add_test(NAME wjr_np_expression COMMAND wjr_np_expression)
endif()
