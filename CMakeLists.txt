cmake_minimum_required(VERSION 3.21)
project(wjr VERSION 0.1.1 LANGUAGES CXX)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 8))
   message(FATAL_ERROR "Not support non 64-bit currently, CMake will exit.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
   set(WJR_ARCH x86)
endif()

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

option(WJR_ENABLE_ASSEMBLY "Link with assembly by using NASM." OFF)
option(WJR_DISABLE_EXCEPTIONS "Disable exceptions." ON)
option(WJR_DISABLE_CXX_20 "Disable C++ 20 even if it's supported." ON)
option(WJR_DEBUG_LEVEL_RELEASE "Debug level in release version(default = 0)." 0)
option(WJR_DEBUG_LEVEL_DEBUG "Debug level in debug version(default = 3)." 3)

#
# third-party
#
add_subdirectory(third-party/cpu_features)

#
# initialize flags
# todo : More SIMD flags for MSVC.
#
set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/src)

set(WJR_CXX_FLAGS)
set(WJR_CXX_FLAGS_DEBUG)
set(WJR_CXX_FLAGS_RELEASE)
set(WJR_CXX_LINKER_FLAGS)
set(WJR_COMPILE_DEFINITIONS)

if(NOT WJR_DISABLE_CXX_20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
   message("Enable C++ 20.")
   set(WJR_USE_CXX_20 ON)
else()
   set(WJR_USE_CXX_20 OFF)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   list(APPEND WJR_CXX_FLAGS /W3 /Zc:preprocessor /Zc:lambda /EHsc /wd4146 /wd4309 /wd4244)
   list(APPEND WJR_CXX_FLAGS_DEBUG /DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_DEBUG})
   list(APPEND WJR_CXX_FLAGS_RELEASE /DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_RELEASE})

   if(WIN32)
      list(APPEND WJR_CXX_LINKER_FLAGS /lsynchronization)
   endif()

   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS /D_HAS_EXCEPTIONS=0 /DWJR_DISABLE_EXCEPTIONS)
   endif()

   if(WJR_LIGHT_ASSERT)
      list(APPEND WJR_CXX_FLAGS /DWJR_LIGHT_ASSERT)
   endif()
else()
   list(APPEND WJR_CXX_FLAGS -march=native -Wall -Wextra -Wshadow -Wformat=2 -Wunused -Waddress-of-packed-member)
   list(APPEND WJR_CXX_FLAGS_DEBUG -DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_DEBUG})
   list(APPEND WJR_CXX_FLAGS_RELEASE -DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_RELEASE})

   if(WIN32)
      list(APPEND WJR_CXX_LINKER_FLAGS -lsynchronization)
   endif()

   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS -fno-exceptions -DWJR_DISABLE_EXCEPTIONS)
   endif()

   if(WJR_LIGHT_ASSERT)
      list(APPEND WJR_CXX_FLAGS -DWJR_LIGHT_ASSERT)
   endif()
endif()

if(WJR_ENABLE_ASSEMBLY AND WJR_ARCH)
   set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
   enable_language(ASM_NASM)
   set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

   set(WJR_ENABLE_ARCH_ASSEMBLY ON)
   set(WJR_COMPILE_DEFINITIONS WJR_ENABLE_ASSEMBLY)
   message("Enable ${WJR_ARCH} assembly.")
endif()

#
# macro
#
macro(setup_cpp_include_and_definitions TARGET)
   target_include_directories(${TARGET} PUBLIC ${WJR_INCLUDE_DIR})

   if(WJR_USE_CXX_20)
      target_compile_features(${TARGET} PUBLIC cxx_std_20)
   else()
      target_compile_features(${TARGET} PUBLIC cxx_std_17)
   endif()

   target_compile_options(${TARGET}
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS}$<SEMICOLON>>
      PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:${WJR_CXX_FLAGS_DEBUG}$<SEMICOLON>>
      PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:${WJR_CXX_FLAGS_RELEASE}$<SEMICOLON>>
   )

   target_link_options(${TARGET}
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>: LINKER: ${WJR_CXX_LINKER_FLAGS} $<SEMICOLON>>
   )

   target_compile_definitions(${TARGET}
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>: ${WJR_COMPILE_DEFINITIONS}$<SEMICOLON>>
   )
endmacro()

macro(wjr_add_cpp_source TARGET PATH)
   file(GLOB_RECURSE TMP_SRCS ${WJR_LIB_DIR}/wjr/${PATH}/*.cpp ${WJR_LIB_DIR}/wjr/${PATH}/**/*.cpp)
   list(APPEND ${TARGET} ${TMP_SRCS})
endmacro()

#
# precompile headers
#
set(WJR_PCH_COMMON PUBLIC ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp ${WJR_INCLUDE_DIR}/wjr/assert.hpp)

#
# library : preprocessor
#

#
# library : assembly
#
if(WJR_ENABLE_ARCH_ASSEMBLY)
   set(WJR_ADDSUB_SRCS
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/addsub_n.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/addrsblsh_n.asm
   )

   add_library(wjr_add_n OBJECT ${WJR_ADDSUB_SRCS})
   add_library(wjr_sub_n OBJECT ${WJR_ADDSUB_SRCS})

   target_compile_options(wjr_sub_n
      PRIVATE $<$<COMPILE_LANGUAGE:ASM_NASM>: -dWJR_ASM_SUB>
   )

   set(WJR_ASSEMBLY_SRCS
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/basecase_mul_s.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/basecase_sqr.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/divexact_dbm1c.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/mul_1.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/addmul_1.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/submul_1.asm
   )

   add_library(wjr_assembly OBJECT ${WJR_ASSEMBLY_SRCS})
   target_link_libraries(wjr_assembly INTERFACE $<TARGET_OBJECTS:wjr_add_n> $<TARGET_OBJECTS:wjr_sub_n>)
endif()

#
# library : arch
#
if(WJR_ARCH)
   set(WJR_ARCH_SRCS)
   wjr_add_cpp_source(WJR_ARCH_SRCS arch/${WJR_ARCH})

   add_library(wjr_arch OBJECT ${WJR_ARCH_SRCS})
   setup_cpp_include_and_definitions(wjr_arch)

   set(WJR_ARCH_LIBS)

   if(WJR_ENABLE_ARCH_ASSEMBLY)
      list(APPEND WJR_ARCH_LIBS $<TARGET_OBJECTS:wjr_assembly>)
   endif()

   target_link_libraries(wjr_arch PRIVATE CpuFeatures::cpu_features INTERFACE ${WJR_ARCH_LIBS})
   target_precompile_headers(wjr_arch INTERFACE ${WJR_PCH_COMMON})
endif()

#
# library : wjr
#
set(WJR_SRCS ${WJR_LIB_DIR}/wjr/assert.cpp)
wjr_add_cpp_source(WJR_SRCS biginteger)
wjr_add_cpp_source(WJR_SRCS format)
wjr_add_cpp_source(WJR_SRCS json)
wjr_add_cpp_source(WJR_SRCS memory)

set(WJR_LIBS ${WJR_SRCS})

if(WJR_ARCH)
   list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr_arch>)
endif()

add_library(wjr STATIC ${WJR_LIBS})
setup_cpp_include_and_definitions(wjr)
target_link_libraries(wjr PRIVATE CpuFeatures::cpu_features)
add_library(wjr::wjr ALIAS wjr)
target_precompile_headers(wjr PRIVATE ${WJR_PCH_COMMON})