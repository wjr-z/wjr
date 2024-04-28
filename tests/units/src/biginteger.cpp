#include "details.hpp"

#if defined(WJR_USE_GMP)
#include <gmp.h>
#endif

#include <wjr/biginteger.hpp>

using namespace wjr;

namespace wjr {
template class basic_biginteger<
    default_biginteger_vector_storage<std::allocator<uint64_t>>>;
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
    urandom_exact_bit(a, n * 64, mt_rand.get());
}

TEST(biginteger, random) {
    std::mt19937_64 mt_rand(std::random_device{}());
    {
        biginteger a;

        urandom_bit(a, 0, mt_rand);
        WJR_ASSERT(a == 0);

        for (int i = 0; i < 8; ++i) {
            urandom_bit(a, 1, mt_rand);
            WJR_ASSERT(a >= 0 && a < 2);

            urandom_bit(a, 2, mt_rand);
            WJR_ASSERT(a >= 0 && a < 4);

            urandom_bit(a, 7, mt_rand);
            WJR_ASSERT(a >= 0 && a < (1u << 7));
        }
    }

    {
        biginteger a;

        urandom_exact_bit(a, 0, mt_rand);
        WJR_ASSERT(a == 0);

        for (int i = 0; i < 8; ++i) {
            urandom_exact_bit(a, 1, mt_rand);
            WJR_ASSERT(a >= 1 && a < 2);

            urandom_exact_bit(a, 2, mt_rand);
            WJR_ASSERT(a >= 2 && a < 4);

            urandom_exact_bit(a, 7, mt_rand);
            WJR_ASSERT(a >= (1u << 6) && a < (1u << 7));
        }
    }
}

TEST(biginteger, construct) {
    {
        biginteger a;

        WJR_ASSERT(a == 0);
    }

    {
        biginteger a;
        biginteger b(a);
        biginteger c(std::move(a));
        biginteger d = c;

        WJR_ASSERT(a == 0);
        WJR_ASSERT(b == 0);
        WJR_ASSERT(c == 0);
        WJR_ASSERT(d == 0);
    }

    {
        biginteger a(1);
        biginteger b(a);

        WJR_ASSERT(a == 1);
        WJR_ASSERT(b == 1);

        biginteger c(std::move(a));

        WJR_ASSERT(a == 0);
        WJR_ASSERT(c == 1);
    }

    {
        biginteger a(-1);
        biginteger b(a);

        WJR_ASSERT(a == -1);
        WJR_ASSERT(b == -1);

        biginteger c(std::move(a));

        WJR_ASSERT(a == 0);
        WJR_ASSERT(c == -1);
    }

    {
        biginteger a(0);

        WJR_ASSERT(a == 0);

        biginteger b(1);

        WJR_ASSERT(b == 1);

        biginteger c(-1);

        WJR_ASSERT(c == -1);
    }

    {
        biginteger a;
        a.reserve(1024);
        a.set_ssize(1024);

        a[1023] = 1;

        biginteger b(a);

        WJR_ASSERT(a == b);
        WJR_ASSERT(b[1023] == 1);
        WJR_ASSERT(b.size() == 1024);

        biginteger c(std::move(a));

        WJR_ASSERT(a == 0);
        WJR_ASSERT(c == b);
        WJR_ASSERT(c[1023] == 1);
        WJR_ASSERT(c.size() == 1024);
    }
}

TEST(biginteger, assignment) {
    {
        biginteger a;
        a = 0;
        biginteger b;
        b = a;

        WJR_ASSERT(a == 0);
        WJR_ASSERT(b == 0);

        biginteger c;
        c = std::move(a);

        WJR_ASSERT(a == 0);
        WJR_ASSERT(c == 0);
    }

    {
        biginteger a;
        a = 1;
        biginteger b;
        b = a;

        WJR_ASSERT(a == 1);
        WJR_ASSERT(b == 1);

        biginteger c;
        c = std::move(a);

        WJR_ASSERT(a == 0);
        WJR_ASSERT(c == 1);
    }

    {
        biginteger a;
        a = -1;
        biginteger b;
        b = a;

        WJR_ASSERT(a == -1);
        WJR_ASSERT(b == -1);

        biginteger c;
        c = std::move(a);

        WJR_ASSERT(a == 0);
        WJR_ASSERT(c == -1);
    }

    {
        biginteger a;
        a = 3;
        biginteger b;
        b = -1;

        WJR_ASSERT(a == 3);
        WJR_ASSERT(b == -1);

        b = std::move(a);

        WJR_ASSERT(a == 0);
        WJR_ASSERT(b == 3);
    }
}

TEST(biginteger, addsub) {
    {
        biginteger a(1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c == 2);

        sub(c, a, b);
        WJR_ASSERT(c == 0);
    }

    {
        biginteger a(1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c == 0);

        sub(c, a, b);
        WJR_ASSERT(c == 2);
    }

    {
        biginteger a(-1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c == 0);

        sub(c, a, b);
        WJR_ASSERT(c == -2);
    }

    {
        biginteger a(-1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c == -2);

        sub(c, a, b);
        WJR_ASSERT(c == 0);
    }

#if defined(WJR_USE_GMP)

    {
        biginteger a, b, c;
        mpz_t a1, b1, c1;

        mpz_init(a1);
        mpz_init(b1);
        mpz_init(c1);

        const int N = 2400;

        for (int n = 1; n < N; n += (n <= 180 ? 1 : n / 7)) {
            for (int m = 1; m < N; m += (m <= 180 ? 1 : m / 7)) {
                random(a, n);
                copy(a1, a);

                random(b, m);
                copy(b1, b);

                for (int i = 0; i < 4; ++i) {
                    add(c, a, b);
                    mpz_add(c1, a1, b1);

                    WJR_ASSERT(equal(c, c1));

                    sub(c, a, b);
                    mpz_sub(c1, a1, b1);
                    WJR_ASSERT(equal(c, c1));

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

                    for (int i = 0; i < 2; ++i) {
                        add(c, a, x);
                        mpz_add_ui(c1, a1, x);

                        WJR_ASSERT(equal(c, c1));

                        add(c, x, a);

                        WJR_ASSERT(equal(c, c1));

                        sub(c, a, x);
                        mpz_sub_ui(c1, a1, x);

                        WJR_ASSERT(equal(c, c1));

                        sub(c, x, a);
                        mpz_ui_sub(c1, x, a1);

                        WJR_ASSERT(equal(c, c1));

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
        biginteger a(1);
        biginteger b(1);
        biginteger c;

        mul(c, a, b);

        WJR_ASSERT(c == 1);
    }

    {
        biginteger a, b, c;
        mpz_t a1, b1, c1;

        mpz_init(a1);
        mpz_init(b1);
        mpz_init(c1);

        const int N = 1200;

        for (int n = 1; n < N; n += (n <= 140 ? 1 : n / 5)) {
            for (int m = 1; m < N; m += (m <= 140 ? 1 : m / 5)) {
                random(a, n);
                copy(a1, a);

                random(b, m);
                copy(b1, b);

                for (int i = 0; i < 4; ++i) {
                    mul(c, a, b);
                    mpz_mul(c1, a1, b1);

                    WJR_ASSERT(equal(c, c1));

                    addmul(c, a, b);
                    mpz_addmul(c1, a1, b1);

                    WJR_ASSERT(equal(c, c1));

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

                        WJR_ASSERT(equal(c, c1));

                        mul(c, x, a);

                        WJR_ASSERT(equal(c, c1));

                        mul_2exp(c, a, x % 1024);
                        mpz_mul_2exp(c1, a1, x % 1024);

                        WJR_ASSERT(equal(c, c1));

                        a.negate();
                        mpz_neg(a1, a1);
                    }
                }
            }
        }

        mpz_clears(a1, b1, c1, nullptr);
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

        for (int n = 1; n < N; n += (n <= 140 ? 1 : n / 5)) {
            for (int m = 1; m < N; m += (m <= 140 ? 1 : m / 5)) {
                random(a, n);
                copy(a1, a);

                random(b, m);
                copy(b1, b);

                for (int i = 0; i < 4; ++i) {
                    tdiv_qr(c, d, a, b);
                    mpz_tdiv_qr(c1, d1, a1, b1);

                    WJR_ASSERT(equal(c, c1));
                    WJR_ASSERT(equal(d, d1));

                    tdiv_q(c, a, b);
                    mpz_tdiv_q(c1, a1, b1);

                    WJR_ASSERT(equal(c, c1));

                    tdiv_r(d, a, b);
                    mpz_tdiv_r(d1, a1, b1);

                    WJR_ASSERT(equal(d, d1));

                    fdiv_qr(c, d, a, b);
                    mpz_fdiv_qr(c1, d1, a1, b1);

                    WJR_ASSERT(equal(c, c1));
                    WJR_ASSERT(equal(d, d1));

                    fdiv_q(c, a, b);
                    mpz_fdiv_q(c1, a1, b1);

                    WJR_ASSERT(equal(c, c1));

                    fdiv_r(d, a, b);
                    mpz_fdiv_r(d1, a1, b1);

                    WJR_ASSERT(equal(d, d1));

                    cdiv_qr(c, d, a, b);
                    mpz_cdiv_qr(c1, d1, a1, b1);

                    WJR_ASSERT(equal(c, c1));
                    WJR_ASSERT(equal(d, d1));

                    cdiv_q(c, a, b);
                    mpz_cdiv_q(c1, a1, b1);

                    WJR_ASSERT(equal(c, c1));

                    cdiv_r(d, a, b);
                    mpz_cdiv_r(d1, a1, b1);

                    WJR_ASSERT(equal(d, d1));

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

TEST(biginteger, convert) {

    {
        biginteger a;

        auto check = [&a](const auto &str, std::string_view expect,
                          unsigned int base = 10) {
            auto get = [&a](unsigned int base) {
                std::string str;
                to_chars_unchecked(std::back_inserter(str), a, base);
                return str;
            };

            auto sp = wjr::span<const char>(str);
            auto sv = std::string_view(str);

            const bool thr = expect.empty();
            {
                auto ret = wjr::from_chars(sp.begin_unsafe(), sp.end_unsafe(), a, base);
                if (!thr) {
                    WJR_ASSERT((bool)ret);
                    WJR_ASSERT(get(base) == expect);
                } else {
                    WJR_ASSERT(get(base) == "0");
                }
            }

            if (sp.size() != sv.size()) {
                auto ret = wjr::from_chars(sv.data(), sv.data() + sv.size(), a, base);
                if (!thr) {
                    WJR_ASSERT((bool)ret);
                    WJR_ASSERT(get(base) == expect);
                } else {
                    WJR_ASSERT(get(base) == "0");
                }
            }
        };

        check("0", "0");
        check("1", "1");
        check("-1", "-1");
        check("123456789012345678901234567890", "123456789012345678901234567890");
        check("-123456789012345678901234567890", "-123456789012345678901234567890");
        check("    123456789012345678901234567890    ", "123456789012345678901234567890");
        check("    -123456789012345678901234567890    ",
              "-123456789012345678901234567890");
        check("   0000123456789012345678901234567890   ",
              "123456789012345678901234567890");
        check("   0000123456789012345678901234567890a   ",
              "123456789012345678901234567890");

        check("", "");
        check("a", "");
        check("-", "");
        check("0", "", 0);
        check("2", "", 2);
    }
}