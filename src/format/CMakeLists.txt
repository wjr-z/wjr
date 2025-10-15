set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS format)

wjr_cc_library(
   NAME
   format-internal

   SRCS
   ${WJR_SRCS}

   DEPS
   wjr::arch-format
   fmt::fmt
   range-v3::range-v3

   OBJECT
)