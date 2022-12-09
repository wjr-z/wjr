cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "D:/LLVM/bin/clang.exe")
set(CMAKE_CXX_COMPILER "D:/LLVM/bin/clang++.exe")

project(wjr2 LANGUAGES CXX)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)
set(WJR_SIMD_DIR ${WJR_LIB_DIR}/simd)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17")
endif() 

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://ghproxy.com/https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(wjr 
                ${PROJECT_SOURCE_DIR}/src/unit_vector.cpp 
                ${PROJECT_SOURCE_DIR}/src/unit_algo.cpp
				${WJR_SRCS}
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ${WJR_SIMD_DIR}
                           ) 

target_link_libraries(
  wjr
  gtest_main
)

include(GoogleTest)
#gtest_discover_tests(wjr)