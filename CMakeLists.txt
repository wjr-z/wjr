cmake_minimum_required(VERSION 3.10)

project(wjr LANGUAGES CXX)

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17 /arch:mavx2")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17 -mavx2 -lpthread -DWJR_DEBUG_LEVEL=3")
endif() 

set(WJR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(WJR_LIB_DIR ${WJR_INCLUDE_DIR}/wjr)

file(GLOB_RECURSE WJR_SRCS ${WJR_LIB_DIR}/*.cpp)

add_executable(
        wjr 
        main.cpp
				${WJR_SRCS}
)

target_include_directories(wjr PUBLIC 
                           ${WJR_INCLUDE_DIR}
                           ) 

target_link_libraries(
  wjr
)