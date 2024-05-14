cmake_minimum_required(VERSION 3.10)
project(wjr LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /Zc:preprocessor")
else()
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -DNDEBUG -DWJR_DEBUG_LEVEL=0 -march=native -Wall -Wextra")
endif()

message(${CMAKE_CXX_FLAGS})

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/../../src)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(
   wjr
   calc.cpp
   ${WJR_SRCS}
)

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

target_link_libraries(
   wjr
)