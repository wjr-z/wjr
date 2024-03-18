#include <random>

#include <benchmark/benchmark.h>
#include <gmp.h>
#include <wjr/math.hpp>

static std::mt19937_64 __mt_rand(time(0));
static auto mt_rand = std::ref(__mt_rand);

template <typename Re, typename Func>
void random_run(benchmark::State &state, Re re, Func fn) {
    auto count = state.max_iterations;
    auto step = std::max<size_t>(16, count / 32);
    auto begin = state.begin();
    for (size_t i = 0; i < count; ++i) {
        ++begin;
    }

    auto end = state.end();

    while (count > step) {
        state.PauseTiming();
        re();
        state.ResumeTiming();
        for (int i = 0; i < step; ++i) {
            fn();
        }
        count -= step;
    }

    while (--count) {
        fn();
    }

    (void)(begin != end);
}

static void wjr_popcount(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n);

    std::generate(a.begin(), a.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::popcount(a[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void wjr_clz(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n);

    std::generate(a.begin(), a.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::clz(a[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void wjr_ctz(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n);

    std::generate(a.begin(), a.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::ctz(a[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void wjr_addc(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n), b(n), cf(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);
    std::generate(cf.begin(), cf.end(), []() { return mt_rand() & 1; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::addc(a[i], b[i], cf[i], cf[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void wjr_addc_cc(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n), b(n);
    std::vector<uint8_t> cf(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);
    std::generate(cf.begin(), cf.end(), []() { return mt_rand() & 1; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::addc_cc(a[i], b[i], cf[i], cf[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

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

static void wjr_subc(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n), b(n), cf(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);
    std::generate(cf.begin(), cf.end(), []() { return mt_rand() & 1; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::subc(a[i], b[i], cf[i], cf[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void wjr_subc_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::subc_1(c.data(), a.data(), n, b[i]);
            benchmark::DoNotOptimize(p);
            i = i == N - 1 ? 0 : i + 1;
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

static void wjr_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::compare_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    b = a;

    for (auto _ : state) {
        auto p = wjr::compare_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_reverse_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::reverse_compare_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_reverse_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    b = a;

    for (auto _ : state) {
        auto p = wjr::reverse_compare_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_find_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::find_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_find_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    for (int64_t i = 0; i < n; ++i) {
        b[i] = a[i] + 1;
    }

    for (auto _ : state) {
        auto p = wjr::find_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_find_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::find_n(a.data(), b, n);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_find_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    std::generate(a.begin(), a.end(), [b]() {
        auto x = mt_rand();
        while (x == b) {
            x = mt_rand();
        }
        return x;
    });

    for (auto _ : state) {
        auto p = wjr::find_n(a.data(), b, n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_find_not_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::find_not_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_find_not_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    b = a;

    for (auto _ : state) {
        auto p = wjr::find_not_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_find_not_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::find_not_n(a.data(), b, n);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_find_not_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    for (auto &i : a) {
        i = b;
    }

    for (auto _ : state) {
        auto p = wjr::find_not_n(a.data(), b, n);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_replace_find_not(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::replace_find_not(b.data(), a.data(), n, -1ul, 0);
            benchmark::DoNotOptimize(p);
        });
}

static void wjr_worst_replace_find_not(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), []() { return -1ul; });

    for (auto _ : state) {
        auto p = wjr::replace_find_not(b.data(), a.data(), n, -1ul, 0);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_not_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        wjr::not_n(b.data(), a.data(), n);
    }
}

static void wjr_neg_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        wjr::neg_n(b.data(), a.data(), n);
    }
}

static void wjr_set_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);

    for (auto _ : state) {
        wjr::set_n(a.data(), -1ul, n);
    }
}

static void wjr_lshift_n(benchmark::State &state) {
    unsigned int k = 1;
    benchmark::DoNotOptimize(k);
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        auto p = wjr::lshift_n(c.data(), a.data(), n, k);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_rshift_n(benchmark::State &state) {
    unsigned int k = 1;
    benchmark::DoNotOptimize(k);
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        auto p = wjr::rshift_n(c.data(), a.data(), n, k);
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_mul_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::mul_1(c.data(), a.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
    }
}

static void wjr_addmul_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::addmul_1(c.data(), a.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
    }
}

static void wjr_submul_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::submul_1(c.data(), a.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
    }
}

static void wjr_addlsh_n(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n), d(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), []() { return mt_rand() % 64; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::addlsh_n(d.data(), a.data(), c.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
    }
}

static void wjr_rsblsh_n(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n), d(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), []() { return mt_rand() % 64; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::rsblsh_n(d.data(), a.data(), c.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
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
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), q(n);
    uint64_t rem;

    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            wjr::div_qr_1(q.data(), rem, a.data(), n, b[i]);
            benchmark::DoNotOptimize(rem);
            i = i == N - 1 ? 0 : i + 1;
        });
}

static void wjr_div_qr_2(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N + 1), q(n), r(2);

    std::generate(b.begin(), b.end(), mt_rand);

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

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() { wjr::div_qr_s(q.data(), r.data(), a.data(), n, b.data(), m); });
}

static void fallback_popcount(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n);

    std::generate(a.begin(), a.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::fallback_popcount(a[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void fallback_clz(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n);

    std::generate(a.begin(), a.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::fallback_clz(a[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void fallback_ctz(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n);

    std::generate(a.begin(), a.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::fallback_ctz(a[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void fallback_addc(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n), b(n), cf(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);
    std::generate(cf.begin(), cf.end(), []() { return mt_rand() & 1; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::fallback_addc(a[i], b[i], cf[i], cf[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void fallback_subc(benchmark::State &state) {
    const int n = 17;
    std::vector<uint64_t> a(n), b(n), cf(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);
    std::generate(cf.begin(), cf.end(), []() { return mt_rand() & 1; });

    int64_t i = 0;

    for (auto _ : state) {
        auto p = wjr::fallback_subc(a[i], b[i], cf[i], cf[i]);
        benchmark::DoNotOptimize(p);
        i = i == n - 1 ? 0 : i + 1;
    }
}

static void fallback_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::fallback_compare_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void fallback_worst_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    b = a;

    for (auto _ : state) {
        auto p = wjr::fallback_compare_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_reverse_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::fallback_reverse_compare_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void fallback_worst_reverse_compare_n(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    b = a;

    for (auto _ : state) {
        auto p = wjr::fallback_reverse_compare_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_find_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::fallback_find_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void fallback_worst_find_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    for (int64_t i = 0; i < n; ++i) {
        b[i] = a[i] + 1;
    }

    for (auto _ : state) {
        auto p = wjr::fallback_find_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_find_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::fallback_find_n(a.data(), b, n);
            benchmark::DoNotOptimize(p);
        });
}

static void fallback_worst_find_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    std::generate(a.begin(), a.end(), [b]() {
        auto x = mt_rand();
        while (x == b) {
            x = mt_rand();
        }
        return x;
    });

    for (auto _ : state) {
        auto p = wjr::fallback_find_n(a.data(), b, n);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_find_not_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() {
            auto p = wjr::fallback_find_not_n(a.data(), b.data(), n);
            benchmark::DoNotOptimize(p);
        });
}

static void fallback_worst_find_not_n_ptr(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n), b(n);

    std::generate(a.begin(), a.end(), mt_rand);
    b = a;

    for (auto _ : state) {
        auto p = wjr::fallback_find_not_n(a.data(), b.data(), n);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_find_not_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = wjr::fallback_find_not_n(a.data(), b, n);
            benchmark::DoNotOptimize(p);
        });
}

static void fallback_worst_find_not_n_val(benchmark::State &state) {
    auto n = state.range(0);
    std::vector<uint64_t> a(n);
    uint64_t b = mt_rand();

    for (auto &i : a) {
        i = b;
    }

    for (auto _ : state) {
        auto p = wjr::fallback_find_not_n(a.data(), b, n);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_lshift_n(benchmark::State &state) {
    unsigned int k = 1;
    benchmark::DoNotOptimize(k);
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        auto p = wjr::fallback_lshift_n<uint64_t>(c.data(), a.data(), n, k, 0);
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_rshift_n(benchmark::State &state) {
    unsigned int k = 1;
    benchmark::DoNotOptimize(k);
    auto n = state.range(0);
    std::vector<uint64_t> a(n), c(n);

    std::generate(a.begin(), a.end(), mt_rand);

    for (auto _ : state) {
        auto p = wjr::fallback_rshift_n<uint64_t>(c.data(), a.data(), n, k, 0);
        benchmark::DoNotOptimize(p);
    }
}

static void gmp_addc_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = mpn_add_1(c.data(), a.data(), n, b[i]);
            benchmark::DoNotOptimize(p);
            i = i == N - 1 ? 0 : i + 1;
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
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            auto p = mpn_sub_1(c.data(), a.data(), n, b[i]);
            benchmark::DoNotOptimize(p);
            i = i == N - 1 ? 0 : i + 1;
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
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = mpn_mul_1(c.data(), a.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
    }
}

static void gmp_addmul_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = mpn_addmul_1(c.data(), a.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
    }
}

static void gmp_submul_1(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), c(n);

    std::generate(a.begin(), a.end(), mt_rand);
    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    for (auto _ : state) {
        auto p = mpn_submul_1(c.data(), a.data(), n, b[i]);
        benchmark::DoNotOptimize(p);
        i = i == N - 1 ? 0 : i + 1;
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
    const int N = 13;
    std::vector<uint64_t> a(n), b(N), q(n);
    uint64_t rem;

    std::generate(b.begin(), b.end(), mt_rand);

    int64_t i = 0;

    random_run(
        state, [&]() { std::generate(a.begin(), a.end(), mt_rand); },
        [&]() {
            rem = mpn_div_qr_1(q.data(), &q[n - 1], a.data(), n, b[i]);
            benchmark::DoNotOptimize(rem);
            i = i == N - 1 ? 0 : i + 1;
        });
}

static void gmp_div_qr_2(benchmark::State &state) {
    auto n = state.range(0);
    const int N = 13;
    std::vector<uint64_t> a(n), b(N + 1), q(n), r(2);

    std::generate(b.begin(), b.end(), mt_rand);

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

    random_run(
        state,
        [&]() {
            std::generate(a.begin(), a.end(), mt_rand);
            std::generate(b.begin(), b.end(), mt_rand);
        },
        [&]() { mpn_tdiv_qr(q.data(), r.data(), 0, a.data(), n, b.data(), m); });
}

static void Product2D(benchmark::internal::Benchmark *state) {
    // 生成二维数据，且第一维大于等于第二维
    // 生成[1, 4] * [1, 4]的二维测试数据
    for (int i = 1; i <= 4; ++i) {
        for (int j = 1; j <= i; ++j) {
            state->Args({i, j});
        }
    }

    for (int i = 8; i <= 128; i *= 2) {
        for (int j = 1; j <= i; j *= 2) {
            state->Args({i, j});
        }
    }

    for (int i = 256; i <= 1024; i *= 4) {
        for (int j = 1; j <= i; j *= 4) {
            state->Args({i, j});
        }
    }
}

#define NORMAL_TESTS(MULTIPLY, MAXN)                                                     \
    DenseRange(1, 4, 1)->RangeMultiplier(MULTIPLY)->Range(8, MAXN)

BENCHMARK(wjr_popcount);
BENCHMARK(wjr_clz);
BENCHMARK(wjr_ctz);
BENCHMARK(wjr_addc);
BENCHMARK(wjr_addc_cc);
BENCHMARK(wjr_addc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_addc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_addc_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_subc);
BENCHMARK(wjr_subc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_subc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_subc_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_reverse_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_reverse_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_find_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_find_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_find_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_find_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_find_not_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_find_not_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_find_not_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_find_not_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_replace_find_not)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_worst_replace_find_not)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_not_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_neg_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_set_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_lshift_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_rshift_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_mul_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_addmul_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_submul_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_addlsh_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_rsblsh_n)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_mul)->Apply(Product2D);
BENCHMARK(wjr_mul_n)->NORMAL_TESTS(2, 1024);
BENCHMARK(wjr_sqr)->NORMAL_TESTS(2, 1024);
BENCHMARK(wjr_div_qr_1)->NORMAL_TESTS(2, 256);
BENCHMARK(wjr_div_qr_2)->DenseRange(2, 4, 1)->RangeMultiplier(2)->Range(8, 256);
BENCHMARK(wjr_div_qr_s)->Apply(Product2D);

BENCHMARK(fallback_popcount);
BENCHMARK(fallback_clz);
BENCHMARK(fallback_ctz);
BENCHMARK(fallback_addc);
BENCHMARK(fallback_subc);
BENCHMARK(fallback_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_worst_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_reverse_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_worst_reverse_compare_n)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_find_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_worst_find_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_find_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_worst_find_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_find_not_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_worst_find_not_n_ptr)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_find_not_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_worst_find_not_n_val)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_lshift_n)->NORMAL_TESTS(2, 256);
BENCHMARK(fallback_rshift_n)->NORMAL_TESTS(2, 256);

BENCHMARK(gmp_addc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_worst_addc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_addc_n)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_subc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_worst_subc_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_subc_n)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_lshift_n)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_rshift_n)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_mul_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_addmul_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_submul_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_mul)->Apply(Product2D);
BENCHMARK(gmp_mul_n)->NORMAL_TESTS(2, 1024);
BENCHMARK(gmp_sqr)->NORMAL_TESTS(2, 1024);
BENCHMARK(gmp_div_qr_1)->NORMAL_TESTS(2, 256);
BENCHMARK(gmp_div_qr_2)->DenseRange(2, 4, 1)->RangeMultiplier(2)->Range(8, 256);
BENCHMARK(gmp_div_qr_s)->Apply(Product2D);

#undef NORMAL_TESTS

BENCHMARK_MAIN();