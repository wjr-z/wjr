cmake_minimum_required(VERSION 3.14)
project(wjr
   LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /arch:AVX2 /Wall /std:c11 /Zc:preprocessor /Zc:lambda /EHsc")
else()
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -Wall -Wextra")
endif()

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

target_precompile_headers(
   wjr
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/preprocessor.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/assert.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/x86/simd/simd.hpp
)

add_library(wjr STATIC ${WJR_SRCS})