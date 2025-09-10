#ifndef WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__
#define WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/detail.hpp>
#include <wjr/preprocessor/queue/call.hpp>

#define WJR_PP_QUEUE_INIT_N(x, N) WJR_PP_QUEUE_INIT_N_I(x, N)
#define WJR_PP_QUEUE_INIT_N_I(x, N) (WJR_PP_REPEAT(x, N))

#define WJR_PP_QUEUE_CALL_N_SAME(args, op, N)                                                      \
    WJR_PP_QUEUE_CALL_N(args, WJR_PP_QUEUE_INIT_N(op, N), N)

#define WJR_PP_QUEUE_CALL_SAME(args, op) WJR_PP_QUEUE_CALL_N_SAME(args, op, WJR_PP_QUEUE_SIZE(args))

// (1,2,3), (f, g, h) -> (f(1), g(2), h(3))
#define WJR_PP_QUEUE_TRANSFORMS(queue, ops)                                                        \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(                                                     \
        WJR_PP_QUEUE_CALL_N_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_PUSH_BACK(ops, 0)),   \
                                 WJR_PP_QUEUE_TRANSFORMS_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_TRANSFORMS_CALLER(x, y)                                                       \
    WJR_PP_QUEUE_PUSH_BACK(WJR_PP_QUEUE_POP_FRONT(x), WJR_PP_QUEUE_FRONT(x)(y))

// (1,2,3), f -> (f(1), f(2), f(3))
#define WJR_PP_QUEUE_TRANSFORM(queue, op)                                                          \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(                                                     \
        WJR_PP_QUEUE_CALL_N_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, (op)),                             \
                                 WJR_PP_QUEUE_TRANSFORM_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_TRANSFORM_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, WJR_PP_QUEUE_FRONT(x)(y))

// 0, (1, 2, 3), (f, g, h) -> h(g(f(0, 1), 2), 3)
#define WJR_PP_QUEUE_ACCUMULATES(init, queue, ops)                                                 \
    WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                                \
        WJR_PP_QUEUE_PUSH_FRONT(queue,                                                             \
                                WJR_PP_QUEUE_PUSH_BACK(WJR_PP_QUEUE_PUSH_FRONT(ops, init), 0)),    \
        WJR_PP_QUEUE_ACCUMULATES_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_ACCUMULATES_CALLER(x, y)                                                      \
    WJR_PP_QUEUE_PUSH_FRONT(                                                                       \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(x)),                                         \
        WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(x))(WJR_PP_QUEUE_FRONT(x), y))

// 0, (1, 2, 3), f
#define WJR_PP_QUEUE_ACCUMULATE(init, queue, op)                                                   \
    WJR_PP_QUEUE_ACCUMULATES(init, queue, WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(queue)))

// (1, 2, 3) -> 3
#define WJR_PP_QUEUE_BACK(queue) WJR_PP_QUEUE_ACCUMULATE(0, queue, WJR_PP_QUEUE_BACK_CALLER)

#define WJR_PP_QUEUE_BACK_CALLER(x, y) y

// (1, 2, 3, 4, 5), 2 -> (1, 2, 3)
#define WJR_PP_QUEUE_POP_BACK_N(queue, N)                                                          \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL(                                   \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (0)),                                                       \
        WJR_PP_QUEUE_POP_FRONT_N(                                                                  \
            (WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER, WJR_PP_QUEUE_SIZE(queue)),       \
             WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER, N)),                             \
            N))))

#define WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, y)
#define WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER(x, y) x

// (1, 2, 3, 4, 5) -> (1, 2, 3, 4)
#define WJR_PP_QUEUE_POP_BACK(queue) WJR_PP_QUEUE_POP_BACK_N(queue, 1)

// (1, 2, 3, 4, 5), 2 -> 3
#define WJR_PP_QUEUE_AT(queue, N) WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT_N(queue, N))

// (a, b, c) -> (c, b, a)
#define WJR_PP_QUEUE_REVERSE(queue)                                                                \
    WJR_PP_QUEUE_POP_BACK(WJR_PP_QUEUE_FRONT(                                                      \
        WJR_PP_QUEUE_CALL_N_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, (0)), WJR_PP_QUEUE_REVERSE_CALLER, \
                                 WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_REVERSE_CALLER(x, y) WJR_PP_QUEUE_PUSH_FRONT(x, y)

// (a, b, c) -> a b c
#define WJR_PP_QUEUE_PUT(queue) WJR_PP_QUEUE_ACCUMULATE(, queue, WJR_PP_QUEUE_PUT_CALLER)

#define WJR_PP_QUEUE_PUT_CALLER(x, y) x y

// ((A), (B), (C)) -> (A, B, C)
#define WJR_PP_QUEUE_UNWRAP(queue) WJR_PP_QUEUE_TRANSFORM(queue, WJR_PP_QUEUE_UNWRAP_CALLER)

#define WJR_PP_QUEUE_UNWRAP_CALLER(x) WJR_PP_QUEUE_UNWRAP_CALLER_I x
#define WJR_PP_QUEUE_UNWRAP_CALLER_I(...) __VA_ARGS__

// ((A), (B), (C)) -> A B C
#define WJR_PP_QUEUE_UNWRAP_PUT(queue)                                                             \
    WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(                                                        \
        WJR_PP_QUEUE_CALL_N_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, ()),                               \
                                 WJR_PP_QUEUE_UNWRAP_PUT_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_UNWRAP_PUT_CALLER(x, y) (WJR_PP_QUEUE_EXPAND(x) WJR_PP_QUEUE_EXPAND(y))

// todo: zip variadic queues

// (A, B, C) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_2(queue1, queue2)                                                         \
    WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME((queue1, queue2), WJR_PP_QUEUE_ZIP_2_CALLER,       \
                                                WJR_PP_QUEUE_SIZE(queue1)))

#define WJR_PP_QUEUE_ZIP_2_CALLER(x, y)                                                            \
    WJR_PP_QUEUE_POP_FRONT(                                                                        \
        WJR_PP_QUEUE_PUSH_BACK(x, (WJR_PP_QUEUE_FRONT(x), WJR_PP_QUEUE_FRONT(y)))),                \
        WJR_PP_QUEUE_POP_FRONT(y)

// ((A), (B), (C)) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_MORE(queue1, queue2)                                                      \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                            \
        ((queue1), queue2), WJR_PP_QUEUE_ZIP_MORE_CALLER, WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_MORE_CALLER(x, y)                                                         \
    WJR_PP_QUEUE_PUSH_FRONT(                                                                       \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                             \
            x, (WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x))),                    \
                WJR_PP_QUEUE_FRONT(y)))),                                                          \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                            \
        WJR_PP_QUEUE_POP_FRONT(y)

// (A, B, C) -> ((0, A), (1, B), (2, C))
#define WJR_PP_QUEUE_ZIP_IOTA(queue)                                                               \
    WJR_PP_QUEUE_ZIP_IOTA_I((WJR_PP_IOTA(WJR_PP_QUEUE_SIZE(queue))), queue)
#define WJR_PP_QUEUE_ZIP_IOTA_I(ID, queue) WJR_PP_QUEUE_ZIP_2(ID, queue)

// (a, b, c) -> f(a) f(b) f(c)
#define WJR_PP_TRANSFORM_PUT(queue, op) WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

// (a, b, c) -> (f(a), f(b), f(c)) (note : f(x) = (g(x))) -> g(a) g(b) g(c)
#define WJR_PP_TRANSFORM_UNWRAP_PUT(queue, op)                                                     \
    WJR_PP_QUEUE_UNWRAP_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

#endif // WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__