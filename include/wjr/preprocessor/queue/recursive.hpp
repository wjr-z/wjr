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

#define WJR_PP_QUEUE_CALL_R_NEW_ARGS(args, ops)                                                    \
    WJR_PP_BOOL_IF(WJR_PP_EQ(WJR_PP_QUEUE_SIZE(args), 2), WJR_PP_QUEUE_CALL_R_EQ,                  \
                   WJR_PP_QUEUE_CALL_R_NE)                                                         \
    (args, ops)
#define WJR_PP_QUEUE_CALL_R_EQ(args, ops) (WJR_PP_QUEUE_CALL_R_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_NE(args, ops)                                                          \
    WJR_PP_QUEUE_CALL_R_NE_I(WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),                 \
                             WJR_PP_QUEUE_CALL_R_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_R_NE_I(args, value) WJR_PP_QUEUE_PUSH_FRONT(args, value)
#define WJR_PP_QUEUE_CALL_R_GEN(args, ops)                                                         \
    WJR_PP_QUEUE_CALL_R_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),                   \
                              WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_R_GEN_I(op, state, elem) WJR_PP_OBSTRUCT(op)(state, elem)

#define WJR_PP_QUEUE_CALL_R_INDIRECT(r) WJR_PP_QUEUE_CALL_R_INDIRECT_I(r)
#define WJR_PP_QUEUE_CALL_R_INDIRECT_I(r) WJR_PP_CONCAT(WJR_PP_CONCAT(WJR_PP_QUEUE_CALL_R_, r), _I)
#define WJR_PP_QUEUE_CALL_R_STEP(r, args, ops, n)                                                  \
    WJR_PP_OBSTRUCT(WJR_PP_QUEUE_CALL_R_INDIRECT)                                                  \
    (r)(WJR_PP_QUEUE_CALL_R_NEW_ARGS(args, ops), WJR_PP_QUEUE_POP_FRONT(ops), WJR_PP_DEC(n))
#define WJR_PP_QUEUE_CALL_R_DONE(args, ops, n) args
#define WJR_PP_QUEUE_CALL_R_I(r, args, ops, n)                                                     \
    WJR_PP_BOOL_IF(WJR_PP_BOOL(n), WJR_PP_CONCAT(WJR_PP_QUEUE_CALL_R_STEP_, r),                    \
                   WJR_PP_QUEUE_CALL_R_DONE)                                                       \
    (args, ops, n)
#define WJR_PP_QUEUE_CALL_R_STEP_0(args, ops, n) WJR_PP_QUEUE_CALL_R_STEP(0, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_STEP_1(args, ops, n) WJR_PP_QUEUE_CALL_R_STEP(1, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_STEP_2(args, ops, n) WJR_PP_QUEUE_CALL_R_STEP(2, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_STEP_3(args, ops, n) WJR_PP_QUEUE_CALL_R_STEP(3, args, ops, n)

#define WJR_PP_QUEUE_CALL_R_N_0(args, ops, n) WJR_PP_QUEUE_CALL_R_0_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_0_I(args, ops, n) WJR_PP_QUEUE_CALL_R_I(0, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_N_1(args, ops, n) WJR_PP_QUEUE_CALL_R_1_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_1_I(args, ops, n) WJR_PP_QUEUE_CALL_R_I(1, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_N_2(args, ops, n) WJR_PP_QUEUE_CALL_R_2_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_2_I(args, ops, n) WJR_PP_QUEUE_CALL_R_I(2, args, ops, n)
#define WJR_PP_QUEUE_CALL_R_N_3(args, ops, n) WJR_PP_QUEUE_CALL_R_3_I(args, ops, n)
#define WJR_PP_QUEUE_CALL_R_3_I(args, ops, n) WJR_PP_QUEUE_CALL_R_I(3, args, ops, n)

#endif // WJR_PREPROCESSOR_QUEUE_RECURSIVE_HPP__
