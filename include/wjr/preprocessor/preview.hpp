#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

// testing ...

#include <cassert>
#include <stdlib.h>

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/compiler.hpp>
#include <wjr/preprocessor/details.hpp>
#include <wjr/preprocessor/logical.hpp>

// TODO : Recursive nested queue algorithm
// Due to the fact that call is not a simple expansion, but takes the previous output as
// the next input, the difficulty of implementing recursion is also high.
#include <wjr/preprocessor/queue/algorithm.hpp>

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

// use abort instead of assert when NDEBUG is defined
#if defined(NDEBUG)
#define WJR_ASSERT_NOMESSAGE_I(expr)                                                     \
    if (!WJR_UNLIKELY(expr)) {                                                           \
        std::abort();                                                                    \
        WJR_UNREACHABLE;                                                                 \
    }
#define WJR_ASSERT_MESSAGE_I(expr)                                                       \
    std::abort();                                                                        \
    WJR_UNREACHABLE;
#else
#define WJR_ASSERT_NOMESSAGE_I(expr) assert(expr)
#define WJR_ASSERT_MESSAGE_I(expr)                                                       \
    assert(expr);                                                                        \
    WJR_UNREACHABLE
#endif

#define WJR_ASSERT_CHECK_I_NOMESSAGE(expr)                                               \
    do {                                                                                 \
        WJR_ASSERT_NOMESSAGE_I(expr);                                                    \
    } while (0)

#define WJR_ASSERT_CHECK_I_MESSAGE(expr, fmt, ...)                                       \
    do {                                                                                 \
        if (!WJR_UNLIKELY(expr)) {                                                       \
            fprintf(stderr, "Additional error message : " fmt "\n", ##__VA_ARGS__);      \
            WJR_ASSERT_MESSAGE_I(expr);                                                  \
        }                                                                                \
    } while (0)

#define WJR_ASSERT_CHECK_I(...)                                                          \
    WJR_ASSERT_CHECK_I_N(WJR_PP_ARGS_LEN(__VA_ARGS__), __VA_ARGS__)
#define WJR_ASSERT_CHECK_I_N(N, ...)                                                     \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_ASSERT_CHECK_I_NOMESSAGE,                        \
                   WJR_ASSERT_CHECK_I_MESSAGE)                                           \
    (__VA_ARGS__)

#define WJR_ASSERT_UNCHECK_I(...)                                                        \
    do {                                                                                 \
    } while (0)

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

// (a, b, c) -> f(a) f(b) f(c)
#define WJR_PP_TRANSFORM_PUT(queue, op)                                                  \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

#define WJR_ATTRIBUTES(...) WJR_PP_TRANSFORM_PUT(WJR_ATTRIBUTES_CALLER)
#define WJR_ATTRIBUTES_CALLER(x) WJR_ATTRIBUTE(x)

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__