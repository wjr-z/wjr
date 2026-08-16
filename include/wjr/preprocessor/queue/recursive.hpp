#ifndef WJR_PREPROCESSOR_QUEUE_RECURSIVE_HPP__
#define WJR_PREPROCESSOR_QUEUE_RECURSIVE_HPP__

#include <wjr/preprocessor/arithmetic.hpp>
#include <wjr/preprocessor/detail/basic.hpp>
#include <wjr/preprocessor/logical.hpp>
#include <wjr/preprocessor/queue/basic.hpp>

#define WJR_PP_QUEUE_CALL_R(r, args, ops)                                                          \
    WJR_PP_QUEUE_CALL_R_N(r, args, ops, WJR_PP_DEC(WJR_PP_QUEUE_SIZE(args)))
#define WJR_PP_QUEUE_CALL_R_N(r, args, ops, n) WJR_PP_QUEUE_CALL_R_N_I(r, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_N_I(r, args, ops, n) WJR_PP_QUEUE_CALL_R_N_II(r, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_N_II(r, args, ops, n)                                                  \
    WJR_PP_CONCAT(WJR_PP_EVAL_R_, r)(WJR_PP_CONCAT(WJR_PP_QUEUE_CALL_R_N_, r)(args, ops, n))

#define WJR_PP_QUEUE_CALL_R_N_0(args, ops, n) WJR_PP_QUEUE_CALL_R_0_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_0_I(args, ops, n)                                                      \
    WJR_PP_BOOL_IF(WJR_PP_BOOL(n), WJR_PP_QUEUE_CALL_R_0_STEP, WJR_PP_QUEUE_CALL_R_0_DONE)         \
    (args, ops, n)
#define WJR_PP_QUEUE_CALL_R_0_STEP(args, ops, n)                                                   \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_CALL_R_0_INDIRECT)                                                \
    ()(WJR_PP_QUEUE_CALL_R_0_NEW_ARGS(args, ops), WJR_PP_QUEUE_POP_FRONT(ops), WJR_PP_DEC(n))
#define WJR_PP_QUEUE_CALL_R_0_DONE(args, ops, n) args
#define WJR_PP_QUEUE_CALL_R_0_INDIRECT() WJR_PP_QUEUE_CALL_R_0_I
#define WJR_PP_QUEUE_CALL_R_0_NEW_ARGS(args, ops)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_EQ(WJR_PP_QUEUE_SIZE(args), 2), WJR_PP_QUEUE_CALL_R_0_EQ,                \
                   WJR_PP_QUEUE_CALL_R_0_NE)                                                       \
    (args, ops)
#define WJR_PP_QUEUE_CALL_R_0_EQ(args, ops) (WJR_PP_QUEUE_CALL_R_0_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_0_NE(args, ops)                                                        \
    WJR_PP_QUEUE_CALL_R_0_NE_I(WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),               \
                               WJR_PP_QUEUE_CALL_R_0_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_0_NE_I(args, value) WJR_PP_QUEUE_PUSH_FRONT(args, value)
#define WJR_PP_QUEUE_CALL_R_0_GEN(args, ops)                                                       \
    WJR_PP_QUEUE_CALL_R_0_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),                 \
                                WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_R_0_GEN_I(op, state, elem) WJR_PP_OBSTRUCT(op)(state, elem)

#define WJR_PP_QUEUE_CALL_R_N_1(args, ops, n) WJR_PP_QUEUE_CALL_R_1_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_1_I(args, ops, n)                                                      \
    WJR_PP_BOOL_IF(WJR_PP_BOOL(n), WJR_PP_QUEUE_CALL_R_1_STEP, WJR_PP_QUEUE_CALL_R_1_DONE)         \
    (args, ops, n)
#define WJR_PP_QUEUE_CALL_R_1_STEP(args, ops, n)                                                   \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_CALL_R_1_INDIRECT)                                                \
    ()(WJR_PP_QUEUE_CALL_R_1_NEW_ARGS(args, ops), WJR_PP_QUEUE_POP_FRONT(ops), WJR_PP_DEC(n))
#define WJR_PP_QUEUE_CALL_R_1_DONE(args, ops, n) args
#define WJR_PP_QUEUE_CALL_R_1_INDIRECT() WJR_PP_QUEUE_CALL_R_1_I
#define WJR_PP_QUEUE_CALL_R_1_NEW_ARGS(args, ops)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_EQ(WJR_PP_QUEUE_SIZE(args), 2), WJR_PP_QUEUE_CALL_R_1_EQ,                \
                   WJR_PP_QUEUE_CALL_R_1_NE)                                                       \
    (args, ops)
#define WJR_PP_QUEUE_CALL_R_1_EQ(args, ops) (WJR_PP_QUEUE_CALL_R_1_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_1_NE(args, ops)                                                        \
    WJR_PP_QUEUE_CALL_R_1_NE_I(WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),               \
                               WJR_PP_QUEUE_CALL_R_1_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_1_NE_I(args, value) WJR_PP_QUEUE_PUSH_FRONT(args, value)
#define WJR_PP_QUEUE_CALL_R_1_GEN(args, ops)                                                       \
    WJR_PP_QUEUE_CALL_R_1_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),                 \
                                WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_R_1_GEN_I(op, state, elem) WJR_PP_OBSTRUCT(op)(state, elem)

#define WJR_PP_QUEUE_CALL_R_N_2(args, ops, n) WJR_PP_QUEUE_CALL_R_2_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_2_I(args, ops, n)                                                      \
    WJR_PP_BOOL_IF(WJR_PP_BOOL(n), WJR_PP_QUEUE_CALL_R_2_STEP, WJR_PP_QUEUE_CALL_R_2_DONE)         \
    (args, ops, n)
#define WJR_PP_QUEUE_CALL_R_2_STEP(args, ops, n)                                                   \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_CALL_R_2_INDIRECT)                                                \
    ()(WJR_PP_QUEUE_CALL_R_2_NEW_ARGS(args, ops), WJR_PP_QUEUE_POP_FRONT(ops), WJR_PP_DEC(n))
#define WJR_PP_QUEUE_CALL_R_2_DONE(args, ops, n) args
#define WJR_PP_QUEUE_CALL_R_2_INDIRECT() WJR_PP_QUEUE_CALL_R_2_I
#define WJR_PP_QUEUE_CALL_R_2_NEW_ARGS(args, ops)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_EQ(WJR_PP_QUEUE_SIZE(args), 2), WJR_PP_QUEUE_CALL_R_2_EQ,                \
                   WJR_PP_QUEUE_CALL_R_2_NE)                                                       \
    (args, ops)
#define WJR_PP_QUEUE_CALL_R_2_EQ(args, ops) (WJR_PP_QUEUE_CALL_R_2_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_2_NE(args, ops)                                                        \
    WJR_PP_QUEUE_CALL_R_2_NE_I(WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),               \
                               WJR_PP_QUEUE_CALL_R_2_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_2_NE_I(args, value) WJR_PP_QUEUE_PUSH_FRONT(args, value)
#define WJR_PP_QUEUE_CALL_R_2_GEN(args, ops)                                                       \
    WJR_PP_QUEUE_CALL_R_2_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),                 \
                                WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_R_2_GEN_I(op, state, elem) WJR_PP_OBSTRUCT(op)(state, elem)

#define WJR_PP_QUEUE_CALL_R_N_3(args, ops, n) WJR_PP_QUEUE_CALL_R_3_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_3_I(args, ops, n)                                                      \
    WJR_PP_BOOL_IF(WJR_PP_BOOL(n), WJR_PP_QUEUE_CALL_R_3_STEP, WJR_PP_QUEUE_CALL_R_3_DONE)         \
    (args, ops, n)
#define WJR_PP_QUEUE_CALL_R_3_STEP(args, ops, n)                                                   \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_CALL_R_3_INDIRECT)                                                \
    ()(WJR_PP_QUEUE_CALL_R_3_NEW_ARGS(args, ops), WJR_PP_QUEUE_POP_FRONT(ops), WJR_PP_DEC(n))
#define WJR_PP_QUEUE_CALL_R_3_DONE(args, ops, n) args
#define WJR_PP_QUEUE_CALL_R_3_INDIRECT() WJR_PP_QUEUE_CALL_R_3_I
#define WJR_PP_QUEUE_CALL_R_3_NEW_ARGS(args, ops)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_EQ(WJR_PP_QUEUE_SIZE(args), 2), WJR_PP_QUEUE_CALL_R_3_EQ,                \
                   WJR_PP_QUEUE_CALL_R_3_NE)                                                       \
    (args, ops)
#define WJR_PP_QUEUE_CALL_R_3_EQ(args, ops) (WJR_PP_QUEUE_CALL_R_3_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_3_NE(args, ops)                                                        \
    WJR_PP_QUEUE_CALL_R_3_NE_I(WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),               \
                               WJR_PP_QUEUE_CALL_R_3_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_3_NE_I(args, value) WJR_PP_QUEUE_PUSH_FRONT(args, value)
#define WJR_PP_QUEUE_CALL_R_3_GEN(args, ops)                                                       \
    WJR_PP_QUEUE_CALL_R_3_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),                 \
                                WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_R_3_GEN_I(op, state, elem) WJR_PP_OBSTRUCT(op)(state, elem)

#endif // WJR_PREPROCESSOR_QUEUE_RECURSIVE_HPP__
