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

    OBJECT
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/biginteger)

wjr_cc_library(
    NAME
    arch-biginteger

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert

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

    OBJECT
)