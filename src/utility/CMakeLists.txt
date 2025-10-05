# todo: cpptrace is only for debug!

if(WJR_ENABLE_ASSERT_BACKTRACE)
  FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG v1.0.4 # <HASH or TAG>
  )
  FetchContent_MakeAvailable(cpptrace)
endif()

set(WJR_DEPS wjr::config)

if(WJR_ENABLE_ASSERT_BACKTRACE)
  list(APPEND WJR_DEPS cpptrace::cpptrace)
  set(WJR_DEFINES WJR_ENABLE_ASSERT_BACKTRACE)
endif()

wjr_cc_library(
  NAME
  assert

  SRCS
  assert.cpp

  DEPS
  ${WJR_DEPS}

  DEFINES
  ${WJR_DEFINES}
)