#ifndef WJR_PREPROCESSOR_EXTEND_HPP__
#define WJR_PREPROCESSOR_EXTEND_HPP__

// testing ...

#include <cassert>
#include <stdlib.h>

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/compiler.hpp>
#include <wjr/preprocessor/details.hpp>
#include <wjr/preprocessor/logical.hpp>
#include <wjr/preprocessor/queue.hpp>

// TODO ...
#if defined(WJR_CPP_17)
#define WJR_INLINE_CVAR inline constexpr
#else
#define WJR_INLINE_CVAR static constexpr
#endif

#define WJR_PP_QUEUE_INIT_N(x, N) WJR_PP_QUEUE_INIT_N_I(x, N)
#define WJR_PP_QUEUE_INIT_N_I(x, N) WJR_PP_QUEUE_INIT(WJR_PP_REPEAT(x, N))

#define WJR_PP_QUEUE_CALL_SAME(args, op)                                                 \
    WJR_PP_QUEUE_CALL(args, WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(args)))

#define WJR_PP_QUEUE_CALL_N(args, ops, N) WJR_PP_QUEUE_CALL_(args, ops, WJR_PP_INC(N))

#define WJR_PP_QUEUE_CALL_N_SAME(args, op, N)                                            \
    WJR_PP_QUEUE_CALL_N(args, WJR_PP_QUEUE_INIT_N(op, N), N)

// (a, b, c) -> (f(a), g(b), h(c)) ...
// ops : WJR_PP_QUEUE_PUSH_BACK(x, f(y)/g(y)/ ...)
// gen a result queue at first of queue
#define WJR_PP_QUEUE_GENERATE(queue, ops)                                                \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(                                           \
        WJR_PP_QUEUE_CALL(WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_INIT(0)), ops)))

#define WJR_PP_QUEUE_GENERATE_SAME(queue, op)                                            \
    WJR_PP_QUEUE_GENERATE(queue, WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(queue)))

#define WJR_PP_QUEUE_BACK(queue)                                                         \
    WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_SAME(queue, WJR_PP_QUEUE_BACK_CALLER))
#define WJR_PP_QUEUE_BACK_CALLER(x, y) y

#define WJR_PP_QUEUE_POP_FRONT_N(queue, N)                                               \
    WJR_PP_QUEUE_CALL_N_SAME(queue, WJR_PP_QUEUE_POP_FRONT_N_CALLER, N)
#define WJR_PP_QUEUE_POP_FRONT_N_CALLER(x, y) y

#define WJR_PP_QUEUE_POP_BACK_N(queue, N)                                                \
    WJR_PP_QUEUE_GENERATE(                                                               \
        queue,                                                                           \
        WJR_PP_QUEUE_POP_FRONT_N(                                                        \
            WJR_PP_QUEUE_INIT(WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER,       \
                                            WJR_PP_QUEUE_SIZE(queue)),                   \
                              WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER, N)),  \
            N))
#define WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, y)
#define WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER(x, y) x

#define WJR_PP_QUEUE_POP_BACK(queue) WJR_PP_QUEUE_POP_BACK_N(queue, 1)

#define WJR_PP_QUEUE_AT(queue, N) WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(queue, N))

#define WJR_PP_QUEUE_REVERSE(queue)                                                      \
    WJR_PP_QUEUE_POP_BACK(WJR_PP_QUEUE_FRONT(                                            \
        WJR_PP_QUEUE_CALL_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_INIT(0)),     \
                               WJR_PP_QUEUE_REVERSE_CALLER)))
#define WJR_PP_QUEUE_REVERSE_CALLER(x, y) WJR_PP_QUEUE_PUSH_FRONT(x, y)

// (a, b, c) -> (a b c)
#define WJR_PP_QUEUE_MERGE(queue) WJR_PP_QUEUE_CALL_SAME(queue, WJR_PP_QUEUE_MERGE_CALLER)
#define WJR_PP_QUEUE_MERGE_CALLER(x, y) x y

// (a, b, c) -> a b c
#define WJR_PP_QUEUE_PUT(queue) WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_MERGE(queue))

// ((A), (B), (C)) -> (A, B, C)
#define WJR_PP_QUEUE_UNWRAP(queue)                                                       \
    WJR_PP_QUEUE_GENERATE_SAME(queue, WJR_PP_QUEUE_UNWRAP_CALLER)
#define WJR_PP_QUEUE_UNWRAP_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, WJR_PP_QUEUE_EXPAND(y))

// (A, B, C) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_2(queue1, queue2)                                               \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(                                           \
        WJR_PP_QUEUE_CALL_N_SAME(WJR_PP_QUEUE_INIT(WJR_PP_QUEUE_INIT(queue1), queue2),   \
                                 WJR_PP_QUEUE_ZIP_2_CALLER, WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_2_CALLER(x, y)                                                  \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                   \
            x, WJR_PP_QUEUE_INIT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x)),              \
                                 WJR_PP_QUEUE_FRONT(y)))),                               \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                  \
        WJR_PP_QUEUE_POP_FRONT(y)

// ((A), (B), (C)) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_MORE(queue1, queue2)                                            \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                  \
        WJR_PP_QUEUE_INIT(WJR_PP_QUEUE_INIT(queue1), queue2),                            \
        WJR_PP_QUEUE_ZIP_MORE_CALLER, WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_MORE_CALLER(x, y)                                               \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                   \
            x, WJR_PP_QUEUE_INIT(                                                        \
                   WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x))),       \
                   WJR_PP_QUEUE_FRONT(y)))),                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                  \
        WJR_PP_QUEUE_POP_FRONT(y)

// (a, b, c) -> (f(a), f(b), f(c))
#define WJR_PP_QUEUE_TRANSFORM(queue, F)                                                 \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(                                           \
        WJR_PP_QUEUE_CALL_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_INIT(F)),     \
                               WJR_PP_QUEUE_TRANSFORM_CALLER)))
#define WJR_PP_QUEUE_TRANSFORM_CALLER(x, y)                                              \
    WJR_PP_QUEUE_PUSH_BACK(x, WJR_PP_QUEUE_FRONT(x)(y))

#define WJR_PP_QUEUE_TRANSFORM_ARG(queue, F, arg)                                        \
    WJR_PP_QUEUE_POP_FRONT(                                                              \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_SAME(                \
            WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_INIT(F, arg)),                   \
            WJR_PP_QUEUE_TRANSFORM_ARG_CALLER))))
#define WJR_PP_QUEUE_TRANSFORM_ARG_CALLER(x, y)                                          \
    WJR_PP_QUEUE_PUSH_BACK(                                                              \
        x, WJR_PP_QUEUE_FRONT(x)(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(x)), y))

// a, b, c -> ATTRIBUTE(a) ATTRIBUTE(b) ATTRIBUTE(c)
#define WJR_ATTRIBUTES(...)                                                              \
    WJR_PP_QUEUE_PUT(                                                                    \
        WJR_PP_QUEUE_TRANSFORM(WJR_PP_QUEUE_INIT(__VA_ARGS__), WJR_ATTRIBUTE))

// typename header0, typename header1, typename header2, ...
#define WJR_TYPENAMES(header, tailer)                                                    \
    WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM_ARG(tailer, WJR_TYPENAMES_CALLER, header))
#define WJR_TYPENAMES_CALLER(header, x) typename WJR_PP_CONCAT(header, x)

#define WJR_DEFINE_FUNC_IMPL(is_template, template_header, template_types,               \
                             template_fowards, template_backwards, func_ret_type,        \
                             func_name, func_types, func_vars)

#define WJR_DEFINE_FUNC()

// ASSERT_LEVEL : 0 ~ 3
// 0 : Release (defined(NDEBUG) && ! defined(WJR_DEBUG_LEVEL))
// 1 : Some simple runtime checks, such as boundary checks (default)
// 2 : Most runtime checks
// 3 : Maximize runtime checks
#if defined WJR_DEBUG_LEVEL
#define WJR_ASSERT_LEVEL WJR_DEBUG_LEVEL
#elif defined(NDEBUG)
#define WJR_ASSERT_LEVEL 0
#else
#define WJR_ASSERT_LEVEL 1
#endif //

#if defined(NDEBUG)
#define WJR_ASSERT_IMPL(expr)                                                            \
    abort();                                                                             \
    WJR_UNREACHABLE
#else
#define WJR_ASSERT_IMPL(expr)                                                            \
    assert(expr);                                                                        \
    WJR_UNREACHABLE
#endif

#define WJR_ASSERT_CHECK_I_NOMESSAGE(expr)                                               \
    do {                                                                                 \
        WJR_ASSERT_IMPL(expr);                                                           \
    } while (0)

#define WJR_ASSERT_CHECK_I_MESSAGE(expr, fmt, ...)                                       \
    do {                                                                                 \
        if (!WJR_UNLIKELY(expr)) {                                                       \
            fprintf(stderr, "Additional error message : " fmt "\n", ##__VA_ARGS__);      \
            WJR_ASSERT_IMPL(expr);                                                       \
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

#define WJR_ASSERT(level, ...)                                                           \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_ASSERT_LEVEL, level), WJR_ASSERT_CHECK_I,               \
                   WJR_ASSERT_UNCHECK_I)                                                 \
    (__VA_ARGS__)

#endif // ! WJR_PREPROCESSOR_EXTEND_HPP__