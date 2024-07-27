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
option(WJR_ENABLE_INLINE_ASM "Enable inline assembly in GCC/Clang" ON)
option(WJR_DISABLE_EXCEPTIONS "Disable exceptions" OFF)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/src)

macro(WJR_ADD_SRCS TARGET PATH)
   file(GLOB_RECURSE TMP_SRCS ${WJR_LIB_DIR}/wjr/${PATH}/*.cpp)
   list(APPEND ${TARGET} ${TMP_SRCS})
endmacro()

set(WJR_SRCS ${WJR_LIB_DIR}/wjr/wjr.cpp)
WJR_ADD_SRCS(WJR_SRCS biginteger)
WJR_ADD_SRCS(WJR_SRCS format)
WJR_ADD_SRCS(WJR_SRCS json)
WJR_ADD_SRCS(WJR_SRCS math)
WJR_ADD_SRCS(WJR_SRCS memory)

if(WJR_X86)
   WJR_ADD_SRCS(WJR_SRCS x86)
endif()

message(${WJR_SRCS})

set(WJR_LIBS "")
set(WJR_COMPILE_DEFINITIONS "")

if(WJR_ENABLE_ASSEMBLY)
   set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
   enable_language(ASM_NASM)
   set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

   if(WJR_X86)
      list(APPEND WJR_SRCS
         ${WJR_LIB_DIR}/wjr/x86/math/basecase_mul_s.asm
         ${WJR_LIB_DIR}/wjr/x86/math/basecase_sqr.asm
         ${WJR_LIB_DIR}/wjr/x86/math/divexact_dbm1c.asm
         ${WJR_LIB_DIR}/wjr/x86/math/mul_1.asm
         ${WJR_LIB_DIR}/wjr/x86/math/addmul_1.asm
         ${WJR_LIB_DIR}/wjr/x86/math/submul_1.asm
      )

      set(WJR_ADDSUB
         ${WJR_LIB_DIR}/wjr/x86/math/addsub_n.asm
         ${WJR_LIB_DIR}/wjr/x86/math/addrsblsh_n.asm
      )

      add_library(wjr_add_n OBJECT ${WJR_ADDSUB})
      add_library(wjr_sub_n OBJECT ${WJR_ADDSUB})
      target_compile_options(wjr_sub_n
         PRIVATE $<$<COMPILE_LANGUAGE:ASM_NASM>: -dWJR_ASM_SUB>
      )

      list(APPEND WJR_LIBS wjr_add_n wjr_sub_n)
      list(APPEND WJR_COMPILE_DEFINITIONS WJR_ENABLE_ASSEMBLY)

      message("enable x86 assembly")
   endif()
endif()

add_library(wjr STATIC ${WJR_SRCS})

set(WJR_CXX_FLAGS)
set(WJR_CXX_FLAGS_DEBUG)
set(WJR_CXX_FLAGS_RELEASE)

if(MSVC)
   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS /D_HAS_EXCEPTIONS:0)
   endif()

   list(APPEND WJR_CXX_FLAGS /Wall /Zc:preprocessor /Zc:lambda /EHsc)

   list(APPEND WJR_CXX_FLAGS_DEBUG /DWJR_DEBUG_LEVEL=3)
   list(APPEND WJR_CXX_FLAGS_RELEASE /DWJR_DEBUG_LEVEL=0)

   if(WJR_X86)
      list(APPEND WJR_CXX_FLAGS /arch:AVX2)
   endif()
else()
   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS -fno-exceptions)
   endif()

   list(APPEND WJR_CXX_FLAGS -march=native -Wall -Wextra -Wshadow -Wformat=2 -Wunused)

   list(APPEND WJR_CXX_FLAGS_DEBUG -DWJR_DEBUG_LEVEL=3)
   list(APPEND WJR_CXX_FLAGS_RELEASE -DWJR_DEBUG_LEVEL=0)
endif()

target_compile_features(wjr PUBLIC cxx_std_17)

target_compile_options(wjr
   PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS}$<SEMICOLON>>
   PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:${WJR_CXX_FLAGS_DEBUG}$<SEMICOLON>>
   PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:${WJR_CXX_FLAGS_RELEASE}$<SEMICOLON>>
)

target_compile_definitions(wjr
   PUBLIC $<$<COMPILE_LANGUAGE:CXX>: ${WJR_COMPILE_DEFINITIONS}$<SEMICOLON>>
)

if(WJR_DISABLE_EXCEPTIONS)
   target_compile_definitions(wjr
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -DWJR_DISABLE_EXCEPTIONS>
   )
endif()

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

target_link_libraries(wjr PRIVATE
   ${WJR_LIBS}
)

set(WJR_PCH PUBLIC ${WJR_INCLUDE_DIR}/wjr/math.hpp)

target_precompile_headers(
   wjr
   ${WJR_PCH}
)