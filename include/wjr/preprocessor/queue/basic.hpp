#ifndef WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__
#define WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__

#include <wjr/preprocessor/detail/basic.hpp>

#define WJR_PP_QUEUE_EXPAND(queue) WJR_PP_QUEUE_EXPAND_I queue
#define WJR_PP_QUEUE_EXPAND_I(...) __VA_ARGS__

#define WJR_PP_QUEUE_FRONT(queue) WJR_PP_QUEUE_FRONT_I queue
#define WJR_PP_QUEUE_FRONT_I(x, ...) x

#define WJR_PP_QUEUE_POP_FRONT(queue) WJR_PP_QUEUE_POP_FRONT_I queue
#define WJR_PP_QUEUE_POP_FRONT_I(x, ...) (__VA_ARGS__)

#define WJR_PP_QUEUE_PUSH_FRONT(queue, ...) WJR_PP_QUEUE_PUSH_FRONT_I(queue, __VA_ARGS__)
#define WJR_PP_QUEUE_PUSH_FRONT_I(queue, ...) (__VA_ARGS__, WJR_PP_QUEUE_EXPAND(queue))

#define WJR_PP_QUEUE_PUSH_BACK(queue, ...) WJR_PP_QUEUE_PUSH_BACK_I(queue, __VA_ARGS__)
#define WJR_PP_QUEUE_PUSH_BACK_I(queue, ...) (WJR_PP_QUEUE_EXPAND(queue), __VA_ARGS__)

#define WJR_PP_QUEUE_SIZE(queue) WJR_PP_QUEUE_SIZE_I(queue)
#define WJR_PP_QUEUE_SIZE_I(queue) WJR_PP_ARGS_LEN queue

// (1, 2, 3, 4, 5), 2 -> (3, 4, 5)
#define WJR_PP_QUEUE_POP_FRONT_N(queue, N) WJR_PP_QUEUE_POP_FRONT_N_I(queue, N)
#define WJR_PP_QUEUE_POP_FRONT_N_I(queue, N) WJR_PP_QUEUE_POP_FRONT_I##N queue

#define WJR_PP_QUEUE_POP_FRONT_I0(...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I1(x0, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I2(x0, x1, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I3(x0, x1, x2, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I4(x0, x1, x2, x3, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I5(x0, x1, x2, x3, x4, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I6(x0, x1, x2, x3, x4, x5, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I7(x0, x1, x2, x3, x4, x5, x6, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I8(x0, x1, x2, x3, x4, x5, x6, x7, ...) (__VA_ARGS__)
#define WJR_PP_QUEUE_POP_FRONT_I9(x0, ...) WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I10(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I11(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I12(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I13(x0, x1, x2, x3, x4, ...) WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I14(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I15(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I16(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I8((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I17(x0, ...) WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I18(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I19(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I20(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I21(x0, x1, x2, x3, x4, ...)                                        \
    WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I22(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I23(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I24(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I16((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I25(x0, ...) WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I26(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I27(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I28(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I29(x0, x1, x2, x3, x4, ...)                                        \
    WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I30(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I31(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I32(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I24((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I33(x0, ...) WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I34(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I35(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I36(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I37(x0, x1, x2, x3, x4, ...)                                        \
    WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I38(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I39(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I40(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I32((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I41(x0, ...) WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I42(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I43(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I44(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I45(x0, x1, x2, x3, x4, ...)                                        \
    WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I46(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I47(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I48(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I40((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I49(x0, ...) WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I50(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I51(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I52(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I53(x0, x1, x2, x3, x4, ...)                                        \
    WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I54(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I55(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I56(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I48((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I57(x0, ...) WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I58(x0, x1, ...) WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I59(x0, x1, x2, ...) WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I60(x0, x1, x2, x3, ...) WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I61(x0, x1, x2, x3, x4, ...)                                        \
    WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I62(x0, x1, x2, x3, x4, x5, ...)                                    \
    WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I63(x0, x1, x2, x3, x4, x5, x6, ...)                                \
    WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))
#define WJR_PP_QUEUE_POP_FRONT_I64(x0, x1, x2, x3, x4, x5, x6, x7, ...)                            \
    WJR_PP_QUEUE_POP_FRONT_I56((__VA_ARGS__))

#endif // ! WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__