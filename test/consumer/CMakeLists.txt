cmake_minimum_required(VERSION 3.20)
project(wjr_consumer LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED)

add_executable(wjr_consumer main.cpp)
target_link_libraries(wjr_consumer PRIVATE wjr::wjr)

add_executable(wjr_module_consumer main.cpp)
target_link_libraries(wjr_module_consumer PRIVATE wjr::json wjr::ring_buffer)

enable_testing()
add_test(NAME wjr_consumer COMMAND wjr_consumer)
add_test(NAME wjr_module_consumer COMMAND wjr_module_consumer)
