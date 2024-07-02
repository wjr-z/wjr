cmake_minimum_required(VERSION 3.14)
project(wjr
   LANGUAGES CXX
)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 8))
   message(FATAL_ERROR "Not support non 64-bit currently, CMake will exit.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
   set(WJR_X86 TRUE)
else()
   set(WJR_X86 FALSE)
endif()

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

add_library(wjr STATIC ${WJR_SRCS})

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

set(WJR_PCH PUBLIC ${WJR_INCLUDE_DIR}/wjr/preprocessor.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/assert.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp)

if(WJR_X86)
   list(APPEND ${WJR_PCH} PUBLIC ${WJR_INCLUDE_DIR}/wjr/x86/simd/intrin.hpp PUBLIC ${WJR_INCLUDE_DIR}/wjr/x86/simd/simd.hpp)
endif()

target_precompile_headers(
   wjr
   ${WJR_PCH}
)
