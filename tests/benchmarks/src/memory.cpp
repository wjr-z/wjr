#include <optional>

#include "detail.hpp"

#include <wjr/memory/stack_allocator.hpp>

using namespace wjr;

static void wjr_unique_stack_allocator_construct(benchmark::State &state) {
    for (auto _ : state) {
        unique_stack_allocator stkal;
        benchmark::DoNotOptimize(stkal);
    }
}

static void wjr_unique_stack_allocator_allocate(benchmark::State &state) {
    size_t n = state.range(0);

    random_run(
        state, []() {},
        [&]() {
            unique_stack_allocator stkal;
            for (size_t i = 0; i < n; ++i) {
                benchmark::DoNotOptimize(stkal.allocate(16));
            }
        });
}

static void wjr_weak_stack_allocator_allocate(benchmark::State &state) {
    size_t n = state.range(0);

    random_run(
        state, []() {},
        [&]() {
            unique_stack_allocator stkal;
            for (size_t i = 0; i < n; ++i) {
                weak_stack_allocator<uint64_t> al(stkal);
                auto ptr = al.allocate(4);
                benchmark::DoNotOptimize(ptr);
                al.deallocate(ptr, 4);
            }
        });
}

BENCHMARK(wjr_unique_stack_allocator_construct);
BENCHMARK(wjr_unique_stack_allocator_allocate)->NORMAL_TESTS(4, 2, 1024);
BENCHMARK(wjr_weak_stack_allocator_allocate)->NORMAL_TESTS(4, 2, 1024);
