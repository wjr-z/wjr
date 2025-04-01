#include "detail.hpp"

#include <wjr/preprocessor.hpp>

using namespace wjr;

#define CASSERT(expr) static_assert(expr, "")

TEST(preprocessor, arithmatic) {
    CASSERT(WJR_PP_ADD(1, 3) == 4);
    CASSERT(WJR_PP_ADD(1, 4) == 5);
    CASSERT(WJR_PP_ADD(0, 4) == 4);
    CASSERT(WJR_PP_ADD(0, 63) == 63);
    CASSERT(WJR_PP_ADD(0, 0) == 0);
    CASSERT(WJR_PP_ADD(1, 63) == 0);
    CASSERT(WJR_PP_ADD(2, 63) == 1);
    CASSERT(WJR_PP_ADD(3, 63) == 2);

    CASSERT(WJR_PP_SUB(1, 1) == 0);
    CASSERT(WJR_PP_SUB(1, 2) == 63);
    CASSERT(WJR_PP_SUB(1, 3) == 62);
    CASSERT(WJR_PP_SUB(1, 4) == 61);
    CASSERT(WJR_PP_SUB(5, 4) == 1);
    CASSERT(WJR_PP_SUB(14, 4) == 10);

    CASSERT(WJR_PP_INC(0) == 1);
    CASSERT(WJR_PP_INC(2) == 3);
    CASSERT(WJR_PP_INC(63) == 0);

    CASSERT(WJR_PP_DEC(0) == 63);
    CASSERT(WJR_PP_DEC(2) == 1);
    CASSERT(WJR_PP_DEC(63) == 62);

    CASSERT(WJR_PP_NOT(0) == 63);
    CASSERT(WJR_PP_NOT(63) == 0);
    CASSERT(WJR_PP_NOT(3) == 60);

    CASSERT(WJR_PP_EQ(0, 0) == 1);
    CASSERT(WJR_PP_EQ(0, 1) == 0);
    CASSERT(WJR_PP_EQ(0, 63) == 0);
    CASSERT(WJR_PP_EQ(1, 4) == 0);

    CASSERT(WJR_PP_NE(0, 0) == 0);
    CASSERT(WJR_PP_NE(0, 1) == 1);
    CASSERT(WJR_PP_NE(0, 63) == 1);
    CASSERT(WJR_PP_NE(1, 4) == 1);

    CASSERT(WJR_PP_LT(0, 1) == 1);
    CASSERT(WJR_PP_LT(3, 4) == 1);
    CASSERT(WJR_PP_LT(4, 5) == 1);
    CASSERT(WJR_PP_LT(0, 63) == 1);
    CASSERT(WJR_PP_LT(63, 0) == 0);
    CASSERT(WJR_PP_LT(5, 3) == 0);
    CASSERT(WJR_PP_LT(3, 3) == 0);
    CASSERT(WJR_PP_LT(0, 0) == 0);
    CASSERT(WJR_PP_LT(63, 63) == 0);

    CASSERT(WJR_PP_GT(0, 1) == 0);
    CASSERT(WJR_PP_GT(3, 4) == 0);
    CASSERT(WJR_PP_GT(4, 5) == 0);
    CASSERT(WJR_PP_GT(0, 63) == 0);
    CASSERT(WJR_PP_GT(63, 0) == 1);
    CASSERT(WJR_PP_GT(5, 3) == 1);
    CASSERT(WJR_PP_GT(3, 3) == 0);
    CASSERT(WJR_PP_GT(0, 0) == 0);
    CASSERT(WJR_PP_GT(63, 63) == 0);

    CASSERT(WJR_PP_LE(0, 1) == 1);
    CASSERT(WJR_PP_LE(3, 4) == 1);
    CASSERT(WJR_PP_LE(4, 5) == 1);
    CASSERT(WJR_PP_LE(0, 63) == 1);
    CASSERT(WJR_PP_LE(63, 0) == 0);
    CASSERT(WJR_PP_LE(5, 3) == 0);
    CASSERT(WJR_PP_LE(3, 3) == 1);
    CASSERT(WJR_PP_LE(0, 0) == 1);
    CASSERT(WJR_PP_LE(63, 63) == 1);

    CASSERT(WJR_PP_GE(0, 1) == 0);
    CASSERT(WJR_PP_GE(3, 4) == 0);
    CASSERT(WJR_PP_GE(4, 5) == 0);
    CASSERT(WJR_PP_GE(0, 63) == 0);
    CASSERT(WJR_PP_GE(63, 0) == 1);
    CASSERT(WJR_PP_GE(5, 3) == 1);
    CASSERT(WJR_PP_GE(3, 3) == 1);
    CASSERT(WJR_PP_GE(0, 0) == 1);
    CASSERT(WJR_PP_GE(63, 63) == 1);
}

TEST(preprocessor, compiler) {
    CASSERT(WJR_HAS_BUILTIN(__TEST) == 0);
#define WJR_HAS_BUILTIN___TEST WJR_HAS_DEF
    CASSERT(WJR_HAS_BUILTIN(__TEST) == 1);
#undef WJR_HAS_BUILTIN___TEST
    CASSERT(WJR_HAS_BUILTIN(__TEST) == 0);
}

#define WJR_TEST_STR(a, b) static_assert(std::string_view(a) == std::string_view(b))

TEST(preprocessor, detail) {
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_IOTA(0))), "()");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_IOTA(1))), "(0)");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_IOTA(2))), "(0, 1)");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_IOTA(3))), "(0, 1, 2)");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_IOTA(4))), "(0, 1, 2, 3)");

    WJR_TEST_STR(WJR_PP_STR((WJR_PP_REPEAT(0, 0))), "()");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_REPEAT(0, 1))), "(0)");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_REPEAT(0, 2))), "(0, 0)");
    WJR_TEST_STR(WJR_PP_STR((WJR_PP_REPEAT(0, 3))), "(0, 0, 0)");
}

#if defined(WJR_COMPILER_MSVC) && !defined(WJR_COMPILER_CLANG)
    #define WJR_TEST_MSVC
#endif

TEST(preprocessor_preview, queue) {

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_FRONT((1))), "1");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_FRONT((1, 2, 3))), "1");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT((1))), "()");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT((1, 2, 3))), "(2, 3)");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUSH_FRONT((1, 2, 3), 0)), "(0, 1, 2, 3)");

#if defined(WJR_TEST_MSVC)
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUSH_FRONT((), 0)), "(0,)");
#else
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUSH_FRONT((), 0)), "(0, )");
#endif

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUSH_BACK((), 0)), "(, 0)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUSH_BACK((1, 2, 3), 0)), "(1, 2, 3, 0)");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_SIZE((1, 2, 3))), "3");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_SIZE((1))), "1");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_BACK((1))), "1");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_BACK((1, 2, 3))), "3");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_BACK((1, 2, 3, 4, 5))), "5");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT_N((1), 0)), "(1)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT_N((1, 2, 3), 0)), "(1, 2, 3)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT_N((1, 2, 3), 1)), "(2, 3)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT_N((1, 2, 3), 2)), "(3)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_FRONT_N((1, 2, 3), 3)), "()");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_BACK((1))), "()");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_BACK((1, 2, 3))), "(1, 2)");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_BACK_N((1, 2, 3), 0)), "(1, 2, 3)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_BACK_N((1, 2, 3), 1)), "(1, 2)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_POP_BACK_N((1, 2, 3), 2)), "(1)");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_AT((1, 2, 3, 4, 5), 0)), "1");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_AT((1, 2, 3, 4, 5), 1)), "2");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_AT((1, 2, 3, 4, 5), 2)), "3");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_AT((1, 2, 3, 4, 5), 3)), "4");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_AT((1, 2, 3, 4, 5), 4)), "5");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_REVERSE((1, 2, 3))), "(3, 2, 1)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_REVERSE((1, 3, 2))), "(2, 3, 1)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_REVERSE((1))), "(1)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_REVERSE(())), "()");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUT((1, 2, 3))), "1 2 3");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUT((1))), "1");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_PUT(())), "");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_UNWRAP(((1), (2), (3)))), "(1, 2, 3)");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ZIP_2((1, 2, 3), (4, 5, 6))), "((1, 4), (2, 5), (3, 6))");

#define WJR_TEST_SELF(x) x
#define WJR_TEST_F(x) f(x)
#define WJR_TEST_ADD(x, y) x + y
#define WJR_TEST_SUB(x, y) x - y

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_TRANSFORM((1, 2, 3), WJR_TEST_SELF)), "(1, 2, 3)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_TRANSFORM((1, 2, 3), WJR_TEST_F)), "(f(1), f(2), f(3))");
    WJR_TEST_STR(
        WJR_PP_STR(WJR_PP_QUEUE_TRANSFORMS((1, 2, 3), (WJR_TEST_F, WJR_TEST_SELF, WJR_TEST_F))),
        "(f(1), 2, f(3))");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ACCUMULATE(0, (1, 2, 3), WJR_TEST_ADD)), "0 + 1 + 2 + 3");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ACCUMULATES(0, (1, 2, 3),
                                                     (WJR_TEST_ADD, WJR_TEST_ADD, WJR_TEST_SUB))),
                 "0 + 1 + 2 - 3");

#undef WJR_TEST_F
#undef WJR_TEST_SELF

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ZIP_IOTA((A, B, C))), "((0, A), (1, B), (2, C))");
}