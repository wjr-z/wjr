cmake_minimum_required(VERSION 3.20)
project(wjr_json_pool_example LANGUAGES CXX)

find_package(wjr CONFIG REQUIRED COMPONENTS json)
add_executable(json_pool_snapshots pool_snapshots.cpp)
target_compile_features(json_pool_snapshots PRIVATE cxx_std_20)
target_link_libraries(json_pool_snapshots PRIVATE wjr::json)
