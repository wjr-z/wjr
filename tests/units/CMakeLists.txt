cmake_minimum_required(VERSION 3.14)
project(wjr LANGUAGES CXX)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_definitions(WJR_USE_GMP)
add_compile_definitions(WJR_ASSERT_THROW)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -Wall -Wextra -DWJR_DEBUG_LEVEL=3 -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" -g")

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

target_link_libraries(
   wjr
   gtest_main
)

enable_testing()