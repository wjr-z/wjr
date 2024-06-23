cmake_minimum_required(VERSION 3.10)
project(wjr LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_USE_GMP on)

if(CMAKE_USE_GMP)
   add_compile_definitions(WJR_USE_GMP)
   set(USE_GMP_FLAGS "-I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
else()
   set(USE_GMP_FLAGS "")
endif()

if(MSVC)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /Zc:preprocessor")
else()
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++2a -DNDEBUG -DWJR_DEBUG_LEVEL=0 -march=native ${USE_GMP_FLAGS} -Wall -Wextra -g")
endif()

message(${CMAKE_CXX_FLAGS})

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../../include)
set(WJR_LIB_DIR ${PROJECT_SOURCE_DIR}/../../src)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(
   wjr
   src/main.cpp
   ${WJR_SRCS}
)

if(CMAKE_USE_GMP)
   set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")

   link_directories(${GMP_LIBS})
   target_link_libraries(wjr
      ${GMP_LIBS}/libgmp.dll.a
      ${GMP_LIBS}/libgmpxx.dll.a
   )
endif()

target_include_directories(wjr PUBLIC
   ${WJR_INCLUDE_DIR}
)

target_link_libraries(
   wjr
)