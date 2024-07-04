#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

// testing ...

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/compiler.hpp>
#include <wjr/preprocessor/details.hpp>
#include <wjr/preprocessor/logical.hpp>

// Due to the fact that call is not a simple expansion, but takes the previous output as
// the next input, the difficulty of implementing recursion is also high.
#include <wjr/preprocessor/queue/algorithm.hpp>

#define WJR_PP_BOOL_IF_NE(expr, t, f) WJR_PP_BOOL_IF(WJR_PP_BOOL(expr), t, f)

// (a, b, c) -> f(a) f(b) f(c)
#define WJR_PP_TRANSFORM_PUT(queue, op)                                                  \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

// (a, b, c) -> (f(a), f(b), f(c)) (note : f(x) = (g(x))) -> g(a) g(b) g(c)
#define WJR_PP_TRANSFORM_UNWRAP_PUT(queue, op)                                           \
    WJR_PP_QUEUE_UNWRAP_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

#define WJR_ATTRIBUTES(...) WJR_PP_TRANSFORM_PUT((__VA_ARGS__), WJR_ATTRIBUTES_CALLER)
#define WJR_ATTRIBUTES_CALLER(x) WJR_ATTRIBUTE(x)

#define WJR_PRAGMA_I(expr) _Pragma(#expr)
#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_MSVC)
#define WJR_PRAGMA(expr) WJR_PRAGMA_I(expr)
#else
#define WJR_PRAGMA(expr)
#endif

#if WJR_HAS_FEATURE(PRAGMA_UNROLL)
#if defined(WJR_COMPILER_GCC)
#define WJR_UNROLL(loop) WJR_PRAGMA(GCC unroll(loop))
#else
#define WJR_UNROLL(loop) WJR_PRAGMA(unroll(loop))
#endif
#else
#define WJR_UNROLL(loop)
#endif

#define WJR_IS_OVERLAP_P(p, pn, q, qn) ((p) + (pn) > (q) && (q) + (qn) > (p))
#define WJR_IS_SEPARATE_P(p, pn, q, qn) (!WJR_IS_OVERLAP_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_SEPARATE_P(p, pn, q, qn)                                          \
    (((p) == (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_INCR_P(p, pn, q, qn)                                              \
    (((p) <= (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_DECR_P(p, pn, q, qn)                                              \
    (((p) >= (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))

#define WJR_ASM_PIC_JMPL(LABEL, TABLE) ".long " #LABEL "-" #TABLE
#define WJR_ASM_NOPIC_JMPL(LABEL) ".quad " #LABEL

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
#define WJR_ASM_CCSET(c) "/* set condition codes */\n\t"
#define WJR_ASM_CCOUT(c) "=@cc" #c
#else
#define WJR_ASM_CCSET(c) "set" #c " %[_cc_" #c "]\n\t"
#define WJR_ASM_CCOUT(c) [_cc_##c] "=r"
#endif

// WJR_EXCEPTIONS_LEVEL : 0 ~ 3
// 0 : Disable exceptions
// 1 ~ 3 : Enable exceptions
#ifndef WJR_EXCEPTIONS_LEVEL
#define WJR_EXCEPTIONS_LEVEL 1 // enable exceptions by default
#endif

#define WJR_EXCEPTIONS_IF(level, expr0, expr1)                                           \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_EXCEPTIONS_LEVEL, level), expr0, expr1)

#define WJR_EXCEPTIONS_EXPR_L(level, expr) WJR_EXCEPTIONS_IF(level, expr, )
#define WJR_EXCEPTIONS_EXPR(expr) WJR_EXCEPTIONS_EXPR_L(0, expr)

#define WJR_ENABLE_EXCEPTIONS_TRY_I try
#define WJR_ENABLE_EXCEPTIONS_CATCH_I(X) catch (X)
#define WJR_ENABLE_EXCEPTIONS_THROW_I(X) throw X

#define WJR_DISABLE_EXCEPTIONS_TRY_I if (true)
#define WJR_DISABLE_EXCEPTIONS_CATCH_I(X) if (false)
#define WJR_DISABLE_EXCEPTIONS_THROW_I(X)

#define WJR_TRY_L(level)                                                                 \
    WJR_EXCEPTIONS_IF(level, WJR_ENABLE_EXCEPTIONS_TRY_I, WJR_DISABLE_EXCEPTIONS_TRY_I)
#define WJR_CATCH_L(level, X)                                                            \
    WJR_EXCEPTIONS_IF(level, WJR_ENABLE_EXCEPTIONS_CATCH_I(X),                           \
                      WJR_DISABLE_EXCEPTIONS_CATCH_I(X))
#define WJR_THROW_L(level, X)                                                            \
    WJR_EXCEPTIONS_IF(level, WJR_ENABLE_EXCEPTIONS_THROW_I(X),                           \
                      WJR_DISABLE_EXCEPTIONS_THROW_I(X))

#define WJR_TRY WJR_TRY_L(0)
#define WJR_CATCH(X) WJR_CATCH_L(0, X)
#define WJR_THROW(X) WJR_THROW_L(0, X)

#define WJR_TRY_L1 WJR_TRY_L(1)
#define WJR_CATCH_L1(X) WJR_CATCH_L(1, X)
#define WJR_THROW_L1(X) WJR_THROW_L(1, X)

#define WJR_TRY_L2 WJR_TRY_L(2)
#define WJR_CATCH_L2(X) WJR_CATCH_L(2, X)
#define WJR_THROW_L2(X) WJR_THROW_L(2, X)

#define WJR_TRY_L3 WJR_TRY_L(3)
#define WJR_CATCH_L3(X) WJR_CATCH_L(3, X)
#define WJR_THROW_L3(X) WJR_THROW_L(3, X)

#define WJR_REQUIRES(...) std::enable_if_t<(__VA_ARGS__), int> = 0
#define WJR_REQUIRES_I(...) std::enable_if_t<(__VA_ARGS__), int>

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__