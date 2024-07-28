#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

// testing ...

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/compiler.hpp>
#include <wjr/preprocessor/detail.hpp>
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

#if defined(WJR_DISABLE_EXCEPTIONS)
#define WJR_EXCEPTIONS_IF(ENABLE, DISABLE) DISABLE
#else
#define WJR_EXCEPTIONS_IF(ENABLE, DISABLE) ENABLE
#endif

#define WJR_ENABLE_EXCEPTIONS_TRY_I try
#define WJR_ENABLE_EXCEPTIONS_CATCH_I(...) catch (__VA_ARGS__)
#define WJR_ENABLE_EXCEPTIONS_THROW_I(X) throw X
#define WJR_ENABLE_EXCEPTIONS_XTHROW_I throw

#define WJR_DISABLE_EXCEPTIONS_TRY_I if constexpr (true)
#define WJR_DISABLE_EXCEPTIONS_CATCH_I(...) if constexpr (false)
#define WJR_DISABLE_EXCEPTIONS_THROW_I(X)
#define WJR_DISABLE_EXCEPTIONS_XTHROW_I

#define WJR_TRY                                                                          \
    WJR_EXCEPTIONS_IF(WJR_ENABLE_EXCEPTIONS_TRY_I, WJR_DISABLE_EXCEPTIONS_TRY_I)
#define WJR_CATCH(...)                                                                   \
    WJR_EXCEPTIONS_IF(WJR_ENABLE_EXCEPTIONS_CATCH_I(__VA_ARGS__),                        \
                      WJR_DISABLE_EXCEPTIONS_CATCH_I(__VA_ARGS__))
#define WJR_THROW(X)                                                                     \
    WJR_EXCEPTIONS_IF(WJR_ENABLE_EXCEPTIONS_THROW_I(X), WJR_DISABLE_EXCEPTIONS_THROW_I(X))
#define WJR_XTHROW                                                                       \
    WJR_EXCEPTIONS_IF(WJR_ENABLE_EXCEPTIONS_XTHROW_I, WJR_DISABLE_EXCEPTIONS_XTHROW_I)

#define WJR_REQUIRES(...) std::enable_if_t<(__VA_ARGS__), int> = 0
#define WJR_REQUIRES_I(...) std::enable_if_t<(__VA_ARGS__), int>

#define WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(CLASS)                                        \
    CLASS() = default;                                                                   \
    CLASS(const CLASS &) = default;                                                      \
    CLASS(CLASS &&) = default;                                                           \
    CLASS &operator=(const CLASS &) = default;                                           \
    CLASS &operator=(CLASS &&) = default;                                                \
    ~CLASS() = default

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__