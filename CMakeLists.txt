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
   option(WJR_ENABLE_ASSEMBLY "Link with assembly by using NASM." ON)
else()
   option(WJR_ENABLE_ASSEMBLY "Link with assembly by using NASM." OFF)
endif()

option(WJR_DISABLE_EXCEPTIONS "Disable exceptions." OFF)
option(WJR_DISABLE_CXX_20 "Disable C++ 20 even if it's supported." ON)
option(WJR_PUBLIC_FLAGS "Set compile flags as public. Useful in tests" OFF)
option(WJR_BUILD_TESTS "Build tests." OFF)
option(WJR_BUILD_BENCHMARKS "Build benchmarks." OFF)
option(WJR_ENABLE_ASSERT_BACKTRACE "Enable assert backtrace." OFF)

set(WJR_DEBUG_LEVEL_DEBUG "1" CACHE STRING "Debug level in debug mode(default = 1).")
set(WJR_DEBUG_LEVEL_RELEASE "0" CACHE STRING "Debug level in release mode(default = 0).")

if(WJR_DISABLE_EXCEPTIONS)
   message(STATUS "Disable exceptions.")
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
   option(WJR_ENABLE_INSTALL "Enable install rule" OFF)
else()
   option(WJR_ENABLE_INSTALL "Enable install rule" ON)
endif()

option(WJR_ENABLE_COVERAGE "Enable coverage for tests." OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

#
# third-party
#
if(WJR_ENABLE_INSTALL)
   set(FMT_INSTALL ON)
   set(ENABLE_INSTALL ON)
endif()

if(NOT TARGET fmt)
   set(FMT_TEST OFF CACHE BOOL "Disable fmt tests")
   add_subdirectory(third-party/fmt)
endif()

if(NOT TARGET CpuFeatures)
   add_subdirectory(third-party/cpu_features)
endif()

include(FetchContent)

# Fetch range-v3
FetchContent_Declare(
   range-v3
   GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
   GIT_TAG 0.12.0
   GIT_SHALLOW ON
   FIND_PACKAGE_ARGS 0.10.0
)

set(RANGES_NATIVE OFF CACHE BOOL "" FORCE)
set(RANGES_TEST OFF CACHE BOOL "" FORCE)
set(RANGES_EXAMPLE OFF CACHE BOOL "" FORCE)
set(RANGES_PERF OFF CACHE BOOL "" FORCE)

if(WJR_ENABLE_INSTALL)
   set(RANGES_ENABLE_INSTALL ON CACHE BOOL "" FORCE)
else()
   set(RANGES_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(range-v3)

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
   ${CMAKE_CURRENT_LIST_DIR}/cmake
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

   if(WJR_ENABLE_COVERAGE)
      list(APPEND WJR_CXX_FLAGS_PRIVATE -fprofile-instr-generate -fcoverage-mapping -fno-inline)
      set(WJR_DEFAULT_LINKOPTS "-fprofile-instr-generate -fcoverage-mapping")
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

set_property(GLOBAL PROPERTY WJR_ALL_TARGETS "")

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
# library : iterator
#
add_subdirectory(src/iterator)

#
# library : arch
#
add_subdirectory(src/arch)

#
# library : memory
#
add_subdirectory(src/memory)

#
# library : math
#
add_subdirectory(src/math)

#
# library : format
#
add_subdirectory(src/format)

#
# library : biginteger
#
add_subdirectory(src/biginteger)

#
# library : json
#
add_subdirectory(src/json)

#
# library : container
#
add_subdirectory(src/container)

#
# library : wjr
#
wjr_cc_library(
   NAME
   wjr

   DEPS
   wjr::json
   wjr::ring_buffer

   FINAL
)

# Build tests
if(WJR_BUILD_TESTS)
   message(STATUS "Build tests.")

   enable_testing()
   add_subdirectory(test)
endif()

# Build benchmarks
if(WJR_BUILD_BENCHMARKS)
   message(STATUS "Build benchmarks.")

   add_subdirectory(benchmarks)
endif()

# install
if(WJR_ENABLE_INSTALL)
   list(APPEND SYSTEM_INSTALL_DIRS "/usr/local" "/usr" "/opt/" "/opt/local" "c:/Program Files/${PROJECT_NAME}")

   if(NOT(DEFINED CMAKE_INSTALL_PREFIX OR CMAKE_INSTALL_PREFIX IN_LIST SYSTEM_INSTALL_DIRS))
      message(WARNING "\
  The default and system-level install directories are unsupported except in LTS \
  releases of wjr.  Please set CMAKE_INSTALL_PREFIX to install WJR in your \
  source or build tree directly.\
    ")
   endif()

   install(EXPORT ${PROJECT_NAME}Targets
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
   )

   configure_package_config_file(
      cmake/wjrConfig.cmake.in
      "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
      INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
   )

   install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
   )

   if(WJR_VERSION)
      write_basic_package_version_file(
         "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         COMPATIBILITY ExactVersion
      )

      install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
      )
   endif() # wjr_VERSION

   install(DIRECTORY include/wjr
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      FILES_MATCHING
      PATTERN "*.hpp"
   )

   get_property(WJR_ALL_TARGETS GLOBAL PROPERTY WJR_ALL_TARGETS)

   # generate a file include all alias libraries
   file(WRITE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Alias.cmake" "# Generated by CMake\n\n")

   foreach(NAME IN LISTS WJR_ALL_TARGETS)
      if(NAME STREQUAL "wjr")
         set(TARGET_NAME "wjr")
      else()
         set(TARGET_NAME "wjr-${NAME}")
      endif()

      file(APPEND "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Alias.cmake" "add_library(wjr::${NAME} ALIAS ${TARGET_NAME})\n")
   endforeach()

   install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Alias.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
   )
endif() # WJR_ENABLE_INSTALL