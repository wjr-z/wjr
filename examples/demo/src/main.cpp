#include <iostream>
#include <wjr/preprocessor.hpp>
#include <wjr/atomic.hpp>

int main() {
    // printf("%s\n", WJR_PP_STR((WJR_PP_QUEUE_CALL_GEN(((1, 2, 3), (4, 5, 6)),
    //                                                  (WJR_PP_QUEUE_ZIP_2_FN)))));

#define SELF(x) x
#define SELF_FN() SELF

#define TRANS(queue, op) WJR_PP_QUEUE_TRANSFORM(queue, op)
#define TRANS_FN() TRANS

#define TESTX(FUNC) WJR_PP_DEFER(WJR_PP_QUEUE_EXPAND(TRANS((FUNC), SELF_FN)))

    // #define TT(x) WJR_PP_EXPAND(x)
    // #define TT_FN() TT

    // #define WJR_PP_INVOKE_FN() WJR_PP_INVOKE
    // #define WJR_PP_DEFER_FN() WJR_PP_DEFER

    // #define GGX(x)                                                                           \
//     WJR_PP_DEFER_FN WJR_PP_PAREN(WJR_PP_QUEUE_EXPAND_FN)(TESTX(TRANS_FN)((x), SELF_FN))

    //     printf("%s\n", WJR_PP_STR(WJR_PP_INVOKE(GGX(wjr))));

    printf("%s\n", WJR_PP_STR(WJR_PP_QUEUE_TRANSFORM((1, 2, 3), SELF)));

#define ADD(x, y) x + y
#define ADD_FN() ADD

    // printf("!!! : %s\n", WJR_PP_STR(WJR_PP_INVOKE(
    //  TESTX(WJR_PP_QUEUE_ACCUMULATE_FN)()(0, (1, 2, 3), ADD_FN))));

    //     printf("%s\n", WJR_PP_STR(WJR_PP_INVOKE(TESTX(WJR_PP_QUEUE_BACK_FN)((1, 2,
    //     3))))); printf("%s\n",
    //            WJR_PP_STR(WJR_PP_INVOKE(TESTX(WJR_PP_QUEUE_UNWRAP_FN)(((1), (2),
    //            (3))))));

    //     printf("%s\n", WJR_PP_STR(WJR_PP_INVOKE(WJR_PP_QUEUE_POP_BACK((1, 2, 3)))));
    //     printf("%s\n", WJR_PP_STR(WJR_PP_INVOKE(TESTX(WJR_PP_QUEUE_POP_BACK_FN)((1, 2,
    //     3))))); printf("%s\n", WJR_PP_STR(WJR_PP_INVOKE(WJR_PP_QUEUE_POP_BACK((3, 2,
    //     1)))));

    //     // printf("reverse : %s\n", WJR_PP_STR(WJR_PP_INVOKE(WJR_PP_QUEUE_REVERSE((1,
    //     2,
    //     // 3))))); printf("reverse : %s\n",
    //     //        WJR_PP_STR(WJR_PP_INVOKE(TESTX(WJR_PP_QUEUE_REVERSE_FN)((1, 2,
    //     3)))));

    //     printf("reverse : %s\n",
    //            WJR_PP_STR(WJR_PP_INVOKE(TESTX(WJR_PP_QUEUE_REVERSE_FN)((1, 2, 3)))));

    // #define GG WJR_PP_INVOKE(TESTX(WJR_PP_QUEUE_REVERSE_FN)((1, 2, 3)))

    // #define WJR_PP_EXPAND2(x) WJR_PP_EXPAND2_I(WJR_PP_EXPAND2_I(WJR_PP_EXPAND2_I(x)))
    // #define WJR_PP_EXPAND2_I(x) x

    //     printf("reverse : %s\n",
    //            WJR_PP_STR(WJR_PP_INVOKE(WJR_PP_DEFER(WJR_PP_QUEUE_POP_BACK_FN)(
    //                TESTX(WJR_PP_QUEUE_REVERSE_FN)((1, 2, 3))))));

    //     printf("reverse : %s\n",
    //            WJR_PP_STR(WJR_PP_INVOKE(WJR_PP_DEFER(WJR_PP_QUEUE_POP_BACK_FN)(
    //                TESTX(WJR_PP_QUEUE_REVERSE_FN)((1, 2, 3))))));

    // #define args ((1, 2), (4, 5))
    // #define ops (WJR_PP_QUEUE_ZIP_2_FN)

    // #define WJR_PP_QUEUE_CALL_GEN_FN() WJR_PP_QUEUE_CALL_GEN

    // #define TEST(queue1, queue2)                                                             \
//     WJR_PP_DEFER(WJR_PP_QUEUE_CALL_GEN_FN)(((queue1), queue2), (TEST_CALLER_FN))
    // #define TEST_FN() TEST
    // #define TEST_CALLER(x, y) WJR_PP_QUEUE_PUSH_FRONT(x, y)
    // #define TEST_CALLER_FN() TEST_CALLER

    //     printf("%s\n", WJR_PP_STR(WJR_PP_INVOKE(WJR_PP_QUEUE_ZIP((1), (2), (3)))));

    //     printf("yes\n");
    return 0;
}