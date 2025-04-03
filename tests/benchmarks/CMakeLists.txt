cmake_minimum_required(VERSION 3.14)
project(tests LANGUAGES CXX)

include(FetchContent)

set(WJR_ENABLE_ASSEMBLY ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT DEFINED WJR_DEBUG_LEVEL_RELEASE)
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

add_subdirectory(googletest)
add_subdirectory(benchmark)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" ")
endif()

add_executable(
    tests
    src/main.cpp
    src/math.cpp
    src/biginteger.cpp
)

set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")
link_directories(${GMP_LIBS})

target_link_libraries(
    tests
    PRIVATE
    wjr
    ${GMP_LIBS}/libgmp.dll.a
    benchmark::benchmark
)