#include "detail.hpp"

#if defined(WJR_USE_GMP)
    #include <gmp.h>
#endif

#include <wjr/biginteger.hpp>

using namespace wjr;

namespace wjr {
template class basic_biginteger<default_biginteger_vector_storage<std::allocator<uint64_t>>>;
}

#if defined(WJR_USE_GMP)

inline void copy(mpz_t dst, const biginteger &src) {
    const auto n = src.size();
    auto ptr = mpz_limbs_write(dst, n);
    std::copy_n(src.data(), n, ptr);
    mpz_limbs_finish(dst, n);
}

inline bool equal(const biginteger &a, const mpz_t b) {
    const auto n = a.size();
    if (mpz_size(b) != n) {
        return false;
    }

    if (a.is_negate() && mpz_sgn(b) >= 0) {
        return false;
    }

    return std::equal(a.data(), a.data() + n, mpz_limbs_read(b));
}

#endif

inline void random(biginteger &a, size_t n) {
    if (n == 0) {
        a.clear();
        return;
    }
    urandom_exact_bit(a, (n - 1) * 64 + mt_rand() % 64, __mt_rand);
}

TEST(biginteger, random) {
    {
        biginteger a;

        urandom_bit(a, 0, __mt_rand);
        WJR_ASSERT_L0(a == 0);

        for (int i = 0; i < 8; ++i) {
            urandom_bit(a, 1, __mt_rand);
            WJR_ASSERT_L0(a >= 0 && a < 2);

            urandom_bit(a, 2, __mt_rand);
            WJR_ASSERT_L0(a >= 0 && a < 4);

            urandom_bit(a, 7, __mt_rand);
            WJR_ASSERT_L0(a >= 0 && a < (1u << 7));
        }
    }

    {
        biginteger a;

        urandom_exact_bit(a, 0, __mt_rand);
        WJR_ASSERT_L0(a == 0);

        for (int i = 0; i < 8; ++i) {
            urandom_exact_bit(a, 1, __mt_rand);
            WJR_ASSERT_L0(a >= 1 && a < 2);

            urandom_exact_bit(a, 2, __mt_rand);
            WJR_ASSERT_L0(a >= 2 && a < 4);

            urandom_exact_bit(a, 7, __mt_rand);
            WJR_ASSERT_L0(a >= (1u << 6) && a < (1u << 7));
        }
    }

    {
        biginteger a;
        a = 1ull << 32 | 1ull << 31 | 13727;
        a *= a;
        biginteger b;

        constexpr float RES = 1.25;
        constexpr float FLOOR = RES * 0.95;
        constexpr float CEIL = RES * 1.05;
        const int N = 1e6;
        const int TRY = 4;

        int T = 0;
        while (true) {
            ++T;

            int num[3] = {0, 0, 0};

            for (int i = 0; i < N; ++i) {
                urandom(b, a, __mt_rand);
                ++num[b.size()];
            }

            const float f = (num[2] * 1.0 / num[1]);

            if (f < FLOOR || f > CEIL) {
                WJR_ASSERT_L0(T != TRY);
                continue;
            }

            break;
        }
    }
}

TEST(biginteger, construct) {
    {
        biginteger a;

        WJR_ASSERT_L0(a == 0);
    }

    {
        biginteger a;
        biginteger b(a);
        biginteger c(std::move(a));
        biginteger d = c;

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(b == 0);
        WJR_ASSERT_L0(c == 0);
        WJR_ASSERT_L0(d == 0);
    }

    {
        biginteger a(1);
        biginteger b(a);

        WJR_ASSERT_L0(a == 1);
        WJR_ASSERT_L0(b == 1);

        biginteger c(std::move(a));

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(c == 1);
    }

    {
        biginteger a(-1);
        biginteger b(a);

        WJR_ASSERT_L0(a == -1);
        WJR_ASSERT_L0(b == -1);

        biginteger c(std::move(a));

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(c == -1);
    }

    {
        biginteger a(0);

        WJR_ASSERT_L0(a == 0);

        biginteger b(1);

        WJR_ASSERT_L0(b == 1);

        biginteger c(-1);

        WJR_ASSERT_L0(c == -1);
    }

    {
        biginteger a;
        a = 1;
        a <<= 1023 * 64;

        biginteger b(a);

        WJR_ASSERT_L0(a == b);
        WJR_ASSERT_L0(b[1023] == 1);
        WJR_ASSERT_L0(b.size() == 1024);

        biginteger c(std::move(a));

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(c == b);
        WJR_ASSERT_L0(c[1023] == 1);
        WJR_ASSERT_L0(c.size() == 1024);
    }
}

TEST(biginteger, assignment) {
    {
        biginteger a;
        a = 0;
        biginteger b;
        b = a;

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(b == 0);

        biginteger c;
        c = std::move(a);

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(c == 0);
    }

    {
        biginteger a;
        a = 1;
        biginteger b;
        b = a;

        WJR_ASSERT_L0(a == 1);
        WJR_ASSERT_L0(b == 1);

        biginteger c;
        c = std::move(a);

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(c == 1);
    }

    {
        biginteger a;
        a = -1;
        biginteger b;
        b = a;

        WJR_ASSERT_L0(a == -1);
        WJR_ASSERT_L0(b == -1);

        biginteger c;
        c = std::move(a);

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(c == -1);
    }

    {
        biginteger a;
        a = 3;
        biginteger b;
        b = -1;

        WJR_ASSERT_L0(a == 3);
        WJR_ASSERT_L0(b == -1);

        b = std::move(a);

        WJR_ASSERT_L0(a == 0);
        WJR_ASSERT_L0(b == 3);
    }
}

TEST(biginteger, addsub) {
    {
        biginteger a(1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT_L0(c == 2);

        sub(c, a, b);
        WJR_ASSERT_L0(c == 0);
    }

    {
        biginteger a(1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT_L0(c == 0);

        sub(c, a, b);
        WJR_ASSERT_L0(c == 2);
    }

    {
        biginteger a(-1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT_L0(c == 0);

        sub(c, a, b);
        WJR_ASSERT_L0(c == -2);
    }

    {
        biginteger a(-1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT_L0(c == -2);

        sub(c, a, b);
        WJR_ASSERT_L0(c == 0);
    }

#if defined(WJR_USE_GMP)

    {
        biginteger a, b, c;
        mpz_t a1, b1, c1;

        mpz_init(a1);
        mpz_init(b1);
        mpz_init(c1);

        const int N = 2400;

        for (int n = 0; n < N; n += (n <= 180 ? 1 : n / 7)) {
            for (int m = 0; m < N; m += (m <= 180 ? 1 : m / 7)) {
                random(a, n);
                copy(a1, a);

                random(b, m);
                copy(b1, b);

                for (int i = 0; i < 4; ++i) {
                    add(c, a, b);
                    mpz_add(c1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));

                    sub(c, a, b);
                    mpz_sub(c1, a1, b1);
                    WJR_ASSERT_L0(equal(c, c1));

                    a.negate();
                    mpz_neg(a1, a1);

                    if (i % 2 == 1) {
                        b.negate();
                        mpz_neg(b1, b1);
                    }
                }
            }

            {
                uint32_t x;

                for (int i = 0; i < 16; ++i) {
                    if (i == 0) {
                        x = 0;
                    } else {
                        x = mt_rand();
                    }

                    random(a, n);
                    copy(a1, a);

                    for (int j = 0; j < 2; ++j) {
                        add(c, a, x);
                        mpz_add_ui(c1, a1, x);

                        WJR_ASSERT_L0(equal(c, c1));

                        add(c, x, a);

                        WJR_ASSERT_L0(equal(c, c1));

                        sub(c, a, x);
                        mpz_sub_ui(c1, a1, x);

                        WJR_ASSERT_L0(equal(c, c1));

                        sub(c, x, a);
                        mpz_ui_sub(c1, x, a1);

                        WJR_ASSERT_L0(equal(c, c1));

                        a.negate();
                        mpz_neg(a1, a1);
                    }
                }
            }
        }

        mpz_clears(a1, b1, c1, nullptr);
    }

#endif
}

TEST(biginteger, mul) {
    {
        biginteger a, b, c;
        mpz_t a1, b1, c1;

        mpz_init(a1);
        mpz_init(b1);
        mpz_init(c1);

        const int N = 1200;

        for (int n = 0; n < N; n += (n <= 140 ? 1 : n / 5)) {
            for (int m = 0; m < N; m += (m <= 140 ? 1 : m / 5)) {
                random(a, n);
                copy(a1, a);

                random(b, m);
                copy(b1, b);

                for (int i = 0; i < 4; ++i) {
                    mul(c, a, b);
                    mpz_mul(c1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));

                    addmul(c, a, b);
                    mpz_addmul(c1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));

                    c = a;
                    mpz_set(c1, a1);
                    mul(a, a, b);
                    mpz_mul(a1, a1, b1);

                    WJR_ASSERT_L0(equal(a, a1));

                    a = c;
                    mpz_set(a1, c1);

                    a.negate();
                    mpz_neg(a1, a1);

                    if (i % 2 == 1) {
                        b.negate();
                        mpz_neg(b1, b1);
                    }
                }
            }

            {
                uint32_t x;

                for (int i = 0; i < 16; ++i) {
                    if (i == 0) {
                        x = 0;
                    } else {
                        x = mt_rand();
                    }

                    random(a, n);
                    copy(a1, a);

                    for (int j = 0; j < 2; ++j) {
                        mul(c, a, x);
                        mpz_mul_ui(c1, a1, x);

                        WJR_ASSERT_L0(equal(c, c1));

                        mul(c, x, a);

                        WJR_ASSERT_L0(equal(c, c1));

                        mul_2exp(c, a, x % 1024);
                        mpz_mul_2exp(c1, a1, x % 1024);

                        WJR_ASSERT_L0(equal(c, c1));

                        a.negate();
                        mpz_neg(a1, a1);
                    }
                }
            }
        }

        mpz_clears(a1, b1, c1, nullptr);
    }
}

TEST(biginteger, sqr) {
    {
        biginteger a, b;
        mpz_t a1, b1;

        mpz_init(a1);
        mpz_init(b1);

        const int N = 1200;

        for (int n = 0; n < N; n += (n <= 140 ? 1 : n / 5)) {
            for (int m = 0; m < N; m += (m <= 140 ? 1 : m / 5)) {
                random(a, n);
                copy(a1, a);

                random(b, m);
                copy(b1, b);

                for (int i = 0; i < 2; ++i) {
                    sqr(b, a);
                    mpz_mul(b1, a1, a1);

                    WJR_ASSERT_L0(equal(b, b1));

                    a.negate();
                    mpz_neg(a1, a1);
                }
            }
        }

        mpz_clears(a1, b1, nullptr);
    }
}

TEST(biginteger, div) {
    {
        biginteger a, b, c, d;
        mpz_t a1, b1, c1, d1;

        mpz_init(a1);
        mpz_init(b1);
        mpz_init(c1);
        mpz_init(d1);

        const int N = 900;

        for (int n = 0; n < N; n += (n <= 140 ? 1 : n / 5)) {
            for (int m = 1; m < N; m += (m <= 140 ? 1 : m / 5)) {
                random(a, n);
                copy(a1, a);

                do {
                    random(b, m);
                    copy(b1, b);
                } while (b.zero());

                for (int i = 0; i < 4; ++i) {
                    tdiv_qr(c, d, a, b);
                    mpz_tdiv_qr(c1, d1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));
                    WJR_ASSERT_L0(equal(d, d1));

                    tdiv_q(c, a, b);
                    mpz_tdiv_q(c1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));

                    tdiv_r(d, a, b);
                    mpz_tdiv_r(d1, a1, b1);

                    WJR_ASSERT_L0(equal(d, d1));

                    fdiv_qr(c, d, a, b);
                    mpz_fdiv_qr(c1, d1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));
                    WJR_ASSERT_L0(equal(d, d1));

                    fdiv_q(c, a, b);
                    mpz_fdiv_q(c1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));

                    fdiv_r(d, a, b);
                    mpz_fdiv_r(d1, a1, b1);

                    WJR_ASSERT_L0(equal(d, d1));

                    cdiv_qr(c, d, a, b);
                    mpz_cdiv_qr(c1, d1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));
                    WJR_ASSERT_L0(equal(d, d1));

                    cdiv_q(c, a, b);
                    mpz_cdiv_q(c1, a1, b1);

                    WJR_ASSERT_L0(equal(c, c1));

                    cdiv_r(d, a, b);
                    mpz_cdiv_r(d1, a1, b1);

                    WJR_ASSERT_L0(equal(d, d1));

                    a.negate();
                    mpz_neg(a1, a1);

                    if (i == 1) {
                        b.negate();
                        mpz_neg(b1, b1);
                    }
                }
            }
        }

        mpz_clears(a1, b1, c1, d1, nullptr);
    }
}

TEST(biginteger, div_2exp) {
    {
        biginteger a, b;
        mpz_t a1, b1;

        mpz_init(a1);
        mpz_init(b1);

        const int N = 900;

        for (int n = 0; n < N; n += (n <= 140 ? 1 : n / 5)) {
            for (int m = 0; m < n; ++m) {
                random(a, n);
                copy(a1, a);

                const unsigned int off = m * 64;
                const unsigned int rd = off + mt_rand() % 64 + 1;

                for (auto shift : {off, off + 1, off + 7, off + rd, off + 63}) {
                    for (int i = 0; i < 4; ++i) {
                        tdiv_q_2exp(b, a, shift);
                        mpz_tdiv_q_2exp(b1, a1, shift);

                        WJR_ASSERT_L0(equal(b, b1));

                        tdiv_r_2exp(b, a, shift);
                        mpz_tdiv_r_2exp(b1, a1, shift);

                        WJR_ASSERT_L0(equal(b, b1));

                        cdiv_q_2exp(b, a, shift);
                        mpz_cdiv_q_2exp(b1, a1, shift);

                        WJR_ASSERT_L0(equal(b, b1));

                        fdiv_q_2exp(b, a, shift);
                        mpz_fdiv_q_2exp(b1, a1, shift);

                        WJR_ASSERT_L0(equal(b, b1));

                        cdiv_r_2exp(b, a, shift);
                        mpz_cdiv_r_2exp(b1, a1, shift);

                        WJR_ASSERT_L0(equal(b, b1));

                        fdiv_r_2exp(b, a, shift);
                        mpz_fdiv_r_2exp(b1, a1, shift);

                        WJR_ASSERT_L0(equal(b, b1));

                        a.negate();
                        mpz_neg(a1, a1);

                        if (i == 1) {
                            b.negate();
                            mpz_neg(b1, b1);
                        }
                    }
                }
            }
        }

        mpz_clears(a1, b1, nullptr);
    }
}

TEST(biginteger, convert) {
#if defined(WJR_USE_GMP)
    {
        biginteger a;
        mpz_t b;
        mpz_init(b);
        std::string str;

        const int N = 900;

        for (int n = 0; n < N; n += (n <= 140 ? 1 : n / 5)) {
            const int maxn = n / 32 + 32;
            str.resize(n);
            for (int M = 0; M < maxn; ++M) {
                for (auto base : {2, 8, 10, 16}) {
                    for (auto &i : str) {
                        i = char_converter.to(mt_rand() % base);
                    }

                    a.from_string(str, base);
                    mpz_set_str(b, str.data(), base);

                    WJR_ASSERT_L0(equal(a, b));
                }
            }
        }
    }
#endif
}

TEST(biginteger, pow) {
    {
        biginteger a;
        pow(a, 0, 0);
        WJR_ASSERT_L0(a == 1u);
        pow(a, -14348907, 3);
        WJR_ASSERT_L0(a == biginteger("-2954312706550833698643"));
    }
#if defined(WJR_USE_GMP)
    {
        biginteger a;
        mpz_t b;
        mpz_init(b);

        const int N = 1 << 14;

        for (int n = 0; n < N; n += (n <= 64 ? 1 : n / 4)) {
            const int maxn = bit_width<uint32_t>(n + 1) + 8;
            for (int M = 0; M < maxn; ++M) {
                uint64_t x = mt_rand();
                a = x;
                copy(b, a);

                pow(a, a, n);
                mpz_pow_ui(b, b, n);

                WJR_ASSERT_L0(equal(a, b));
            }
        }
    }
#endif
}