cmake_minimum_required(VERSION 3.10)

project(wjr2 LANGUAGES CXX)

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17")
endif() 

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://ghproxy.com/https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(wjr 
                ${PROJECT_SOURCE_DIR}/src/unit_vector.cpp 
                ${PROJECT_SOURCE_DIR}/src/unit_algo.cpp
                ${WJR_LIB_DIR}/random.cpp 
                ${WJR_LIB_DIR}/type_traits.cpp
                ${WJR_LIB_DIR}/algorithm.cpp
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ) 

target_link_libraries(
  wjr
  gtest_main
)

include(GoogleTest)
#gtest_discover_tests(wjr)