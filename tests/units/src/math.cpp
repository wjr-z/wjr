#include "detail.hpp"

#include <charconv>
#include <list>

#if defined(WJR_USE_GMP)
    #include <gmp.h>
#endif

#include <wjr/biginteger/detail.hpp>
#include <wjr/math.hpp>

using namespace wjr;

TEST(math, popcount_ctz_clz) {

#define WJR_TEST_PTZ(queue) WJR_PP_TRANSFORM_PUT(queue, WJR_TEST_PTZ_I_CALLER)

#define WJR_TEST_POPCOUNT_I(type, x, ans)                                                          \
    WJR_CHECK(fallback_popcount<type>(x) == ans)                                               \
    WJR_PP_BOOL_IF_NE(WJR_HAS_BUILTIN(POPCOUNT), ;                                                 \
                      WJR_CHECK((builtin_popcount<type>(x) == ans)), )
#define WJR_TEST_CTZ_I(type, x, ans)                                                               \
    type n = x;                                                                                    \
    auto ctz_ans = popcount<type>((type)(lowbit(n) - 1));                                          \
    WJR_CHECK((x == 0 ? std::numeric_limits<type>::digits : fallback_ctz<type>(x)) == ctz_ans) \
    WJR_PP_BOOL_IF_NE(WJR_HAS_BUILTIN(CTZ), ; WJR_CHECK((countr_zero<type>(x) == ctz_ans)), )
#define WJR_TEST_CLZ_I(type, x, ans)                                                               \
    auto clz_ans = []() -> int {                                                                   \
        type n = x;                                                                                \
        constexpr auto nd = std::numeric_limits<type>::digits;                                     \
        if (n == 0) {                                                                              \
            return nd;                                                                             \
        }                                                                                          \
        n |= n >> 1;                                                                               \
        n |= n >> 2;                                                                               \
        n |= n >> 4;                                                                               \
        if constexpr (nd >= 16) {                                                                  \
            n |= n >> 8;                                                                           \
        }                                                                                          \
        if constexpr (nd >= 32) {                                                                  \
            n |= n >> 16;                                                                          \
        }                                                                                          \
        if constexpr (nd >= 64) {                                                                  \
            n |= n >> 32;                                                                          \
        }                                                                                          \
        ++n;                                                                                       \
        return nd - countr_zero<type>(n);                                                          \
    }();                                                                                           \
    WJR_CHECK((x == 0 ? std::numeric_limits<type>::digits : fallback_clz<type>(x)) == clz_ans) \
    WJR_PP_BOOL_IF_NE(WJR_HAS_BUILTIN(CTZ), ; WJR_CHECK((countl_zero<type>(x) == clz_ans)), )

#define WJR_TEST_PTZ_I_CALLER(args)                                                                \
    do {                                                                                           \
        WJR_TEST_POPCOUNT_I args;                                                                  \
    } while (false);                                                                               \
    do {                                                                                           \
        WJR_TEST_CTZ_I args;                                                                       \
    } while (false);                                                                               \
    do {                                                                                           \
        WJR_TEST_CLZ_I args;                                                                       \
    } while (false)

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

#define WJR_TEST_ADDC(type, x, y, ci, ans, ans_co) WJR_TEST_ADDC_I(type, x, y, ci, co, ans, ans_co)
#define WJR_TEST_ADDC_I(type, x, y, ci, co, ans, ans_co)                                           \
    WJR_CHECK((math::fallback_addc<type, type>(x, y, ci, co) == ans && co == ans_co));         \
    WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC),                                                          \
                   do {                                                                            \
                       WJR_CHECK(                                                              \
                           (math::builtin_addc<type, type>(x, y, ci, co) == ans && co == ans_co)); \
                   } while (false),                                                                \
                   {});                                                                            \
    WJR_PP_BOOL_IF_NE(                                                                             \
        WJR_HAS_BUILTIN(ASM_ADDC), do {                                                            \
            if constexpr (std::is_same_v<type, uint64_t>) {                                        \
                WJR_CHECK((math::asm_addc<type>(x, y, ci, co) == ans && co == ans_co));        \
                WJR_CHECK(WJR_PP_BOOL_IF(                                                      \
                    WJR_HAS_BUILTIN(ASM_ADDC_CC),                                                  \
                    (math::asm_addc_cc(x, y, ci, co2) == ans && co2 == ans_co), true));            \
            }                                                                                      \
        } while (false), )

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_ASM_VOLATILE_TEST_ADDC_F() asm volatile("" : "+r"(c), "+r"(e)::"memory")
#else
    #define WJR_ASM_VOLATILE_TEST_ADDC_F()
#endif

#define WJR_TEST_ADDC_F(type)                                                                      \
    do {                                                                                           \
        constexpr auto maxn = std::numeric_limits<type>::max();                                    \
        type co;                                                                                   \
        uint8_t co2;                                                                               \
        WJR_TEST_ADDC(type, 0, 0, 0, 0, 0);                                                        \
        WJR_TEST_ADDC(type, 0, 0, 1, 1, 0);                                                        \
        WJR_TEST_ADDC(type, 0, maxn, 0, maxn, 0);                                                  \
        WJR_TEST_ADDC(type, 1, maxn, 0, 0, 1);                                                     \
        WJR_TEST_ADDC(type, 0, maxn, 1, 0, 1);                                                     \
        WJR_TEST_ADDC(type, 1, maxn, 1, 1, 1);                                                     \
        type c = 1;                                                                                \
        type e = 3;                                                                                \
        WJR_ASM_VOLATILE_TEST_ADDC_F();                                                            \
        WJR_TEST_ADDC(type, e, c, c, 5, 0);                                                        \
    } while (false);

    WJR_PP_TRANSFORM_PUT((unsigned char, unsigned int, unsigned long, unsigned long long),
                         WJR_TEST_ADDC_F);

#undef WJR_ASM_VOLATILE_TEST_ADDC_F

#undef WJR_TEST_ADDC_F
#undef WJR_TEST_ADDC_I
#undef WJR_TEST_ADDC
}

TEST(math, addc_1) {

    {
        std::vector<uint64_t> in, tmp, copy;
        uint64_t add;
        for (size_t n = 1; n <= 256; n = n + (n == 1 ? 1 : n / 2)) {
            in.resize(n);
            tmp.resize(n);
            for (auto &x : in) {
                x = 1;
            }

            for (auto arr : {&in, &tmp}) {
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

                        auto &out = *arr;

                        uint64_t c_out = addc_1(out.data(), in.data(), n, add, cf);

                        if (l == n && k) {
                            WJR_CHECK(c_out == 1);
                        } else {
                            WJR_CHECK(c_out == 0);
                        }

                        if (!k) {
                            WJR_CHECK(out[0] == 4);
                            for (size_t p = 1; p < n; ++p) {
                                WJR_CHECK(out[p] == 1);
                            }
                        } else {
                            for (size_t p = 0; p < l; ++p) {
                                WJR_CHECK(out[p] == 0);
                            }
                            if (l != n) {
                                WJR_CHECK(out[l] == 2);
                                for (size_t p = l + 1; p < n; ++p) {
                                    WJR_CHECK(out[p] == 1);
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

            auto cf = addc_1(y.data(), x.data(), n, 1, 0u);
            WJR_CHECK(cf == 1);
            for (auto &j : y) {
                WJR_CHECK(j == 0);
            }

            cf = addc_1(y.data(), x.data(), n, 0, 1u);
            WJR_CHECK(cf == 1);
            for (auto &j : y) {
                WJR_CHECK(j == 0);
            }

            cf = addc_1(y.data(), x.data(), n, 1, 1u);
            WJR_CHECK(cf == 1);
            WJR_CHECK(y[0] == 1);
            for (size_t j = 1; j < n; ++j) {
                WJR_CHECK(y[j] == 0);
            }

            cf = addc_1(x.data(), x.data(), n, 1, 0u);
            WJR_CHECK(cf == 1);
            for (auto &j : x) {
                WJR_CHECK(j == 0);
                j = -1;
            }

            if (n > 1) {
                cf = addc_1(x.data(), x.data() + 1, n - 1, 1, 0u);
                WJR_CHECK(cf == 1);
                for (size_t j = 0; j < n - 1; ++j) {
                    WJR_CHECK(x[j] == 0);
                }
                WJR_CHECK(x.back() == -1ull);
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

        auto cf = addc_n(c.data(), a.data(), b.data(), n, 0u);
        WJR_CHECK(cf == 1);

        for (auto &i : c) {
            WJR_CHECK(i == 0);
        }
    }
}

TEST(math, sub) {

#define WJR_TEST_SUBC(type, x, y, ci, ans, ans_co) WJR_TEST_SUBC_I(type, x, y, ci, co, ans, ans_co)
#define WJR_TEST_SUBC_I(type, x, y, ci, co, ans, ans_co)                                           \
    WJR_CHECK((math::fallback_subc<type, type>(x, y, ci, co) == ans && co == ans_co))          \
    WJR_PP_BOOL_IF(                                                                                \
        WJR_HAS_BUILTIN(SUBC), ; do {                                                              \
            WJR_CHECK((math::builtin_subc<type, type>(x, y, ci, co) == ans && co == ans_co));  \
        } while (false), )                                                                         \
    WJR_PP_BOOL_IF_NE(                                                                             \
        WJR_HAS_BUILTIN(ASM_SUBC), ; do {                                                          \
            if constexpr (std::is_same_v<type, uint64_t>) {                                        \
                WJR_CHECK((math::asm_subc<type>(x, y, ci, co) == ans && co == ans_co));        \
                WJR_CHECK(WJR_PP_BOOL_IF(                                                      \
                    WJR_HAS_BUILTIN(ASM_SUBC_CC),                                                  \
                    (math::asm_subc_cc(x, y, ci, co2) == ans && co2 == ans_co), true));            \
            }                                                                                      \
        } while (false), )

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_ASM_VOLATILE_TEST_SUBC_F() asm volatile("" : "+r"(c), "+r"(e)::"memory")
#else
    #define WJR_ASM_VOLATILE_TEST_SUBC_F()
#endif

#define WJR_TEST_SUBC_F(type)                                                                      \
    do {                                                                                           \
        constexpr auto maxn = std::numeric_limits<type>::max();                                    \
        type co;                                                                                   \
        uint8_t co2;                                                                               \
        WJR_TEST_SUBC(type, 0, 0, 0, 0, 0);                                                        \
        WJR_TEST_SUBC(type, 0, 0, 1, maxn, 1);                                                     \
        WJR_TEST_SUBC(type, 1, 0, 1, 0, 0);                                                        \
        WJR_TEST_SUBC(type, 0, maxn, 0, 1, 1);                                                     \
        WJR_TEST_SUBC(type, 1, maxn, 0, 2, 1);                                                     \
        WJR_TEST_SUBC(type, 0, maxn, 1, 0, 1);                                                     \
        WJR_TEST_SUBC(type, 1, maxn, 1, 1, 1);                                                     \
        type c = 1;                                                                                \
        type e = 3;                                                                                \
        WJR_ASM_VOLATILE_TEST_SUBC_F();                                                            \
        WJR_TEST_SUBC(type, e, c, c, 1, 0);                                                        \
    } while (false);

    WJR_PP_TRANSFORM_PUT((unsigned char, unsigned int, unsigned long, unsigned long long),
                         WJR_TEST_SUBC_F);

#undef WJR_ASM_VOLATILE_TEST_SUBC_F

#undef WJR_TEST_SUBC_F
#undef WJR_TEST_SUBC_I
#undef WJR_TEST_SUBC

#define WJR_TEST_SUBC_1(queue) WJR_PP_TRANSFORM_PUT(queue, WJR_TEST_SUBC_1_I_CALLER)
#define WJR_TEST_SUBC_1_EXPAND(args) WJR_TEST_SUBC_1_EXPAND_I args
#define WJR_TEST_SUBC_1_EXPAND_I(...) __VA_ARGS__
#define WJR_TEST_SUBC_1_I(inputs, c, c_in, outputs, ans)                                           \
    {                                                                                              \
        constexpr auto N = WJR_PP_QUEUE_SIZE(inputs);                                              \
        auto init = [](uint64_t *arr, auto... args) {                                              \
            auto il = {(args)...};                                                                 \
            for (size_t i = 0; i < il.size(); ++i) {                                               \
                arr[i] = il.begin()[i];                                                            \
            }                                                                                      \
        };                                                                                         \
        uint64_t in[N];                                                                            \
        uint64_t out[N];                                                                           \
        uint64_t expect[N];                                                                        \
        init(in, WJR_PP_QUEUE_EXPAND(inputs));                                                     \
        init(expect, WJR_PP_QUEUE_EXPAND(outputs));                                                \
        WJR_CHECK((subc_1(out, in, N, c, c_in) == ans));                                       \
        WJR_CHECK((memcmp(out, expect, sizeof(out)) == 0),                                     \
                      "uncorrect array of subc_1<uint64_t>, different array");                     \
        WJR_CHECK((subc_1(in, in, N, c, c_in) == ans));                                        \
        WJR_CHECK((memcmp(in, expect, sizeof(out)) == 0),                                      \
                      "uncorrect array of subc_1<uint64_t>, same array");                          \
    }
#define WJR_TEST_SUBC_1_I_CALLER(args) WJR_TEST_SUBC_1_I args

    WJR_TEST_SUBC_1((((1, 2, 3), 0, 0, (1, 2, 3), 0), ((1, 2, 3), 1, 0, (0, 2, 3), 0),
                     ((0, 2, 3), 1, 0, (-1, 1, 3), 0), ((1, 2, 3), 1, 1, (-1, 1, 3), 0)));

    WJR_TEST_SUBC_1((((2, 2, 3), 1, 1, (0, 2, 3), 0), ((1, 0, 3), 1, 1, (-1, -1, 2), 0),
                     ((0, 0, 0), 3, 1, (-4, -1, -1), 1), ((5, 0, 0), 10, 1, (-6, -1, -1), 1)));

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
        for (size_t n = 1; n <= 256; n = n + (n == 1 ? 1 : n / 2)) {
            in.resize(n);
            tmp.resize(n);
            for (auto &x : in) {
                x = 4;
            }

            for (auto arr : {&in, &tmp}) {
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

                        auto &out = *arr;

                        uint64_t c_out = subc_1(out.data(), in.data(), n, sub, cf);

                        if (l == n && k) {
                            WJR_CHECK(c_out == 1);
                        } else {
                            WJR_CHECK(c_out == 0);
                        }

                        if (!k) {
                            WJR_CHECK(out[0] == 2);
                            for (size_t p = 1; p < n; ++p) {
                                WJR_CHECK(out[p] == 4);
                            }
                        } else {
                            for (size_t p = 0; p < l; ++p) {
                                WJR_CHECK(out[p] == -1ull);
                            }
                            if (l != n) {
                                WJR_CHECK(out[l] == 3);
                                for (size_t p = l + 1; p < n; ++p) {
                                    WJR_CHECK(out[p] == 4);
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

            auto cf = subc_1(y.data(), x.data(), n, 1, 0u);
            WJR_CHECK(cf == 1);
            for (auto &j : y) {
                WJR_CHECK(j == -1ull);
            }

            cf = subc_1(y.data(), x.data(), n, 0, 1u);
            WJR_CHECK(cf == 1);
            for (auto &j : y) {
                WJR_CHECK(j == -1ull);
            }

            cf = subc_1(y.data(), x.data(), n, 1, 1u);
            WJR_CHECK(cf == 1);
            WJR_CHECK(y[0] == -2ull);
            for (size_t j = 1; j < n; ++j) {
                WJR_CHECK(y[j] == -1ull);
            }

            cf = subc_1(x.data(), x.data(), n, 1, 0u);
            WJR_CHECK(cf == 1);
            for (auto &j : x) {
                WJR_CHECK(j == -1ull);
                j = 0;
            }

            if (n > 1) {
                cf = subc_1(x.data(), x.data() + 1, n - 1, 1, 0u);
                WJR_CHECK(cf == 1);
                for (size_t j = 0; j < n - 1; ++j) {
                    WJR_CHECK(x[j] == -1ull);
                }
                WJR_CHECK(x.back() == 0);
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

        auto cf = subc_n(c.data(), a.data(), b.data(), n, 0u);
        WJR_CHECK(cf == 1);

        for (auto &i : c) {
            WJR_CHECK(i == -1ull);
        }
    }
}

TEST(math, broadcast) {
#define WJR_TEST_BROADCAST(ft, tt, x, expect) WJR_CHECK((broadcast<ft, tt>(x) == expect))

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

            auto idx = find_n(a.data(), 0, n);

            WJR_CHECK(idx == m);

            idx = find_n(a.data(), b.data(), n);

            WJR_CHECK(idx == m);

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

            auto idx = find_not_n(a.data(), 0, n);

            WJR_CHECK(idx == m);

            idx = find_not_n(a.data(), b.data(), n);

            WJR_CHECK(idx == m);

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

            auto idx = reverse_find_n(a.data(), 0, n);

            WJR_CHECK(idx == n - m);

            idx = reverse_find_n(a.data(), b.data(), n);

            WJR_CHECK(idx == n - m);

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

            auto idx = reverse_find_not_n(a.data(), 0, n);

            WJR_CHECK(idx == n - m);

            idx = reverse_find_not_n(a.data(), b.data(), n);

            WJR_CHECK(idx == n - m);

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

                auto idx = replace_find_not(b.data(), a.data(), n, 0, 1);

                WJR_CHECK(idx == m);

                for (size_t i = 0; i < m; ++i) {
                    WJR_CHECK(b[i] == 1);
                }

                if (n != m) {
                    for (size_t i = m; i < n; ++i) {
                        WJR_CHECK(b[i] == 0);
                    }
                }

                idx = replace_find_not(a.data(), a.data(), n, 0, 0);

                WJR_CHECK(idx == m);

                for (size_t i = 0; i < m; ++i) {
                    WJR_CHECK(a[i] == 0);
                }

                if (n != m) {
                    WJR_CHECK(a[m] == -1ull);
                    for (size_t i = m + 1; i < n; ++i) {
                        WJR_CHECK(a[i] == 0);
                    }
                }

                idx = replace_find_not(a.data(), a.data(), n, 0, 1);

                WJR_CHECK(idx == m);

                for (size_t i = 0; i < m; ++i) {
                    WJR_CHECK(a[i] == 1);
                }

                if (n != m) {
                    WJR_CHECK(a[m] == -1ull);
                    for (size_t i = m + 1; i < n; ++i) {
                        WJR_CHECK(a[i] == 0);
                    }
                }
            }
        }
    }
}

TEST(math, bi_not_n) {
    std::vector<uint64_t> a, b;
    for (size_t n = 0; n <= 384; ++n) {
        a.resize(n);
        b.resize(n);
        for (auto &i : a) {
            i = 0;
        }

        math::bi_not_n(b.data(), a.data(), n);
        for (auto &i : b) {
            WJR_CHECK(i == -1ull);
        }

        math::bi_not_n(a.data(), a.data(), n);
        for (auto &i : a) {
            WJR_CHECK(i == -1ull);
        }
    }
}

TEST(math, bi_negate_n) {
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

            bool zero = math::bi_negate_n(b.data(), a.data(), n);

            if (n == m) {
                WJR_CHECK(zero);
            } else {
                WJR_CHECK(!zero);
            }

            for (size_t i = 0; i < m; ++i) {
                WJR_CHECK(b[i] == 0);
            }

            if (n != m) {
                WJR_CHECK(b[m] == -xx);
                for (size_t i = m + 1; i < n; ++i) {
                    WJR_CHECK(b[i] == ~c[i]);
                }
            }

            auto cf = [&]() -> uint64_t {
                if (!n) {
                    return 0u;
                }
                return addc_n(c.data(), a.data(), b.data(), n, 0u);
            }();

            if (n != m) {
                WJR_CHECK(cf);
            } else {
                WJR_CHECK(!cf);
            }

            for (size_t i = 0; i < n; ++i) {
                WJR_CHECK(c[i] == 0);
            }
        }
    }
}

TEST(math, set_n) {
    auto test = [](auto x) {
        using type = decltype(x);
        std::vector<type> a;
        for (size_t n = 0; n <= 384; ++n) {
            a.resize(n);
            type b = mt_rand();

            std::fill(a.begin(), a.end(), 0);
            set_n(a.data(), b, n);

            for (auto &i : a) {
                WJR_CHECK(i == b);
            }
        }

        for (size_t n = 512; n <= (1ull << 24); n *= 2) {
            a.resize(n);
            type b = mt_rand();

            std::fill(a.begin(), a.end(), 0);
            set_n(a.data(), b, n);

            for (auto &i : a) {
                WJR_CHECK(i == b);
            }
        }
    };

    test((uint16_t)0);
    test((uint32_t)0);
    test((uint64_t)0);
}

TEST(math, compare_n) {
    {
        std::vector<uint64_t> a = {0, 1ull << 31, 0, 0};
        std::vector<uint64_t> b = {0, 0, 0, 0};
        WJR_CHECK(compare_n(a.data(), b.data(), 4) > 0);
    }

    std::vector<uint64_t> a, b;
    for (size_t n = 0; n <= 64; ++n) {
        a.resize(n);
        for (size_t m = 0; m <= n; ++m) {
            for (auto &i : a) {
                i = mt_rand();
            }

            b = a;

            if (m != n) {
                while (a[m] == -1ull) {
                    a[m] = mt_rand();
                }

                b[m] = a[m] + 1;
            }

            int f = compare_n(a.data(), b.data(), n);
            if (n == m) {
                WJR_CHECK(f == 0);
                continue;
            }

            WJR_CHECK(f < 0);

            if (n != m + 1) {
                for (size_t j = m + 1; j < n; ++j) {
                    a[j] = mt_rand();
                }
                f = compare_n(a.data(), b.data(), n);
                WJR_CHECK(f < 0);
            }

            while (a[m] == 0) {
                a[m] = mt_rand();
            }
            b[m] = a[m] - 1;

            f = compare_n(a.data(), b.data(), n);

            WJR_CHECK(f > 0);

            if (n != m + 1) {
                for (size_t j = m + 1; j < n; ++j) {
                    a[j] = mt_rand();
                }
                f = compare_n(a.data(), b.data(), n);
                WJR_CHECK(f > 0);
            }
        }
    }
}

TEST(math, reverse_compare_n) {
    std::vector<uint64_t> a, b;
    for (size_t n = 0; n <= 64; ++n) {
        a.resize(n);
        for (size_t m = 0; m <= n; ++m) {
            for (auto &i : a) {
                i = mt_rand();
            }

            b = a;

            if (m != n) {
                while (a[n - m - 1] == -1ull) {
                    a[n - m - 1] = mt_rand();
                }
                b[n - m - 1] = a[n - m - 1] + 1;
            }

            int f = reverse_compare_n(a.data(), b.data(), n);
            if (n == m) {
                WJR_CHECK(f == 0);
                continue;
            }

            WJR_CHECK(f < 0);

            if (n != m + 1) {
                for (size_t j = 0; j < n - m - 1; ++j) {
                    a[j] = mt_rand();
                }
                f = reverse_compare_n(a.data(), b.data(), n);
                WJR_CHECK(f < 0);
            }

            while (a[n - m - 1] == 0) {
                a[n - m - 1] = mt_rand();
            }
            b[n - m - 1] = a[n - m - 1] - 1;

            f = reverse_compare_n(a.data(), b.data(), n);

            WJR_CHECK(f > 0);

            if (n != m + 1) {
                for (size_t j = 0; j < n - m - 1; ++j) {
                    a[j] = mt_rand();
                }
                f = reverse_compare_n(a.data(), b.data(), n);
                WJR_CHECK(f > 0);
            }
        }
    }
}

TEST(math, shld) {
#define WJR_TEST_SHLD(hi, lo, c, expect) WJR_CHECK(shld<uint64_t>((hi), (lo), (c)) == (expect))

    WJR_TEST_SHLD(0, 0, 1, 0);
    WJR_TEST_SHLD(0, 1, 1, 0);
    WJR_TEST_SHLD(0, 1ull << 63, 1, 1);
    WJR_TEST_SHLD(1, 1, 1, 2);
    WJR_TEST_SHLD(1, 1ull << 63, 1, 3);

#undef WJR_TEST_SHLD
}

TEST(math, shrd) {
#define WJR_TEST_SHRD(lo, hi, c, expect) WJR_CHECK(shrd<uint64_t>((lo), (hi), (c)) == (expect))

    WJR_TEST_SHRD(0, 0, 1, 0);
    WJR_TEST_SHRD(0, 1, 1, 1ull << 63);
    WJR_TEST_SHRD(1, 1, 1, 1ull << 63);
    WJR_TEST_SHRD(1, 1, 2, 1ull << 62);
    WJR_TEST_SHRD(1, 1, 63, 2);

#undef WJR_TEST_SHRD
}

TEST(math, lshift_n) {
    std::vector<uint64_t> a, b;

    for (size_t n = 1; n <= 512; ++n) {
        a.resize(n);
        b.resize(n);

        for (size_t i = 0; i < n; ++i) {
            a[i] = mt_rand();
        }

        for (unsigned int c = 0; c < 64; ++c) {
            uint64_t ex = c ? (a[n - 1] >> (64 - c)) : 0;
            auto z = lshift_n(b.data(), a.data(), n, c);
            WJR_CHECK(z == ex);

            for (size_t i = 1; i < n; ++i) {
                ex = c ? ((a[i] << c) | (a[i - 1] >> (64 - c))) : a[i];
                WJR_CHECK(b[i] == ex);
            }

            ex = a[0] << c;

            WJR_CHECK(b[0] == ex);
        }
    }
}

TEST(math, rshift_n) {
    std::vector<uint64_t> a, b;

    for (size_t n = 1; n <= 512; ++n) {
        a.resize(n);
        b.resize(n);

        for (size_t i = 0; i < n; ++i) {
            a[i] = mt_rand();
        }

        for (unsigned int c = 0; c < 64; ++c) {
            uint64_t ex = c ? (a[0] << (64 - c)) : 0;
            auto z = rshift_n(b.data(), a.data(), n, c);
            WJR_CHECK(z == ex);

            for (size_t i = 0; i < n - 1; ++i) {
                ex = c ? ((a[i] >> c) | (a[i + 1] << (64 - c))) : a[i];
                WJR_CHECK(b[i] == ex);
            }

            ex = a[n - 1] >> c;

            WJR_CHECK(b[n - 1] == ex);
        }
    }
}

TEST(math_detail, default_stack_allocator) {}

TEST(math, add_128) {
    auto check = [](uint64_t lo0, uint64_t hi0, uint64_t lo1, uint64_t hi1, uint64_t anslo,
                    uint64_t anshi) {
        add_128(lo0, hi0, lo0, hi0, lo1, hi1);
        WJR_CHECK(lo0 == anslo);
        WJR_CHECK(hi0 == anshi);
    };

    check(0, 0, 0, 0, 0, 0);
    check(1, 0, 0, 0, 1, 0);
    check(0, 0, 1, 0, 1, 0);
    check(1, 1, 0, 0, 1, 1);
    check(0, 1, 0, 0, 0, 1);
    check(0, 0, 0, 1, 0, 1);
    check(UINT64_MAX, 0, 0, 0, UINT64_MAX, 0);
    check(0, 0, UINT64_MAX, 0, UINT64_MAX, 0);
    check(1, 0, UINT64_MAX, 0, 0, 1);
    check(UINT64_MAX, 0, 1, 0, 0, 1);
    check(UINT64_MAX - 1, UINT64_MAX - 1, 1, 1, UINT64_MAX, UINT64_MAX);
    check(UINT64_MAX, 0, UINT64_MAX, 1, UINT64_MAX - 1, 2);
    check(1, 1, 1, 1, 2, 2);
    check(UINT64_MAX, UINT64_MAX, 0, 1, UINT64_MAX, 0);
    check(0, 1, 0, UINT64_MAX, 0, 0);
    check(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX - 1, UINT64_MAX);
}

TEST(math, __addc_128) {
    auto check = [](uint64_t lo0, uint64_t hi0, uint64_t lo1, uint64_t hi1, uint64_t c,
                    uint64_t anslo, uint64_t anshi, uint64_t ansc) {
        auto cf = math::__addc_128(lo0, hi0, lo0, hi0, lo1, hi1, c);
        WJR_CHECK(lo0 == anslo);
        WJR_CHECK(hi0 == anshi);
        WJR_CHECK(cf == ansc);
    };

    check(0, 0, 0, 0, 0, 0, 0, 0);
    check(1, 0, 0, 0, 0, 1, 0, 0);
    check(0, 0, 1, 0, 0, 1, 0, 0);
    check(1, 1, 0, 0, 0, 1, 1, 0);
    check(0, 1, 0, 0, 0, 0, 1, 0);
    check(0, 0, 0, 1, 0, 0, 1, 0);
    check(UINT64_MAX, 0, 0, 0, 0, UINT64_MAX, 0, 0);
    check(0, 0, UINT64_MAX, 0, 0, UINT64_MAX, 0, 0);
    check(1, 0, UINT64_MAX, 0, 0, 0, 1, 0);
    check(UINT64_MAX, 0, 1, 0, 0, 0, 1, 0);
    check(UINT64_MAX - 1, UINT64_MAX - 1, 1, 1, 0, UINT64_MAX, UINT64_MAX, 0);
    check(UINT64_MAX, 0, UINT64_MAX, 1, 0, UINT64_MAX - 1, 2, 0);
    check(1, 1, 1, 1, 0, 2, 2, 0);
    check(UINT64_MAX, UINT64_MAX, 0, 1, 0, UINT64_MAX, 0, 1);
    check(0, 1, 0, UINT64_MAX, 0, 0, 0, 1);
    check(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, 0, UINT64_MAX - 1, UINT64_MAX, 1);

    check(0, 0, 0, 0, 1, 1, 0, 0);
    check(1, 0, 0, 0, 1, 2, 0, 0);
    check(0, 0, 1, 0, 1, 2, 0, 0);
    check(1, 1, 0, 0, 1, 2, 1, 0);
    check(0, 1, 0, 0, 1, 1, 1, 0);
    check(0, 0, 0, 1, 1, 1, 1, 0);
    check(UINT64_MAX, 0, 0, 0, 1, 0, 1, 0);
    check(0, 0, UINT64_MAX, 0, 1, 0, 1, 0);
    check(1, 0, UINT64_MAX, 0, 1, 1, 1, 0);
    check(UINT64_MAX, 0, 1, 0, 1, 1, 1, 0);
    check(UINT64_MAX - 1, UINT64_MAX - 1, 1, 1, 1, 0, 0, 1);
    check(UINT64_MAX, 0, UINT64_MAX, 1, 1, UINT64_MAX, 2, 0);
    check(1, 1, 1, 1, 1, 3, 2, 0);
    check(UINT64_MAX, UINT64_MAX, 0, 1, 1, 0, 1, 1);
    check(0, 1, 0, UINT64_MAX, 1, 1, 0, 1);
    check(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, 1, UINT64_MAX, UINT64_MAX, 1);
}

TEST(math, __sub_128) {
    auto check = [](uint64_t lo0, uint64_t hi0, uint64_t lo1, uint64_t hi1, uint64_t anslo,
                    uint64_t anshi) {
        sub_128(lo0, hi0, lo0, hi0, lo1, hi1);
        WJR_CHECK(lo0 == anslo);
        WJR_CHECK(hi0 == anshi);
    };

    check(0, 0, 0, 0, 0, 0);
    check(1, 0, 0, 0, 1, 0);
    check(0, 0, 1, 0, UINT64_MAX, UINT64_MAX);
    check(1, 1, 0, 0, 1, 1);
    check(0, 1, 0, 0, 0, 1);
    check(0, 0, 0, 1, 0, UINT64_MAX);
    check(UINT64_MAX, 0, 0, 0, UINT64_MAX, 0);
    check(0, 0, UINT64_MAX, 0, 1, UINT64_MAX);
    check(1, 0, UINT64_MAX, 0, 2, UINT64_MAX);
    check(UINT64_MAX, 0, 1, 0, UINT64_MAX - 1, 0);
    check(UINT64_MAX - 1, UINT64_MAX - 1, 1, 1, UINT64_MAX - 2, UINT64_MAX - 2);
    check(UINT64_MAX, 0, UINT64_MAX, 1, 0, UINT64_MAX);
}

TEST(math, __subc_128) {
    auto check = [](uint64_t lo0, uint64_t hi0, uint64_t lo1, uint64_t hi1, uint64_t c,
                    uint64_t anslo, uint64_t anshi, uint64_t ansc) {
        auto cf = math::__subc_128(lo0, hi0, lo0, hi0, lo1, hi1, c);
        WJR_CHECK(lo0 == anslo);
        WJR_CHECK(hi0 == anshi);
        WJR_CHECK(cf == ansc);
    };

    check(0, 0, 0, 0, 0, 0, 0, 0);
    check(1, 0, 0, 0, 0, 1, 0, 0);
    check(0, 0, 1, 0, 0, UINT64_MAX, UINT64_MAX, 1);
    check(1, 1, 0, 0, 0, 1, 1, 0);
    check(0, 1, 0, 0, 0, 0, 1, 0);
    check(0, 0, 0, 1, 0, 0, UINT64_MAX, 1);
    check(UINT64_MAX, 0, 0, 0, 0, UINT64_MAX, 0, 0);
    check(0, 0, UINT64_MAX, 0, 0, 1, UINT64_MAX, 1);
    check(1, 0, UINT64_MAX, 0, 0, 2, UINT64_MAX, 1);
    check(UINT64_MAX, 0, 1, 0, 0, UINT64_MAX - 1, 0, 0);
    check(UINT64_MAX - 1, UINT64_MAX - 1, 1, 1, 0, UINT64_MAX - 2, UINT64_MAX - 2, 0);
    check(UINT64_MAX, 0, UINT64_MAX, 1, 0, 0, UINT64_MAX, 1);

    check(0, 0, 0, 0, 1, UINT64_MAX, UINT64_MAX, 1);
    check(1, 0, 0, 0, 1, 0, 0, 0);
    check(0, 0, 1, 0, 1, UINT64_MAX - 1, UINT64_MAX, 1);
    check(1, 1, 0, 0, 1, 0, 1, 0);
    check(0, 1, 0, 0, 1, UINT64_MAX, 0, 0);
    check(0, 0, 0, 1, 1, UINT64_MAX, UINT64_MAX - 1, 1);
    check(UINT64_MAX, 0, 0, 0, 1, UINT64_MAX - 1, 0, 0);
    check(0, 0, UINT64_MAX, 0, 1, 0, UINT64_MAX, 1);
    check(1, 0, UINT64_MAX, 0, 1, 1, UINT64_MAX, 1);
    check(UINT64_MAX, 0, 1, 0, 1, UINT64_MAX - 2, 0, 0);
    check(UINT64_MAX - 1, UINT64_MAX - 1, 1, 1, 1, UINT64_MAX - 3, UINT64_MAX - 2, 0);
    check(UINT64_MAX, 0, UINT64_MAX, 1, 1, UINT64_MAX, UINT64_MAX - 1, 1);
}

TEST(math, mul_128) {

    const int T = 256;
    for (int i = 0; i < T; ++i) {
        uint64_t x = mt_rand();
        uint64_t y = mt_rand();

        uint64_t anslo, anshi;
        anslo = fallback_mul64(x, y, anshi);

        uint64_t lo, hi;
        lo = mul(x, y, hi);
        WJR_CHECK(lo == anslo);
        WJR_CHECK(hi == anshi);
    }
}

#if defined(WJR_USE_GMP)

TEST(math, mul_1) {

    const int T = 64;
    const int N = 240;

    std::vector<uint64_t> a(N), b(N), c(N);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; ++j) {
            uint64_t ml = mt_rand();

            for (int k = 0; k < j; ++k) {
                a[k] = mt_rand();
            }

            auto cf = mul_1(b.data(), a.data(), j, ml);
            auto cf2 = mpn_mul_1(c.data(), a.data(), j, ml);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(b.begin(), b.begin() + j, c.begin()));
        }
    }
}

TEST(math, addmul_1) {

    const int T = 64;
    const int N = 240;

    std::vector<uint64_t> a(N), b(N), c(N);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; ++j) {
            uint64_t ml = mt_rand();

            for (int k = 0; k < j; ++k) {
                a[k] = mt_rand();
            }

            auto cf = addmul_1(b.data(), a.data(), j, ml);
            auto cf2 = mpn_addmul_1(c.data(), a.data(), j, ml);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(b.begin(), b.begin() + j, c.begin()));
        }
    }
}

TEST(math, submul_1) {

    const int T = 64;
    const int N = 240;

    std::vector<uint64_t> a(N), b(N), c(N);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; ++j) {
            uint64_t ml = mt_rand();

            for (int k = 0; k < j; ++k) {
                a[k] = mt_rand();
            }

            auto cf = submul_1(b.data(), a.data(), j, ml);
            auto cf2 = mpn_submul_1(c.data(), a.data(), j, ml);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(b.begin(), b.begin() + j, c.begin()));
        }
    }
}

TEST(math, addlsh_n) {

    const int T = 64;
    const int N = 240;

    std::vector<uint64_t> a(N), b(N), c(N), d(N), e(N);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; ++j) {
            unsigned int cl = mt_rand() % 64;

            for (int k = 0; k < j; ++k) {
                a[k] = mt_rand();
            }

            for (int k = 0; k < j; ++k) {
                b[k] = mt_rand();
            }

            // c = a + (b << cl)
            // d = a + (b << cl)

            auto cf = addlsh_n(c.data(), a.data(), b.data(), j, cl);
            auto cf2 = lshift_n(e.data(), b.data(), j, cl);
            cf2 += addc_n(d.data(), a.data(), e.data(), j);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(c.begin(), c.begin() + j, d.begin()));

            // c = b + (c << cl)
            // d = b + (d << cl)

            cf = addlsh_n(c.data(), b.data(), c.data(), j, cl);
            cf2 = lshift_n(e.data(), d.data(), j, cl);
            cf2 += addc_n(d.data(), b.data(), e.data(), j);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(c.begin(), c.begin() + j, d.begin()));
        }
    }
}

TEST(math, rsblsh_n) {

    const int T = 64;
    const int N = 240;

    std::vector<uint64_t> a(N), b(N), c(N), d(N), e(N);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; ++j) {
            unsigned int cl = mt_rand() % 64;

            for (int k = 0; k < j; ++k) {
                a[k] = mt_rand();
            }

            for (int k = 0; k < j; ++k) {
                b[k] = mt_rand();
            }

            // c = (b << cl) - a
            // d = (b << cl) - a

            auto cf = rsblsh_n(c.data(), a.data(), b.data(), j, cl);
            auto cf2 = lshift_n(e.data(), b.data(), j, cl);
            cf2 -= subc_n(d.data(), e.data(), a.data(), j);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(c.begin(), c.begin() + j, d.begin()));

            // c = (c << cl) - b
            // d = (d << cl) - b

            cf = rsblsh_n(c.data(), b.data(), c.data(), j, cl);
            cf2 = lshift_n(e.data(), d.data(), j, cl);
            cf2 -= subc_n(d.data(), e.data(), b.data(), j);

            WJR_CHECK(cf == cf2);
            WJR_CHECK(std::equal(c.begin(), c.begin() + j, d.begin()));
        }
    }
}

template <uint64_t c>
constexpr bool __test_divexact_fast() {
    constexpr auto ss = __divexact_init<c>();
    return ss.mode == 2 && ss.cl == 0;
}

static_assert(__test_divexact_fast<3>(), "");
static_assert(__test_divexact_fast<5>(), "");
static_assert(__test_divexact_fast<15>(), "");
static_assert(__test_divexact_fast<17>(), "");

TEST(math, mul_s) {

    const int T = 4;
    const int N = 2400;
    std::vector<uint64_t> a(N), b(N), c(N * 2), d(N * 2);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; j += (j <= 180 ? 1 : j / 7)) {
            for (int k = 1; k <= j; k += (k <= 180 ? 1 : k / 7)) {
                for (int p = 0; p < j; ++p) {
                    a[p] = mt_rand();
                }
                for (int p = 0; p < k; ++p) {
                    b[p] = mt_rand();
                }

                mul_s(c.data(), a.data(), j, b.data(), k);
                mpn_mul(d.data(), a.data(), j, b.data(), k);
                WJR_CHECK(std::equal(c.begin(), c.begin() + j + k, d.begin()));
            }
        }
    }
}

TEST(math, mul_n) {

    const int T = 16;
    const int N = 2400;
    std::vector<uint64_t> a(N), b(N), c(N * 2), d(N * 2);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; j += (j <= 180 ? 1 : j / 7)) {
            for (int p = 0; p < j; ++p) {
                a[p] = mt_rand();
            }
            for (int p = 0; p < j; ++p) {
                b[p] = mt_rand();
            }

            mul_n(c.data(), a.data(), b.data(), j);
            mpn_mul_n(d.data(), a.data(), b.data(), j);
            WJR_CHECK(std::equal(c.begin(), c.begin() + j * 2, d.begin()));
        }
    }
}

TEST(math, sqr) {

    const int T = 16;
    const int N = 2400;
    std::vector<uint64_t> a(N), b(N * 2), c(N * 2);

    for (int i = 0; i < T; ++i) {
        for (int j = 1; j < N; j += (j <= 180 ? 1 : j / 7)) {
            for (int p = 0; p < j; ++p) {
                a[p] = mt_rand();
            }

            sqr(b.data(), a.data(), j);
            mpn_sqr(c.data(), a.data(), j);
            WJR_CHECK(std::equal(b.begin(), b.begin() + j * 2, c.begin()));
        }
    }
}

#endif

TEST(math, div_qr_1) {

    const int T = 8;
    const int N = 32;
    const int M = 2400;

    std::vector<uint64_t> a(M), b(M), c(M);

    auto check = [&](size_t n, uint64_t div, auto divc) {
        uint64_t rem;
        div_qr_1(b.data(), rem, a.data(), n, divc);
        auto cf = mul_1(c.data(), b.data(), n, div);
        WJR_CHECK(cf == 0);
        cf = addc_1(c.data(), c.data(), n, rem);
        WJR_CHECK(cf == 0);
        WJR_CHECK(std::equal(a.begin(), a.begin() + n, c.begin()));
    };

    for (int i = 0; i < N; ++i) {
        const uint64_t div = mt_rand();
        div2by1_divider<uint64_t> divc(div);

        for (int t = 0; t < T; ++t) {
            for (int j = 1; j < M; j += (j <= 180 ? 1 : j / 7)) {
                for (int k = 0; k < j; ++k) {
                    a[k] = mt_rand();
                }

                check(j, div, div);
                check(j, div, divc);
            }
        }
    }
}

TEST(math, div_qr_2) {

    const int T = 8;
    const int N = 32;
    const int M = 2400;

    std::vector<uint64_t> a(M), b(M), c(M + 1);

    auto check = [&](size_t n, uint64_t *div) {
        uint64_t rem[2];
        div_qr_2(b.data(), rem, a.data(), n, div);

        if (n - 1 >= 2) {
            mul_s(c.data(), b.data(), n - 1, div, 2);
        } else {
            mul_s(c.data(), div, 2, b.data(), n - 1);
        }

        WJR_CHECK(c[n] == 0);
        auto cf = addc_s(c.data(), c.data(), n, rem, 2);
        WJR_CHECK(cf == 0);
        WJR_CHECK(std::equal(a.begin(), a.begin() + n, c.begin()));
    };

    for (int i = 0; i < N; ++i) {
        uint64_t div[2] = {mt_rand(), mt_rand()};

        for (int t = 0; t < T; ++t) {
            for (int j = 2; j < M; j += (j <= 180 ? 1 : j / 7)) {
                for (int k = 0; k < j; ++k) {
                    a[k] = mt_rand();
                }

                check(j, div);
            }
        }
    }
}

TEST(math, div_qr_s) {

    const int T = 8;
    const int N = 2400;

    std::vector<uint64_t> a(N), b(N + 1), c(N + 1), d(N + 1), rem(N);

    auto check = [&](size_t n, size_t m) {
        div_qr_s(c.data(), rem.data(), a.data(), n, b.data(), m);

        if (n - m + 1 >= m) {
            mul_s(d.data(), c.data(), n - m + 1, b.data(), m);
        } else {
            mul_s(d.data(), b.data(), m, c.data(), n - m + 1);
        }

        WJR_CHECK(d[n] == 0);
        auto cf = addc_s(d.data(), d.data(), n, rem.data(), m);
        WJR_CHECK(cf == 0);
        WJR_CHECK(std::equal(a.begin(), a.begin() + n, d.begin()));
    };

    for (int t = 0; t < T; ++t) {
        for (int i = 2; i < N; i += (i <= 180 ? 1 : i / 7)) {
            for (int j = 1; j <= i; j += (j <= 180 ? 1 : j / 7)) {
                for (int k = 0; k < i; ++k) {
                    a[k] = mt_rand();
                }

                for (int k = 0; k < j; ++k) {
                    b[k] = mt_rand();
                }

                check(i, j);
            }
        }
    }
}

namespace convert_tests {
using namespace wjr;
using namespace charconv_detail;

static_assert(std::is_same_v<fast_buffer_t<char *>, char>, "");
static_assert(std::is_same_v<fast_buffer_t<uint8_t *>, uint8_t>, "");
static_assert(std::is_same_v<fast_buffer_t<int *>, char>, "");
static_assert(std::is_same_v<fast_buffer_t<std::back_insert_iterator<std::string>>, char>, "");
static_assert(
    std::is_same_v<fast_buffer_t<std::back_insert_iterator<std::vector<uint8_t>>>, uint8_t>, "");

static_assert(is_fast_container_inserter_v<int *> == 0, "");
static_assert(is_fast_container_inserter_v<std::back_insert_iterator<std::vector<char>>> == 1, "");
static_assert(is_fast_container_inserter_v<std::back_insert_iterator<std::string>> ==
                  WJR_PP_BOOL_IF(WJR_HAS_FEATURE(STRING_UNINITIALIZED_RESIZE), 2, 1),
              "");
static_assert(is_fast_container_inserter_v<std::back_insert_iterator<std::wstring>> == 0, "");

struct Traits : public std::char_traits<char> {};
static_assert(
    is_fast_container_inserter_v<std::back_insert_iterator<std::basic_string<char, Traits>>> == 0,
    "");

static_assert(is_fast_container_inserter_v<std::back_insert_iterator<vector<char>>> == 2, "");
static_assert(is_fast_container_inserter_v<std::back_insert_iterator<vector<int>>> == 0, "");

} // namespace convert_tests

TEST(math, to_chars) {
    const int T = 16;

    char b[64], c[64];
    std::vector<char> vec(64);
    std::list<char> lst;
    lst.resize(64);

    auto check = [&](int k, int base, auto x) {
        // test __fast_to_chars
        auto ret0 = to_chars(b, b + k, x, base);

        if (!ret0) {
            WJR_CHECK(ret0.ptr == b + k);
        }

        do {
            auto ret1 = std::to_chars(c, c + k, x, base);

            WJR_CHECK(ret0.ec == ret1.ec);

            if ((bool)ret0) {
                WJR_CHECK(std::string_view(b, ret0.ptr - b) ==
                              std::string_view(c, ret1.ptr - c));
            } else {
                WJR_CHECK(ret1.ptr == c + k);
            }
        } while (false);

        // test random access iterator of __fallback_to_chars
        do {
            auto ret1 = to_chars(vec.data(), vec.data() + k, x, base);

            WJR_CHECK(ret0.ec == ret1.ec);

            if ((bool)ret0) {
                WJR_CHECK(std::string_view(b, ret0.ptr - b) ==
                              std::string_view(vec.data(), ret1.ptr - vec.data()));
            } else {
                WJR_CHECK(ret1.ptr == vec.data() + k);
            }
        } while (false);

        // test forward iterator of __fallback_to_chars
        do {
            auto ret1 = to_chars(lst.begin(), std::next(lst.begin(), k), x, base);

            WJR_CHECK(ret0.ec == ret1.ec);

            if ((bool)ret0) {
                WJR_CHECK(std::string_view(b, ret0.ptr - b) ==
                              std::string(lst.begin(), ret1.ptr));
            } else {
                WJR_CHECK(std::distance(lst.begin(), ret1.ptr) == k);
            }
        } while (false);

        // test non-validate
        if (k == 64) {
            // test __fast_to_chars_backward_unchecked
            do {
                auto ret1 = to_chars_backward_unchecked(c + 64, x, base);

                WJR_CHECK(std::string_view(b, ret0.ptr - b) ==
                              std::string_view(ret1, c + 64 - ret1));
            } while (false);

            // test __fast_to_chars_unchecked
            do {
                auto ret1 = to_chars_unchecked(c, x, base);

                WJR_CHECK(std::string_view(b, ret0.ptr - b) == std::string_view(c, ret1 - c));
            } while (false);

            // test random access iterator of __fallback_to_chars_unchecked
            do {
                vec.clear();

                (void)to_chars_unchecked(std::back_inserter(vec), x, base);

                WJR_CHECK(std::string_view(b, ret0.ptr - b) ==
                              std::string_view((char *)vec.data(), vec.size()));
            } while (false);

            // test forward iterator of __fallback_to_chars_unchecked
            do {
                lst.clear();

                (void)to_chars_unchecked(std::back_inserter(lst), x, base);

                WJR_CHECK(std::string_view(b, ret0.ptr - b) ==
                              std::string(lst.begin(), lst.end()));

                lst.resize(64);
            } while (false);
        }
    };

    for (int t = 0; t < T; ++t) {
        for (unsigned base : {2, 4, 8, 16, 32, 10}) {
            for (int i = 1; i <= 64; ++i) {
                auto mask = (i == 64 ? (uint64_t)in_place_max : (1ull << i) - 1);

                uint64_t x = mt_rand() & mask;

                for (int k = 0; k <= 64; ++k) {
                    check(k, base, x);
                }

                int64_t y = 0;
                for (int p : {0, 1}) {
                    y = mt_rand() & mask;
                    if (p) {
                        y = -y;
                    }

                    for (int k = 0; k < 64; ++k) {
                        check(k, base, y);
                    }
                }
            }
        }
    }
}

TEST(math, from_chars) {
    const int T = 16;

    char str[65];

    auto check = [&](int n, int base, auto type) {
        decltype(type) x = 1;
        // test __fast_to_chars
        auto ret0 = from_chars(str, str + n, x, base);

        do {
            decltype(type) y = 3;
            auto ret1 = std::from_chars(str, str + n, y, base);

            WJR_CHECK(ret0.ec == ret1.ec);
            WJR_CHECK(ret0.ptr - str == ret1.ptr - str);
            if ((bool)ret0) {
                WJR_CHECK(x == y);
            } else {
                WJR_CHECK(x == 1);
                WJR_CHECK(y == 3);
            }
        } while (false);
    };

    for (int t = 0; t < T; ++t) {
        for (unsigned base : {2, 10}) {
            for (int i = 1; i <= 64; ++i) {
                for (int k = 0; k <= i; ++k) {
                    for (int p = 0; p < k; ++p) {
                        str[p] = mt_rand() % base + '0';
                    }

                    if (k != i) {
                        str[k] = '\0';
                    }

                    check(i, base, int8_t());
                    check(i, base, uint8_t());
                    check(i, base, int16_t());
                    check(i, base, uint16_t());
                    check(i, base, int32_t());
                    check(i, base, uint32_t());
                    check(i, base, int64_t());
                    check(i, base, uint64_t());
                }
            }
        }
    }
}

#if defined(WJR_USE_GMP)

TEST(math, biginteger_to_chars) {

    const int T = 4;
    const int N = 1024;
    const int M = N * 64;

    std::vector<uint64_t> a(N), d(N);
    std::string b(M, '0'), c(M, '0');

    for (int t = 0; t < T; ++t) {
        for (unsigned base : {2, 4, 8, 16, 32, 10}) {
            for (int i = 1; i < N; ++i) {

                for (int j = 0; j < i; ++j) {
                    a[j] = mt_rand();
                }
                while (a[i - 1] == 0) {
                    a[i - 1] = mt_rand();
                }

                d = a;

                b.reserve(i * 64);
                size_t len = biginteger_to_chars(b.data(), d.data(), i, base) - b.data();
                size_t len2 = mpn_get_str((unsigned char *)c.data(), base, a.data(), i);

                for (auto &ch : c) {
                    ch = ch < 10 ? ch + '0' : ch - 10 + 'a';
                }

                WJR_CHECK(std::string_view(b.data(), len) == std::string_view(c.data(), len2));

                b.clear();
                biginteger_to_chars(std::back_inserter(b), d.data(), i, base);
                len = b.size();
                WJR_CHECK(std::string_view(b.data(), len) == std::string_view(c.data(), len2));
            }
        }
    }
}

TEST(math, biginteger_from_chars) {
    const int T = 4;
    const int N = 8096;
    const int M = N;

    std::vector<uint64_t> a(N), b(N);
    std::string c(M, '0');

    for (int t = 0; t < T; ++t) {
        for (int i = 1; i < M;) {
            for (auto base : {2, 8, 16, 10}) {
                for (int j = 0; j < i; ++j) {
                    c[j] = mt_rand() % base;
                }
                while (c[0] == 0) {
                    c[0] = mt_rand() % base;
                }

                size_t len2 = mpn_set_str(b.data(), (unsigned char *)c.data(), i, base);

                for (auto &ch : c) {
                    ch = ch < 10 ? ch + '0' : ch - 10 + 'a';
                }

                size_t len =
                    biginteger_from_chars(c.data(), c.data() + i, a.data(), base) - a.data();

                WJR_CHECK(len == len2);
                WJR_CHECK(std::equal(a.begin(), a.begin() + len, b.begin()));
            }

            if (i < 64) {
                ++i;
            } else if (i < 256) {
                i += 7;
            } else if (i < 1024) {
                i += 29;
            } else {
                i += 53;
            }
        }
    }
}

#endif