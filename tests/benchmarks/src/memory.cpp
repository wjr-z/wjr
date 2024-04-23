#include <optional>

#include "details.hpp"

#include <wjr/math.hpp>

static void wjr_stack_allocator_construct(benchmark::State &state) {
    using namespace wjr::math_details;

    for (auto _ : state) {
        unique_stack_alloc stkal(stack_alloc);
        benchmark::DoNotOptimize(stkal);
    }
}

static void wjr_stack_allocator_allocate(benchmark::State &state) {
    using namespace wjr::math_details;
    size_t n = state.range(0);

    random_run(
        state, []() {},
        [&]() {
            unique_stack_alloc stkal(stack_alloc);
            for (size_t i = 0; i < n; ++i) {
                benchmark::DoNotOptimize(stkal.allocate(16));
            }
        });
}

BENCHMARK(wjr_stack_allocator_construct);
BENCHMARK(wjr_stack_allocator_allocate)->NORMAL_TESTS(4, 2, 1024);
