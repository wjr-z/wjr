cmake_minimum_required(VERSION 3.10)
project(demo LANGUAGES CXX)

if (NOT DEFINED WJR_ENABLE_ASSEMBLY)
   set(WJR_ENABLE_ASSEMBLY ON)
endif()

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

if (NOT DEFINED WJR_DEBUG_LEVEL_RELEASE)
   set(WJR_DEBUG_LEVEL_RELEASE 0)
endif()

if(MSVC)
   add_compile_definitions(__ADX__)
   add_compile_definitions(__BMI2__)
   add_compile_definitions(__AVX2__)
   add_compile_definitions(__POPCNT__)
   add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../wjr ${CMAKE_CURRENT_BINARY_DIR}/wjr)

set(CMAKE_USE_GMP on)

if(CMAKE_USE_GMP)
   add_compile_definitions(WJR_USE_GMP)
   if(MSVC)
      set(USE_GMP_FLAGS "/I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
   else()
      set(USE_GMP_FLAGS "-I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
   endif()
else()
   set(USE_GMP_FLAGS "")
endif()

if(MSVC)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /Zc:preprocessor ${USE_GMP_FLAGS}")
else()
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++2a -DNDEBUG -DWJR_DEBUG_LEVEL=0 \
   -march=native ${USE_GMP_FLAGS} -Wall -Wextra -g")
endif()

message(${CMAKE_CXX_FLAGS})

add_executable(
   demo
   src/main.cpp
   src/simdjson.cpp
)

if(CMAKE_USE_GMP)
   set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")

   link_directories(${GMP_LIBS})
   target_link_libraries(demo
      ${GMP_LIBS}/libgmp.dll.a
      ${GMP_LIBS}/libgmpxx.dll.a
   )
endif()


target_link_libraries(
   demo
   wjr
)