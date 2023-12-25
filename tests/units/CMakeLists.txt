cmake_minimum_required(VERSION 3.14)
project(wjr LANGUAGES CXX)

include(FetchContent)

if (NOT CMAKE_BUILD_TYPE) 
   set(CMAKE_BUILD_TYPE Debug)
endif()

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /Zc:preprocessor")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17 -masm=intel -march=native")
endif() 


set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)

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
        src/main.cpp
		    ${WJR_SRCS}
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ) 

target_link_libraries(
   wjr
   gtest_main
)

enable_testing()