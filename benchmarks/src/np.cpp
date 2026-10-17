#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/config/attribute.hpp>
#include <wjr/np/expression.hpp>

#if defined(WJR_BENCHMARK_XSIMD)
    #include <wjr/np/expression_xsimd.hpp>
#endif

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 1>;
using input = np::tensor_view<const double, shape>;
using output = np::tensor_view<double, shape>;

struct manual {};
struct expression {};
struct manual_simd {};
struct expression_simd {};

template <bool Square>
constexpr auto make_expression(input x, input y, double alpha, double beta) noexcept {
    if constexpr (Square) {
        const auto sum = np::as_expr(x) + np::as_expr(y);
        return sum * sum;
    } else {
        return alpha * np::as_expr(x) + beta * np::as_expr(y);
    }
}

template <bool Square, class T>
WJR_INTRINSIC_INLINE auto apply(const T &x, const T &y, const T &alpha, const T &beta) noexcept {
    if constexpr (Square) {
        const auto sum = x + y;
        return sum * sum;
    } else {
        return alpha * x + beta * y;
    }
}

template <class Backend, bool Square>
WJR_INTRINSIC_INLINE void evaluate(double *dst, const double *x, const double *y, size_t count,
                                   double alpha, double beta) noexcept {
    if constexpr (std::same_as<Backend, manual>) {
        for (size_t i = 0; i != count; ++i)
            dst[i] = apply<Square>(x[i], y[i], alpha, beta);
    } else if constexpr (std::same_as<Backend, expression>) {
        np::eval_to(output(dst, count),
                    make_expression<Square>(input(x, count), input(y, count), alpha, beta));
#if defined(WJR_BENCHMARK_XSIMD)
    } else if constexpr (std::same_as<Backend, expression_simd>) {
        np::eval_to(output(dst, count),
                    make_expression<Square>(input(x, count), input(y, count), alpha, beta),
                    np::xsimd_policy<>{});
    } else {
        using batch = xsimd::batch<double>;
        const batch a(alpha), b(beta);
        size_t i = 0;
        for (; count - i >= batch::size; i += batch::size)
            apply<Square>(batch::load_unaligned(x + i), batch::load_unaligned(y + i), a, b)
                .store_unaligned(dst + i);
        for (; i != count; ++i)
            dst[i] = apply<Square>(x[i], y[i], alpha, beta);
#endif
    }
}

// Both sides cross the same scalar/pointer call boundary. The expression side
// includes real mdspan construction, value capture and per-call binding.
template <class Backend, bool Square>
WJR_NOINLINE void construct_and_evaluate(double *dst, const double *x, const double *y,
                                         size_t count, double alpha, double beta) noexcept {
    evaluate<Backend, Square>(dst, x, y, count, alpha, beta);
}

struct buffers {
    std::vector<double> first, second, actual, expected;

    explicit buffers(size_t count) : first(count), second(count), actual(count), expected(count) {
        for (size_t i = 0; i != count; ++i) {
            first[i] = static_cast<double>(i % 17) / 4.0;
            second[i] = static_cast<double>(i % 23) / 8.0;
        }
    }

    template <class Backend, bool Square>
    bool verify(size_t count) {
        evaluate<manual, Square>(expected.data(), first.data(), second.data(), count, 1.5, -0.5);
        evaluate<Backend, Square>(actual.data(), first.data(), second.data(), count, 1.5, -0.5);
        return actual == expected;
    }
};

template <class Backend>
void label(benchmark::State &state) {
#if defined(WJR_BENCHMARK_XSIMD)
    if constexpr (std::same_as<Backend, manual_simd> || std::same_as<Backend, expression_simd>) {
        state.SetLabel(std::string(xsimd::default_arch::name()) +
                       "; lanes=" + std::to_string(xsimd::batch<double>::size));
    }
#else
    (void)state;
#endif
}

template <class Backend, bool Square, bool Reuse>
void np_fixed(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    buffers data(count);
    if (!data.verify<Backend, Square>(count)) {
        state.SkipWithError("tensor kernel differs from its scalar reference");
        return;
    }
    auto *dst = data.actual.data();
    const auto *x = data.first.data(), *y = data.second.data();
    benchmark::DoNotOptimize(dst);
    benchmark::DoNotOptimize(x);
    benchmark::DoNotOptimize(y);
    double alpha = 1.5, beta = -0.5;
    benchmark::DoNotOptimize(alpha);
    benchmark::DoNotOptimize(beta);
    const auto tree = make_expression<Square>(input(x, count), input(y, count), alpha, beta);
    for (auto _ : state) {
        if constexpr (!Reuse) {
            construct_and_evaluate<Backend, Square>(dst, x, y, count, alpha, beta);
        } else if constexpr (std::same_as<Backend, expression>) {
            np::eval_to(output(dst, count), tree);
#if defined(WJR_BENCHMARK_XSIMD)
        } else if constexpr (std::same_as<Backend, expression_simd>) {
            np::eval_to(output(dst, count), tree, np::xsimd_policy<>{});
#endif
        } else {
            evaluate<Backend, Square>(dst, x, y, count, alpha, beta);
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
    state.counters["expression_bytes"] = static_cast<double>(sizeof(tree));
    state.counters["view_bytes"] = static_cast<double>(sizeof(input));
    label<Backend>(state);
}

template <class Backend, bool Alternating>
void np_mixed(benchmark::State &state) {
    constexpr std::array<size_t, 8> lengths{0, 1, 7, 31, 300, 511, 2048, 4096};
    std::array<size_t, 4096> counts{};
    for (size_t i = 0; i != counts.size(); ++i)
        counts[i] = Alternating ? (i % 2 == 0 ? 0 : lengths.back()) : lengths[i % lengths.size()];
    if constexpr (!Alternating) {
        std::mt19937 random(41);
        std::shuffle(counts.begin(), counts.end(), random);
    }
    buffers data(lengths.back());
    for (auto count : lengths) {
        if (!data.verify<Backend, false>(count)) {
            state.SkipWithError("mixed-length tensor kernel differs from its reference");
            return;
        }
    }
    auto *dst = data.actual.data();
    const auto *x = data.first.data(), *y = data.second.data();
    benchmark::DoNotOptimize(dst);
    benchmark::DoNotOptimize(x);
    benchmark::DoNotOptimize(y);
    double alpha = 1.5, beta = -0.5;
    benchmark::DoNotOptimize(alpha);
    benchmark::DoNotOptimize(beta);
    size_t iteration = 0;
    for (auto _ : state) {
        const auto count = counts[iteration++ % counts.size()];
        construct_and_evaluate<Backend, false>(dst, x, y, count, alpha, beta);
        benchmark::ClobberMemory();
    }
    label<Backend>(state);
}

void sizes(benchmark::internal::Benchmark *entry) {
    for (auto count : {0, 1, 4, 7, 31, 300, 4096, 262144})
        entry->Arg(count);
    entry->UseRealTime();
}

#define WJR_NP_BENCHMARKS(Backend)                                                                 \
    BENCHMARK_TEMPLATE(np_fixed, Backend, false, false)->Apply(sizes);                             \
    BENCHMARK_TEMPLATE(np_fixed, Backend, false, true)->Apply(sizes);                              \
    BENCHMARK_TEMPLATE(np_fixed, Backend, true, false)->Apply(sizes);                              \
    BENCHMARK_TEMPLATE(np_mixed, Backend, false)->UseRealTime();                                   \
    BENCHMARK_TEMPLATE(np_mixed, Backend, true)->UseRealTime()

WJR_NP_BENCHMARKS(manual);
WJR_NP_BENCHMARKS(expression);
#if defined(WJR_BENCHMARK_XSIMD)
WJR_NP_BENCHMARKS(manual_simd);
WJR_NP_BENCHMARKS(expression_simd);
#endif

#undef WJR_NP_BENCHMARKS

} // namespace
