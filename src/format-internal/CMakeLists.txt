set(WJR_SRCS)
add_cpp_source(WJR_SRCS format-internal)

wjr_cc_library(
   NAME
   format-internal

   SRCS
   ${WJR_SRCS}

   DEPS
   fmt::fmt

   OBJECT
)