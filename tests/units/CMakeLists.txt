cmake_minimum_required(VERSION 3.14)
project(tests LANGUAGES CXX)

include(FetchContent)

set(WJR_ENABLE_ASSEMBLY ON)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

if (NOT DEFINED WJR_DEBUG_LEVEL_RELEASE)
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
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -coverage -march=native -DWJR_DEBUG_LEVEL=3 -Wall -Wextra -lgmp -lgmpxx -g")

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
      set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
   else()
      set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" ")
   endif()
   message(${CMAKE_CXX_FLAGS})
endif()

add_executable(
   tests
   src/preprocessor.cpp
   src/math.cpp
   src/memory.cpp
   src/container.cpp
   src/compressed_pair.cpp
   src/tuple.cpp
   src/biginteger.cpp
   src/expected.cpp
   src/fastfloat.cpp
   src/json.cpp
)

set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")

link_directories(${GMP_LIBS})
target_link_libraries(tests
   wjr
   ${GMP_LIBS}/libgmp.dll.a
   ${GMP_LIBS}/libgmpxx.dll.a
   gtest_main
)

target_precompile_headers(
   tests
   PUBLIC src/detail.hpp
)

enable_testing()