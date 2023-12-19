#ifndef WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__
#define WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/details.hpp>
#include <wjr/preprocessor/queue/call.hpp>

#define WJR_PP_QUEUE_INIT_N(x, N) WJR_PP_QUEUE_INIT_N_I(x, N)
#define WJR_PP_QUEUE_INIT_N_I(x, N) (WJR_PP_REPEAT(x, N))

#define WJR_PP_QUEUE_CALL_N_SAME(args, op, N)                                            \
    WJR_PP_QUEUE_CALL_N(args, WJR_PP_QUEUE_INIT_N(op, N), N)

#define WJR_PP_QUEUE_CALL_SAME(args, op)                                                 \
    WJR_PP_QUEUE_CALL_N_SAME(args, op, WJR_PP_QUEUE_SIZE(args))

// (1,2,3), (f, g, h) -> (f()(1), g()(2), h()(3))
#define WJR_PP_QUEUE_TRANSFORMS(queue, ops)                                              \
    WJR_PP_DEFER(WJR_PP_QUEUE_POP_FRONT_FN)                                              \
    ()(WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)()(WJR_PP_QUEUE_CALL_N_SAME(                   \
        WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_PUSH_BACK(ops, 0)),                  \
        WJR_PP_QUEUE_TRANSFORMS_CALLER_FN, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_TRANSFORMS_FN() WJR_PP_QUEUE_TRANSFORMS

#define WJR_PP_QUEUE_TRANSFORMS_CALLER(x, y)                                             \
    WJR_PP_QUEUE_PUSH_BACK(WJR_PP_QUEUE_POP_FRONT(x), WJR_PP_QUEUE_FRONT(x)()(y))

#define WJR_PP_QUEUE_TRANSFORMS_CALLER_FN() WJR_PP_QUEUE_TRANSFORMS_CALLER

// (1,2,3), f -> (f()(1), f()(2), f()(3))
#define WJR_PP_QUEUE_TRANSFORM(queue, op)                                                \
    WJR_PP_QUEUE_TRANSFORMS(queue, WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(queue)))

#define WJR_PP_QUEUE_TRANSFORM_FN() WJR_PP_QUEUE_TRANSFORM

// 0, (1, 2, 3), (f, g, h) -> h()(g()(f()(0, 1), 2), 3)
#define WJR_PP_QUEUE_ACCUMULATES(init, queue, ops)                                       \
    WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)                                                  \
    ()(WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)()(WJR_PP_QUEUE_CALL_N_SAME(                   \
        WJR_PP_QUEUE_PUSH_FRONT(                                                         \
            queue, WJR_PP_QUEUE_PUSH_BACK(WJR_PP_QUEUE_PUSH_FRONT(ops, init), 0)),       \
        WJR_PP_QUEUE_ACCUMULATES_CALLER_FN, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_ACCUMULATES_FN() WJR_PP_QUEUE_ACCUMULATES

#define WJR_PP_QUEUE_ACCUMULATES_CALLER(x, y)                                            \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(x)),                               \
        WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(x))()(WJR_PP_QUEUE_FRONT(x), y))

#define WJR_PP_QUEUE_ACCUMULATES_CALLER_FN() WJR_PP_QUEUE_ACCUMULATES_CALLER

// 0, (1, 2, 3), f
#define WJR_PP_QUEUE_ACCUMULATE(init, queue, op)                                         \
    WJR_PP_QUEUE_ACCUMULATES(init, queue,                                                \
                             WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(queue)))

#define WJR_PP_QUEUE_ACCUMULATE_FN() WJR_PP_QUEUE_ACCUMULATE

// (1, 2, 3) -> 3
#define WJR_PP_QUEUE_BACK(queue)                                                         \
    WJR_PP_QUEUE_ACCUMULATE(0, queue, WJR_PP_QUEUE_BACK_CALLER_FN)

#define WJR_PP_QUEUE_BACK_FN() WJR_PP_QUEUE_BACK

#define WJR_PP_QUEUE_BACK_CALLER(x, y) y
#define WJR_PP_QUEUE_BACK_CALLER_FN() WJR_PP_QUEUE_BACK_CALLER

// (1, 2, 3, 4, 5), 2 -> (3, 4, 5)
#define WJR_PP_QUEUE_POP_FRONT_N(queue, N)                                               \
    WJR_PP_QUEUE_CALL_N_SAME(queue, WJR_PP_QUEUE_POP_FRONT_N_CALLER_FN, N)

#define WJR_PP_QUEUE_POP_FRONT_N_FN() WJR_PP_QUEUE_POP_FRONT_N

#define WJR_PP_QUEUE_POP_FRONT_N_CALLER(x, y) y
#define WJR_PP_QUEUE_POP_FRONT_N_CALLER_FN() WJR_PP_QUEUE_POP_FRONT_N_CALLER

// (1, 2, 3, 4, 5), 2 -> (1, 2, 3)
#define WJR_PP_QUEUE_POP_BACK_N(queue, N)                                                \
    WJR_PP_DEFER(WJR_PP_QUEUE_POP_FRONT_FN)                                              \
    (WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)(WJR_PP_QUEUE_CALL(                              \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (0)),                                             \
        WJR_PP_QUEUE_POP_FRONT_N(                                                        \
            (WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER_FN,                     \
                           WJR_PP_QUEUE_SIZE(queue)),                                    \
             WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER_FN, N)),                \
            N))))

#define WJR_PP_QUEUE_POP_BACK_N_FN() WJR_PP_QUEUE_POP_BACK_N

#define WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, y)
#define WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER_FN() WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER
#define WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER(x, y) x
#define WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER_FN() WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER

#define WJR_PP_QUEUE_POP_BACK(queue) WJR_PP_QUEUE_POP_BACK_N(queue, 1)

#define WJR_PP_QUEUE_POP_BACK_FN() WJR_PP_QUEUE_POP_BACK

#define WJR_PP_QUEUE_AT(queue, N)                                                        \
    WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)(WJR_PP_DEFER(WJR_PP_QUEUE_POP_FRONT_FN)(queue, N))

#define WJR_PP_QUEUE_AT_FN() WJR_PP_QUEUE_AT

#define WJR_PP_QUEUE_REVERSE(queue)                                                      \
    WJR_PP_QUEUE_POP_BACK(WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)(WJR_PP_QUEUE_CALL_N_SAME(  \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (0)), WJR_PP_QUEUE_REVERSE_CALLER_FN,             \
        WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_REVERSE_FN() WJR_PP_QUEUE_REVERSE

#define WJR_PP_QUEUE_REVERSE_CALLER(x, y) WJR_PP_QUEUE_PUSH_FRONT(x, y)

#define WJR_PP_QUEUE_REVERSE_CALLER_FN() WJR_PP_QUEUE_REVERSE_CALLER

// (a, b, c) -> a b c
#define WJR_PP_QUEUE_PUT(queue)                                                          \
    WJR_PP_DEFER(WJR_PP_QUEUE_EXPAND_FN)                                                 \
    (WJR_PP_QUEUE_CALL_SAME(queue, WJR_PP_QUEUE_PUT_CALLER_FN))

#define WJR_PP_QUEUE_PUT_FN() WJR_PP_QUEUE_PUT

#define WJR_PP_QUEUE_PUT_CALLER(x, y) x y
#define WJR_PP_QUEUE_PUT_CALLER_FN() WJR_PP_QUEUE_PUT_CALLER

// ((A), (B), (C)) -> (A, B, C)
#define WJR_PP_QUEUE_UNWRAP(queue)                                                       \
    WJR_PP_QUEUE_TRANSFORM(queue, WJR_PP_QUEUE_UNWRAP_CALLER_FN)

#define WJR_PP_QUEUE_UNWRAP_FN() WJR_PP_QUEUE_UNWRAP

#define WJR_PP_QUEUE_UNWRAP_CALLER(x) WJR_PP_QUEUE_UNWRAP_CALLER_I x
#define WJR_PP_QUEUE_UNWRAP_CALLER_I(x) x
#define WJR_PP_QUEUE_UNWRAP_CALLER_FN() WJR_PP_QUEUE_UNWRAP_CALLER

// (A, B, C) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_2(queue1, queue2)                                               \
    WJR_PP_DEFER(WJR_PP_QUEUE_POP_FRONT_FN)                                              \
    (WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)(WJR_PP_QUEUE_CALL_N_SAME(                       \
        ((queue1), queue2), WJR_PP_QUEUE_ZIP_2_CALLER_FN, WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_2_FN() WJR_PP_QUEUE_ZIP_2

#define WJR_PP_QUEUE_ZIP_2_CALLER(x, y)                                                  \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                   \
            x, (WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x)), WJR_PP_QUEUE_FRONT(y)))),     \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                  \
        WJR_PP_QUEUE_POP_FRONT(y)

#define WJR_PP_QUEUE_ZIP_2_CALLER_FN() WJR_PP_QUEUE_ZIP_2_CALLER

// ((A), (B), (C)) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_MORE(queue1, queue2)                                            \
    WJR_PP_DEFER(WJR_PP_QUEUE_POP_FRONT_FN)                                              \
    (WJR_PP_DEFER(WJR_PP_QUEUE_FRONT_FN)(                                                \
        WJR_PP_QUEUE_CALL_N_SAME(((queue1), queue2), WJR_PP_QUEUE_ZIP_MORE_CALLER_FN,    \
                                 WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_MORE_CALLER(x, y)                                               \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                   \
            x, (WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x))),          \
                WJR_PP_QUEUE_FRONT(y)))),                                                \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                  \
        WJR_PP_QUEUE_POP_FRONT(y)

#define WJR_PP_QUEUE_ZIP_MORE_CALLER_FN() WJR_PP_QUEUE_ZIP_MORE_CALLER

#define WJR_PP_QUEUE_ZIP_MORE_FN() WJR_PP_QUEUE_ZIP_MORE

#define WJR_PP_QUEUE_ZIP(...)                                                            \
    WJR_PP_QUEUE_ZIP_I(WJR_PP_ARGS_LEN(__VA_ARGS__), __VA_ARGS__)
#define WJR_PP_QUEUE_ZIP_I(N, ...)                                                       \
    WJR_PP_QUEUE_CALL(                                                                   \
        (__VA_ARGS__),                                                                   \
        WJR_PP_QUEUE_PUSH_FRONT(WJR_PP_QUEUE_INIT_N(WJR_PP_QUEUE_ZIP_MORE_FN, N),        \
                                WJR_PP_QUEUE_ZIP_2_FN))

/*
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
*/

#endif // WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__