set(WJR_SRCS)
wjr_add_cpp_source(WJR_SRCS memory)

wjr_cc_library(
   NAME
   memory

   SRCS
   ${WJR_SRCS}

   DEPS
   wjr::assert
)