cmake_minimum_required(VERSION 3.20)
set(WJR_VERSION 0.1.1)
project(wjr VERSION ${WJR_VERSION} LANGUAGES CXX)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 8))
   message(FATAL_ERROR "Not support non 64-bit currently, CMake will exit.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
   set(WJR_ARCH x86)
   message(STATUS "Target architecture: x86")
else()
   message(WARNING "Target architecture: ${CMAKE_SYSTEM_PROCESSOR} is not tested.")
endif()

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

include(CheckLanguage)
check_language(ASM_NASM)

if(CMAKE_ASM_NASM_COMPILER)
   set(WJR_ENABLE_ASSEMBLY_DEFAULT ON)
else()
   set(WJR_ENABLE_ASSEMBLY_DEFAULT OFF)
endif()

option(WJR_ENABLE_ASSEMBLY "Link with assembly by using NASM." ${WJR_ENABLE_ASSEMBLY_DEFAULT})
option(WJR_DISABLE_EXCEPTIONS "Disable exceptions." OFF)
option(WJR_DISABLE_CXX_20 "Disable C++ 20 even if it's supported." ON)
option(WJR_PUBLIC_FLAGS "Set compile flags as public. Useful in tests" OFF)
option(WJR_BUILD_TESTS "Build tests." OFF)

set(WJR_DEBUG_LEVEL_DEBUG "1" CACHE STRING "Debug level in debug mode(default = 1).")
set(WJR_DEBUG_LEVEL_RELEASE "0" CACHE STRING "Debug level in release mode(default = 0).")

if(WJR_DISABLE_EXCEPTIONS)
   message(STATUS "Disable exceptions.")
endif()

#
# third-party
#
set(FMT_TEST OFF CACHE BOOL "Disable fmt tests")
set(FMT_INSTALL ON)
add_subdirectory(third-party/fmt)

set(ENABLE_INSTALL ON)
add_subdirectory(third-party/cpu_features)

#
# initialize flags
# todo : More SIMD flags for MSVC.
#
set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/src)

set(WJR_CXX_FLAGS)
set(WJR_CXX_FLAGS_PRIVATE)
set(WJR_CXX_FLAGS_DEBUG)
set(WJR_CXX_FLAGS_RELEASE)
set(WJR_COMPILE_DEFINITIONS)

if(NOT WJR_DISABLE_CXX_20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
   message(STATUS "Enable C++ 20.")
   set(WJR_USE_CXX_20 ON)
else()
   set(WJR_USE_CXX_20 OFF)
endif()

include(cmake/AddFlags.cmake)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   list(APPEND WJR_CXX_FLAGS_PRIVATE /W3)
   list(APPEND WJR_CXX_FLAGS /Zc:preprocessor /Zc:lambda /EHsc /wd4146 /wd4309 /wd4244 /utf-8)
   list(APPEND WJR_CXX_FLAGS_DEBUG /DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_DEBUG})
   list(APPEND WJR_CXX_FLAGS_RELEASE /DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_RELEASE})

   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS /D_HAS_EXCEPTIONS=0 /DWJR_DISABLE_EXCEPTIONS)
   endif()

   if(WJR_LIGHT_ASSERT)
      list(APPEND WJR_CXX_FLAGS /DWJR_LIGHT_ASSERT)
   endif()
else()
   list(APPEND WJR_CXX_FLAGS_PRIVATE
      -Wall -Wextra -Wshadow -Wformat=2 -Wunused
   )

   AddFlagsIfExists(WJR_CXX_FLAGS_PRIVATE
      "-Waddress-of-packed-member -Wpointer-arith -Wcast-align -Wreorder -Wpacked -Wcast-qual")

   list(APPEND WJR_CXX_FLAGS -march=native)
   list(APPEND WJR_CXX_FLAGS_DEBUG -DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_DEBUG})
   list(APPEND WJR_CXX_FLAGS_RELEASE -DWJR_DEBUG_LEVEL=${WJR_DEBUG_LEVEL_RELEASE})

   if(WJR_DISABLE_EXCEPTIONS)
      list(APPEND WJR_CXX_FLAGS -fno-exceptions -DWJR_DISABLE_EXCEPTIONS)
   endif()

   if(WJR_LIGHT_ASSERT)
      list(APPEND WJR_CXX_FLAGS -DWJR_LIGHT_ASSERT)
   endif()
endif()

if(WJR_PUBLIC_FLAGS)
   list(APPEND WJR_CXX_FLAGS ${WJR_CXX_FLAGS_PRIVATE})
endif()

if(WJR_ENABLE_ASSEMBLY AND WJR_ARCH)
   enable_language(ASM_NASM)

   set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
   set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

   set(WJR_ENABLE_ARCH_ASSEMBLY ON)
   list(APPEND WJR_COMPILE_DEFINITIONS WJR_ENABLE_ASSEMBLY)
   message(STATUS "Enable assembly : ASM_NASM")
endif()

#
# macro
#
macro(setup_cpp_include_and_definitions TARGET)
   target_include_directories(${TARGET} PUBLIC
      $<BUILD_INTERFACE:${WJR_INCLUDE_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/wjr>
   )

   if(WJR_USE_CXX_20)
      target_compile_features(${TARGET} PUBLIC cxx_std_20)
   else()
      target_compile_features(${TARGET} PUBLIC cxx_std_17)
   endif()

   target_compile_options(${TARGET}
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS}$<SEMICOLON>>
      PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS_PRIVATE}$<SEMICOLON>>
      PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:${WJR_CXX_FLAGS_DEBUG}$<SEMICOLON>>
      PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:${WJR_CXX_FLAGS_RELEASE}$<SEMICOLON>>
   )

   target_compile_definitions(${TARGET}
      PUBLIC $<$<COMPILE_LANGUAGE:CXX>: ${WJR_COMPILE_DEFINITIONS}$<SEMICOLON>>
   )
endmacro()

macro(add_cpp_source TARGET PATH)
   file(GLOB_RECURSE TMP_SRCS ${WJR_LIB_DIR}/wjr/${PATH}/*.cpp ${WJR_LIB_DIR}/wjr/${PATH}/**/*.cpp)
   list(APPEND ${TARGET} ${TMP_SRCS})
endmacro()

set(WJR_LIBS)

#
# precompile headers
#
set(WJR_PCH_COMMON ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp ${WJR_INCLUDE_DIR}/wjr/assert.hpp)
set(WJR_PCH_SIMD)

if(WJR_ARCH)
   list(APPEND WJR_PCH_SIMD ${WJR_INCLUDE_DIR}/wjr/arch/${WJR_ARCH}/simd/simd.hpp)
endif()

#
# library : preprocessor
#

#
# library : config
#

#
# library : arch
#
if(WJR_ARCH)
   set(WJR_ARCH_SRCS)
   add_cpp_source(WJR_ARCH_SRCS arch/${WJR_ARCH})

   add_library(wjr-arch OBJECT ${WJR_ARCH_SRCS})

   target_include_directories(wjr-arch PRIVATE $<TARGET_PROPERTY:CpuFeatures::cpu_features,INTERFACE_INCLUDE_DIRECTORIES>)
   target_link_libraries(wjr-arch INTERFACE CpuFeatures::cpu_features)
   target_precompile_headers(wjr-arch PUBLIC ${WJR_PCH_COMMON} ${WJR_PCH_SIMD})

   list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr-arch>)
else()
   message(WARNING "No architecture is specified, so arch library is disabled.")
   add_library(wjr-arch INTERFACE)
endif()

add_library(wjr::arch ALIAS wjr-arch)
setup_cpp_include_and_definitions(wjr-arch)

#
# library : memory
#
set(WJR_SRCS)
add_cpp_source(WJR_SRCS memory)

add_library(wjr-memory OBJECT ${WJR_SRCS})
add_library(wjr::memory ALIAS wjr-memory)
setup_cpp_include_and_definitions(wjr-memory)

target_precompile_headers(wjr-memory PRIVATE ${WJR_PCH_COMMON})

list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr-memory>)

#
# library : format
#
set(WJR_SRCS)
add_cpp_source(WJR_SRCS format)

add_library(wjr-format OBJECT ${WJR_SRCS})
add_library(wjr::format ALIAS wjr-format)
setup_cpp_include_and_definitions(wjr-format)

target_include_directories(wjr-format PUBLIC $<TARGET_PROPERTY:wjr-biginteger,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_directories(wjr-format INTERFACE wjr-biginteger)
target_precompile_headers(wjr-format PRIVATE ${WJR_PCH_COMMON} ${WJR_PCH_SIMD})

list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr-format>)

#
# library : biginteger-assembly
#
if(WJR_ENABLE_ARCH_ASSEMBLY)
   set(WJR_ADDSUB_SRCS
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/addsub_n.asm
      ${WJR_LIB_DIR}/wjr/arch/${WJR_ARCH}/biginteger/detail/addrsblsh_n.asm
   )

   add_library(wjr-add_n OBJECT ${WJR_ADDSUB_SRCS})
   add_library(wjr-sub_n OBJECT ${WJR_ADDSUB_SRCS})

   target_compile_options(wjr-sub_n
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

   add_library(wjr-biginteger-assembly OBJECT ${WJR_ASSEMBLY_SRCS})
   add_library(wjr::biginteger-assembly ALIAS wjr-biginteger-assembly)
   list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr-biginteger-assembly> $<TARGET_OBJECTS:wjr-add_n> $<TARGET_OBJECTS:wjr-sub_n>)
endif()

#
# library : biginteger
#
set(WJR_SRCS)
add_cpp_source(WJR_SRCS biginteger)

add_library(wjr-biginteger OBJECT ${WJR_SRCS})
add_library(wjr::biginteger ALIAS wjr-biginteger)
setup_cpp_include_and_definitions(wjr-biginteger)

target_include_directories(wjr-biginteger PUBLIC $<TARGET_PROPERTY:fmt::fmt,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_directories(wjr-biginteger INTERFACE fmt::fmt)
target_precompile_headers(wjr-biginteger PRIVATE ${WJR_PCH_COMMON} ${WJR_PCH_SIMD})

list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr-biginteger>)

#
# library : json
#
set(WJR_SRCS)
add_cpp_source(WJR_SRCS json)

add_library(wjr-json OBJECT ${WJR_SRCS})
add_library(wjr::json ALIAS wjr-json)
setup_cpp_include_and_definitions(wjr-json)

target_include_directories(wjr-json PUBLIC $<TARGET_PROPERTY:wjr-format,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_directories(wjr-json INTERFACE wjr-format)
target_precompile_headers(wjr-json PRIVATE ${WJR_PCH_COMMON} ${WJR_PCH_SIMD})

list(APPEND WJR_LIBS $<TARGET_OBJECTS:wjr-json>)

#
# library : wjr
#
set(WJR_SRCS ${WJR_LIB_DIR}/wjr/assert.cpp)
list(APPEND WJR_LIBS ${WJR_SRCS})

add_library(wjr STATIC ${WJR_LIBS})
add_library(wjr::wjr ALIAS wjr)
setup_cpp_include_and_definitions(wjr)
target_link_libraries(wjr PUBLIC fmt::fmt PRIVATE CpuFeatures::cpu_features)

target_precompile_headers(wjr PUBLIC ${WJR_PCH_COMMON})

# Build tests
if(WJR_BUILD_TESTS)
   enable_testing()
   add_subdirectory(test/units)
endif()

# install
set(PackageName wjr)
set(INCLUDE_INSTALL_DIR include)
set(LIB_INSTALL_DIR lib)
set(LIB_NAME ${PackageName})
set(dest ${LIB_INSTALL_DIR}/cmake/${PackageName})

include(CMakePackageConfigHelpers)
configure_package_config_file(
   cmake/${PackageName}Config.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/${PackageName}Config.cmake
   INSTALL_DESTINATION ${dest}
   PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(
   ${CMAKE_CURRENT_BINARY_DIR}/${PackageName}ConfigVersion.cmake
   VERSION 1.2.3
   COMPATIBILITY AnyNewerVersion
)

install(
   TARGETS ${LIB_NAME}
   EXPORT ${PackageName}Targets
   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wjr
)

install(
   DIRECTORY ${CMAKE_SOURCE_DIR}/include/
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
   EXPORT ${PackageName}Targets
   NAMESPACE wjr::
   DESTINATION ${dest}
)

install(
   FILES
   ${CMAKE_CURRENT_BINARY_DIR}/${PackageName}Config.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/${PackageName}ConfigVersion.cmake
   DESTINATION ${dest}
)
