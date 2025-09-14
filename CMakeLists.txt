cmake_minimum_required(VERSION 3.21)
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
   option(WJR_ENABLE_INSTALL "Enable install rule" OFF)
else()
   option(WJR_ENABLE_INSTALL "Enable install rule" ON)
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

list(APPEND CMAKE_MODULE_PATH
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(AddFlags)

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
# precompile headers
#
set(WJR_PCH_COMMON ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp ${WJR_INCLUDE_DIR}/wjr/assert.hpp)
set(WJR_PCH_SIMD)

if(WJR_ARCH)
   list(APPEND WJR_PCH_SIMD ${WJR_INCLUDE_DIR}/wjr/arch/${WJR_ARCH}/simd/simd.hpp)
endif()

include(wjrHelpers)

set(WJR_COMMON_INCLUDE_DIRS ${WJR_INCLUDE_DIR})
set(WJR_COMMON_CXX_FLAGS ${WJR_CXX_FLAGS})
set(WJR_COMMON_DEFINES ${WJR_COMPILE_DEFINITIONS})

set(WJR_PROPAGATE_CXX_STD ON)

if(WJR_USE_CXX_20)
   set(WJR_INTERNAL_CXX_STD_FEATURE cxx_std_20)
else()
   set(WJR_INTERNAL_CXX_STD_FEATURE cxx_std_17)
endif()

set(WJR_VERSION "0.0.6")

#
# library : preprocessor
#

#
# library : config
#
add_subdirectory(src/config)

#
# library : utility
#
add_subdirectory(src/utility)

#
# library : arch
#
add_subdirectory(src/arch)

#
# library : memory
#
add_subdirectory(src/memory)

#
# library : format-internal
#
add_subdirectory(src/format-internal)

#
# library : biginteger
#
add_subdirectory(src/biginteger)

#
# library : json
#
add_subdirectory(src/json)

#
# library : wjr
#
wjr_cc_library(
   NAME
   wjr

   DEPS
   $<BUILD_INTERFACE:wjr-json>

   FINAL
)

# Build tests
if(WJR_BUILD_TESTS)
   message(STATUS "Build tests.")

   enable_testing()
   add_subdirectory(test)
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
