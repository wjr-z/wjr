#ifndef WJR_PREPROCESSOR_QUEUE_CALL_HPP__
#define WJR_PREPROCESSOR_QUEUE_CALL_HPP__

#include <wjr/preprocessor/arithmatic/dec.hpp>
#include <wjr/preprocessor/arithmatic/inc.hpp>
#include <wjr/preprocessor/detail/basic.hpp>
#include <wjr/preprocessor/queue/basic.hpp>

#define WJR_PP_QUEUE_CALL(args, ops)                                                     \
    WJR_PP_QUEUE_CALL_N_I(args, ops, WJR_PP_QUEUE_SIZE(args))
#define WJR_PP_QUEUE_CALL_N(args, ops, N) WJR_PP_QUEUE_CALL_N_I(args, ops, WJR_PP_INC(N))
#define WJR_PP_QUEUE_CALL_N_I(args, ops, N) WJR_PP_QUEUE_CALL_N_II(args, ops, N)
#define WJR_PP_QUEUE_CALL_N_II(args, ops, N) WJR_PP_QUEUE_CALL_##N(args, ops)

#define WJR_PP_QUEUE_CALL_GEN(args, ops)                                                 \
    WJR_PP_QUEUE_CALL_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),           \
                            WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_GEN_I(op, arg1, arg2) op(arg1, arg2)

#define WJR_PP_QUEUE_CALL_NEW_ARGS_EQ(args, ops) (WJR_PP_QUEUE_CALL_GEN(args, ops))

#define WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)                                         \
    WJR_PP_QUEUE_CALL_NEW_ARGS_NE_I(                                                     \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),                            \
        WJR_PP_QUEUE_CALL_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_NEW_ARGS_NE_I(arg1, arg2) WJR_PP_QUEUE_PUSH_FRONT(arg1, arg2)

#define WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops)                                            \
    WJR_PP_QUEUE_CALL_NEW_ARGS_I(args, ops, WJR_PP_QUEUE_SIZE(args))
#define WJR_PP_QUEUE_CALL_NEW_ARGS_I(args, ops, N)                                       \
    WJR_PP_QUEUE_CALL_NEW_ARGS_II(args, ops, N)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_II(args, ops, N)                                      \
    WJR_PP_QUEUE_CALL_NEW_ARGS_##N(args, ops)

#define WJR_PP_QUEUE_CALL_1(args, ops) args

#define WJR_PP_QUEUE_CALL_2(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops)

#define WJR_PP_QUEUE_CALL_3(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_2(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_4(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_3(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_5(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_4(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_6(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_5(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_7(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_6(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_8(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_7(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_9(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_8(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_10(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_9(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_11(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_10(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_12(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_11(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_13(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_12(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_14(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_13(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_15(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_14(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_16(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_15(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_17(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_16(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_18(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_17(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_19(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_18(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_20(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_19(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_21(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_20(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_22(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_21(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_23(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_22(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_24(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_23(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_25(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_24(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_26(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_25(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_27(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_26(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_28(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_27(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_29(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_28(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_30(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_29(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_31(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_30(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_32(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_31(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_33(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_32(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_34(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_33(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_35(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_34(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_36(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_35(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_37(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_36(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_38(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_37(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_39(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_38(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_40(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_39(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_41(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_40(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_42(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_41(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_43(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_42(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_44(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_43(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_45(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_44(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_46(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_45(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_47(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_46(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_48(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_47(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_49(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_48(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_50(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_49(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_51(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_50(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_52(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_51(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_53(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_52(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_54(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_53(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_55(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_54(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_56(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_55(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_57(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_56(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_58(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_57(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_59(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_58(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_60(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_59(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_61(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_60(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_62(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_61(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_63(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_62(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_64(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_63(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))

#define WJR_PP_QUEUE_CALL_NEW_ARGS_2(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_EQ(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_3(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_4(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_5(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_6(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_7(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_8(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_9(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_10(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_11(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_12(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_13(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_14(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_15(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_16(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_17(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_18(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_19(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_20(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_21(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_22(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_23(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_24(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_25(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_26(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_27(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_28(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_29(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_30(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_31(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_32(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_33(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_34(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_35(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_36(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_37(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_38(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_39(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_40(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_41(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_42(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_43(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_44(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_45(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_46(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_47(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_48(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_49(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_50(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_51(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_52(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_53(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_54(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_55(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_56(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_57(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_58(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_59(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_60(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_61(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_62(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_63(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_64(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)

#endif // ! WJR_PREPROCESSOR_QUEUE_CALL_HPP__