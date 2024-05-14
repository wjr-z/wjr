cmake_minimum_required(VERSION 3.14)
project(wjr LANGUAGES CXX)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_definitions(WJR_USE_GMP)

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
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -Wall -Wextra -DWJR_DEBUG_LEVEL=3 -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" -g")
endif()

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/../../src)

FetchContent_Declare(
   googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG release-1.10.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

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
)

target_link_libraries(
   wjr
   gtest_main
)

enable_testing()