if(WJR_ENABLE_ARCH_ASSEMBLY)
   set(WJR_ADDSUB_SRCS
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/addsub_n.asm
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/addrsblsh_n.asm
   )

   add_library(wjr-add_n-asm OBJECT ${WJR_ADDSUB_SRCS})
   add_library(wjr-sub_n-asm OBJECT ${WJR_ADDSUB_SRCS})

   target_compile_options(wjr-sub_n-asm
      PRIVATE $<$<COMPILE_LANGUAGE:ASM_NASM>: -dWJR_ASM_SUB>
   )

   set(WJR_ASSEMBLY_SRCS
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/basecase_mul_s.asm
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/basecase_sqr.asm
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/divexact_dbm1c.asm
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/mul_1.asm
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/addmul_1.asm
      ${WJR_LIB_DIR}/arch/${WJR_ARCH}/biginteger/detail/submul_1.asm
   )

   add_library(wjr-biginteger-asm OBJECT ${WJR_ASSEMBLY_SRCS})
endif()

#
# library : biginteger
#
set(WJR_SRCS)
add_cpp_source(WJR_SRCS biginteger)

set(WJR_DEPS
   wjr::arch
   wjr::memory
   wjr::format-internal
)

if(WJR_ENABLE_ASSEMBLY)
   list(APPEND WJR_DEPS
      wjr-add_n-asm
      wjr-sub_n-asm
      wjr-biginteger-asm
   )
endif()

wjr_cc_library(
   NAME
   biginteger

   SRCS
   ${WJR_SRCS}

   DEPS
   ${WJR_DEPS}
)