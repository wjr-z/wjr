#ifndef WJR_PREPROCESSOR_DETAIL_REPEAT_HPP__
#define WJR_PREPROCESSOR_DETAIL_REPEAT_HPP__

#define WJR_PP_REPEAT(x, n) WJR_PP_REPEAT_I(x, n)
#define WJR_PP_REPEAT_I(x, n) WJR_PP_REPEAT_##n(x)

#define WJR_PP_REPEAT_0(x)
#define WJR_PP_REPEAT_1(x) x
#define WJR_PP_REPEAT_2(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_1(x)
#define WJR_PP_REPEAT_3(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x)
#define WJR_PP_REPEAT_4(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_2(x)
#define WJR_PP_REPEAT_5(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_6(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_7(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_8(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_9(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_10(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_11(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_12(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_13(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_14(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_15(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_16(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_17(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_18(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_19(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_20(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_21(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_22(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_23(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_24(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_25(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_26(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_27(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_28(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_29(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_30(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_31(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x),      \
        WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_32(x) WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_33(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_34(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_35(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_36(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_37(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_38(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_39(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_40(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_41(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_42(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_43(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_44(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_45(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_46(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_47(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x),      \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_48(x) WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_49(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_50(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_51(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_52(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_53(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_54(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_55(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_56(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_57(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_58(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_59(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_60(x)                                                              \
    WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_61(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_62(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_63(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x),      \
        WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_64(x) WJR_PP_REPEAT_32(x), WJR_PP_REPEAT_32(x)

#endif // ! WJR_PREPROCESSOR_DETAIL_REPEAT_HPP__