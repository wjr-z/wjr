set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS json)

wjr_cc_library(
   NAME
   json

   SRCS
   ${WJR_SRCS}

   DEPS
   wjr::biginteger
   wjr::arch-json
)