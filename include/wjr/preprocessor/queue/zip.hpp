#ifndef WJR_PREPROCESSOR_QUEUE_ZIP_HPP__
#define WJR_PREPROCESSOR_QUEUE_ZIP_HPP__

#include <wjr/preprocessor/arithmetic.hpp>
#include <wjr/preprocessor/detail.hpp>
#include <wjr/preprocessor/queue/call.hpp>

// The recursive algorithms use this header directly, while algorithm.hpp only
// exposes the high-level wrappers.

#define WJR_PP_QUEUE_ZIP_INIT_N(op, n) WJR_PP_QUEUE_ZIP_INIT_N_I(op, n)
#define WJR_PP_QUEUE_ZIP_INIT_N_I(op, n) (WJR_PP_REPEAT(op, n))

// (A, B, C) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_2(queue1, queue2)                                                         \
    WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME((queue1, queue2), WJR_PP_QUEUE_ZIP_2_CALLER,       \
                                                WJR_PP_QUEUE_SIZE(queue1)))

#define WJR_PP_QUEUE_ZIP_2_CALLER(x, y)                                                            \
    WJR_PP_QUEUE_POP_FRONT(                                                                        \
        WJR_PP_QUEUE_PUSH_BACK(x, (WJR_PP_QUEUE_FRONT(x), WJR_PP_QUEUE_FRONT(y)))),                \
        WJR_PP_QUEUE_POP_FRONT(y)

#define WJR_PP_QUEUE_ZIP_2_R(r, queue1, queue2)                                                    \
    WJR_PP_CONCAT(__WJR_PP_QUEUE_ZIP_2_R_, r)(queue1, queue2)
#define __WJR_PP_QUEUE_ZIP_2_R_0(queue1, queue2)                                                   \
    __WJR_PP_QUEUE_ZIP_2_R_0_I(WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                            \
        (queue1, queue2),                                                                          \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_2_CALLER_0, WJR_PP_QUEUE_SIZE(queue1)),           \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_2_R_0_I(result) WJR_PP_QUEUE_FRONT(result)
#define WJR_PP_QUEUE_ZIP_2_CALLER_0(x, y)                                                          \
    WJR_PP_QUEUE_POP_FRONT(                                                                        \
        WJR_PP_QUEUE_PUSH_BACK(x, (WJR_PP_QUEUE_FRONT(x), WJR_PP_QUEUE_FRONT(y)))),                \
        WJR_PP_QUEUE_POP_FRONT(y)

#define WJR_PP_QUEUE_ZIP_3_R(r, queue1, queue2, queue3)                                            \
    WJR_PP_CONCAT(__WJR_PP_QUEUE_ZIP_3_R_, r)(queue1, queue2, queue3)
#define WJR_PP_QUEUE_ZIP_R_QUEUE(state, n) WJR_PP_QUEUE_AT(WJR_PP_QUEUE_POP_FRONT(state), n)
#define __WJR_PP_QUEUE_ZIP_3_R_0(queue1, queue2, queue3)                                           \
    __WJR_PP_QUEUE_ZIP_3_R_0_I(WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                            \
        (((0), queue1, queue2, queue3), WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),       \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_3_R_CALLER_0, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_3_R_0_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_3_R_CALLER_0(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_3_R_APPLY_0)(state, unused)
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_0(state, unused)                                                \
    WJR_PP_QUEUE_ZIP_3_R_APPLY_0_I(WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),  \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1),                             \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2))
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_0_I(result, queue1, queue2, queue3)                             \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3))),                                 \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3))

#define __WJR_PP_QUEUE_ZIP_3_R_1(queue1, queue2, queue3)                                           \
    __WJR_PP_QUEUE_ZIP_3_R_1_I(WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                            \
        (((0), queue1, queue2, queue3), WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),       \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_3_R_CALLER_1, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_3_R_1_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_3_R_CALLER_1(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_3_R_APPLY_1)(state, unused)
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_1(state, unused)                                                \
    WJR_PP_QUEUE_ZIP_3_R_APPLY_1_I(WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),  \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1),                             \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2))
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_1_I(result, queue1, queue2, queue3)                             \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3))),                                 \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3))

#define __WJR_PP_QUEUE_ZIP_3_R_2(queue1, queue2, queue3)                                           \
    __WJR_PP_QUEUE_ZIP_3_R_2_I(WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                            \
        (((0), queue1, queue2, queue3), WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),       \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_3_R_CALLER_2, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_3_R_2_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_3_R_CALLER_2(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_3_R_APPLY_2)(state, unused)
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_2(state, unused)                                                \
    WJR_PP_QUEUE_ZIP_3_R_APPLY_2_I(WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),  \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1),                             \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2))
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_2_I(result, queue1, queue2, queue3)                             \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3))),                                 \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3))

#define __WJR_PP_QUEUE_ZIP_3_R_3(queue1, queue2, queue3)                                           \
    __WJR_PP_QUEUE_ZIP_3_R_3_I(WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                            \
        (((0), queue1, queue2, queue3), WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),       \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_3_R_CALLER_3, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_3_R_3_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_3_R_CALLER_3(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_3_R_APPLY_3)(state, unused)
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_3(state, unused)                                                \
    WJR_PP_QUEUE_ZIP_3_R_APPLY_3_I(WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),  \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1),                             \
                                   WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2))
#define WJR_PP_QUEUE_ZIP_3_R_APPLY_3_I(result, queue1, queue2, queue3)                             \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3))),                                 \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3))

#define WJR_PP_QUEUE_ZIP_4_R(r, queue1, queue2, queue3, queue4)                                    \
    WJR_PP_CONCAT(__WJR_PP_QUEUE_ZIP_4_R_, r)(queue1, queue2, queue3, queue4)
#define WJR_PP_QUEUE_ZIP_4_R_APPLY(state, unused)                                                  \
    WJR_PP_QUEUE_ZIP_4_R_APPLY_I(WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),    \
                                 WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1),                               \
                                 WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2),                               \
                                 WJR_PP_QUEUE_ZIP_R_QUEUE(state, 3))
#define WJR_PP_QUEUE_ZIP_4_R_APPLY_I(result, queue1, queue2, queue3, queue4)                       \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3), WJR_PP_QUEUE_FRONT(queue4))),     \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3), WJR_PP_QUEUE_POP_FRONT(queue4))

#define __WJR_PP_QUEUE_ZIP_4_R_0(queue1, queue2, queue3, queue4)                                   \
    __WJR_PP_QUEUE_ZIP_4_R_0_I(WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                            \
        (((0), queue1, queue2, queue3, queue4),                                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_4_R_CALLER_0, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_4_R_0_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_4_R_CALLER_0(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_4_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_4_R_1(queue1, queue2, queue3, queue4)                                   \
    __WJR_PP_QUEUE_ZIP_4_R_1_I(WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                            \
        (((0), queue1, queue2, queue3, queue4),                                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_4_R_CALLER_1, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_4_R_1_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_4_R_CALLER_1(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_4_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_4_R_2(queue1, queue2, queue3, queue4)                                   \
    __WJR_PP_QUEUE_ZIP_4_R_2_I(WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                            \
        (((0), queue1, queue2, queue3, queue4),                                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_4_R_CALLER_2, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_4_R_2_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_4_R_CALLER_2(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_4_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_4_R_3(queue1, queue2, queue3, queue4)                                   \
    __WJR_PP_QUEUE_ZIP_4_R_3_I(WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                            \
        (((0), queue1, queue2, queue3, queue4),                                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_4_R_CALLER_3, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_4_R_3_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_4_R_CALLER_3(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_4_R_APPLY)(state, unused)

#define WJR_PP_QUEUE_ZIP_5_R(r, queue1, queue2, queue3, queue4, queue5)                            \
    WJR_PP_CONCAT(__WJR_PP_QUEUE_ZIP_5_R_, r)(queue1, queue2, queue3, queue4, queue5)
#define WJR_PP_QUEUE_ZIP_5_R_APPLY(state, unused)                                                  \
    WJR_PP_QUEUE_ZIP_5_R_APPLY_I(                                                                  \
        WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),                             \
        WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2),                    \
        WJR_PP_QUEUE_ZIP_R_QUEUE(state, 3), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 4))
#define WJR_PP_QUEUE_ZIP_5_R_APPLY_I(result, queue1, queue2, queue3, queue4, queue5)               \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3), WJR_PP_QUEUE_FRONT(queue4),       \
                                     WJR_PP_QUEUE_FRONT(queue5))),                                 \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3), WJR_PP_QUEUE_POP_FRONT(queue4),                               \
     WJR_PP_QUEUE_POP_FRONT(queue5))

#define __WJR_PP_QUEUE_ZIP_5_R_0(queue1, queue2, queue3, queue4, queue5)                           \
    __WJR_PP_QUEUE_ZIP_5_R_0_I(WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                            \
        (((0), queue1, queue2, queue3, queue4, queue5),                                            \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_5_R_CALLER_0, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_5_R_0_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_5_R_CALLER_0(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_5_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_5_R_1(queue1, queue2, queue3, queue4, queue5)                           \
    __WJR_PP_QUEUE_ZIP_5_R_1_I(WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                            \
        (((0), queue1, queue2, queue3, queue4, queue5),                                            \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_5_R_CALLER_1, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_5_R_1_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_5_R_CALLER_1(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_5_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_5_R_2(queue1, queue2, queue3, queue4, queue5)                           \
    __WJR_PP_QUEUE_ZIP_5_R_2_I(WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                            \
        (((0), queue1, queue2, queue3, queue4, queue5),                                            \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_5_R_CALLER_2, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_5_R_2_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_5_R_CALLER_2(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_5_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_5_R_3(queue1, queue2, queue3, queue4, queue5)                           \
    __WJR_PP_QUEUE_ZIP_5_R_3_I(WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                            \
        (((0), queue1, queue2, queue3, queue4, queue5),                                            \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_5_R_CALLER_3, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_5_R_3_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_5_R_CALLER_3(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_5_R_APPLY)(state, unused)

#define WJR_PP_QUEUE_ZIP_6_R(r, queue1, queue2, queue3, queue4, queue5, queue6)                    \
    WJR_PP_CONCAT(__WJR_PP_QUEUE_ZIP_6_R_, r)(queue1, queue2, queue3, queue4, queue5, queue6)
#define WJR_PP_QUEUE_ZIP_6_R_APPLY(state, unused)                                                  \
    WJR_PP_QUEUE_ZIP_6_R_APPLY_I(                                                                  \
        WJR_PP_QUEUE_FRONT(state), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 0),                             \
        WJR_PP_QUEUE_ZIP_R_QUEUE(state, 1), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 2),                    \
        WJR_PP_QUEUE_ZIP_R_QUEUE(state, 3), WJR_PP_QUEUE_ZIP_R_QUEUE(state, 4),                    \
        WJR_PP_QUEUE_ZIP_R_QUEUE(state, 5))
#define WJR_PP_QUEUE_ZIP_6_R_APPLY_I(result, queue1, queue2, queue3, queue4, queue5, queue6)       \
    (WJR_PP_QUEUE_PUSH_BACK(result, (WJR_PP_QUEUE_FRONT(queue1), WJR_PP_QUEUE_FRONT(queue2),       \
                                     WJR_PP_QUEUE_FRONT(queue3), WJR_PP_QUEUE_FRONT(queue4),       \
                                     WJR_PP_QUEUE_FRONT(queue5), WJR_PP_QUEUE_FRONT(queue6))),     \
     WJR_PP_QUEUE_POP_FRONT(queue1), WJR_PP_QUEUE_POP_FRONT(queue2),                               \
     WJR_PP_QUEUE_POP_FRONT(queue3), WJR_PP_QUEUE_POP_FRONT(queue4),                               \
     WJR_PP_QUEUE_POP_FRONT(queue5), WJR_PP_QUEUE_POP_FRONT(queue6))

#define __WJR_PP_QUEUE_ZIP_6_R_0(queue1, queue2, queue3, queue4, queue5, queue6)                   \
    __WJR_PP_QUEUE_ZIP_6_R_0_I(WJR_PP_EVAL_R_0(WJR_PP_QUEUE_CALL_R_N_0(                            \
        (((0), queue1, queue2, queue3, queue4, queue5, queue6),                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_6_R_CALLER_0, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_6_R_0_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_6_R_CALLER_0(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_6_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_6_R_1(queue1, queue2, queue3, queue4, queue5, queue6)                   \
    __WJR_PP_QUEUE_ZIP_6_R_1_I(WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                            \
        (((0), queue1, queue2, queue3, queue4, queue5, queue6),                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_6_R_CALLER_1, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_6_R_1_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_6_R_CALLER_1(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_6_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_6_R_2(queue1, queue2, queue3, queue4, queue5, queue6)                   \
    __WJR_PP_QUEUE_ZIP_6_R_2_I(WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                            \
        (((0), queue1, queue2, queue3, queue4, queue5, queue6),                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_6_R_CALLER_2, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_6_R_2_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_6_R_CALLER_2(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_6_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_6_R_3(queue1, queue2, queue3, queue4, queue5, queue6)                   \
    __WJR_PP_QUEUE_ZIP_6_R_3_I(WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                            \
        (((0), queue1, queue2, queue3, queue4, queue5, queue6),                                    \
         WJR_PP_QUEUE_INIT_N((), WJR_PP_QUEUE_SIZE(queue1))),                                      \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_6_R_CALLER_3, WJR_PP_QUEUE_SIZE(queue1)),         \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_6_R_3_I(result)                                                         \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(result)))
#define WJR_PP_QUEUE_ZIP_6_R_CALLER_3(state, unused)                                               \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_ZIP_6_R_APPLY)(state, unused)

#define __WJR_PP_QUEUE_ZIP_2_R_1(queue1, queue2)                                                   \
    __WJR_PP_QUEUE_ZIP_2_R_1_I(WJR_PP_EVAL_R_1(WJR_PP_QUEUE_CALL_R_N_1(                            \
        (queue1, queue2),                                                                          \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_2_CALLER_1, WJR_PP_QUEUE_SIZE(queue1)),           \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_2_R_1_I(result) WJR_PP_QUEUE_FRONT(result)
#define WJR_PP_QUEUE_ZIP_2_CALLER_1(x, y) WJR_PP_QUEUE_ZIP_2_CALLER_0(x, y)

#define __WJR_PP_QUEUE_ZIP_2_R_2(queue1, queue2)                                                   \
    __WJR_PP_QUEUE_ZIP_2_R_2_I(WJR_PP_EVAL_R_2(WJR_PP_QUEUE_CALL_R_N_2(                            \
        (queue1, queue2),                                                                          \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_2_CALLER_2, WJR_PP_QUEUE_SIZE(queue1)),           \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_2_R_2_I(result) WJR_PP_QUEUE_FRONT(result)
#define WJR_PP_QUEUE_ZIP_2_CALLER_2(x, y) WJR_PP_QUEUE_ZIP_2_CALLER_0(x, y)

#define __WJR_PP_QUEUE_ZIP_2_R_3(queue1, queue2)                                                   \
    __WJR_PP_QUEUE_ZIP_2_R_3_I(WJR_PP_EVAL_R_3(WJR_PP_QUEUE_CALL_R_N_3(                            \
        (queue1, queue2),                                                                          \
        WJR_PP_QUEUE_ZIP_INIT_N(WJR_PP_QUEUE_ZIP_2_CALLER_3, WJR_PP_QUEUE_SIZE(queue1)),           \
        WJR_PP_QUEUE_SIZE(queue1))))
#define __WJR_PP_QUEUE_ZIP_2_R_3_I(result) WJR_PP_QUEUE_FRONT(result)
#define WJR_PP_QUEUE_ZIP_2_CALLER_3(x, y) WJR_PP_QUEUE_ZIP_2_CALLER_0(x, y)

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

#define WJR_PP_QUEUE_ZIP(...) WJR_PP_QUEUE_ZIP_I(WJR_PP_ARGS_LEN(__VA_ARGS__), __VA_ARGS__)
#define WJR_PP_QUEUE_ZIP_I(N, ...) WJR_PP_QUEUE_ZIP_II(N, __VA_ARGS__)
#define WJR_PP_QUEUE_ZIP_II(N, ...) WJR_PP_QUEUE_ZIP_##N(__VA_ARGS__)

#define WJR_PP_QUEUE_ZIP_3(queue1, queue2, queue3)                                                 \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_2(queue1, queue2), queue3)
#define WJR_PP_QUEUE_ZIP_4(queue1, queue2, queue3, queue4)                                         \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_3(queue1, queue2, queue3), queue4)
#define WJR_PP_QUEUE_ZIP_5(queue1, queue2, queue3, queue4, queue5)                                 \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_4(queue1, queue2, queue3, queue4), queue5)
#define WJR_PP_QUEUE_ZIP_6(queue1, queue2, queue3, queue4, queue5, queue6)                         \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_5(queue1, queue2, queue3, queue4, queue5), queue6)
#define WJR_PP_QUEUE_ZIP_7(queue1, queue2, queue3, queue4, queue5, queue6, queue7)                 \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_6(queue1, queue2, queue3, queue4, queue5, queue6),      \
                          queue7)
#define WJR_PP_QUEUE_ZIP_8(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8)         \
    WJR_PP_QUEUE_ZIP_MORE(                                                                         \
        WJR_PP_QUEUE_ZIP_7(queue1, queue2, queue3, queue4, queue5, queue6, queue7), queue8)
#define WJR_PP_QUEUE_ZIP_9(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8, queue9) \
    WJR_PP_QUEUE_ZIP_MORE(                                                                         \
        WJR_PP_QUEUE_ZIP_8(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8),        \
        queue9)
#define WJR_PP_QUEUE_ZIP_10(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10)                                                       \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_9(queue1, queue2, queue3, queue4, queue5, queue6,       \
                                             queue7, queue8, queue9),                              \
                          queue10)
#define WJR_PP_QUEUE_ZIP_11(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10, queue11)                                              \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_10(queue1, queue2, queue3, queue4, queue5, queue6,      \
                                              queue7, queue8, queue9, queue10),                    \
                          queue11)
#define WJR_PP_QUEUE_ZIP_12(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10, queue11, queue12)                                     \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_11(queue1, queue2, queue3, queue4, queue5, queue6,      \
                                              queue7, queue8, queue9, queue10, queue11),           \
                          queue12)
#define WJR_PP_QUEUE_ZIP_13(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10, queue11, queue12, queue13)                            \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_12(queue1, queue2, queue3, queue4, queue5, queue6,      \
                                              queue7, queue8, queue9, queue10, queue11, queue12),  \
                          queue13)
#define WJR_PP_QUEUE_ZIP_14(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10, queue11, queue12, queue13, queue14)                   \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_13(queue1, queue2, queue3, queue4, queue5, queue6,      \
                                              queue7, queue8, queue9, queue10, queue11, queue12,   \
                                              queue13),                                            \
                          queue14)
#define WJR_PP_QUEUE_ZIP_15(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10, queue11, queue12, queue13, queue14, queue15)          \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_14(queue1, queue2, queue3, queue4, queue5, queue6,      \
                                              queue7, queue8, queue9, queue10, queue11, queue12,   \
                                              queue13, queue14),                                   \
                          queue15)
#define WJR_PP_QUEUE_ZIP_16(queue1, queue2, queue3, queue4, queue5, queue6, queue7, queue8,        \
                            queue9, queue10, queue11, queue12, queue13, queue14, queue15, queue16) \
    WJR_PP_QUEUE_ZIP_MORE(WJR_PP_QUEUE_ZIP_15(queue1, queue2, queue3, queue4, queue5, queue6,      \
                                              queue7, queue8, queue9, queue10, queue11, queue12,   \
                                              queue13, queue14, queue15),                          \
                          queue16)

// (A, B, C) -> ((0, A), (1, B), (2, C))
#define WJR_PP_QUEUE_ZIP_IOTA(queue)                                                               \
    WJR_PP_QUEUE_ZIP_IOTA_I((WJR_PP_IOTA(WJR_PP_QUEUE_SIZE(queue))), queue)
#define WJR_PP_QUEUE_ZIP_IOTA_I(ID, queue) WJR_PP_QUEUE_ZIP_2(ID, queue)

#endif // WJR_PREPROCESSOR_QUEUE_ZIP_HPP__
