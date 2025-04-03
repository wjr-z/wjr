#include <charconv>
#include <vector>

#include "detail.hpp"

#include <wjr/biginteger.hpp>
#include <wjr/math.hpp>

#include <fstream>

#ifdef WJR_USE_GMP
    #include <gmp.h>
#endif

static void wjr_addc_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::addc_1(c.data(), a.data(), n, b[i]);
            benchmark::DoNotOptimize(p);
            i = i == N - 1 ? 0 : i + 1;
        });
}

static void wjr_worst_addc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    for (auto &i : a) {
        i = -1ul;
    }

    for (auto _ : state) {
        auto p = wjr::addc_1(c.data(), a.data(), n, 1);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_addc_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        auto p = wjr::addc_n(c.data(), a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_subc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    RandomSeq b;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::subc_1(c.data(), a.data(), n, b());
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_subc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    for (auto &i : a) {
        i = -1ul;
    }

    for (auto _ : state) {
        auto p = wjr::subc_1(c.data(), a.data(), n, 1);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_subc_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        auto p = wjr::subc_n(c.data(), a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_not_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        wjr::math::bi_not_n(b.data(), a.data(), n);
    }
}

static void wjr_negate_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        wjr::math::bi_negate_n(b.data(), a.data(), n);
    }
}

static void wjr_mul_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b;

    for (auto _ : state) {
        auto p = wjr::mul_1(c.data(), a.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_addmul_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b;

    for (auto _ : state) {
        auto p = wjr::addmul_1(c.data(), a.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_submul_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b;

    for (auto _ : state) {
        auto p = wjr::submul_1(c.data(), a.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_addlsh_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n), d(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b(0, 63);

    for (auto _ : state) {
        auto p = wjr::addlsh_n(d.data(), a.data(), c.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_rsblsh_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n), d(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b(0, 63);

    for (auto _ : state) {
        auto p = wjr::rsblsh_n(d.data(), a.data(), c.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_mul(benchmark::State &state) {
    auto n = state.range(0);
    auto m = state.range(1);
    std::vector<uint64_t> a(n), b(m), c(n + m);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        wjr::mul_s(c.data(), a.data(), n, b.data(), m);
    }
}

static void wjr_mul_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n), c(n * 2);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        wjr::mul_n(c.data(), a.data(), b.data(), n);
    }
}

static void wjr_sqr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n * 2);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        wjr::sqr(c.data(), a.data(), n);
    }
}

static void wjr_div_qr_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), q(n);
    uint64_t rem;

    RandomSeq b(1);

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            wjr::div_qr_1(q.data(), rem, a.data(), n, b());
            benchmark::DoNotOptimize(rem);
        });
}

static void wjr_div_qr_2(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 137;
    std::vector<uint64_t> a(n), b(N + 1), q(n), r(2);

    RandomSeq seq(1);
    std::generate(b.begin(), b.end(), seq);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            wjr::div_qr_2(q.data(), r.data(), a.data(), n, b.data());
            benchmark::DoNotOptimize(r);
            i = i == N - 1 ? 0 : i + 1;
        });
}

static void wjr_div_qr_s(benchmark::State &state) {
    auto n = state.range(0);
    auto m = state.range(1);
    std::vector<uint64_t> a(n), b(m), q(n), r(m);

    RandomSeq seq(1);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), seq);
        },
        [&]() { wjr::div_qr_s(q.data(), r.data(), a.data(), n, b.data(), m); });
}

static void wjr_biginteger_to_chars(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    std::vector<uint64_t> a(n);
    std::vector<char> s(n * 64);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        wjr::biginteger_to_chars(s.data(), a.data(), n, base);
    }
}

static void wjr_biginteger_from_chars(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    std::vector<uint64_t> a(n);
    std::vector<char> s(n);

    std::generate(s.begin(), s.end(), []() { return '0' + mt_rand() % 10; });

    for (auto _ : state) {
        wjr::biginteger_from_chars(s.data(), s.data() + s.size(), a.data(), base);
    }
}

inline void random(wjr::biginteger &a, size_t n) {
    if (n == 0) {
        a.clear();
        return;
    }
    urandom_exact_bit(a, (n - 1) * 64 + mt_rand() % 64, __mt_rand);
}

static void wjr_bi_add(benchmark::State &state) {
    auto n = state.range(0);
    wjr::biginteger a, b, c;
    random(a, n);
    random(b, n);

    for (auto _ : state) {
        add(c, a, b);
    }
}

static void wjr_bi_add_ui(benchmark::State &state) {
    static int m = 1e3;
    auto n = state.range(0);
    wjr::vector<uint64_t> vec(m);
}

BENCHMARK(wjr_bi_add)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_bi_add_ui)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_ui_add)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_sub)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_sub_ui)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_ui_sub)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_mul)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_mul_ui)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_addmul)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_submul)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_div)->Apply(Product2D);
// BENCHMARK(wjr_bi_to_chars)->Apply(biginteger_to_chars_tests);
// BENCHMARK(wjr_bi_from_chars)->BIGINTEGER_FROM_CHARS_TESTS();

#ifdef WJR_USE_GMP

static void gmp_addc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    RandomSeq b;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = mpn_add_1(c.data(), a.data(), n, b());
            benchmark::DoNotOptimize(p);
        });
}

static void gmp_worst_addc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    for (auto &i : a) {
        i = -1ul;
    }

    for (auto _ : state) {
        auto p = mpn_add_1(c.data(), a.data(), n, 1);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_addc_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        auto p = mpn_add_n(c.data(), a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_subc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    RandomSeq b;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = mpn_sub_1(c.data(), a.data(), n, b());
            benchmark::DoNotOptimize(p);
        });
}

static void gmp_worst_subc_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    for (auto &i : a) {
        i = -1ul;
    }

    for (auto _ : state) {
        auto p = mpn_sub_1(c.data(), a.data(), n, 1);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_subc_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        auto p = mpn_sub_n(c.data(), a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_lshift_n(benchmark::State &state) {
    unsigned int k = 1;
    benchmark::DoNotOptimize(k);
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        auto p = mpn_lshift(c.data(), a.data(), n, k);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_rshift_n(benchmark::State &state) {
    unsigned int k = 1;
    benchmark::DoNotOptimize(k);
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        auto p = mpn_rshift(c.data(), a.data(), n, k);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_mul_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b;

    for (auto _ : state) {
        auto p = mpn_mul_1(c.data(), a.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_addmul_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b;

    for (auto _ : state) {
        auto p = mpn_addmul_1(c.data(), a.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_submul_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    RandomSeq b;

    for (auto _ : state) {
        auto p = mpn_submul_1(c.data(), a.data(), n, b());
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_mul(benchmark::State &state) {
    auto n = state.range(0);
    auto m = state.range(1);
    std::vector<uint64_t> a(n), b(m), c(n + m);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        mpn_mul(c.data(), a.data(), n, b.data(), m);
    }
}

static void gmp_mul_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n), c(n * 2);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    for (auto _ : state) {
        mpn_mul_n(c.data(), a.data(), b.data(), n);
    }
}

static void gmp_sqr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n * 2);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        mpn_sqr(c.data(), a.data(), n);
    }
}

static void gmp_div_qr_1(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), q(n);
    uint64_t rem;

    RandomSeq b(1);

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            rem = mpn_div_qr_1(q.data(), &q[n - 1], a.data(), n, b());
            benchmark::DoNotOptimize(rem);
        });
}

static void gmp_div_qr_2(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 137;
    std::vector<uint64_t> a(n), b(N + 1), q(n), r(2);

    RandomSeq seq(1);
    std::generate(b.begin(), b.end(), seq);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            q[n - 1] = mpn_div_qr_2(q.data(), r.data(), a.data(), n, b.data());
            benchmark::DoNotOptimize(r);
            i = i == N - 1 ? 0 : i + 1;
        });
}

static void gmp_div_qr_s(benchmark::State &state) {
    auto n = state.range(0);
    auto m = state.range(1);
    std::vector<uint64_t> a(n), b(m), q(n), r(m);

    RandomSeq seq(1);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), seq);
        },
        [&]() { mpn_tdiv_qr(q.data(), r.data(), 0, a.data(), n, b.data(), m); });
}

static void gmp_biginteger_to_chars(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    std::vector<uint64_t> a(n);
    std::vector<char> s(n * 64);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        mpn_get_str((unsigned char *)s.data(), base, a.data(), n);
    }
}

static void gmp_biginteger_from_chars(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    std::vector<uint64_t> a(n);
    std::vector<char> s(n);

    std::generate(s.begin(), s.end(), []() { return '0' + mt_rand() % 10; });

    for (auto _ : state) {
        mpn_set_str(a.data(), (unsigned char *)s.data(), s.size(), base);
    }
}

#endif // WJR_USE_GMP

static void to_chars_tests(benchmark::internal::Benchmark *state) {
    for (int base : {2, 8, 16, 10}) {
        for (int n = 1; n <= 8; ++n) {
            state->Args({base, n});
        }
        for (int n = 10; n <= 16; n += 2) {
            state->Args({base, n});
        }
        for (int n = 32; n <= 64; n += 8) {
            state->Args({base, n});
        }
    }
}

static void biginteger_to_chars_tests(benchmark::internal::Benchmark *state) {
    for (int base : {2, 8, 16, 10}) {
        for (int n = 1; n <= 4; ++n) {
            state->Args({base, n});
        }
        for (int n = 8; n <= 14; n += 2) {
            state->Args({base, n});
        }
        for (int n = 16; n <= 1024; n *= 2) {
            state->Args({base, n});
        }
    }
}

#define __BIGINTEGER_FROM_CHARS_TESTS_I(base)                                                      \
    Args({base, 1})                                                                                \
        ->Args({base, 2})                                                                          \
        ->Args({base, 4})                                                                          \
        ->Args({base, 8})                                                                          \
        ->Args({base, 16})                                                                         \
        ->Args({base, 32})                                                                         \
        ->Args({base, 64})                                                                         \
        ->Args({base, 256})                                                                        \
        ->Args({base, 1024})                                                                       \
        ->Args({base, 4096})                                                                       \
        ->Args({base, 16384})                                                                      \
        ->Args({base, 65536})
#define BIGINTEGER_FROM_CHARS_TESTS()                                                              \
    __BIGINTEGER_FROM_CHARS_TESTS_I(2)                                                             \
        ->__BIGINTEGER_FROM_CHARS_TESTS_I(8)                                                       \
        ->__BIGINTEGER_FROM_CHARS_TESTS_I(10)                                                      \
        ->__BIGINTEGER_FROM_CHARS_TESTS_I(16)

BENCHMARK(wjr_addc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_addc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_addc_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_subc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_subc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_subc_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_not_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_negate_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_mul_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_addmul_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_submul_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_addlsh_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_rsblsh_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_mul)->Apply(Product2D);
BENCHMARK(wjr_mul_n)->NORMAL_TESTS(4, 2, 1024);
BENCHMARK(wjr_sqr)->NORMAL_TESTS(4, 2, 1024);
BENCHMARK(wjr_div_qr_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_div_qr_2)->DenseRange(2, 4, 1)->RangeMultiplier(2)->Range(8, 256);
BENCHMARK(wjr_div_qr_s)->Apply(Product2D);
BENCHMARK(wjr_biginteger_to_chars)->Apply(biginteger_to_chars_tests);
BENCHMARK(wjr_biginteger_from_chars)->BIGINTEGER_FROM_CHARS_TESTS();

BENCHMARK(wjr_bi_add)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_bi_add_ui)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_ui_add)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_sub)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_sub_ui)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_ui_sub)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_mul)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_mul_ui)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_addmul)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_submul)->NORMAL_TESTS(4, 2, 256);
// BENCHMARK(wjr_bi_div)->Apply(Product2D);
// BENCHMARK(wjr_bi_to_chars)->Apply(biginteger_to_chars_tests);
// BENCHMARK(wjr_bi_from_chars)->BIGINTEGER_FROM_CHARS_TESTS();

#ifdef WJR_USE_GMP

BENCHMARK(gmp_addc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_worst_addc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_addc_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_subc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_worst_subc_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_subc_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_lshift_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_rshift_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_mul_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_addmul_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_submul_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_mul)->Apply(Product2D);
BENCHMARK(gmp_mul_n)->NORMAL_TESTS(4, 2, 1024);
BENCHMARK(gmp_sqr)->NORMAL_TESTS(4, 2, 1024);
BENCHMARK(gmp_div_qr_1)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(gmp_div_qr_2)->DenseRange(2, 4, 1)->RangeMultiplier(2)->Range(8, 256);
BENCHMARK(gmp_div_qr_s)->Apply(Product2D);
BENCHMARK(gmp_biginteger_to_chars)->Apply(biginteger_to_chars_tests);
BENCHMARK(gmp_biginteger_from_chars)->BIGINTEGER_FROM_CHARS_TESTS();

#endif // WJR_USE_GMP
