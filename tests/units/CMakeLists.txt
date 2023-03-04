cmake_minimum_required(VERSION 3.10)

#set(CMAKE_C_COMPILER "D:/LLVM/bin/clang.exe")
#set(CMAKE_CXX_COMPILER "D:/LLVM/bin/clang++.exe")

project(wjr LANGUAGES CXX)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)
set(WJR_SIMD_DIR ${WJR_LIB_DIR}/simd)
set(WJR_ASM_DIR ${WJR_LIB_DIR}/asm)
set(WJR_ALGO_DIR ${WJR_LIB_DIR}/algo)
set(WJR_MP_DIR ${WJR_LIB_DIR}/mp)

if(ENABLE_COVERAGE)
  # set compiler flags
  set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -std=c++17 -coverage -mavx2")
  # find required tools
  find_program(LCOV lcov REQUIRED)
  find_program(GENHTML genhtml REQUIRED)

  # add coverage target
  add_custom_target(coverage
    # gather data
    COMMAND ${LCOV} --directory . --capture --output-file coverage.info
    COMMAND ${LCOV} --extract coverage.info '*/wjr/*' '*/units/src/*' --output-file coverage.info
    # generate report
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
  if (MSVC)
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /arch:mavx2")
  else()
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17 -mavx2")
  endif() 
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://ghproxy.com/https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(wjr 
                ${PROJECT_SOURCE_DIR}/src/unit_vector.cpp 
                ${PROJECT_SOURCE_DIR}/src/unit_algo.cpp
				${WJR_SRCS}
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ${WJR_SIMD_DIR}
                           ${WJR_ASM_DIR}
                           ${WJR_ALGO_DIR}
                           ${WJR_MP_DIR}
                           ) 

target_link_libraries(
  wjr
  gtest_main
)

include(GoogleTest)
#gtest_discover_tests(wjr)