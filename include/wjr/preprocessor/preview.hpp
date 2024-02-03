#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

// testing ...

#include <cassert>

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/compiler.hpp>
#include <wjr/preprocessor/details.hpp>
#include <wjr/preprocessor/logical.hpp>

// TODO : Recursive nested queue algorithm
// Due to the fact that call is not a simple expansion, but takes the previous output as
// the next input, the difficulty of implementing recursion is also high.
#include <wjr/preprocessor/queue/algorithm.hpp>

#ifdef __LINE__
#define WJR_LINE __LINE__
#else
#define WJR_LINE -1
#endif

#ifdef __FILE__
#define WJR_FILE __FILE__
#else
#define WJR_FILE ""
#endif

// ASSERT_LEVEL : 0 ~ 3
// 0 : Release (defined(NDEBUG) && ! defined(WJR_DEBUG_LEVEL))
// 1 : Some simple runtime checks, such as boundary checks (default)
// 2 : Most runtime checks
// 3 : Maximize runtime checks
#if defined WJR_DEBUG_LEVEL
#define WJR_ASSERT_LEVEL WJR_DEBUG_LEVEL
#elif defined(NDEBUG)
#define WJR_DEBUG_LEVEL 0
#else
#define WJR_DEBUG_LEVEL 1
#endif //

template <typename Handler, typename... Args,
          std::enable_if_t<std::is_invocable_v<Handler, Args...>, int> = 0>
void __assert_fail(Handler handler, Args &&...args) {
    handler(std::forward<Args>(args)...);
    std::abort();
}

template <typename Handler, typename... Args,
          std::enable_if_t<std::is_invocable_v<Handler, Args...>, int> = 0>
WJR_COLD WJR_NOINLINE void __assert_fail(const char *expr, const char *file, int line,
                                         Handler handler, Args &&...args) {
    (void)fprintf(stderr, "Assertion failed: %s", expr);
    if ((file != nullptr) && (file[0] != '\0')) {
        (void)fprintf(stderr, ", file %s", file);
    }
    if (line != -1) {
        (void)fprintf(stderr, ", line %d", line);
    }
    (void)fprintf(stderr, "\n");
    __assert_fail(handler, std::forward<Args>(args)...);
}

struct __assert_t {
    void operator()() const {}

    template <typename... Args>
    void operator()(const char *fmt, Args &&...args) {
        (void)fprintf(stderr, "Additional message: ");
        (void)fprintf(stderr, fmt, std::forward<Args>(args)...);
        (void)fprintf(stderr, "\n");
    }
};

inline constexpr __assert_t __assert{};

#define WJR_ASSERT_NOMESSAGE_FAIL(handler, exprstr)                                      \
    __assert_fail(exprstr, WJR_FILE, WJR_LINE, handler)
#define WJR_ASSERT_MESSAGE_FAIL(handler, exprstr, ...)                                   \
    __assert_fail(exprstr, WJR_FILE, WJR_LINE, handler, __VA_ARGS__)

#define WJR_ASSERT_CHECK_I_NOMESSAGE(handler, expr)                                      \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            WJR_ASSERT_NOMESSAGE_FAIL(handler, #expr);                                   \
        }                                                                                \
    } while (0)
#define WJR_ASSERT_CHECK_I_MESSAGE(handler, expr, ...)                                   \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            WJR_ASSERT_MESSAGE_FAIL(handler, #expr, __VA_ARGS__);                        \
        }                                                                                \
    } while (0)

#define WJR_ASSERT_CHECK_I(...)                                                          \
    WJR_ASSERT_CHECK_I_N(WJR_PP_ARGS_LEN(__VA_ARGS__), __VA_ARGS__)
#define WJR_ASSERT_CHECK_I_N(N, ...)                                                     \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_ASSERT_CHECK_I_NOMESSAGE,                        \
                   WJR_ASSERT_CHECK_I_MESSAGE)                                           \
    (__assert, __VA_ARGS__)

// do nothing
#define WJR_ASSERT_UNCHECK_I(expr, ...)

#define WJR_ALWAYS_ASSERT_UNCHECK_I(expr, ...) (expr)

// level = [0, 2]
// The higher the level, the less likely it is to be detected
// Runtime detect  : 1
// Maximize detect : 2
#define WJR_ASSERT_L(level, ...)                                                         \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), WJR_ASSERT_CHECK_I,                \
                   WJR_ASSERT_UNCHECK_I)                                                 \
    (__VA_ARGS__)

// level of assert is zero at default.
#define WJR_ASSERT(...) WJR_ASSERT_L(0, __VA_ARGS__)

#define WJR_ALWAYS_ASSERT_L(level, ...)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), WJR_ASSERT_CHECK_I,                \
                   WJR_ALWAYS_ASSERT_UNCHECK_I)                                          \
    (__VA_ARGS__)

// level of assert is zero at default.
#define WJR_ALWAYS_ASSERT(...) WJR_ALWAYS_ASSERT_L(0, __VA_ARGS__)

#define WJR_ASSERT_ASSUME_L(level, ...)                                                  \
    WJR_ASSERT_L(level, __VA_ARGS__);                                                    \
    __WJR_ASSERT_ASSUME_L_ASSUME(__VA_ARGS__)
#define __WJR_ASSERT_ASSUME_L_ASSUME(expr, ...) WJR_ASSUME(expr)

#define WJR_ASSERT_ASSUME(...) WJR_ASSERT_ASSUME_L(0, __VA_ARGS__)

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
    (p == q || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_INCR_P(p, pn, q, qn)                                              \
    (((p) <= (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_DECR_P(p, pn, q, qn)                                              \
    (((p) >= (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))

#define WJR_ASM_PIC_JMPL(LABEL, TABLE) ".long " #LABEL "-" #TABLE
#define WJR_ASM_NOPIC_JMPL(LABEL) ".quad " #LABEL

#if defined(__linux__)
#define WJR_ASM_SECTION(...) ".section " WJR_PP_STRS(__VA_ARGS__) "\n\t"
#define WJR_ASM_PREVIOUS() ".previous\n\t"
#else
#define WJR_ASM_SECTION(...) "\n\t"
#define WJR_ASM_PREVIOUS() "\n\t"
#endif

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__