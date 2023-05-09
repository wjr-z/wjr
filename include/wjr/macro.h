#pragma once
#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H

#include <cstdint>
#include <cassert>

#include <wjr/preprocessor/all.h>

#define WJR_MACRO_NULL(...)

#define WJR_MACRO_LABEL(NAME) __wjr_label_##NAME

#if defined(WJR_MSVC)
#define WJR_COUNTER __COUNTER__
#else
#define WJR_COUNTER __LINE__
#endif 

#define _WJR_CONCAT(x, y) x##y
#define WJR_MACRO_CONCAT(x, y) _WJR_CONCAT(x, y)

#define WJR_EXPAND(x) x

#define __WJR_DEFINE_0(x)
#define __WJR_DEFINE_1(x) x
#define WJR_DEFINE(x, y) __WJR_DEFINE_##y (x)

#define _WJR_STR(x) #x
#define WJR_MACRO_STR(x) _WJR_STR(x)

#define WJR_MACRO_RP(M, x) M x
#define _WJR_MACRO_ESC(...) __VA_ARGS__
#define WJR_MACRO_ESC(x) WJR_MACRO_RP(_WJR_MACRO_ESC, x)

#define WJR_MACRO_IOTA(N) WJR_MACRO_IOTA_##N
#define WJR_MACRO_IOTA_0
#define WJR_MACRO_IOTA_1 0
#define WJR_MACRO_IOTA_2 WJR_MACRO_IOTA_1, 1
#define WJR_MACRO_IOTA_3 WJR_MACRO_IOTA_2, 2
#define WJR_MACRO_IOTA_4 WJR_MACRO_IOTA_3, 3
#define WJR_MACRO_IOTA_5 WJR_MACRO_IOTA_4, 4
#define WJR_MACRO_IOTA_6 WJR_MACRO_IOTA_5, 5
#define WJR_MACRO_IOTA_7 WJR_MACRO_IOTA_6, 6
#define WJR_MACRO_IOTA_8 WJR_MACRO_IOTA_7, 7
#define WJR_MACRO_IOTA_9 WJR_MACRO_IOTA_8, 8
#define WJR_MACRO_IOTA_10 WJR_MACRO_IOTA_9, 9
#define WJR_MACRO_IOTA_11 WJR_MACRO_IOTA_10, 10
#define WJR_MACRO_IOTA_12 WJR_MACRO_IOTA_11, 11
#define WJR_MACRO_IOTA_13 WJR_MACRO_IOTA_12, 12
#define WJR_MACRO_IOTA_14 WJR_MACRO_IOTA_13, 13
#define WJR_MACRO_IOTA_15 WJR_MACRO_IOTA_14, 14
#define WJR_MACRO_IOTA_16 WJR_MACRO_IOTA_15, 15
#define WJR_MACRO_IOTA_17 WJR_MACRO_IOTA_16, 16
#define WJR_MACRO_IOTA_18 WJR_MACRO_IOTA_17, 17
#define WJR_MACRO_IOTA_19 WJR_MACRO_IOTA_18, 18
#define WJR_MACRO_IOTA_20 WJR_MACRO_IOTA_19, 19
#define WJR_MACRO_IOTA_21 WJR_MACRO_IOTA_20, 20
#define WJR_MACRO_IOTA_22 WJR_MACRO_IOTA_21, 21
#define WJR_MACRO_IOTA_23 WJR_MACRO_IOTA_22, 22
#define WJR_MACRO_IOTA_24 WJR_MACRO_IOTA_23, 23
#define WJR_MACRO_IOTA_25 WJR_MACRO_IOTA_24, 24
#define WJR_MACRO_IOTA_26 WJR_MACRO_IOTA_25, 25
#define WJR_MACRO_IOTA_27 WJR_MACRO_IOTA_26, 26
#define WJR_MACRO_IOTA_28 WJR_MACRO_IOTA_27, 27
#define WJR_MACRO_IOTA_29 WJR_MACRO_IOTA_28, 28
#define WJR_MACRO_IOTA_30 WJR_MACRO_IOTA_29, 29
#define WJR_MACRO_IOTA_31 WJR_MACRO_IOTA_30, 30
#define WJR_MACRO_IOTA_32 WJR_MACRO_IOTA_31, 31
#define WJR_MACRO_IOTA_33 WJR_MACRO_IOTA_32, 32
#define WJR_MACRO_IOTA_34 WJR_MACRO_IOTA_33, 33
#define WJR_MACRO_IOTA_35 WJR_MACRO_IOTA_34, 34
#define WJR_MACRO_IOTA_36 WJR_MACRO_IOTA_35, 35
#define WJR_MACRO_IOTA_37 WJR_MACRO_IOTA_36, 36
#define WJR_MACRO_IOTA_38 WJR_MACRO_IOTA_37, 37
#define WJR_MACRO_IOTA_39 WJR_MACRO_IOTA_38, 38
#define WJR_MACRO_IOTA_40 WJR_MACRO_IOTA_39, 39
#define WJR_MACRO_IOTA_41 WJR_MACRO_IOTA_40, 40
#define WJR_MACRO_IOTA_42 WJR_MACRO_IOTA_41, 41
#define WJR_MACRO_IOTA_43 WJR_MACRO_IOTA_42, 42
#define WJR_MACRO_IOTA_44 WJR_MACRO_IOTA_43, 43
#define WJR_MACRO_IOTA_45 WJR_MACRO_IOTA_44, 44
#define WJR_MACRO_IOTA_46 WJR_MACRO_IOTA_45, 45
#define WJR_MACRO_IOTA_47 WJR_MACRO_IOTA_46, 46
#define WJR_MACRO_IOTA_48 WJR_MACRO_IOTA_47, 47
#define WJR_MACRO_IOTA_49 WJR_MACRO_IOTA_48, 48
#define WJR_MACRO_IOTA_50 WJR_MACRO_IOTA_49, 49
#define WJR_MACRO_IOTA_51 WJR_MACRO_IOTA_50, 50
#define WJR_MACRO_IOTA_52 WJR_MACRO_IOTA_51, 51
#define WJR_MACRO_IOTA_53 WJR_MACRO_IOTA_52, 52
#define WJR_MACRO_IOTA_54 WJR_MACRO_IOTA_53, 53
#define WJR_MACRO_IOTA_55 WJR_MACRO_IOTA_54, 54
#define WJR_MACRO_IOTA_56 WJR_MACRO_IOTA_55, 55
#define WJR_MACRO_IOTA_57 WJR_MACRO_IOTA_56, 56
#define WJR_MACRO_IOTA_58 WJR_MACRO_IOTA_57, 57
#define WJR_MACRO_IOTA_59 WJR_MACRO_IOTA_58, 58
#define WJR_MACRO_IOTA_60 WJR_MACRO_IOTA_59, 59
#define WJR_MACRO_IOTA_61 WJR_MACRO_IOTA_60, 60
#define WJR_MACRO_IOTA_62 WJR_MACRO_IOTA_61, 61
#define WJR_MACRO_IOTA_63 WJR_MACRO_IOTA_62, 62
#define WJR_MACRO_IOTA_64 WJR_MACRO_IOTA_63, 63

// don't support 0 agument
#define WJR_MACRO_ARG_LEN(...) WJR_EXPAND(WJR_MACRO_ARG_LEN_(0, ##__VA_ARGS__ , WJR_MACRO_ARG_LEN_RSEQ_N()))
#define WJR_MACRO_ARG_LEN_RSEQ_N()                                                   \
64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48,                  \
47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32,                      \
31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,                      \
15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define WJR_MACRO_ARG_LEN_(...) WJR_EXPAND(WJR_MACRO_ARG_LEN_N(__VA_ARGS__))
#define WJR_MACRO_ARG_LEN_N(	                                                     \
_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,               \
_17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,      \
_33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,      \
_49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, \
N, ...) N

// WJR_MACRO_CALL(Func, OP, ...) -> Func(Args) OP ...
#define WJR_MACRO_CALL(Func, OP, ...) WJR_EXPAND(WJR_MACRO_CALL_(Func, OP, WJR_MACRO_ARG_LEN(__VA_ARGS__), __VA_ARGS__))
#define WJR_MACRO_CALL_(Func, OP, N, ...) WJR_EXPAND(WJR_MACRO_CALL_N(Func, OP, N, __VA_ARGS__))
#define WJR_MACRO_CALL_N(Func, OP, N, ...) WJR_EXPAND(WJR_MACRO_CALL_##N(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_1(Func, OP, x) Func(x)
#define WJR_MACRO_CALL_2(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_1(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_3(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_2(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_4(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_3(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_5(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_4(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_6(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_5(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_7(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_6(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_8(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_7(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_9(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_8(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_10(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_9(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_11(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_10(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_12(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_11(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_13(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_12(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_14(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_13(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_15(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_14(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_16(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_15(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_17(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_16(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_18(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_17(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_19(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_18(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_20(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_19(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_21(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_20(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_22(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_21(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_23(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_22(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_24(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_23(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_25(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_24(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_26(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_25(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_27(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_26(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_28(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_27(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_29(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_28(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_30(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_29(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_31(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_30(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_32(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_31(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_33(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_32(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_34(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_33(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_35(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_34(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_36(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_35(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_37(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_36(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_38(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_37(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_39(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_38(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_40(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_39(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_41(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_40(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_42(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_41(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_43(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_42(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_44(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_43(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_45(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_44(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_46(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_45(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_47(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_46(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_48(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_47(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_49(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_48(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_50(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_49(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_51(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_50(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_52(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_51(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_53(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_52(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_54(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_53(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_55(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_54(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_56(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_55(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_57(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_56(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_58(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_57(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_59(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_58(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_60(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_59(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_61(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_60(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_62(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_61(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_63(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_62(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_64(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_63(Func, OP, __VA_ARGS__))

#define WJR_MACRO_CALL_COMMA(Func,  ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_(Func,  WJR_MACRO_ARG_LEN(__VA_ARGS__), __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_(Func,  N, ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_N(Func,  N, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_N(Func,  N, ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_##N(Func,  __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_1(Func,  x) Func(x)
#define WJR_MACRO_CALL_COMMA_2(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_1(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_3(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_2(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_4(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_3(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_5(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_4(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_6(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_5(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_7(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_6(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_8(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_7(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_9(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_8(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_10(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_9(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_11(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_10(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_12(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_11(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_13(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_12(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_14(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_13(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_15(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_14(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_16(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_15(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_17(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_16(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_18(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_17(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_19(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_18(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_20(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_19(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_21(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_20(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_22(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_21(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_23(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_22(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_24(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_23(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_25(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_24(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_26(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_25(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_27(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_26(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_28(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_27(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_29(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_28(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_30(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_29(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_31(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_30(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_32(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_31(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_33(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_32(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_34(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_33(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_35(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_34(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_36(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_35(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_37(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_36(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_38(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_37(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_39(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_38(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_40(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_39(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_41(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_40(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_42(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_41(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_43(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_42(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_44(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_43(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_45(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_44(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_46(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_45(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_47(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_46(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_48(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_47(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_49(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_48(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_50(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_49(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_51(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_50(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_52(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_51(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_53(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_52(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_54(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_53(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_55(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_54(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_56(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_55(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_57(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_56(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_58(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_57(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_59(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_58(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_60(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_59(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_61(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_60(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_62(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_61(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_63(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_62(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_64(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_63(Func, __VA_ARGS__))

#define WJR_MACRO_PREFIX(N, ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_N(WJR_EXPAND, N, __VA_ARGS__))

#define WJR_OVERLAP_P(p1, s1, p2, s2) \
	((p1) + (s1) > (p2) && (p2) + (s2) > (p1))

#define WJR_OVERLAP_P_N(p1, p2, n)	\
	WJR_OVERLAP_P(p1, n, p2, n)

#define WJR_SAME_OR_SEPARATE_P(p1, s1, p2, s2)	\
	((p1) == (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_SAME_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_SAME_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_PRE_OR_SEPARATE_P(p1, s1, p2, s2) \
	((p1) <= (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_PRE_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_PRE_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_POST_OR_SEPARATE_P(p1, s1, p2, s2) \
	((p1) >= (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_POST_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_POST_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_REGISTER_HAS_MEMBER_FUNCTION(FUNC, NAME)							\
template<typename Enable, typename T, typename...Args>							\
struct __has_member_function_##NAME : std::false_type {};						\
template<typename T, typename...Args>											\
struct __has_member_function_##NAME <std::void_t<decltype(						\
	std::declval<T>(). FUNC (std::declval<Args>()...))>, T, Args...>			\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_member_function_##NAME :												\
	__has_member_function_##NAME<void, T, Args...> {};							\
template<typename T, typename...Args>											\
constexpr bool has_member_function_##NAME##_v =									\
	has_member_function_##NAME<T, Args...>::value;

#define WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(FUNC, NAME)						\
template<typename Enable, typename T, typename...Args>							\
struct __has_static_member_function_##NAME : std::false_type {};				\
template<typename T, typename...Args>											\
struct __has_static_member_function_##NAME <std::void_t<decltype(				\
	T:: FUNC (std::declval<Args>()...))>, T, Args...>							\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_static_member_function_##NAME :										\
	__has_static_member_function_##NAME<void, T, Args...> {};					\
template<typename T, typename...Args>											\
constexpr bool has_static_member_function_##NAME##_v =							\
	has_static_member_function_##NAME<T, Args...>::value;

#define WJR_REGISTER_HAS_GLOBAL_FUNCTION(FUNC, NAME)							\
template<typename Enable, typename...Args>										\
struct __has_global_function_##NAME : std::false_type {};						\
template<typename...Args>														\
struct __has_global_function_##NAME <std::void_t<decltype(						\
	FUNC (std::declval<Args>()...))>, Args...>									\
	: std::true_type {};														\
template<typename...Args>														\
struct has_global_function_##NAME :												\
	__has_global_function_##NAME<void, Args...> {};								\
template<typename...Args>														\
constexpr bool has_global_function_##NAME##_v =									\
	has_global_function_##NAME<Args...>::value;

#define WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(OP, NAME)						\
template<typename Enable, typename T, typename U>								\
struct __has_global_binary_operator_##NAME : std::false_type {};				\
template<typename T, typename U>												\
struct __has_global_binary_operator_##NAME <std::void_t<decltype(				\
	std::declval<T>() OP std::declval<U>())>, T, U>								\
	: std::true_type {};														\
template<typename T, typename U>												\
struct has_global_binary_operator_##NAME :										\
	__has_global_binary_operator_##NAME<void, T, U> {};							\
template<typename T, typename U>												\
constexpr bool has_global_binary_operator_##NAME##_v =							\
	has_global_binary_operator_##NAME<T, U>::value;								\
template<typename T, typename U>												\
struct has_global_binary_operator<T, U, std:: NAME<>> :							\
	has_global_binary_operator_##NAME<T, U> {};

#define WJR_REGISTER_HAS_GLOBAL_UNARY_OPERATOR(OP, NAME)						\
template<typename Enable, typename T>											\
struct __has_global_unary_operator_##NAME : std::false_type {};					\
template<typename T>															\
struct __has_global_unary_operator_##NAME <std::void_t<decltype(				\
	OP std::declval<T>())>, T>													\
	: std::true_type {};														\
template<typename T>															\
struct has_global_unary_operator_##NAME :										\
	__has_global_unary_operator_##NAME<void, T> {};								\
template<typename T>															\
constexpr bool has_global_unary_operator_##NAME##_v =							\
	has_global_unary_operator_##NAME<T>::value;

#define WJR_REGISTER_HAS_STD_INVOKE												\
template<typename Enable, typename T, typename...Args>							\
struct __has_std_invoke : std::false_type {};					                \
template<typename T, typename...Args>											\
struct __has_std_invoke <std::void_t<std::invoke_result_t<			            \
	T, Args...>>, T, Args...>													\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_std_invoke :											                \
	__has_std_invoke<void, T, Args...> {};						                \
template<typename T, typename...Args>											\
constexpr bool has_std_invoke##_v =								                \
	has_std_invoke<T, Args...>::value;

#define WJR_REGISTER_HAS_TYPE(TYPE, NAME)										\
template<typename Enable, typename T>							                \
struct __has_type_##NAME : std::false_type {};				                    \
template<typename T>											                \
struct __has_type_##NAME <std::void_t<decltype(				                    \
	std::declval<typename T::TYPE>())>, T>							            \
	: std::true_type {};														\
template<typename T>											                \
struct has_type_##NAME :										                \
	__has_static_member_function_##NAME<void, T> {};					        \
template<typename T>											                \
constexpr bool has_type_##NAME##_v =							                \
	has_type_##NAME<T>::value;

#define WJR_REGISTER_HAS_TYPE_ARGS(TYPE, NAME)									\
template<typename Enable, typename T, typename...Args>							\
struct __has_type_##NAME : std::false_type {};				                    \
template<typename T, typename...Args>											\
struct __has_type_##NAME <std::void_t<decltype(				                    \
	std::declval<typename T::template TYPE<Args...>>())>, T, Args...>			\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_type_##NAME :										                \
	__has_type_##NAME<void, T, Args...> {};					                    \
template<typename T, typename...Args>											\
constexpr bool has_type_##NAME##_v =							                \
	has_type_##NAME<T, Args...>::value;

#endif