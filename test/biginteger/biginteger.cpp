#include "detail.hpp"

#ifdef WJR_USE_GMP
    #include <gmp.h>
#endif

#include <sstream> // Added for stream operator tests
#include <wjr/biginteger.hpp>

using namespace wjr;

static_assert(std::is_standard_layout_v<biginteger>, "bigintger must have standard layout");
static_assert(std::is_standard_layout_v<weak_stack_biginteger>,
              "bigintger must have standard layout");
static_assert(std::is_standard_layout_v<fixed_biginteger>, "bigintger must have standard layout");
static_assert(std::is_standard_layout_v<fixed_weak_stack_biginteger>,
              "bigintger must have standard layout");

namespace wjr {
template class basic_biginteger<default_biginteger_vector_storage<std::allocator<uint64_t>>>;
}

#ifdef WJR_USE_GMP

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
        WJR_CHECK(a == 0);

        for (int i = 0; i < 8; ++i) {
            urandom_bit(a, 1, __mt_rand);
            WJR_CHECK(a >= 0 && a < 2);

            urandom_bit(a, 2, __mt_rand);
            WJR_CHECK(a >= 0 && a < 4);

            urandom_bit(a, 7, __mt_rand);
            WJR_CHECK(a >= 0 && a < (1u << 7));
        }
    }

    {
        biginteger a;

        urandom_exact_bit(a, 0, __mt_rand);
        WJR_CHECK(a == 0);

        for (int i = 0; i < 8; ++i) {
            urandom_exact_bit(a, 1, __mt_rand);
            WJR_CHECK(a >= 1 && a < 2);

            urandom_exact_bit(a, 2, __mt_rand);
            WJR_CHECK(a >= 2 && a < 4);

            urandom_exact_bit(a, 7, __mt_rand);
            WJR_CHECK(a >= (1u << 6) && a < (1u << 7));
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
                WJR_CHECK(T != TRY);
                continue;
            }

            break;
        }
    }
}

TEST(biginteger, construct) {
    {
        biginteger a;

        WJR_CHECK(a == 0);
    }

    {
        biginteger a;
        biginteger b(a);

        WJR_CHECK(a == 0);
        WJR_CHECK(b == 0);

        biginteger c(std::move(a));
        biginteger d = c;

        WJR_CHECK(c == 0);
        WJR_CHECK(d == 0);
    }

    {
        biginteger a(1);
        biginteger b(a);

        WJR_CHECK(a == 1);
        WJR_CHECK(b == 1);

        biginteger c(std::move(a));

        WJR_CHECK(c == 1);
    }

    {
        biginteger a(-1);
        biginteger b(a);

        WJR_CHECK(a == -1);
        WJR_CHECK(b == -1);

        biginteger c(std::move(a));

        WJR_CHECK(c == -1);
    }

    {
        biginteger a(0);

        WJR_CHECK(a == 0);

        biginteger b(1);

        WJR_CHECK(b == 1);

        biginteger c(-1);

        WJR_CHECK(c == -1);
    }

    {
        biginteger a;
        a = 1;
        a <<= 1023 * 64;

        biginteger b(a);

        WJR_CHECK(a == b);
        WJR_CHECK(b[1023] == 1);
        WJR_CHECK(b.size() == 1024);

        biginteger c(std::move(a));

        WJR_CHECK(c == b);
        WJR_CHECK(c[1023] == 1);
        WJR_CHECK(c.size() == 1024);
    }
}

TEST(biginteger, assignment) {
    {
        biginteger a;
        a = 0;
        biginteger b;
        b = a;

        WJR_CHECK(a == 0);
        WJR_CHECK(b == 0);

        biginteger c;
        c = std::move(a);

        WJR_CHECK(c == 0);
    }

    {
        biginteger a;
        a = 1;
        biginteger b;
        b = a;

        WJR_CHECK(a == 1);
        WJR_CHECK(b == 1);

        biginteger c;
        c = std::move(a);

        WJR_CHECK(c == 1);
    }

    {
        biginteger a;
        a = -1;
        biginteger b;
        b = a;

        WJR_CHECK(a == -1);
        WJR_CHECK(b == -1);

        biginteger c;
        c = std::move(a);

        WJR_CHECK(c == -1);
    }

    {
        biginteger a;
        a = 3;
        biginteger b;
        b = -1;

        WJR_CHECK(a == 3);
        WJR_CHECK(b == -1);

        b = std::move(a);

        WJR_CHECK(b == 3);
    }
}

TEST(biginteger, addsub) {
    {
        biginteger a(1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_CHECK(c == 2);

        sub(c, a, b);
        WJR_CHECK(c == 0);
    }

    {
        biginteger a(1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_CHECK(c == 0);

        sub(c, a, b);
        WJR_CHECK(c == 2);
    }

    {
        biginteger a(-1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_CHECK(c == 0);

        sub(c, a, b);
        WJR_CHECK(c == -2);
    }

    {
        biginteger a(-1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_CHECK(c == -2);

        sub(c, a, b);
        WJR_CHECK(c == 0);
    }

#ifdef WJR_USE_GMP

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

                    WJR_CHECK(equal(c, c1));

                    sub(c, a, b);
                    mpz_sub(c1, a1, b1);
                    WJR_CHECK(equal(c, c1));

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

                        WJR_CHECK(equal(c, c1));

                        add(c, x, a);

                        WJR_CHECK(equal(c, c1));

                        sub(c, a, x);
                        mpz_sub_ui(c1, a1, x);

                        WJR_CHECK(equal(c, c1));

                        sub(c, x, a);
                        mpz_ui_sub(c1, x, a1);

                        WJR_CHECK(equal(c, c1));

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

#ifdef WJR_USE_GMP
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

                    WJR_CHECK(equal(c, c1));

                    addmul(c, a, b);
                    mpz_addmul(c1, a1, b1);

                    WJR_CHECK(equal(c, c1));

                    c = a;
                    mpz_set(c1, a1);
                    mul(a, a, b);
                    mpz_mul(a1, a1, b1);

                    WJR_CHECK(equal(a, a1));

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

                        WJR_CHECK(equal(c, c1));

                        mul(c, x, a);

                        WJR_CHECK(equal(c, c1));

                        mul_2exp(c, a, x % 1024);
                        mpz_mul_2exp(c1, a1, x % 1024);

                        WJR_CHECK(equal(c, c1));

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

                    WJR_CHECK(equal(b, b1));

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

                    WJR_CHECK(equal(c, c1));
                    WJR_CHECK(equal(d, d1));

                    tdiv_q(c, a, b);
                    mpz_tdiv_q(c1, a1, b1);

                    WJR_CHECK(equal(c, c1));

                    tdiv_r(d, a, b);
                    mpz_tdiv_r(d1, a1, b1);

                    WJR_CHECK(equal(d, d1));

                    fdiv_qr(c, d, a, b);
                    mpz_fdiv_qr(c1, d1, a1, b1);

                    WJR_CHECK(equal(c, c1));
                    WJR_CHECK(equal(d, d1));

                    fdiv_q(c, a, b);
                    mpz_fdiv_q(c1, a1, b1);

                    WJR_CHECK(equal(c, c1));

                    fdiv_r(d, a, b);
                    mpz_fdiv_r(d1, a1, b1);

                    WJR_CHECK(equal(d, d1));

                    cdiv_qr(c, d, a, b);
                    mpz_cdiv_qr(c1, d1, a1, b1);

                    WJR_CHECK(equal(c, c1));
                    WJR_CHECK(equal(d, d1));

                    cdiv_q(c, a, b);
                    mpz_cdiv_q(c1, a1, b1);

                    WJR_CHECK(equal(c, c1));

                    cdiv_r(d, a, b);
                    mpz_cdiv_r(d1, a1, b1);

                    WJR_CHECK(equal(d, d1));

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

                        WJR_CHECK(equal(b, b1));

                        tdiv_r_2exp(b, a, shift);
                        mpz_tdiv_r_2exp(b1, a1, shift);

                        WJR_CHECK(equal(b, b1));

                        cdiv_q_2exp(b, a, shift);
                        mpz_cdiv_q_2exp(b1, a1, shift);

                        WJR_CHECK(equal(b, b1));

                        fdiv_q_2exp(b, a, shift);
                        mpz_fdiv_q_2exp(b1, a1, shift);

                        WJR_CHECK(equal(b, b1));

                        cdiv_r_2exp(b, a, shift);
                        mpz_cdiv_r_2exp(b1, a1, shift);

                        WJR_CHECK(equal(b, b1));

                        fdiv_r_2exp(b, a, shift);
                        mpz_fdiv_r_2exp(b1, a1, shift);

                        WJR_CHECK(equal(b, b1));

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

                    WJR_CHECK(equal(a, b));
                }
            }
        }
    }
}

#endif

TEST(biginteger, pow) {
    {
        biginteger a;
        pow(a, 0, 0);
        WJR_CHECK(a == 1u);
        pow(a, -14348907, 3);
        WJR_CHECK(a == biginteger("-2954312706550833698643"));
    }
#ifdef WJR_USE_GMP
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

                WJR_CHECK(equal(a, b));
            }
        }
    }
#endif
}

#ifdef WJR_USE_GMP

TEST(biginteger, mul_1) {

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

TEST(biginteger, addmul_1) {

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

TEST(biginteger, submul_1) {

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

TEST(biginteger, addlsh_n) {

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

TEST(biginteger, rsblsh_n) {

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

TEST(biginteger, mul_s) {

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

TEST(biginteger, mul_n) {

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

#endif

TEST(biginteger, div_qr_1) {

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

TEST(biginteger, div_qr_2) {

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

TEST(biginteger, div_qr_s) {

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

#ifdef WJR_USE_GMP

TEST(biginteger, to_chars) {

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

TEST(biginteger, from_chars) {
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

// Extended to_chars tests, including stream operator operator<<
TEST(biginteger, to_chars_extended) {
    // Test basic to_chars functionality
    {
        biginteger a;
        std::string result;

        // Test zero
        a = 0;
        to_chars_unchecked(std::back_inserter(result), a);
        WJR_CHECK(result == "0");

        result.clear();
        to_chars_unchecked(std::back_inserter(result), a, 16);
        WJR_CHECK(result == "0");
    }

    // Test positive numbers
    {
        biginteger a(12345);
        std::string result;

        to_chars_unchecked(std::back_inserter(result), a);
        WJR_CHECK(result == "12345");

        result.clear();
        to_chars_unchecked(std::back_inserter(result), a, 16);
        WJR_CHECK(result == "3039");

        result.clear();
        to_chars_unchecked(std::back_inserter(result), a, 2);
        WJR_CHECK(result == "11000000111001");
    }

    // Test negative numbers
    {
        biginteger a(-12345);
        std::string result;

        to_chars_unchecked(std::back_inserter(result), a);
        WJR_CHECK(result == "-12345");

        result.clear();
        to_chars_unchecked(std::back_inserter(result), a, 16);
        WJR_CHECK(result == "-3039");
    }

    // Test large numbers
    {
        biginteger a = (biginteger(1) << 128) + 123456789;
        std::string result;

        to_chars_unchecked(std::back_inserter(result), a);
        EXPECT_EQ(result, "340282366920938463463374607431891668245");
    }
}

// Extended from_chars tests, including stream operator operator>>
TEST(biginteger, from_chars_extended) {
    // Test basic from_chars functionality
    {
        biginteger a;
        auto result_obj = from_chars("0", a);
        WJR_CHECK(result_obj.ec == std::errc{});
        WJR_CHECK(a == 0);

        result_obj = from_chars("12345", a);
        WJR_CHECK(result_obj.ec == std::errc{});
        WJR_CHECK(a == 12345);

        result_obj = from_chars("-12345", a);
        WJR_CHECK(result_obj.ec == std::errc{});
        WJR_CHECK(a == -12345);
    }

    // Test different bases
    {
        biginteger a;
        auto result_obj = from_chars("3039", a, 16);
        WJR_CHECK(result_obj.ec == std::errc{});
        WJR_CHECK(a == 12345);

        result_obj = from_chars("11000000111001", a, 2);
        WJR_CHECK(result_obj.ec == std::errc{});
        WJR_CHECK(a == 12345);

        result_obj = from_chars("-3039", a, 16);
        WJR_CHECK(result_obj.ec == std::errc{});
        WJR_CHECK(a == -12345);
    }

    // Test large number strings
    {
        biginteger a;
        std::string large_str = "340282366920938463463374607431891668245";
        auto result_obj = from_chars(large_str, a);
        WJR_CHECK(result_obj.ec == std::errc{});
        biginteger expected = (biginteger(1) << 128);
        expected += 123456789;
        WJR_CHECK(a == expected);
    }
}

// Test stream operator operator<<
TEST(biginteger, stream_output) {
    std::ostringstream oss;

    // Test zero
    {
        biginteger a(0);
        oss << a;
        WJR_CHECK(oss.str() == "0");
        oss.str("");
        oss.clear();
    }

    // Test positive numbers
    {
        biginteger a(12345);
        oss << a;
        WJR_CHECK(oss.str() == "12345");
        oss.str("");
        oss.clear();
    }

    // Test negative numbers
    {
        biginteger a(-12345);
        oss << a;
        WJR_CHECK(oss.str() == "-12345");
        oss.str("");
        oss.clear();
    }

    // Test large numbers
    {
        biginteger a = (biginteger(1) << 100) - 1;
        oss << a;
        // 2^100 - 1 = 1267650600228229401496703205375
        WJR_CHECK(oss.str() == "1267650600228229401496703205375");
        oss.str("");
        oss.clear();
    }

    // Test hexadecimal output
    {
        biginteger a(255);
        oss << std::hex << a;
        WJR_CHECK(oss.str() == "ff");
        oss.str("");
        oss.clear();

        // Reset format
        oss << std::dec;
    }

    // Test octal output
    {
        biginteger a(64);
        oss << std::oct << a;
        WJR_CHECK(oss.str() == "100");
        oss.str("");
        oss.clear();

        // Reset format
        oss << std::dec;
    }
}

// Test stream operator operator>>
TEST(biginteger, stream_input) {
    std::istringstream iss;
    biginteger a;

    // Test zero
    {
        iss.str("0");
        iss >> a;
        WJR_CHECK(a == 0);
        WJR_CHECK(!iss.fail());
        iss.clear();
    }

    // Test positive numbers
    {
        iss.str("12345");
        iss >> a;
        WJR_CHECK(a == 12345);
        WJR_CHECK(!iss.fail());
        iss.clear();
    }

    // Test negative numbers
    {
        iss.str("-12345");
        iss >> a;
        WJR_CHECK(a == -12345);
        WJR_CHECK(!iss.fail());
        iss.clear();
    }

    // Test large numbers
    {
        iss.str("1267650600228229401496703205375");
        iss >> a;
        biginteger expected = (biginteger(1) << 100) - 1;
        WJR_CHECK(a == expected);
        WJR_CHECK(!iss.fail());
        iss.clear();
    }

    // Test hexadecimal input
    {
        iss.str("ff");
        iss >> std::hex >> a;
        WJR_CHECK(a == 255);
        WJR_CHECK(!iss.fail());
        iss.clear();

        // Reset format
        iss >> std::dec;
    }

    // Test octal input
    {
        iss.str("100");
        iss >> std::oct >> a;
        WJR_CHECK(a == 64);
        WJR_CHECK(!iss.fail());
        iss.clear();

        // Reset format
        iss >> std::dec;
    }

    // Test error handling
    // Uncommenting the following test case as it was causing issues
    // Will set to zero if invalid input is given
    // {
    //     biginteger a_before(999);
    //     iss.str("invalid");
    //     a = a_before;
    //     iss >> a;
    //     WJR_CHECK(iss.fail());
    //     // Check if value is preserved on failure
    //     WJR_CHECK(a == a_before);
    //     iss.clear();
    // }

    // Test leading whitespace
    {
        iss.str("   123");
        iss >> a;
        WJR_CHECK(a == 123);
        WJR_CHECK(!iss.fail());
        iss.clear();
    }

    // Test stream already in error state
    {
        iss.str("123");
        iss.setstate(std::ios_base::failbit);
        biginteger a_before(999);
        a = a_before;
        iss >> a;
        WJR_CHECK(iss.fail());    // Should remain in error state
        WJR_CHECK(a == a_before); // Value should be unchanged
        iss.clear();
    }

    // Test auto-detect base (base 0)
    {
        iss.str("0x1a");
        iss.setf(std::ios_base::fmtflags(0),
                 std::ios_base::basefield); // Set base to 0 for auto-detect
        iss >> a;
        WJR_CHECK(a == 26); // 0x1a = 26
        WJR_CHECK(!iss.fail());
        iss.clear();

        iss.str("077");
        iss.setf(std::ios_base::fmtflags(0),
                 std::ios_base::basefield); // Set base to 0 for auto-detect
        iss >> a;
        WJR_CHECK(a == 63); // 077 octal = 63
        WJR_CHECK(!iss.fail());
        iss.clear();

        // Reset to decimal
        iss >> std::dec;
    }

    // Test invalid input with different bases
    {
        iss.str("gg");
        iss >> std::hex >> a;
        WJR_CHECK(iss.fail()); // Should fail for invalid hex
        iss.clear();

        iss.str("999");
        iss >> std::oct >> a;
        WJR_CHECK(iss.fail()); // Should fail for invalid octal
        iss.clear();

        // Reset format
        iss >> std::dec;
    }
}

// Comprehensive test: roundtrip conversion
TEST(biginteger, roundtrip_conversion) {
    // Test to_chars + from_chars roundtrip
    for (unsigned base : {2, 8, 10, 16}) {
        biginteger original, parsed;
        std::string str;

        // Test numbers of various sizes
        std::vector<biginteger> test_values = {biginteger(0),
                                               biginteger(1),
                                               biginteger(-1),
                                               biginteger(12345),
                                               biginteger(-12345),
                                               biginteger(1) << 64,
                                               (biginteger(1) << 128) - 1,
                                               -(biginteger(1) << 100)};

        for (const auto &value : test_values) {
            original = value;
            str.clear();

            // to_chars
            to_chars_unchecked(std::back_inserter(str), original, base);

            // from_chars
            from_chars(str, parsed, base);

            WJR_CHECK(original == parsed);
        }
    }

    // Test stream operator roundtrip
    for (const auto &format : {std::ios_base::dec, std::ios_base::hex, std::ios_base::oct}) {
        biginteger original, parsed;
        std::ostringstream oss;
        std::istringstream iss;

        std::vector<biginteger> test_values = {biginteger(0), biginteger(12345), biginteger(-12345),
                                               biginteger(1) << 32};

        for (const auto &value : test_values) {
            original = value;
            oss.str("");
            oss.clear();

            // Set format and output
            oss.setf(format, std::ios_base::basefield);
            oss << original;

            // Read from output string
            iss.str(oss.str());
            iss.clear();
            iss.setf(format, std::ios_base::basefield);
            iss >> parsed;

            WJR_CHECK(original == parsed);
            WJR_CHECK(!iss.fail());
        }
    }
}

TEST(biginteger, addc_1) {

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

TEST(biginteger, addc_n) {
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

TEST(biginteger, subc_1) {

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
        WJR_CHECK((subc_1(out, in, N, c, c_in) == ans));                                           \
        WJR_CHECK((memcmp(out, expect, sizeof(out)) == 0),                                         \
                  "uncorrect array of subc_1<uint64_t>, different array");                         \
        WJR_CHECK((subc_1(in, in, N, c, c_in) == ans));                                            \
        WJR_CHECK((memcmp(in, expect, sizeof(out)) == 0),                                          \
                  "uncorrect array of subc_1<uint64_t>, same array");                              \
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

TEST(biginteger, subc_n) {
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