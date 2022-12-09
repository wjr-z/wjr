cmake_minimum_required(VERSION 3.10)

project(wjr LANGUAGES CXX)

if (MSVC)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} /O2 /std:c++17")
else()
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++17")
endif() 

add_executable(wjr compare.cpp)