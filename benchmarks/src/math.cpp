#include <charconv>
#include <fstream>
#include <vector>

#include "detail.hpp"
#include <wjr/charconv.hpp>
#include <wjr/math.hpp>

static void wjr_popcount(benchmark::State &state) {
    RandomSeq seq;

    for (auto _ : state) {
        auto p = wjr::popcount(seq());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_clz(benchmark::State &state) {
    RandomSeq seq;

    for (auto _ : state) {
        auto p = wjr::clz(seq());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_ctz(benchmark::State &state) {
    RandomSeq seq;

    for (auto _ : state) {
        auto p = wjr::ctz(seq());
        benchmark::DoNotOptimize(p);
    }
}

static void wjr_addc(benchmark::State &state) {
    RandomSeq a(0, UINT64_MAX, 137);
    RandomSeq b(0, UINT64_MAX, 173);
    RandomSeq cf(0, 1, 171);

    uint64_t c_out;

    for (auto _ : state) {
        auto p = wjr::addc(a(), b(), cf(), c_out);
        benchmark::DoNotOptimize(p);
        benchmark::DoNotOptimize(c_out);
    }
}

static void wjr_addc_cc(benchmark::State &state) {
    RandomSeq a(0, UINT64_MAX, 137);
    RandomSeq b(0, UINT64_MAX, 173);
    RandomSeq cf(0, 1, 171);

    uint8_t c_out;

    for (auto _ : state) {
        auto p = wjr::addc_cc(a(), b(), cf(), c_out);
        benchmark::DoNotOptimize(p);
        benchmark::DoNotOptimize(c_out);
    }
}

static void wjr_subc(benchmark::State &state) {
    RandomSeq a(0, UINT64_MAX, 137);
    RandomSeq b(0, UINT64_MAX, 173);
    RandomSeq cf(0, 1, 171);

    uint64_t c_out;

    for (auto _ : state) {
        auto p = wjr::subc(a(), b(), cf(), c_out);
        benchmark::DoNotOptimize(p);
        benchmark::DoNotOptimize(c_out);
    }
}

static void wjr_subc_cc(benchmark::State &state) {
    RandomSeq a(0, UINT64_MAX, 137);
    RandomSeq b(0, UINT64_MAX, 173);
    RandomSeq cf(0, 1, 171);

    uint8_t c_out;

    for (auto _ : state) {
        auto p = wjr::subc_cc(a(), b(), cf(), c_out);
        benchmark::DoNotOptimize(p);
        benchmark::DoNotOptimize(c_out);
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

static void wjr_to_chars_backward_unchecked(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    uint64_t mask = n == 64 ? (uint64_t)wjr::in_place_max : (1ull << n) - 1;

    char str[64];
    uint64_t x;
    uint64_t sum = 0;

    random_run(
        state,
        [&]() {
            do {
                x = mt_rand();
            } while (!wjr::__has_high_bit(x));
            x &= mask;
        },
        [&]() {
            (void)wjr::to_chars_backward_unchecked(str + 64, x, base);
            benchmark::DoNotOptimize(str);
        });

    benchmark::DoNotOptimize(sum);
}

static void wjr_to_chars(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    uint64_t mask = n == 64 ? (uint64_t)wjr::in_place_max : (1ull << n) - 1;

    char str[64];
    uint64_t x;
    uint64_t sum = 0;

    random_run(
        state,
        [&]() {
            do {
                x = mt_rand();
            } while (!wjr::__has_high_bit(x));
            x &= mask;
        },
        [&]() {
            auto ret = wjr::to_chars(str, str + 64, x, base);
            benchmark::DoNotOptimize(ret);
            benchmark::DoNotOptimize(str);
        });

    benchmark::DoNotOptimize(sum);
}

static void wjr_to_chars_unchecked(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    uint64_t mask = n == 64 ? (uint64_t)wjr::in_place_max : (1ull << n) - 1;

    char str[64];
    uint64_t x;
    uint64_t sum = 0;

    random_run(
        state,
        [&]() {
            do {
                x = mt_rand();
            } while (!wjr::__has_high_bit(x));
            x &= mask;
        },
        [&]() {
            wjr::to_chars_unchecked(str, x, base);
            benchmark::DoNotOptimize(str);
        });

    benchmark::DoNotOptimize(sum);
}

static void fallback_popcount(benchmark::State &state) {
    RandomSeq seq;

    for (auto _ : state) {
        auto p = wjr::fallback_popcount(seq());
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_clz(benchmark::State &state) {
    RandomSeq seq;

    for (auto _ : state) {
        auto p = wjr::fallback_clz(seq());
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_ctz(benchmark::State &state) {
    RandomSeq seq;

    for (auto _ : state) {
        auto p = wjr::fallback_ctz(seq());
        benchmark::DoNotOptimize(p);
    }
}

static void fallback_addc(benchmark::State &state) {
    RandomSeq a(0, UINT64_MAX, 137);
    RandomSeq b(0, UINT64_MAX, 173);
    RandomSeq cf(0, 1, 171);

    uint64_t c_out;

    for (auto _ : state) {
        auto p = wjr::math::fallback_addc(a(), b(), cf(), c_out);
        benchmark::DoNotOptimize(p);
        benchmark::DoNotOptimize(c_out);
    }
}

static void fallback_subc(benchmark::State &state) {
    RandomSeq a(0, UINT64_MAX, 137);
    RandomSeq b(0, UINT64_MAX, 173);
    RandomSeq cf(0, 1, 171);

    uint64_t c_out;

    for (auto _ : state) {
        auto p = wjr::math::fallback_subc(a(), b(), cf(), c_out);
        benchmark::DoNotOptimize(p);
        benchmark::DoNotOptimize(c_out);
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

static void fallback_to_chars(benchmark::State &state) {
    auto base = state.range(0);
    auto n = state.range(1);
    uint64_t mask = n == 64 ? (uint64_t)wjr::in_place_max : (1ull << n) - 1;

    char str[64];
    uint64_t x;
    uint64_t sum = 0;

    random_run(
        state,
        [&]() {
            do {
                x = mt_rand();
            } while (!wjr::__has_high_bit(x));
            x &= mask;
        },
        [&]() {
            auto ret = std::to_chars(str, str + 64, x, base);
            benchmark::DoNotOptimize(ret);
            benchmark::DoNotOptimize(str);
        });

    benchmark::DoNotOptimize(sum);
}

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

BENCHMARK(wjr_popcount);
BENCHMARK(wjr_clz);
BENCHMARK(wjr_ctz);
BENCHMARK(wjr_addc);
BENCHMARK(wjr_addc_cc);
BENCHMARK(wjr_subc);
BENCHMARK(wjr_subc_cc);
BENCHMARK(wjr_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_reverse_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_reverse_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_find_n_ptr)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_find_n_ptr)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_find_n_val)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_find_n_val)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_find_not_n_ptr)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_find_not_n_ptr)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_find_not_n_val)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_find_not_n_val)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_replace_find_not)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_worst_replace_find_not)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_set_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_lshift_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_rshift_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(wjr_to_chars_backward_unchecked)->Apply(to_chars_tests);
BENCHMARK(wjr_to_chars)->Apply(to_chars_tests);
BENCHMARK(wjr_to_chars_unchecked)->Apply(to_chars_tests);

BENCHMARK(fallback_popcount);
BENCHMARK(fallback_clz);
BENCHMARK(fallback_ctz);
BENCHMARK(fallback_addc);
BENCHMARK(fallback_subc);
BENCHMARK(fallback_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(fallback_worst_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(fallback_reverse_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(fallback_worst_reverse_compare_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(fallback_lshift_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(fallback_rshift_n)->NORMAL_TESTS(4, 2, 256);
BENCHMARK(fallback_to_chars)->Apply(to_chars_tests);