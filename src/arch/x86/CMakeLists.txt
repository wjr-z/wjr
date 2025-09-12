set(WJR_SRCS)
add_cpp_source(WJR_SRCS arch/x86)

wjr_cc_library(
    NAME
    arch

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    CpuFeatures::cpu_features
)