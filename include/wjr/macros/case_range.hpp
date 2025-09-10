#ifndef WJR_MACROS_CASE_RANGE_HPP__
#define WJR_MACROS_CASE_RANGE_HPP__

#include <wjr/config.hpp>
#include <wjr/preprocessor/preview.hpp>

#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
    #define WJR_CASE_RANGE(START, END) case START ... END:
#else
    #define __WJR_INDEXS_RANGE_I(START, END)                                                       \
        WJR_PP_QUEUE_POP_FRONT_N((WJR_PP_IOTA(WJR_PP_INC(END))), START)
    #define WJR__INDEXS_RANGE(START, END) WJR_PP_QUEUE_EXPAND(__WJR_INDEXS_RANGE_I(START, END))

    #define WJR_CASE_RANGE(START, END)                                                             \
        WJR_PP_QUEUE_PUT(                                                                          \
            WJR_PP_QUEUE_TRANSFORM(__WJR_INDEXS_RANGE_I(START, END), __WJR_CASE_RANGE_CALLBACK))

    #define __WJR_CASE_RANGE_CALLBACK(x) case x:
#endif

#endif // WJR_MACROS_CASE_RANGE_HPP__