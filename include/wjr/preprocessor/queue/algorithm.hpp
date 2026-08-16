#ifndef WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__
#define WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__

#include <wjr/preprocessor/arithmetic.hpp>
#include <wjr/preprocessor/detail.hpp>
#include <wjr/preprocessor/queue/call.hpp>
#include <wjr/preprocessor/queue/recursive.hpp>
#include <wjr/preprocessor/queue/zip.hpp>

#define WJR_PP_QUEUE_INIT_N(x, N) WJR_PP_QUEUE_INIT_N_I(x, N)
#define WJR_PP_QUEUE_INIT_N_I(x, N) (WJR_PP_REPEAT(x, N))

#define WJR_PP_QUEUE_CALL_N_SAME(args, op, N)                                                      \
    WJR_PP_QUEUE_CALL_N(args, WJR_PP_QUEUE_INIT_N(op, N), N)

#define WJR_PP_QUEUE_CALL_SAME(args, op) WJR_PP_QUEUE_CALL_N_SAME(args, op, WJR_PP_QUEUE_SIZE(args))

#define WJR_PP_QUEUE_CALL_R_SAME(r, args, op)                                                      \
    WJR_PP_QUEUE_CALL_R(r, args, WJR_PP_QUEUE_INIT_N(op, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))))

// (state, a, b, c), f -> (f(f(f(state, a), b), c))
#define WJR_PP_QUEUE_FOLD(args, op)                                                                \
    WJR_PP_QUEUE_CALL_N_SAME(args, op, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args)))

#define WJR_PP_QUEUE_FOLD_R(r, args, op) WJR_PP_CONCAT(__WJR_PP_QUEUE_FOLD_R_, r)(args, op)
#define __WJR_PP_QUEUE_FOLD_R_0(args, op)                                                          \
    WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                                                       \
        args, WJR_PP_QUEUE_INIT_N(op, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))),                        \
        WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))))
#define __WJR_PP_QUEUE_FOLD_R_1(args, op)                                                          \
    WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                                                       \
        args, WJR_PP_QUEUE_INIT_N(op, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))),                        \
        WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))))
#define __WJR_PP_QUEUE_FOLD_R_2(args, op)                                                          \
    WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                                                       \
        args, WJR_PP_QUEUE_INIT_N(op, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))),                        \
        WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))))
#define __WJR_PP_QUEUE_FOLD_R_3(args, op)                                                          \
    WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                                                       \
        args, WJR_PP_QUEUE_INIT_N(op, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))),                        \
        WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args))))

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

// (a, b, c), f -> (f(a), f(b), f(c))
#define WJR_PP_QUEUE_MAP(queue, op) WJR_PP_QUEUE_TRANSFORM(queue, op)
#define WJR_PP_QUEUE_MAP_R(r, queue, op) WJR_PP_QUEUE_TRANSFORM_R(r, queue, op)

#define WJR_PP_QUEUE_TRANSFORM_R(r, queue, op)                                                     \
    WJR_PP_CONCAT(__WJR_PP_QUEUE_TRANSFORM_R_, r)(queue, op)
#define __WJR_PP_QUEUE_TRANSFORM_R_0(queue, op)                                                    \
    __WJR_PP_QUEUE_TRANSFORM_R_0_I(WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                        \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (op)),                                                      \
        WJR_PP_QUEUE_INIT_N(WJR_PP_QUEUE_TRANSFORM_R_CALLER_0, WJR_PP_QUEUE_SIZE(queue)),          \
        WJR_PP_QUEUE_SIZE(queue))))
#define __WJR_PP_QUEUE_TRANSFORM_R_0_I(result) WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(result))
#define WJR_PP_QUEUE_TRANSFORM_R_CALLER_0(state, elem)                                             \
    WJR_PP_QUEUE_PUSH_BACK(                                                                        \
        state, WJR_PP_OBSTRUCT(WJR_PP_QUEUE_TRANSFORM_R_APPLY_0)(WJR_PP_QUEUE_FRONT(state), elem))
#define WJR_PP_QUEUE_TRANSFORM_R_APPLY_0(op, elem) op(elem)

#define __WJR_PP_QUEUE_TRANSFORM_R_1(queue, op)                                                    \
    __WJR_PP_QUEUE_TRANSFORM_R_1_I(WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                        \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (op)),                                                      \
        WJR_PP_QUEUE_INIT_N(WJR_PP_QUEUE_TRANSFORM_R_CALLER_1, WJR_PP_QUEUE_SIZE(queue)),          \
        WJR_PP_QUEUE_SIZE(queue))))
#define __WJR_PP_QUEUE_TRANSFORM_R_1_I(result) WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(result))
#define WJR_PP_QUEUE_TRANSFORM_R_CALLER_1(state, elem)                                             \
    WJR_PP_QUEUE_PUSH_BACK(                                                                        \
        state, WJR_PP_OBSTRUCT(WJR_PP_QUEUE_TRANSFORM_R_APPLY_1)(WJR_PP_QUEUE_FRONT(state), elem))
#define WJR_PP_QUEUE_TRANSFORM_R_APPLY_1(op, elem) op(elem)

#define __WJR_PP_QUEUE_TRANSFORM_R_2(queue, op)                                                    \
    __WJR_PP_QUEUE_TRANSFORM_R_2_I(WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                        \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (op)),                                                      \
        WJR_PP_QUEUE_INIT_N(WJR_PP_QUEUE_TRANSFORM_R_CALLER_2, WJR_PP_QUEUE_SIZE(queue)),          \
        WJR_PP_QUEUE_SIZE(queue))))
#define __WJR_PP_QUEUE_TRANSFORM_R_2_I(result) WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(result))
#define WJR_PP_QUEUE_TRANSFORM_R_CALLER_2(state, elem)                                             \
    WJR_PP_QUEUE_PUSH_BACK(                                                                        \
        state, WJR_PP_OBSTRUCT(WJR_PP_QUEUE_TRANSFORM_R_APPLY_2)(WJR_PP_QUEUE_FRONT(state), elem))
#define WJR_PP_QUEUE_TRANSFORM_R_APPLY_2(op, elem) op(elem)

#define __WJR_PP_QUEUE_TRANSFORM_R_3(queue, op)                                                    \
    __WJR_PP_QUEUE_TRANSFORM_R_3_I(WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                        \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (op)),                                                      \
        WJR_PP_QUEUE_INIT_N(WJR_PP_QUEUE_TRANSFORM_R_CALLER_3, WJR_PP_QUEUE_SIZE(queue)),          \
        WJR_PP_QUEUE_SIZE(queue))))
#define __WJR_PP_QUEUE_TRANSFORM_R_3_I(result) WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(result))
#define WJR_PP_QUEUE_TRANSFORM_R_CALLER_3(state, elem)                                             \
    WJR_PP_QUEUE_PUSH_BACK(                                                                        \
        state, WJR_PP_OBSTRUCT(WJR_PP_QUEUE_TRANSFORM_R_APPLY_3)(WJR_PP_QUEUE_FRONT(state), elem))
#define WJR_PP_QUEUE_TRANSFORM_R_APPLY_3(op, elem) op(elem)

#define WJR_PP_QUEUE_TRANSFORMS_R(r, queue, ops)                                                   \
    WJR_PP_QUEUE_TRANSFORM_R(r, WJR_PP_QUEUE_ZIP_2_R(r, queue, ops),                               \
                             WJR_PP_QUEUE_TRANSFORMS_R_APPLY)
#define WJR_PP_QUEUE_TRANSFORMS_R_APPLY(pair) WJR_PP_QUEUE_TRANSFORMS_R_APPLY_I pair
#define WJR_PP_QUEUE_TRANSFORMS_R_APPLY_I(elem, op) op(elem)

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

#define WJR_PP_QUEUE_ACCUMULATE_R(r, init, queue, op)                                              \
    WJR_PP_QUEUE_FOLD_R(r, WJR_PP_QUEUE_PUSH_FRONT(queue, init), op)

#define WJR_PP_QUEUE_ACCUMULATES_R(r, init, queue, ops)                                            \
    WJR_PP_QUEUE_FOLD_R(r, WJR_PP_QUEUE_PUSH_FRONT(WJR_PP_QUEUE_ZIP_2_R(r, ops, queue), init),     \
                        WJR_PP_QUEUE_ACCUMULATES_R_APPLY)
#define WJR_PP_QUEUE_ACCUMULATES_R_APPLY(state, pair)                                              \
    WJR_PP_QUEUE_ACCUMULATES_R_APPLY_I(state, WJR_PP_QUEUE_FRONT(pair),                            \
                                       WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(pair)))
#define WJR_PP_QUEUE_ACCUMULATES_R_APPLY_I(state, op, elem) op(state, elem)

// (1, 2, 3) -> 3
#define WJR_PP_QUEUE_BACK(queue) WJR_PP_QUEUE_AT(queue, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(queue)))

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

#define WJR_PP_QUEUE_PUT_R(r, queue)                                                               \
    WJR_PP_QUEUE_EXPAND(                                                                           \
        WJR_PP_QUEUE_FOLD_R(r, WJR_PP_QUEUE_PUSH_FRONT(queue, ), WJR_PP_QUEUE_PUT_CALLER))

// ((A), (B), (C)) -> (A, B, C)
#define WJR_PP_QUEUE_UNWRAP(queue) WJR_PP_QUEUE_TRANSFORM(queue, WJR_PP_QUEUE_UNWRAP_CALLER)

#define WJR_PP_QUEUE_UNWRAP_CALLER(x) WJR_PP_QUEUE_UNWRAP_CALLER_I x
#define WJR_PP_QUEUE_UNWRAP_CALLER_I(...) __VA_ARGS__

#define WJR_PP_QUEUE_UNWRAP_R(r, queue)                                                            \
    WJR_PP_QUEUE_TRANSFORM_R(r, queue, WJR_PP_QUEUE_UNWRAP_CALLER)

// ((A), (B), (C)) -> A B C
#define WJR_PP_QUEUE_UNWRAP_PUT(queue)                                                             \
    WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(                                                        \
        WJR_PP_QUEUE_CALL_N_SAME(WJR_PP_QUEUE_PUSH_FRONT(queue, ()),                               \
                                 WJR_PP_QUEUE_UNWRAP_PUT_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_UNWRAP_PUT_CALLER(x, y) (WJR_PP_QUEUE_EXPAND(x) WJR_PP_QUEUE_EXPAND(y))

// (a, b, c) -> f(a) f(b) f(c)
#define WJR_PP_TRANSFORM_PUT(queue, op) WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

// (a, b, c) -> (f(a), f(b), f(c)) (note : f(x) = (g(x))) -> g(a) g(b) g(c)
#define WJR_PP_TRANSFORM_UNWRAP_PUT(queue, op)                                                     \
    WJR_PP_QUEUE_UNWRAP_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

#define WJR_PP_QUEUE_IF(cond, t, f) WJR_PP_QUEUE_EXPAND(WJR_PP_BOOL_IF(cond, t, f))
#define WJR_PP_QUEUE_IF_NZ(expr, t, f) WJR_PP_QUEUE_IF(WJR_PP_BOOL(expr), t, f)

#endif // WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__