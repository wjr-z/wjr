cmake_minimum_required(VERSION 3.14)
project(tests LANGUAGES CXX)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT DEFINED WJR_DEBUG_LEVEL_RELEASE)
   set(WJR_DEBUG_LEVEL_RELEASE 3)
endif()

if(MSVC)
   add_compile_definitions(__ADX__)
   add_compile_definitions(__BMI2__)
   add_compile_definitions(__AVX2__)
   add_compile_definitions(__POPCNT__)
   add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../wjr ${CMAKE_CURRENT_BINARY_DIR}/wjr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(GMP REQUIRED)
add_compile_definitions(WJR_USE_GMP)

if(EXISTS googletest)
   add_subdirectory(googletest)
else()
   FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.10.0
   )

   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
   set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
   set(BUILD_GTEST ON CACHE BOOL "" FORCE)

   FetchContent_MakeAvailable(googletest)
endif()

aux_source_directory(src TEST_SRCS)

add_executable(
   tests
   ${TEST_SRCS}
)

if(WJR_ENABLE_COVERAGE)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -DWJR_DEBUG_LEVEL=3 -Wall -Wextra -g")

   # find required tools
   # find_program(LCOV lcov REQUIRED)
   # find_program(GENHTML genhtml REQUIRED)
   set(target tests)

   target_compile_options(${target}
      PRIVATE -fprofile-instr-generate -fcoverage-mapping -fno-inline)
   target_link_options(${target}
      PUBLIC -fprofile-instr-generate -fcoverage-mapping)

   # add coverage target
   find_program(LLVM_COV_PATH llvm-cov REQUIRED)
   find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)
   add_custom_target(coverage-${target}
      COMMAND $<TARGET_FILE:${target}>

      # COMMAND del coverage /S /Q
      COMMAND ${LLVM_PROFDATA_PATH} merge
      -sparse default.profraw -o default.profdata
      COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:${target}>
      -instr-profile=default.profdata
      -show-line-counts-or-regions
      -use-color
      -show-instantiation-summary
      -show-branches=count
      -format=html
      -output-dir=coverage-${target}
      --ignore-filename-regex="\(tests\|gmp\)"
      COMMAND ${LLVM_COV_PATH} report $<TARGET_FILE:${target}>
      -instr-profile=default.profdata
      -show-region-summary=false
      -show-branch-summary=false
      -show-branch-summary=false
      --ignore-filename-regex="\(tests\|gmp\)"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   )
endif()

target_include_directories(tests PRIVATE ${GMP_INCLUDE_DIRS})

target_link_libraries(tests
   PRIVATE
   wjr
   gtest_main
   ${GMP_LIBRARIES}
)

target_precompile_headers(tests PRIVATE ${WJR_PCH_COMMON} ${WJR_PCH_SIMD})
enable_testing()