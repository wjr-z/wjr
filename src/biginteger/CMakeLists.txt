#
# library : biginteger
#
set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS biginteger)

set(WJR_DEPS
   wjr::arch-biginteger
   wjr::memory
   wjr::math
   wjr::format-internal
)

wjr_cc_library(
   NAME
   biginteger

   SRCS
   ${WJR_SRCS}

   DEPS
   ${WJR_DEPS}
)