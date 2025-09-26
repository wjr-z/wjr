set(WJR_ADDSUB_SRCS
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/addsub_n.asm
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/addrsblsh_n.asm
)

wjr_asm_library(
    NAME
    asm-biginteger-sub_n

    SRCS
    ${WJR_ADDSUB_SRCS}

    PRIVATE_COPTS
    -dWJR_ASM_SUB
)

set(WJR_ASSEMBLY_SRCS
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/basecase_mul_s.asm
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/basecase_sqr.asm
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/divexact_dbm1c.asm
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/mul_1.asm
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/addmul_1.asm
    ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/submul_1.asm
)

wjr_asm_library(
    NAME
    asm-biginteger-internal

    SRCS
    ${WJR_ADDSUB_SRCS}
    ${WJR_ASSEMBLY_SRCS}
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
    wjr::asm-biginteger-sub_n
    wjr::asm-biginteger-internal

    OBJECT
)