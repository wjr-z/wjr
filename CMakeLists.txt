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

option(WJR_ENABLE_ASSEMBLY "Link with assembly by using NASM" OFF)

if(MSVC)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /std:c++17 /Wall /std:c11 /Zc:preprocessor /Zc:lambda /EHsc")

   if(WJR_X86)
      list(APPEND ${CMAKE_CXX_FLAGS} " /arch:AVX2")
   endif()

else()
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++17 -march=native -Wall -Wextra")
endif()

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

set(WJR_LIBS "")

if(WJR_ENABLE_ASSEMBLY)
   set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
   enable_language(ASM_NASM)
   set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

   list(APPEND WJR_SRCS
      ${WJR_LIB_DIR}/wjr/x86/math/basecase_mul_s.asm
      ${WJR_LIB_DIR}/wjr/x86/math/basecase_sqr.asm
      ${WJR_LIB_DIR}/wjr/x86/math/divexact_dbm1c.asm
      ${WJR_LIB_DIR}/wjr/x86/math/mul_1.asm
      ${WJR_LIB_DIR}/wjr/x86/math/addmul_1.asm
      ${WJR_LIB_DIR}/wjr/x86/math/submul_1.asm
   )

   set(WJR_ADDSUB
      ${WJR_LIB_DIR}/wjr/math/addsub_n.asm
      ${WJR_LIB_DIR}/wjr/math/addrsblsh_n.asm
   )

   add_library(wjr_add_n OBJECT ${WJR_ADDSUB})
   add_library(wjr_sub_n OBJECT ${WJR_ADDSUB})
   target_compile_options(wjr_sub_n PRIVATE
      $<$<COMPILE_LANGUAGE:ASM_NASM>: -dWJR_ASM_SUB>
   )

   list(APPEND WJR_LIBS wjr_add_n wjr_sub_n)

   unset(WJR_ADDSUB)

   add_definitions(-DWJR_ENABLE_ASSEMBLY)
   message("enable assembly")
endif()

add_library(wjr STATIC ${WJR_SRCS})

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

target_link_libraries(wjr PRIVATE
   ${WJR_LIBS}
)

set(WJR_PCH PUBLIC ${WJR_INCLUDE_DIR}/wjr/preprocessor.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/assert.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp)

if(WJR_X86)
   list(APPEND WJR_PCH PUBLIC ${WJR_INCLUDE_DIR}/wjr/x86/simd/intrin.hpp PUBLIC ${WJR_INCLUDE_DIR}/wjr/x86/simd/simd.hpp)
endif()

target_precompile_headers(
   wjr
   ${WJR_PCH}
)
