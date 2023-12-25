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
    WJR_ASSERT(wjr::fallback_clz<type>(x) == clz_ans, "%d %d",                           \
               wjr::fallback_clz<type>(x), clz_ans)                                      \
    WJR_PP_BOOL_IF(                                                                      \
        WJR_HAS_BUILTIN(CTZ), ; do {                                                     \
            WJR_ASSERT((wjr::builtin_clz<type>(x) == clz_ans), "%d %d",                  \
                       wjr::builtin_clz<type>(x), clz_ans);                              \
        } while (0), );

#define WJR_TEST_PTZ_I_CALLER(args)                                                      \
    do {                                                                                 \
        WJR_TEST_POPCOUNT_I args WJR_TEST_CTZ_I args WJR_TEST_CLZ_I args                 \
    } while (0);

    WJR_TEST_PTZ(((uint8_t, 0x00, 0), (uint8_t, 0x01, 1), (uint8_t, 0x02, 1),
                  (uint8_t, 0x03, 2), (uint8_t, 0x04, 1), (uint8_t, 0x0a, 2),
                  (uint8_t, 0x0b, 3), (uint8_t, 0x0c, 2), (uint8_t, 0x0f, 4),
                  (uint8_t, 0x17, 4), (uint8_t, 0xff, 8)));

    WJR_TEST_PTZ(((uint16_t, 0x0000, 0), (uint16_t, 0x0001, 1), (uint16_t, 0x0002, 1),
                  (uint16_t, 0x0003, 2), (uint16_t, 0x0004, 1), (uint16_t, 0x000a, 2),
                  (uint16_t, 0x000b, 3), (uint16_t, 0x000c, 2), (uint16_t, 0x000f, 4),
                  (uint16_t, 0x0017, 4), (uint16_t, 0x00ff, 8), (uint16_t, 0x0a0f, 6),
                  (uint16_t, 0xffff, 16)));

    WJR_TEST_PTZ(((uint32_t, 0x0000'0000, 0), (uint32_t, 0x0000'0001, 1),
                  (uint32_t, 0x0000'0002, 1), (uint32_t, 0x0000'0003, 2),
                  (uint32_t, 0x0000'0004, 1), (uint32_t, 0x0000'000a, 2),
                  (uint32_t, 0x0000'000b, 3), (uint32_t, 0x0000'000c, 2)));

    WJR_TEST_PTZ(((uint32_t, 0x0000'000f, 4), (uint32_t, 0x0000'0013, 3),
                  (uint32_t, 0x0000'0017, 4), (uint32_t, 0x0000'00ff, 8),
                  (uint32_t, 0x0000'00cf, 6), (uint32_t, 0x0001'00cd, 6),
                  (uint32_t, 0x0100'0000, 1), (uint32_t, 0x0200'0000, 1),
                  (uint32_t, 0x0300'0000, 2), (uint32_t, 0x0400'0000, 1),
                  (uint32_t, 0xffff'ffff, 32)));

    WJR_TEST_PTZ(((uint64_t, 0x0000'0000, 0), (uint64_t, 0x0000'0001, 1),
                  (uint64_t, 0x0000'0002, 1), (uint64_t, 0x0000'0003, 2),
                  (uint64_t, 0x0000'0004, 1), (uint64_t, 0x0000'000a, 2),
                  (uint64_t, 0x0000'000b, 3), (uint64_t, 0x0000'000c, 2)));

    WJR_TEST_PTZ(((uint64_t, 0x0000'000f, 4), (uint64_t, 0x0000'0013, 3),
                  (uint64_t, 0x0000'0017, 4), (uint64_t, 0x0000'00ff, 8),
                  (uint64_t, 0x0000'00cf, 6), (uint64_t, 0x0001'00cd, 6),
                  (uint64_t, 0x0100'0000, 1), (uint64_t, 0x0200'0000, 1),
                  (uint64_t, 0x0300'0000, 2), (uint64_t, 0x0400'0000, 1),
                  (uint64_t, 0xffff'ffff, 32)));

    WJR_TEST_PTZ(
        ((uint64_t, 0x0000'0000'0000'0000, 0), (uint64_t, 0x0000'0001'0000'000, 1),
         (uint64_t, 0x0000'0002'0000'000, 1), (uint64_t, 0x0000'0003'0000'000, 2),
         (uint64_t, 0x0000'0004'0000'000, 1), (uint64_t, 0x0000'000a'0000'000, 2),
         (uint64_t, 0x0000'000b'0000'000, 3), (uint64_t, 0x0000'000c'0000'000, 2),
         (uint64_t, 0x0000'000f'0000'000, 4), (uint64_t, 0x0000'0013'0000'000, 3)));

    WJR_TEST_PTZ(
        ((uint64_t, 0x0000'0017'0000'000, 4), (uint64_t, 0x0000'00ff'0000'000, 8),
         (uint64_t, 0x0000'00cf'0000'000, 6), (uint64_t, 0x0001'00cd'0000'000, 6),
         (uint64_t, 0x0100'0000'0000'000, 1), (uint64_t, 0x0200'0000'0000'000, 1),
         (uint64_t, 0x0300'0000'0000'000, 2), (uint64_t, 0x0400'0000'0000'000, 1),
         (uint64_t, 0xffff'ffff'0000'000, 32), (uint64_t, 0xffff'ffff'ffff'ffff, 64)));

#undef WJR_TEST_PTZ_I_CALLER
#undef WJR_TEST_CLZ_I
#undef WJR_TEST_CTZ_I
#undef WJR_TEST_POPCOUNT_I
#undef WJR_TEST_PTZ
}

TEST(math, add) {

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
                       if (ci == 1) {                                                    \
                           WJR_ASSERT((wjr::asm_addc_1<type, type>(x, y, co) == ans &&   \
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

#define WJR_TEST_ADDC_1(queue) WJR_PP_TRANSFORM_PUT(queue, WJR_TEST_ADDC_1_I_CALLER)
#define WJR_TEST_ADDC_1_EXPAND(args) WJR_TEST_ADDC_1_EXPAND_I args
#define WJR_TEST_ADDC_1_EXPAND_I(...) __VA_ARGS__
#define WJR_TEST_ADDC_1_I(type, inputs, c, c_in, outputs, ans)                           \
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
        WJR_ASSERT((wjr::addc_1<type, type>(out, in, N, c, c_in) == ans));               \
        WJR_ASSERT((memcmp(out, expect, sizeof(out)) == 0),                              \
                   "uncorrect array of addc_1<%s, %s>, different array", #type, #type);  \
        WJR_ASSERT((wjr::addc_1<type, type>(in, in, N, c, c_in) == ans));                \
        WJR_ASSERT((memcmp(in, expect, sizeof(out)) == 0),                               \
                   "uncorrect array of addc_1<%s, %s>, same array", #type, #type);       \
    }
#define WJR_TEST_ADDC_1_I_CALLER(args) WJR_TEST_ADDC_1_I args

    WJR_TEST_ADDC_1(((uint64_t, (1, 2, 3), 0, 0, (1, 2, 3), 0),
                     (uint64_t, (1, 2, 3), 1, 0, (2, 2, 3), 0),
                     (uint64_t, (1, 2, 3), -1, 0, (0, 3, 3), 0),
                     (uint64_t, (1, 2, 3), -2, 1, (0, 3, 3), 0)));

    WJR_TEST_ADDC_1(((uint64_t, (-2, 2, 3), 1, 1, (0, 3, 3), 0),
                     (uint64_t, (-1, 2, 3), 1, 0, (0, 3, 3), 0),
                     (uint64_t, (-1, -1, 3), 1, 0, (0, 0, 4), 0),
                     (uint64_t, (-1, -1, -1), 1, 0, (0, 0, 0), 1),
                     (uint64_t, (-5, -1, -1), -5, 1, (-9, 0, 0), 1)));

#undef WJR_TEST_ADDC_1_I_CALLER
#undef WJR_TEST_ADDC_1_I
#undef WJR_TEST_ADDC_1_EXPAND_I
#undef WJR_TEST_ADDC_1_EXPAND
#undef WJR_TEST_ADDC_1

    {
        std::vector<uint64_t> in, out;
        uint64_t add;
        std::mt19937_64 mt_rand(time(0));
        for (size_t n = 1; n <= 256; n = n + (n == 1 ? 1 : n / 2)) {
            in.resize(n);
            out.resize(n);
            for (auto &x : in) {
                x = 1;
            }

            for (uint64_t cf : {0, 1}) {
                for (size_t k : {0, 1, 2, -1}) {
                    if (!k) {
                        add = 3 - cf;
                    } else {
                        add = -1 - cf;
                    }

                    auto l = std::min(k, n);

                    for (size_t p = 1; p < l; ++p) {
                        in[p] = -1;
                    }

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
                        WJR_ASSERT(out[0] == 0);
                        for (size_t p = 1; p < l; ++p) {
                            WJR_ASSERT(out[p] == 0);
                        }
                        if (l != n) {
                            WJR_ASSERT(out[l] == 2);
                            for (size_t p = l + 1; p < n; ++p) {
                                WJR_ASSERT(out[p] == 1);
                            }
                        }
                    }

                    for (size_t p = 1; p < l; ++p) {
                        in[p] = 1;
                    }
                }
            }
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
            if (ci == 1) {                                                               \
                WJR_ASSERT(                                                              \
                    (wjr::asm_subc_1<type, type>(x, y, co) == ans && co == ans_co));     \
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