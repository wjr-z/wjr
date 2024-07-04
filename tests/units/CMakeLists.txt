cmake_minimum_required(VERSION 3.14)
project(wjr
   LANGUAGES CXX
)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_definitions(WJR_USE_GMP)

FetchContent_Declare(
   googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG release-1.10.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

if(WJR_ENABLE_COVERAGE)
   # set compiler flags
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -coverage -march=native -DWJR_DEBUG_LEVEL=3 -Wall -Wextra -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" -g")

   # find required tools
   find_program(LCOV lcov REQUIRED)
   find_program(GENHTML genhtml REQUIRED)

   # add coverage target
   add_custom_target(coverage

      # gather data
      COMMAND ${LCOV} --directory . --capture --output-file coverage.info
      COMMAND ${LCOV} --extract coverage.info '*/include/wjr/*' --output-file coverage.info

      # generate report
      COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
   if(MSVC)
      set(CMAKE_CXX_FLAGS
         "$ENV{CXXFLAGS} /O2 /std:c++17 /arch:AVX2 /W4 /std:c11 /Zc:preprocessor /Zc:lambda /EHsc \
   /DWJR_DEBUG_LEVEL=3 /D__ADX__ /D__BMI__ /D__BMI2__ /I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
   else()
      set(CMAKE_CXX_FLAGS
         "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -Wall -Wextra  \
-Wno-error -pedantic -Wno-gnu-zero-variadic-macro-arguments \
-DWJR_DEBUG_LEVEL=3 -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" -g")

      if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
         set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -ftime-trace")
      endif()
   endif()
endif()

message(${CMAKE_CXX_FLAGS})

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/../../src)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

set(WJR_LIBS "")

if(WJR_ENABLE_ASSEMBLY)
   set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
   enable_language(CXX ASM_NASM)
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

add_executable(
   wjr
   src/preprocessor.cpp
   src/math.cpp
   src/memory.cpp
   src/container.cpp
   src/compressed_pair.cpp
   src/tuple.cpp
   src/biginteger.cpp
   ${WJR_SRCS}
)

set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")

link_directories(${GMP_LIBS})
target_link_libraries(wjr
   ${GMP_LIBS}/libgmp.dll.a
   ${GMP_LIBS}/libgmpxx.dll.a
   ${WJR_LIBS}
)

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

target_precompile_headers(
   wjr
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/preprocessor.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/assert.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/type_traits.hpp
   PUBLIC ${WJR_INCLUDE_DIR}/wjr/x86/simd/simd.hpp
   PUBLIC src/details.hpp
)

target_link_libraries(
   wjr
   gtest_main
)

enable_testing()