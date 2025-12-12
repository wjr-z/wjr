include(FetchContent)

# Fetch cpu_features for x86 architecture
FetchContent_Declare(
    cpu_features
    GIT_REPOSITORY https://github.com/google/cpu_features.git
    GIT_TAG v0.10.1
    GIT_SHALLOW ON
    FIND_PACKAGE_ARGS 0.10.1
)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cpu_features)

wjr_cc_library(
    NAME
    arch-cpuinfo

    SRCS
    cpuinfo.cpp

    DEPS
    CpuFeatures::cpu_features

    OBJECT
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/math)

wjr_cc_library(
    NAME
    arch-math

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    wjr::arch-cpuinfo
    wjr::iterator
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/format)

wjr_cc_library(
    NAME
    arch-format

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    wjr::iterator

    OBJECT
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/json)

wjr_cc_library(
    NAME
    arch-json

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    wjr::iterator

    OBJECT
)

add_subdirectory(biginteger)