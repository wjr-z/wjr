#include <gtest/gtest.h>
#include <random>
#include <string_view>

#if defined(NDEBUG)
#undef NDEBUG
#endif

#if defined(WJR_DEBUG_LEVEL)
#undef WJR_DEBUG_LEVEL
#define WJR_DEBUG_LEVEL 3
#endif

#include <wjr/math.hpp>
#include <wjr/preprocessor.hpp>

#include <wjr/compressed_pair.hpp>

TEST(preprocessor, arithmatic) {
    WJR_ASSERT(WJR_PP_ADD(1, 3) == 4);
    WJR_ASSERT(WJR_PP_ADD(1, 4) == 5);
    WJR_ASSERT(WJR_PP_ADD(0, 4) == 4);
    WJR_ASSERT(WJR_PP_ADD(0, 63) == 63);
    WJR_ASSERT(WJR_PP_ADD(0, 0) == 0);
    WJR_ASSERT(WJR_PP_ADD(1, 63) == 0);
    WJR_ASSERT(WJR_PP_ADD(2, 63) == 1);
    WJR_ASSERT(WJR_PP_ADD(3, 63) == 2);

    WJR_ASSERT(WJR_PP_SUB(1, 1) == 0);
    WJR_ASSERT(WJR_PP_SUB(1, 2) == 63);
    WJR_ASSERT(WJR_PP_SUB(1, 3) == 62);
    WJR_ASSERT(WJR_PP_SUB(1, 4) == 61);
    WJR_ASSERT(WJR_PP_SUB(5, 4) == 1);
    WJR_ASSERT(WJR_PP_SUB(14, 4) == 10);

    WJR_ASSERT(WJR_PP_INC(0) == 1);
    WJR_ASSERT(WJR_PP_INC(2) == 3);
    WJR_ASSERT(WJR_PP_INC(63) == 0);

    WJR_ASSERT(WJR_PP_DEC(0) == 63);
    WJR_ASSERT(WJR_PP_DEC(2) == 1);
    WJR_ASSERT(WJR_PP_DEC(63) == 62);

    WJR_ASSERT(WJR_PP_NEG(0) == 63);
    WJR_ASSERT(WJR_PP_NEG(63) == 0);
    WJR_ASSERT(WJR_PP_NEG(3) == 60);

    WJR_ASSERT(WJR_PP_EQ(0, 0) == 1);
    WJR_ASSERT(WJR_PP_EQ(0, 1) == 0);
    WJR_ASSERT(WJR_PP_EQ(0, 63) == 0);
    WJR_ASSERT(WJR_PP_EQ(1, 4) == 0);

    WJR_ASSERT(WJR_PP_NE(0, 0) == 0);
    WJR_ASSERT(WJR_PP_NE(0, 1) == 1);
    WJR_ASSERT(WJR_PP_NE(0, 63) == 1);
    WJR_ASSERT(WJR_PP_NE(1, 4) == 1);

    WJR_ASSERT(WJR_PP_LT(0, 1) == 1);
    WJR_ASSERT(WJR_PP_LT(3, 4) == 1);
    WJR_ASSERT(WJR_PP_LT(4, 5) == 1);
    WJR_ASSERT(WJR_PP_LT(0, 63) == 1);
    WJR_ASSERT(WJR_PP_LT(63, 0) == 0);
    WJR_ASSERT(WJR_PP_LT(5, 3) == 0);
    WJR_ASSERT(WJR_PP_LT(3, 3) == 0);
    WJR_ASSERT(WJR_PP_LT(0, 0) == 0);
    WJR_ASSERT(WJR_PP_LT(63, 63) == 0);

    WJR_ASSERT(WJR_PP_GT(0, 1) == 0);
    WJR_ASSERT(WJR_PP_GT(3, 4) == 0);
    WJR_ASSERT(WJR_PP_GT(4, 5) == 0);
    WJR_ASSERT(WJR_PP_GT(0, 63) == 0);
    WJR_ASSERT(WJR_PP_GT(63, 0) == 1);
    WJR_ASSERT(WJR_PP_GT(5, 3) == 1);
    WJR_ASSERT(WJR_PP_GT(3, 3) == 0);
    WJR_ASSERT(WJR_PP_GT(0, 0) == 0);
    WJR_ASSERT(WJR_PP_GT(63, 63) == 0);

    WJR_ASSERT(WJR_PP_LE(0, 1) == 1);
    WJR_ASSERT(WJR_PP_LE(3, 4) == 1);
    WJR_ASSERT(WJR_PP_LE(4, 5) == 1);
    WJR_ASSERT(WJR_PP_LE(0, 63) == 1);
    WJR_ASSERT(WJR_PP_LE(63, 0) == 0);
    WJR_ASSERT(WJR_PP_LE(5, 3) == 0);
    WJR_ASSERT(WJR_PP_LE(3, 3) == 1);
    WJR_ASSERT(WJR_PP_LE(0, 0) == 1);
    WJR_ASSERT(WJR_PP_LE(63, 63) == 1);

    WJR_ASSERT(WJR_PP_GE(0, 1) == 0);
    WJR_ASSERT(WJR_PP_GE(3, 4) == 0);
    WJR_ASSERT(WJR_PP_GE(4, 5) == 0);
    WJR_ASSERT(WJR_PP_GE(0, 63) == 0);
    WJR_ASSERT(WJR_PP_GE(63, 0) == 1);
    WJR_ASSERT(WJR_PP_GE(5, 3) == 1);
    WJR_ASSERT(WJR_PP_GE(3, 3) == 1);
    WJR_ASSERT(WJR_PP_GE(0, 0) == 1);
    WJR_ASSERT(WJR_PP_GE(63, 63) == 1);
}

TEST(preprocessor, compiler) {
    WJR_ASSERT(WJR_HAS_BUILTIN(__TEST) == 0);
#define WJR_HAS_BUILTIN___TEST WJR_HAS_DEF
    WJR_ASSERT(WJR_HAS_BUILTIN(__TEST) == 1);
#undef WJR_HAS_BUILTIN___TEST
    WJR_ASSERT(WJR_HAS_BUILTIN(__TEST) == 0);
}

#define WJR_TEST_STR(a, b) WJR_ASSERT(std::string_view(a) == std::string_view(b))

TEST(preprocessor, details) {
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

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ZIP_2((1, 2, 3), (4, 5, 6))),
                 "((1, 4), (2, 5), (3, 6))");

#define WJR_TEST_SELF(x) x
#define WJR_TEST_F(x) f(x)
#define WJR_TEST_ADD(x, y) x + y
#define WJR_TEST_SUB(x, y) x - y

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_TRANSFORM((1, 2, 3), WJR_TEST_SELF)),
                 "(1, 2, 3)");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_TRANSFORM((1, 2, 3), WJR_TEST_F)),
                 "(f(1), f(2), f(3))");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_TRANSFORMS(
                     (1, 2, 3), (WJR_TEST_F, WJR_TEST_SELF, WJR_TEST_F))),
                 "(f(1), 2, f(3))");

    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ACCUMULATE(0, (1, 2, 3), WJR_TEST_ADD)),
                 "0 + 1 + 2 + 3");
    WJR_TEST_STR(WJR_PP_STR(WJR_PP_QUEUE_ACCUMULATES(
                     0, (1, 2, 3), (WJR_TEST_ADD, WJR_TEST_ADD, WJR_TEST_SUB))),
                 "0 + 1 + 2 - 3");

#undef WJR_TEST_F
#undef WJR_TEST_SELF
}

TEST(math, popcount_ctz_clz) {

#define WJR_TEST_PTZ(queue) WJR_PP_TRANSFORM_PUT(queue, WJR_TEST_PTZ_I_CALLER)

#define WJR_TEST_POPCOUNT_I(type, x, ans)                                                \
    WJR_ASSERT(wjr::fallback_popcount<type>(x) == ans)                                   \
    WJR_PP_BOOL_IF(                                                                      \
        WJR_HAS_BUILTIN(POPCOUNT), ;                                                     \
        do { WJR_ASSERT((wjr::builtin_popcount<type>(x) == ans)); } while (0), );
#define WJR_TEST_CTZ_I(type, x, ans)                                                     \
    type n = x;                                                                          \
    auto ctz_ans = wjr::popcount<type>((type)(wjr::lowbit(n) - 1));                      \
    WJR_ASSERT(wjr::fallback_ctz<type>(x) == ctz_ans)                                    \
    WJR_PP_BOOL_IF(                                                                      \
        WJR_HAS_BUILTIN(CTZ), ;                                                          \
        do { WJR_ASSERT((wjr::builtin_ctz<type>(x) == ctz_ans)); } while (0), );
#define WJR_TEST_CLZ_I(type, x, ans)                                                     \
    auto clz_ans = []() -> int {                                                         \
        type n = x;                                                                      \
        constexpr auto nd = std::numeric_limits<type>::digits;                           \
        if (n == 0) {                                                                    \
            return nd;                                                                   \
        }                                                                                \
        n |= n >> 1;                                                                     \
        n |= n >> 2;                                                                     \
        n |= n >> 4;                                                                     \
        if constexpr (nd >= 16) {                                                        \
            n |= n >> 8;                                                                 \
        }                                                                                \
        if constexpr (nd >= 32) {                                                        \
            n |= n >> 16;                                                                \
        }                                                                                \
        if constexpr (nd >= 64) {                                                        \
            n |= n >> 32;                                                                \
        }                                                                                \
        ++n;                                                                             \
        return nd - wjr::ctz<type>(n);                                                   \
    }();                                                                                 \
    WJR_ASSERT(wjr::fallback_clz<type>(x) == clz_ans)                                    \
    WJR_PP_BOOL_IF(                                                                      \
        WJR_HAS_BUILTIN(CTZ), ;                                                          \
        do { WJR_ASSERT((wjr::builtin_clz<type>(x) == clz_ans)); } while (0), );

#define WJR_TEST_PTZ_I_CALLER(args)                                                      \
    do {                                                                                 \
        WJR_TEST_POPCOUNT_I args WJR_TEST_CTZ_I args WJR_TEST_CLZ_I args                 \
    } while (0);

    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x00, 0));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x01, 1));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x02, 1));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x03, 2));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x04, 1));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x0a, 2));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x0b, 3));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x0c, 2));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x0f, 4));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0x17, 4));
    WJR_TEST_PTZ_I_CALLER((uint8_t, 0xff, 8));

    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0000, 0));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0001, 1));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0002, 1));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0003, 2));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0004, 1));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x000a, 2));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x000b, 3));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x000c, 2));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x000f, 4));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0017, 4));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x00ff, 8));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0x0a0f, 6));
    WJR_TEST_PTZ_I_CALLER((uint16_t, 0xffff, 16));

    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0000, 0));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0001, 1));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0002, 1));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0003, 2));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0004, 1));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'000a, 2));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'000b, 3));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'000c, 2));

    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'000f, 4));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0013, 3));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'0017, 4));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'00ff, 8));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0000'00cf, 6));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0001'00cd, 6));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0100'0000, 1));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0200'0000, 1));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0300'0000, 2));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0x0400'0000, 1));
    WJR_TEST_PTZ_I_CALLER((uint32_t, 0xffff'ffff, 32));

    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0000, 0));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0001, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0002, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0003, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0004, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000a, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000b, 3));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000c, 2));

    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000f, 4));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0013, 3));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0017, 4));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'00ff, 8));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'00cf, 6));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0001'00cd, 6));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0100'0000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0200'0000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0300'0000, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0400'0000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0xffff'ffff, 32));

    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0000'0000'0000, 0));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0001'0000'000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0002'0000'000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0003'0000'000, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0004'0000'000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000a'0000'000, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000b'0000'000, 3));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000c'0000'000, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'000f'0000'000, 4));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0013'0000'000, 3));

    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'0017'0000'000, 4));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'00ff'0000'000, 8));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0000'00cf'0000'000, 6));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0001'00cd'0000'000, 6));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0100'0000'0000'000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0200'0000'0000'000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0300'0000'0000'000, 2));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0x0400'0000'0000'000, 1));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0xffff'ffff'0000'000, 32));
    WJR_TEST_PTZ_I_CALLER((uint64_t, 0xffff'ffff'ffff'ffff, 64));

#undef WJR_TEST_PTZ_I_CALLER
#undef WJR_TEST_CLZ_I
#undef WJR_TEST_CTZ_I
#undef WJR_TEST_POPCOUNT_I
#undef WJR_TEST_PTZ
}

TEST(math, addc) {

#define WJR_TEST_ADDC(type, x, y, ci, ans, ans_co)                                       \
    WJR_TEST_ADDC_I(type, x, y, ci, co, ans, ans_co)
#define WJR_TEST_ADDC_I(type, x, y, ci, co, ans, ans_co)                                 \
    WJR_ASSERT((wjr::fallback_addc<type, type>(x, y, ci, co) == ans && co == ans_co));   \
    WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC),                                                \
                   do {                                                                  \
                       WJR_ASSERT((wjr::builtin_addc<type, type>(x, y, ci, co) == ans && \
                                   co == ans_co));                                       \
                   } while (0),                                                          \
                   {});                                                                  \
    WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ASM_ADDC),                                            \
                   do {                                                                  \
                       if constexpr (std::is_same_v<type, uint64_t>) {                   \
                           WJR_ASSERT((wjr::asm_addc<type, type>(x, y, ci, co) == ans && \
                                       co == ans_co));                                   \
                       }                                                                 \
                   } while (0),                                                          \
                   {})

#define WJR_TEST_ADDC_F(type)                                                            \
    do {                                                                                 \
        constexpr auto maxn = std::numeric_limits<type>::max();                          \
        type co;                                                                         \
        WJR_TEST_ADDC(type, 0, 0, 0, 0, 0);                                              \
        WJR_TEST_ADDC(type, 0, 0, 1, 1, 0);                                              \
        WJR_TEST_ADDC(type, 0, maxn, 0, maxn, 0);                                        \
        WJR_TEST_ADDC(type, 1, maxn, 0, 0, 1);                                           \
        WJR_TEST_ADDC(type, 0, maxn, 1, 0, 1);                                           \
        WJR_TEST_ADDC(type, 1, maxn, 1, 1, 1);                                           \
    } while (0);

    WJR_PP_TRANSFORM_PUT(
        (unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long),
        WJR_TEST_ADDC_F);

#undef WJR_TEST_ADDC_F
#undef WJR_TEST_ADDC_I
#undef WJR_TEST_ADDC
}

TEST(math, addc_1) {

    {
        std::vector<uint64_t> in, tmp, copy;
        uint64_t add;
        std::mt19937_64 mt_rand(time(0));
        for (size_t n = 1; n <= 256; n = n + (n == 1 ? 1 : n / 2)) {
            in.resize(n);
            tmp.resize(n);
            for (auto &x : in) {
                x = 1;
            }

            for (auto p : {&in, &tmp}) {
                for (uint64_t cf : {0, 1}) {
                    for (size_t k : {0, 1, 2, -1}) {
                        copy = in;

                        if (!k) {
                            add = 3 - cf;
                        } else {
                            add = -1 - cf;
                        }

                        auto l = std::min(k, n);

                        for (size_t p = 1; p < l; ++p) {
                            in[p] = -1;
                        }

                        auto &out = *p;

                        uint64_t c_out = wjr::addc_1(out.data(), in.data(), n, add, cf);

                        if (l == n && k) {
                            WJR_ASSERT(c_out == 1);
                        } else {
                            WJR_ASSERT(c_out == 0);
                        }

                        if (!k) {
                            WJR_ASSERT(out[0] == 4);
                            for (size_t p = 1; p < n; ++p) {
                                WJR_ASSERT(out[p] == 1);
                            }
                        } else {
                            for (size_t p = 0; p < l; ++p) {
                                WJR_ASSERT(out[p] == 0);
                            }
                            if (l != n) {
                                WJR_ASSERT(out[l] == 2);
                                for (size_t p = l + 1; p < n; ++p) {
                                    WJR_ASSERT(out[p] == 1);
                                }
                            }
                        }

                        in = copy;
                    }
                }
            }
        }
    }

    {
        std::vector<uint64_t> x, y;
        for (size_t n = 1; n <= 256; ++n) {
            x.resize(n);
            y.resize(n);
            for (auto &j : x) {
                j = -1;
            }

            auto cf = wjr::addc_1(y.data(), x.data(), n, 1, 0u);
            WJR_ASSERT(cf == 1);
            for (auto &j : y) {
                WJR_ASSERT(j == 0);
            }

            cf = wjr::addc_1(y.data(), x.data(), n, 0, 1u);
            WJR_ASSERT(cf == 1);
            for (auto &j : y) {
                WJR_ASSERT(j == 0);
            }

            cf = wjr::addc_1(y.data(), x.data(), n, 1, 1u);
            WJR_ASSERT(cf == 1);
            WJR_ASSERT(y[0] == 1);
            for (size_t j = 1; j < n; ++j) {
                WJR_ASSERT(y[j] == 0);
            }

            cf = wjr::addc_1(x.data(), x.data(), n, 1, 0u);
            WJR_ASSERT(cf == 1);
            for (auto &j : x) {
                WJR_ASSERT(j == 0);
                j = -1;
            }

            if (n > 1) {
                cf = wjr::addc_1(x.data(), x.data() + 1, n - 1, 1, 0u);
                WJR_ASSERT(cf == 1);
                for (size_t j = 0; j < n - 1; ++j) {
                    WJR_ASSERT(x[j] == 0);
                }
                WJR_ASSERT(x.back() == -1ull);
            }
        }
    }
}

TEST(math, addc_n) {
    std::vector<uint64_t> a, b, c;
    for (size_t n = 1; n <= 384; ++n) {
        a.resize(n);
        b.resize(n);
        c.resize(n);

        a[0] = 1;
        b[0] = -1;

        for (size_t i = 1; i < n; ++i) {
            a[i] = 1;
            b[i] = -2;
        }

        auto cf = wjr::addc_n(c.data(), a.data(), b.data(), n, 0u);
        WJR_ASSERT(cf == 1);

        for (auto &i : c) {
            WJR_ASSERT(i == 0);
        }
    }
}

TEST(math, sub) {

#define WJR_TEST_SUBC(type, x, y, ci, ans, ans_co)                                       \
    WJR_TEST_SUBC_I(type, x, y, ci, co, ans, ans_co)
#define WJR_TEST_SUBC_I(type, x, y, ci, co, ans, ans_co)                                 \
    WJR_ASSERT((wjr::fallback_subc<type, type>(x, y, ci, co) == ans && co == ans_co))    \
    WJR_PP_BOOL_IF(                                                                      \
        WJR_HAS_BUILTIN(SUBC), ; do {                                                    \
            WJR_ASSERT(                                                                  \
                (wjr::builtin_subc<type, type>(x, y, ci, co) == ans && co == ans_co));   \
        } while (0), )                                                                   \
    WJR_PP_BOOL_IF(                                                                      \
        WJR_HAS_BUILTIN(ASM_SUBC), ; do {                                                \
            if constexpr (std::is_same_v<type, uint64_t>) {                              \
                WJR_ASSERT(                                                              \
                    (wjr::asm_subc<type, type>(x, y, ci, co) == ans && co == ans_co));   \
            }                                                                            \
        } while (0), )

#define WJR_TEST_SUBC_F(type)                                                            \
    do {                                                                                 \
        constexpr auto maxn = std::numeric_limits<type>::max();                          \
        type co;                                                                         \
        WJR_TEST_SUBC(type, 0, 0, 0, 0, 0);                                              \
        WJR_TEST_SUBC(type, 0, 0, 1, maxn, 1);                                           \
        WJR_TEST_SUBC(type, 1, 0, 1, 0, 0);                                              \
        WJR_TEST_SUBC(type, 0, maxn, 0, 1, 1);                                           \
        WJR_TEST_SUBC(type, 1, maxn, 0, 2, 1);                                           \
        WJR_TEST_SUBC(type, 0, maxn, 1, 0, 1);                                           \
        WJR_TEST_SUBC(type, 1, maxn, 1, 1, 1);                                           \
    } while (0);

    WJR_PP_TRANSFORM_PUT(
        (unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long),
        WJR_TEST_SUBC_F);

#undef WJR_TEST_SUBC_F
#undef WJR_TEST_SUBC_I
#undef WJR_TEST_SUBC

#define WJR_TEST_SUBC_1(queue) WJR_PP_TRANSFORM_PUT(queue, WJR_TEST_SUBC_1_I_CALLER)
#define WJR_TEST_SUBC_1_EXPAND(args) WJR_TEST_SUBC_1_EXPAND_I args
#define WJR_TEST_SUBC_1_EXPAND_I(...) __VA_ARGS__
#define WJR_TEST_SUBC_1_I(type, inputs, c, c_in, outputs, ans)                           \
    {                                                                                    \
        constexpr auto N = WJR_PP_QUEUE_SIZE(inputs);                                    \
        auto init = [](type *arr, auto... args) {                                        \
            auto il = {(args)...};                                                       \
            for (size_t i = 0; i < il.size(); ++i) {                                     \
                arr[i] = il.begin()[i];                                                  \
            }                                                                            \
        };                                                                               \
        type in[N];                                                                      \
        type out[N];                                                                     \
        type expect[N];                                                                  \
        init(in, WJR_PP_QUEUE_EXPAND(inputs));                                           \
        init(expect, WJR_PP_QUEUE_EXPAND(outputs));                                      \
        WJR_ASSERT((wjr::subc_1<type, type>(out, in, N, c, c_in) == ans));               \
        WJR_ASSERT((memcmp(out, expect, sizeof(out)) == 0),                              \
                   "uncorrect array of subc_1<%s, %s>, different array", #type, #type);  \
        WJR_ASSERT((wjr::subc_1<type, type>(in, in, N, c, c_in) == ans));                \
        WJR_ASSERT((memcmp(in, expect, sizeof(out)) == 0),                               \
                   "uncorrect array of subc_1<%s, %s>, same array", #type, #type);       \
    }
#define WJR_TEST_SUBC_1_I_CALLER(args) WJR_TEST_SUBC_1_I args

    WJR_TEST_SUBC_1(((uint64_t, (1, 2, 3), 0, 0, (1, 2, 3), 0),
                     (uint64_t, (1, 2, 3), 1, 0, (0, 2, 3), 0),
                     (uint64_t, (0, 2, 3), 1, 0, (-1, 1, 3), 0),
                     (uint64_t, (1, 2, 3), 1, 1, (-1, 1, 3), 0)));

    WJR_TEST_SUBC_1(((uint64_t, (2, 2, 3), 1, 1, (0, 2, 3), 0),
                     (uint64_t, (1, 0, 3), 1, 1, (-1, -1, 2), 0),
                     (uint64_t, (0, 0, 0), 3, 1, (-4, -1, -1), 1),
                     (uint64_t, (5, 0, 0), 10, 1, (-6, -1, -1), 1)));

#undef WJR_TEST_SUBC_1_I_CALLER
#undef WJR_TEST_SUBC_1_I
#undef WJR_TEST_SUBC_1_EXPAND_I
#undef WJR_TEST_SUBC_1_EXPAND
#undef WJR_TEST_SUBC_1
}

TEST(math, subc_1) {

    {
        std::vector<uint64_t> in, tmp, copy;
        uint64_t sub;
        std::mt19937_64 mt_rand(time(0));
        for (size_t n = 1; n <= 256; n = n + (n == 1 ? 1 : n / 2)) {
            in.resize(n);
            tmp.resize(n);
            for (auto &x : in) {
                x = 4;
            }

            for (auto p : {&in, &tmp}) {
                for (uint64_t cf : {0, 1}) {
                    for (size_t k : {0, 1, 2, -1}) {
                        copy = in;

                        if (!k) {
                            sub = 2 - cf;
                        } else {
                            sub = 5 - cf;
                        }

                        auto l = std::min(k, n);

                        for (size_t p = 1; p < l; ++p) {
                            in[p] = 0;
                        }

                        auto &out = *p;

                        uint64_t c_out = wjr::subc_1(out.data(), in.data(), n, sub, cf);

                        if (l == n && k) {
                            WJR_ASSERT(c_out == 1);
                        } else {
                            WJR_ASSERT(c_out == 0);
                        }

                        if (!k) {
                            WJR_ASSERT(out[0] == 2);
                            for (size_t p = 1; p < n; ++p) {
                                WJR_ASSERT(out[p] == 4);
                            }
                        } else {
                            for (size_t p = 0; p < l; ++p) {
                                WJR_ASSERT(out[p] == -1ull);
                            }
                            if (l != n) {
                                WJR_ASSERT(out[l] == 3);
                                for (size_t p = l + 1; p < n; ++p) {
                                    WJR_ASSERT(out[p] == 4);
                                }
                            }
                        }

                        in = copy;
                    }
                }
            }
        }
    }

    {
        std::vector<uint64_t> x, y;
        for (size_t n = 1; n <= 256; ++n) {
            x.resize(n);
            y.resize(n);
            for (auto &j : x) {
                j = 0;
            }

            auto cf = wjr::subc_1(y.data(), x.data(), n, 1, 0u);
            WJR_ASSERT(cf == 1);
            for (auto &j : y) {
                WJR_ASSERT(j == -1ull);
            }

            cf = wjr::subc_1(y.data(), x.data(), n, 0, 1u);
            WJR_ASSERT(cf == 1);
            for (auto &j : y) {
                WJR_ASSERT(j == -1ull);
            }

            cf = wjr::subc_1(y.data(), x.data(), n, 1, 1u);
            WJR_ASSERT(cf == 1);
            WJR_ASSERT(y[0] == -2ull);
            for (size_t j = 1; j < n; ++j) {
                WJR_ASSERT(y[j] == -1ull);
            }

            cf = wjr::subc_1(x.data(), x.data(), n, 1, 0u);
            WJR_ASSERT(cf == 1);
            for (auto &j : x) {
                WJR_ASSERT(j == -1ull);
                j = 0;
            }

            if (n > 1) {
                cf = wjr::subc_1(x.data(), x.data() + 1, n - 1, 1, 0u);
                WJR_ASSERT(cf == 1);
                for (size_t j = 0; j < n - 1; ++j) {
                    WJR_ASSERT(x[j] == -1ull);
                }
                WJR_ASSERT(x.back() == 0);
            }
        }
    }
}

TEST(math, subc_n) {
    std::vector<uint64_t> a, b, c;
    for (size_t n = 1; n <= 384; ++n) {
        a.resize(n);
        b.resize(n);
        c.resize(n);

        a[0] = 1;
        b[0] = 2;

        for (size_t i = 1; i < n; ++i) {
            a[i] = 1;
            b[i] = 1;
        }

        auto cf = wjr::subc_n(c.data(), a.data(), b.data(), n, 0u);
        WJR_ASSERT(cf == 1, "%zu", n);

        for (auto &i : c) {
            WJR_ASSERT(i == -1ull);
        }
    }
}

TEST(math, broadcast) {
#define WJR_TEST_BROADCAST(ft, tt, x, expect)                                            \
    WJR_ASSERT((wjr::broadcast<ft, tt>(x) == expect))

    WJR_TEST_BROADCAST(uint8_t, uint8_t, 0, 0);
    WJR_TEST_BROADCAST(uint8_t, uint8_t, 0x3f, 0x3f);

    WJR_TEST_BROADCAST(uint16_t, uint16_t, 0, 0);
    WJR_TEST_BROADCAST(uint16_t, uint16_t, 0x3f, 0x3f);
    WJR_TEST_BROADCAST(uint16_t, uint16_t, 0x1f03, 0x1f03);
    WJR_TEST_BROADCAST(uint16_t, uint16_t, 0x1f00, 0x1f00);

    WJR_TEST_BROADCAST(uint32_t, uint32_t, 0, 0);
    WJR_TEST_BROADCAST(uint32_t, uint32_t, 0x3f, 0x3f);
    WJR_TEST_BROADCAST(uint32_t, uint32_t, 0x1f03, 0x1f03);
    WJR_TEST_BROADCAST(uint32_t, uint32_t, 0x1f00, 0x1f00);
    WJR_TEST_BROADCAST(uint32_t, uint32_t, 0xac'0000, 0xac'0000);

    WJR_TEST_BROADCAST(uint64_t, uint64_t, 0, 0);
    WJR_TEST_BROADCAST(uint64_t, uint64_t, 0x3f, 0x3f);
    WJR_TEST_BROADCAST(uint64_t, uint64_t, 0x1f03, 0x1f03);
    WJR_TEST_BROADCAST(uint64_t, uint64_t, 0x1f00, 0x1f00);
    WJR_TEST_BROADCAST(uint64_t, uint64_t, 0xac'0000, 0xac'0000);

    WJR_TEST_BROADCAST(uint8_t, uint16_t, 0, 0);
    WJR_TEST_BROADCAST(uint8_t, uint16_t, 0x0001, 0x0101);
    WJR_TEST_BROADCAST(uint8_t, uint16_t, 0x0031, 0x3131);

    WJR_TEST_BROADCAST(uint8_t, uint32_t, 0, 0);
    WJR_TEST_BROADCAST(uint8_t, uint32_t, 0x0000'0031, 0x3131'3131);
    WJR_TEST_BROADCAST(uint8_t, uint32_t, 0x0000'000c, 0x0c0c'0c0c);

    WJR_TEST_BROADCAST(uint8_t, uint64_t, 0, 0);
    WJR_TEST_BROADCAST(uint8_t, uint64_t, 0x0000'0000'0000'000c, 0x0c0c'0c0c'0c0c'0c0c);

    WJR_TEST_BROADCAST(uint16_t, uint32_t, 0, 0);
    WJR_TEST_BROADCAST(uint16_t, uint32_t, 0x0001, 0x0001'0001);
    WJR_TEST_BROADCAST(uint16_t, uint32_t, 0x0c01, 0x0c01'0c01);

    WJR_TEST_BROADCAST(uint16_t, uint64_t, 0, 0);
    WJR_TEST_BROADCAST(uint16_t, uint64_t, 0x0001, 0x0001'0001'0001'0001);
    WJR_TEST_BROADCAST(uint16_t, uint64_t, 0x0c01, 0x0c01'0c01'0c01'0c01);
    WJR_TEST_BROADCAST(uint16_t, uint64_t, 0xacb1, 0xacb1'acb1'acb1'acb1);

    WJR_TEST_BROADCAST(uint32_t, uint64_t, 0, 0);
    WJR_TEST_BROADCAST(uint32_t, uint64_t, 0x0001'0001, 0x0001'0001'0001'0001);
    WJR_TEST_BROADCAST(uint32_t, uint64_t, 0xabc2'def3, 0xabc2'def3'abc2'def3);

#undef WJR_TEST_BROADCAST
}

TEST(math, find_n) {
    std::vector<uint64_t> a, b;

    for (size_t n = 0; n < 384; ++n) {
        a.resize(n);
        b.resize(n);
        for (size_t i = 0; i < n; ++i) {
            a[i] = -1;
            b[i] = -2;
        }
        for (size_t m = 0; m <= n; ++m) {

            if (m != n) {
                a[m] = 0;
                b[m] = 0;
            }

            auto idx = wjr::find_n(a.data(), 0, n);

            WJR_ASSERT(idx == m);

            idx = wjr::find_n(a.data(), b.data(), n);

            WJR_ASSERT(idx == m);

            if (m != n) {
                a[m] = -1;
                b[m] = -2;
            }
        }
    }
}

TEST(math, find_not_n) {
    std::vector<uint64_t> a, b;

    for (size_t n = 0; n < 384; ++n) {
        a.resize(n);
        b.resize(n);
        for (size_t i = 0; i < n; ++i) {
            a[i] = 0;
            b[i] = 0;
        }
        for (size_t m = 0; m <= n; ++m) {

            if (m != n) {
                a[m] = -1;
                b[m] = -2;
            }

            auto idx = wjr::find_not_n(a.data(), 0, n);

            WJR_ASSERT(idx == m);

            idx = wjr::find_not_n(a.data(), b.data(), n);

            WJR_ASSERT(idx == m);

            if (m != n) {
                a[m] = 0;
                b[m] = 0;
            }
        }
    }
}

TEST(math, reverse_find_n) {
    std::vector<uint64_t> a, b;

    for (size_t n = 0; n < 384; ++n) {
        a.resize(n);
        b.resize(n);
        for (size_t i = 0; i < n; ++i) {
            a[i] = -1;
            b[i] = -2;
        }
        for (size_t m = 0; m <= n; ++m) {

            if (m != n) {
                a[n - m - 1] = 0;
                b[n - m - 1] = 0;
            }

            auto idx = wjr::reverse_find_n(a.data(), 0, n);

            WJR_ASSERT(idx == n - m);

            idx = wjr::reverse_find_n(a.data(), b.data(), n);

            WJR_ASSERT(idx == n - m);

            if (m != n) {
                a[n - m - 1] = -1;
                b[n - m - 1] = -2;
            }
        }
    }
}

TEST(math, reverse_find_not_n) {
    std::vector<uint64_t> a, b;

    for (size_t n = 0; n < 384; ++n) {
        a.resize(n);
        b.resize(n);
        for (size_t i = 0; i < n; ++i) {
            a[i] = 0;
            b[i] = 0;
        }
        for (size_t m = 0; m <= n; ++m) {

            if (m != n) {
                a[n - m - 1] = -1;
                b[n - m - 1] = -2;
            }

            auto idx = wjr::reverse_find_not_n(a.data(), 0, n);

            WJR_ASSERT(idx == n - m);

            idx = wjr::reverse_find_not_n(a.data(), b.data(), n);

            WJR_ASSERT(idx == n - m);

            if (m != n) {
                a[n - m - 1] = 0;
                b[n - m - 1] = 0;
            }
        }
    }
}

TEST(math, replace_find_not) {
    {
        std::vector<uint64_t> a, b;

        for (size_t n = 0; n < 384; ++n) {
            a.resize(n + 32);
            b.resize(n + 32);

            for (size_t m = 0; m <= n; ++m) {

                for (size_t i = 0; i < n; ++i) {
                    a[i] = 0;
                }

                for (size_t i = n; i < n + 32; ++i) {
                    a[i] = -1;
                }

                for (size_t i = 0; i < n; ++i) {
                    b[i] = 0;
                }

                for (size_t i = n; i < n + 32; ++i) {
                    b[i] = -1;
                }

                if (m != n) {
                    a[m] = -1;
                }

                auto idx = wjr::replace_find_not(b.data(), a.data(), n, 0, 1);

                WJR_ASSERT(idx == m);

                for (size_t i = 0; i < m; ++i) {
                    WJR_ASSERT(b[i] == 1);
                }

                if (n != m) {
                    for (size_t i = m; i < n; ++i) {
                        WJR_ASSERT(b[i] == 0);
                    }
                }

                idx = wjr::replace_find_not(a.data(), a.data(), n, 0, 0);

                WJR_ASSERT(idx == m);

                for (size_t i = 0; i < m; ++i) {
                    WJR_ASSERT(a[i] == 0);
                }

                if (n != m) {
                    WJR_ASSERT(a[m] == -1ull);
                    for (size_t i = m + 1; i < n; ++i) {
                        WJR_ASSERT(a[i] == 0);
                    }
                }

                idx = wjr::replace_find_not(a.data(), a.data(), n, 0, 1);

                WJR_ASSERT(idx == m);

                for (size_t i = 0; i < m; ++i) {
                    WJR_ASSERT(a[i] == 1);
                }

                if (n != m) {
                    WJR_ASSERT(a[m] == -1ull);
                    for (size_t i = m + 1; i < n; ++i) {
                        WJR_ASSERT(a[i] == 0);
                    }
                }
            }
        }
    }
}

TEST(math, not_n) {
    std::vector<uint64_t> a, b;
    for (size_t n = 0; n <= 384; ++n) {
        a.resize(n);
        b.resize(n);
        for (auto &i : a) {
            i = 0;
        }

        wjr::not_n(b.data(), a.data(), n);
        for (auto &i : b) {
            WJR_ASSERT(i == -1ull);
        }

        wjr::not_n(a.data(), a.data(), n);
        for (auto &i : a) {
            WJR_ASSERT(i == -1ull);
        }
    }
}

TEST(math, neg) {
    std::mt19937_64 mt_rand(time(0));
    std::vector<uint64_t> a, b, c;
    for (size_t n = 0; n <= 384; ++n) {
        a.resize(n);
        b.resize(n);
        c.resize(n);
        for (size_t m = 0; m <= n; ++m) {
            for (size_t i = 0; i < m; ++i) {
                a[i] = 0;
            }
            uint64_t xx = 0;
            if (n != m) {
                do {
                    xx = a[m] = mt_rand();
                } while (!xx);
                for (size_t i = m + 1; i < n; ++i) {
                    a[i] = mt_rand();
                }
            }

            c = a;

            for (auto &i : b) {
                i = -1;
            }

            bool zero = wjr::neg_n(b.data(), a.data(), n);

            if (n == m) {
                WJR_ASSERT(zero);
            } else {
                WJR_ASSERT(!zero);
            }

            for (size_t i = 0; i < m; ++i) {
                WJR_ASSERT(b[i] == 0);
            }

            if (n != m) {
                WJR_ASSERT(b[m] == -xx);
                for (size_t i = m + 1; i < n; ++i) {
                    WJR_ASSERT(b[i] == ~c[i]);
                }
            }

            auto cf = wjr::addc_n(c.data(), a.data(), b.data(), n, 0u);
            if (n != m) {
                WJR_ASSERT(cf);
            } else {
                WJR_ASSERT(!cf);
            }

            for (size_t i = 0; i < n; ++i) {
                WJR_ASSERT(c[i] == 0);
            }
        }
    }
}

TEST(math, set_n) {
    std::mt19937_64 mt_rand(time(0));
    std::vector<uint64_t> a;
    for (size_t n = 0; n <= 384; ++n) {
        a.resize(n);

        uint64_t b = mt_rand();

        for (auto &i : a) {
            i = mt_rand();
        }

        wjr::set_n(a.data(), b, n);

        for (auto &i : a) {
            WJR_ASSERT(i == b);
        }
    }
}

TEST(math, compare_n) {
    std::mt19937_64 mt_rand(time(0));
    std::vector<uint64_t> a, b;
    for (size_t n = 0; n <= 384; ++n) {
        a.resize(n);
        for (size_t m = 0; m <= n; ++m) {
            for (auto &i : a) {
                i = mt_rand();
            }

            b = a;

            for (size_t i = m; i < n; ++i) {
                while (a[m] == -1ull) {
                    a[m] = mt_rand();
                }
                b[m] = a[m] + 1;
            }

            int f = wjr::compare_n(a.data(), b.data(), n);
            if (n == m) {
                WJR_ASSERT(f == 0);
                continue;
            }

            WJR_ASSERT(f < 0);

            for (size_t i = m; i < n; ++i) {
                while (a[m] == 0) {
                    a[m] = mt_rand();
                }
                b[m] = a[m] - 1;
            }

            f = wjr::compare_n(a.data(), b.data(), n);

            WJR_ASSERT(f > 0);
        }
    }
}

TEST(math, reverse_compare_n) {
    std::mt19937_64 mt_rand(time(0));
    std::vector<uint64_t> a, b;
    for (size_t n = 0; n <= 384; ++n) {
        a.resize(n);
        for (size_t m = 0; m <= n; ++m) {
            for (auto &i : a) {
                i = mt_rand();
            }

            b = a;

            for (size_t i = m; i < n; ++i) {
                while (a[n - m - 1] == -1ull) {
                    a[n - m - 1] = mt_rand();
                }
                b[n - m - 1] = a[n - m - 1] + 1;
            }

            int f = wjr::reverse_compare_n(a.data(), b.data(), n);
            if (n == m) {
                WJR_ASSERT(f == 0);
                continue;
            }

            WJR_ASSERT(f < 0);

            for (size_t i = m; i < n; ++i) {
                while (a[n - m - 1] == 0) {
                    a[n - m - 1] = mt_rand();
                }
                b[n - m - 1] = a[n - m - 1] - 1;
            }

            f = wjr::reverse_compare_n(a.data(), b.data(), n);

            WJR_ASSERT(f > 0);
        }
    }
}

TEST(math, shld) {
#define WJR_TEST_SHLD(hi, lo, c, expect)                                                 \
    WJR_ASSERT(wjr::shld<uint64_t>((hi), (lo), (c)) == (expect))

    WJR_TEST_SHLD(0, 0, 1, 0);
    WJR_TEST_SHLD(0, 1, 1, 0);
    WJR_TEST_SHLD(0, 1ull << 63, 1, 1);
    WJR_TEST_SHLD(1, 1, 1, 2);
    WJR_TEST_SHLD(1, 1ull << 63, 1, 3);

#undef WJR_TEST_SHLD
}

TEST(math, shrd) {}

TEST(math, lshift_n) {
    {
        std::vector<uint64_t> a, b;
        std::mt19937_64 mt_rand(time(0));

        for (size_t n = 1; n <= 512; ++n) {
            a.resize(n);
            b.resize(n);

            for (size_t i = 0; i < n; ++i) {
                a[i] = mt_rand();
            }

            for (unsigned int c = 0; c < 64; ++c) {
                uint64_t ex = c ? (a[n - 1] >> (64 - c)) : 0;
                auto z = wjr::lshift_n(b.data(), a.data(), n, c);
                WJR_ASSERT(z == ex);

                for (size_t i = 1; i < n; ++i) {
                    ex = c ? ((a[i] << c) | (a[i - 1] >> (64 - c))) : a[i];
                    WJR_ASSERT(b[i] == ex);
                }

                ex = a[0] << c;

                WJR_ASSERT(b[0] == ex);
            }
        }
    }
}

TEST(math, rshift_n) {
    {
        std::vector<uint64_t> a, b;
        std::mt19937_64 mt_rand(time(0));

        for (size_t n = 1; n <= 512; ++n) {
            a.resize(n);
            b.resize(n);

            for (size_t i = 0; i < n; ++i) {
                a[i] = mt_rand();
            }

            for (unsigned int c = 0; c < 64; ++c) {
                uint64_t ex = c ? (a[0] << (64 - c)) : 0;
                auto z = wjr::rshift_n(b.data(), a.data(), n, c);
                WJR_ASSERT(z == ex);

                for (size_t i = 0; i < n - 1; ++i) {
                    ex = c ? ((a[i] >> c) | (a[i + 1] << (64 - c))) : a[i];
                    WJR_ASSERT(b[i] == ex);
                }

                ex = a[n - 1] >> c;

                WJR_ASSERT(b[n - 1] == ex);
            }
        }
    }
}