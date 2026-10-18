cmake_minimum_required(VERSION 3.20)
project(wjr_tbb_example LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED COMPONENTS tbb)
add_executable(wjr_tbb_batches batches.cpp)
target_link_libraries(wjr_tbb_batches PRIVATE wjr::tbb)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME wjr_tbb_batches COMMAND wjr_tbb_batches)
endif()
