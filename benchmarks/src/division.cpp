#include <array>
#include <cstdint>

#include "detail.hpp"
#include <wjr/math.hpp>

namespace {

struct dividend_type {
    uint64_t low;
    uint64_t high;
};

constexpr std::array<dividend_type, 4> dividends = {{
    {0xfedcba9876543210ULL, 0x0123456789abcdefULL},
    {0x0123456789abcdefULL, 0xfedcba9876543210ULL},
    {0xffff0000ffff0001ULL, 0x0000ffff0000ffffULL},
    {0x13579bdf2468ace0ULL, 0xfdb97531eca86420ULL},
}};

constexpr uint64_t shifted_divisor = 0x0123456789abcdefULL;
constexpr uint64_t normalized_divisor = 0x8123456789abcdefULL;

template <typename Function>
void benchmark_division(benchmark::State &state, uint64_t divisor, Function &&function) {
    size_t index = 0;
    for (auto _ : state) {
        const auto value = dividends[index];
        index = (index + 1) % dividends.size();
        function(value, divisor);
    }
}

template <uint64_t Divisor>
void wjr_divider_reused(benchmark::State &state) {
    const wjr::div2by1_divider<uint64_t> divider(Divisor);
    benchmark_division(state, Divisor, [&](dividend_type value, uint64_t) {
        uint64_t rem;
        const auto quotient = wjr::div128by64to128(rem, value.low, value.high, divider);
        benchmark::DoNotOptimize(quotient);
        benchmark::DoNotOptimize(rem);
    });
}

template <uint64_t Divisor>
void wjr_divider_initialized(benchmark::State &state) {
    benchmark_division(state, Divisor, [&](dividend_type value, uint64_t divisor) {
        const wjr::div2by1_divider<uint64_t> divider(divisor);
        uint64_t rem;
        const auto quotient = wjr::div128by64to128(rem, value.low, value.high, divider);
        benchmark::DoNotOptimize(quotient);
        benchmark::DoNotOptimize(rem);
    });
}

template <uint64_t Divisor>
void wjr_divider_convenience(benchmark::State &state) {
    benchmark_division(state, Divisor, [&](dividend_type value, uint64_t divisor) {
        uint64_t rem;
        const auto quotient = wjr::div128by64to128(rem, value.low, value.high, divisor);
        benchmark::DoNotOptimize(quotient);
        benchmark::DoNotOptimize(rem);
    });
}

#if WJR_HAS_FEATURE(INT128_DIV)
template <uint64_t Divisor>
void native_division(benchmark::State &state) {
    benchmark_division(state, Divisor, [](dividend_type value, uint64_t divisor) {
        const auto dividend = (static_cast<__uint128_t>(value.high) << 64) | value.low;
        const auto quotient = dividend / divisor;
        const auto remainder = dividend % divisor;
        benchmark::DoNotOptimize(quotient);
        benchmark::DoNotOptimize(remainder);
    });
}
#endif

} // namespace

BENCHMARK(wjr_divider_reused<shifted_divisor>);
BENCHMARK(wjr_divider_initialized<shifted_divisor>);
BENCHMARK(wjr_divider_convenience<shifted_divisor>);
BENCHMARK(wjr_divider_reused<normalized_divisor>);
BENCHMARK(wjr_divider_initialized<normalized_divisor>);
BENCHMARK(wjr_divider_convenience<normalized_divisor>);

#if WJR_HAS_FEATURE(INT128_DIV)
BENCHMARK(native_division<shifted_divisor>);
BENCHMARK(native_division<normalized_divisor>);
#endif