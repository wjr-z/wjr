#include <random>

#include <benchmark/benchmark.h>

static std::mt19937_64 __mt_rand(time(0));
static auto mt_rand = std::ref(__mt_rand);

template <typename Re, typename Func>
void random_run(benchmark::State &state, Re re, Func fn, size_t step = 0) {
    auto count = state.max_iterations;
    if (step == 0) {
        step = std::max<size_t>(16, count / 32);
    }
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

#define NORMAL_TESTS(MULTIPLY, MAXN)                                                     \
    DenseRange(1, 4, 1)->RangeMultiplier(MULTIPLY)->Range(8, MAXN)

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