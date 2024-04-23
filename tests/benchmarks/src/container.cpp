#include "details.hpp"

#include <wjr/vector.hpp>

using namespace wjr;

template <typename T>
static void wjr_vector_default_construct(benchmark::State &state) {
    for (auto _ : state) {
        vector<T> vec;
        benchmark::DoNotOptimize(vec);
    }
}

template <typename T>
static void wjr_vector_construct_with_size(benchmark::State &state) {
    size_t n = state.range(0);

    for (auto _ : state) {
        vector<T> vec(n);
        benchmark::DoNotOptimize(vec);
    }
}

template <typename T>
static void wjr_vector_construct_with_size_and_value(benchmark::State &state) {
    size_t n = state.range(0);
    T value{};

    for (auto _ : state) {
        vector<T> vec(n, value);
        benchmark::DoNotOptimize(vec);
    }
}

template <typename T>
static void wjr_vector_copy_construct(benchmark::State &state) {
    size_t n = state.range(0);
    vector<T> vec(n);

    for (auto _ : state) {
        vector<T> copy(vec);
        benchmark::DoNotOptimize(copy);
    }
}

template <typename T>
static void wjr_vector_move_construct(benchmark::State &state) {
    size_t n = state.range(0);
    vector<T> vec(n);

    for (auto _ : state) {
        vector<T> copy(std::move(vec));
        benchmark::DoNotOptimize(copy);
    }
}

template <typename T>
static void wjr_vector_emplace_back(benchmark::State &state) {
    size_t n = state.range(0);

    for (auto _ : state) {
        vector<T> vec;
        for (size_t i = 0; i < n; ++i) {
            vec.emplace_back();
        }
        benchmark::DoNotOptimize(vec);
    }
}

BENCHMARK(wjr_vector_default_construct<int>);
BENCHMARK(wjr_vector_default_construct<double>);
BENCHMARK(wjr_vector_default_construct<std::string>);

BENCHMARK(wjr_vector_construct_with_size<int>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_construct_with_size<double>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_construct_with_size<std::string>)->NORMAL_TESTS(1, 2, 1 << 6);

BENCHMARK(wjr_vector_construct_with_size_and_value<int>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_construct_with_size_and_value<double>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_construct_with_size_and_value<std::string>)
    ->NORMAL_TESTS(1, 2, 1 << 6);

BENCHMARK(wjr_vector_copy_construct<int>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_copy_construct<double>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_copy_construct<std::string>)->NORMAL_TESTS(1, 2, 1 << 6);

BENCHMARK(wjr_vector_move_construct<int>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_move_construct<double>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_move_construct<std::string>)->NORMAL_TESTS(1, 2, 1 << 6);

BENCHMARK(wjr_vector_emplace_back<int>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_emplace_back<double>)->NORMAL_TESTS(1, 2, 1 << 6);
BENCHMARK(wjr_vector_emplace_back<std::string>)->NORMAL_TESTS(1, 2, 1 << 6);
