cmake_minimum_required(VERSION 3.20)
project(wjr_arch_examples LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED)
add_executable(wjr_bounded_store bounded_store.cpp)
target_link_libraries(wjr_bounded_store PRIVATE wjr::json)

include(CTest)
if(BUILD_TESTING)
    foreach(count IN ITEMS 0 7 16)
        add_test(NAME wjr_bounded_store_${count} COMMAND wjr_bounded_store ${count})
    endforeach()
endif()
