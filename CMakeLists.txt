cmake_minimum_required(VERSION 3.14)
project(wjr LANGUAGES CXX)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 8))
   message(FATAL_ERROR "Not support non 64-bit currently, CMake will exit.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
   set(WJR_ARCH x86)
endif()

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

option(WJR_ENABLE_ASSEMBLY "Link with assembly by using NASM" OFF)
option(WJR_DISABLE_EXCEPTIONS "Disable exceptions" ON)
option(WJR_DISABLE_CXX_20 "Disable C++ 20 even if it's supported." ON)

if (DEFINED WJR_DEBUG_LEVEL AND (NOT DEFINED WJR_DEBUG_LEVEL_DEBUG))
   set(WJR_DEBUG_LEVEL_DEBUG ${WJR_DEBUG_LEVEL})
endif()

if (DEFINED WJR_DEBUG_LEVEL AND (NOT DEFINED WJR_DEBUG_LEVEL_RELEASE))
   set(WJR_DEBUG_LEVEL_RELEASE ${WJR_DEBUG_LEVEL})
endif()

if (NOT DEFINED WJR_DEBUG_LEVEL_DEBUG)
   set(WJR_DEBUG_LEVEL_DEBUG 3)
else()
   set(WJR_DEBUG_LEVEL_DEBUG ${WJR_DEBUG_LEVEL_DEBUG})
endif()

if (NOT DEFINED WJR_DEBUG_LEVEL_RELEASE)
   set(WJR_DEBUG_LEVEL_RELEASE 0)
else()
   set(WJR_DEBUG_LEVEL_RELEASE ${WJR_DEBUG_LEVEL_RELEASE})
endif()

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/src)

macro(WJR_ADD_SRCS TARGET PATH)
   file(GLOB_RECURSE TMP_SRCS ${WJR_LIB_DIR}/wjr/${PATH}/*.cpp)
   list(APPEND ${TARGET} ${TMP_SRCS})
endmacro()

set(WJR_SRCS ${WJR_LIB_DIR}/wjr/assert.cpp)
WJR_ADD_SRCS(WJR_SRCS biginteger)
WJR_ADD_SRCS(WJR_SRCS format)
WJR_ADD_SRCS(WJR_SRCS json)
WJR_ADD_SRCS(WJR_SRCS math)
WJR_ADD_SRCS(WJR_SRCS memory)

if(WJR_ARCH)
   WJR_ADD_SRCS(WJR_SRCS arch/${WJR_ARCH})
endif()

set(WJR_LIBS "")
set(WJR_ASSEMBLY_LIBS "")
set(WJR_COMPILE_DEFINITIONS "")

if(WJR_ENABLE_ASSEMBLY)
   set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
   enable_language(ASM_NASM)
   set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

   if(WJR_ARCH)
      set(WJR_ASSEMBLY_SRCS
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/basecase_mul_s.asm
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/basecase_sqr.asm
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/divexact_dbm1c.asm
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/mul_1.asm
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/addmul_1.asm
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/submul_1.asm
      )

      set(WJR_ADDSUB_SRCS
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/addsub_n.asm
         ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/math/addrsblsh_n.asm
      )

      add_library(wjr_asm OBJECT ${WJR_ASSEMBLY_SRCS})
      add_library(wjr_add_n OBJECT ${WJR_ADDSUB_SRCS})
      add_library(wjr_sub_n OBJECT ${WJR_ADDSUB_SRCS})
      
      target_compile_options(wjr_sub_n
         PRIVATE $<$<COMPILE_LANGUAGE:ASM_NASM>: -dWJR_ASM_SUB>
      )

      list(APPEND WJR_ASSEMBLY_LIBS wjr_asm wjr_add_n wjr_sub_n)
      list(APPEND WJR_COMPILE_DEFINITIONS WJR_ENABLE_ASSEMBLY)

      message("Enable ${WJR_ARCH} assembly")
   endif()
endif()

add_library(wjr STATIC ${WJR_SRCS})

target_include_directories(wjr PUBLIC ${WJR_INCLUDE_DIR})

set(WJR_CXX_FLAGS)
set(WJR_CXX_FLAGS_DEBUG)
set(WJR_CXX_FLAGS_RELEASE)
set(WJR_CXX_LINKER_FLAGS)

if(MSVC)
   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS /D_HAS_EXCEPTIONS:0)
   endif()

   list(APPEND WJR_CXX_FLAGS /W3 /Zc:preprocessor /Zc:lambda /EHsc)

   list(APPEND WJR_CXX_FLAGS_DEBUG /DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_DEBUG})
   list(APPEND WJR_CXX_FLAGS_RELEASE /DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_RELEASE})

   if (WIN32)
      list(APPEND WJR_CXX_LINKER_FLAGS /lsynchronization)
   endif()
else()
   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS -fno-exceptions)
   endif()

   list(APPEND WJR_CXX_FLAGS -march=native -Wall -Wextra -Wshadow -Wformat=2 -Wunused)

   list(APPEND WJR_CXX_FLAGS_DEBUG -DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_DEBUG})
   list(APPEND WJR_CXX_FLAGS_RELEASE -DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_RELEASE})

   if (WIN32)
      list(APPEND WJR_CXX_LINKER_FLAGS -lsynchronization)
   endif()
endif()

if (NOT WJR_DISABLE_CXX_20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
   message("Enable C++ 20.")
   set (WJR_USE_CXX_20 ON)
else()
   set (WJR_USE_CXX_20 OFF)
endif()

if (WJR_USE_CXX_20)
   target_compile_features(wjr PUBLIC cxx_std_20)
else()
   target_compile_features(wjr PUBLIC cxx_std_17)
endif()

target_compile_options(wjr
   PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS}$<SEMICOLON>>
   PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:${WJR_CXX_FLAGS_DEBUG}$<SEMICOLON>>
   PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:${WJR_CXX_FLAGS_RELEASE}$<SEMICOLON>>
)

target_link_options(wjr
   PUBLIC $<$<COMPILE_LANGUAGE:CXX>: LINKER: ${WJR_CXX_LINKER_FLAGS} $<SEMICOLON>>
)

target_compile_definitions(wjr
   PUBLIC $<$<COMPILE_LANGUAGE:CXX>: ${WJR_COMPILE_DEFINITIONS}$<SEMICOLON>>
)

if(WJR_DISABLE_EXCEPTIONS)
   target_compile_definitions(wjr
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -DWJR_DISABLE_EXCEPTIONS >
   )
endif()

if(WJR_LIGHT_ASSERT)
   target_compile_definitions(wjr
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -DWJR_LIGHT_ASSERT >
   )
endif()

if (WJR_USE_CXX_20)
   set(WJR_ATOMIC_LIBS)
else()
   add_subdirectory(third-party/boost/config)
   add_subdirectory(third-party/boost/predef)
   if (WIN32)
      add_subdirectory(third-party/boost/winapi)
   endif()
   add_subdirectory(third-party/boost/atomic)

   if(WJR_DISABLE_EXCEPTIONS)
      target_compile_definitions(boost_atomic PUBLIC BOOST_NO_EXCEPTIONS)
   endif()
   
   set(WJR_ATOMIC_LIBS Boost::atomic)
endif()

target_link_libraries(wjr 
   PUBLIC
      ${WJR_LIBS}
      ${WJR_ASSEMBLY_LIBS}
      ${WJR_ATOMIC_LIBS}
)

set(WJR_PCH PUBLIC ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp ${WJR_INCLUDE_DIR}/wjr/assert.hpp)
target_precompile_headers(wjr PUBLIC ${WJR_PCH})