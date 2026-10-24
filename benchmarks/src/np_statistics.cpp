#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/statistics.hpp>

namespace {
namespace np = wjr::np;

template <bool SkipNaN, bool Masked>
double manual_covariance(const double *x, const double *y, size_t count, size_t step) noexcept {
    if (count < 2)
        return std::numeric_limits<double>::quiet_NaN();
    size_t first = 0;
    if constexpr (SkipNaN) {
        while (first != count &&
               (x[first * step] != x[first * step] || y[first * step] != y[first * step]))
            ++first;
        if (first == count)
            return std::numeric_limits<double>::quiet_NaN();
    }
    const double xanchor = x[first * step], yanchor = y[first * step];
    double sx = 0, sy = 0;
    size_t valid = SkipNaN ? 0 : count;
    for (size_t i = first; i != count; ++i) {
        const double a = x[i * step], b = y[i * step];
        if constexpr (SkipNaN)
            if (a != a || b != b)
                continue;
        sx += a - xanchor;
        sy += b - yanchor;
        if constexpr (SkipNaN)
            ++valid;
    }
    if (valid < 2)
        return std::numeric_limits<double>::quiet_NaN();
    const double mx = xanchor + sx / valid, my = yanchor + sy / valid;
    double cross = 0;
    for (size_t i = first; i != count; ++i) {
        const double a = x[i * step], b = y[i * step];
        if constexpr (SkipNaN && Masked) {
            const bool keep = a == a && b == b;
            const double dx = keep ? a - mx : 0;
            const double dy = keep ? b - my : 0;
            cross += dx * dy;
        } else {
            if constexpr (SkipNaN)
                if (a != a || b != b)
                    continue;
            cross += (a - mx) * (b - my);
        }
    }
    return cross / (valid - 1);
}

template <bool Library, bool SkipNaN, bool Strided, bool Masked = false>
WJR_NOINLINE double covariance_call(const double *x, const double *y, size_t count) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (!Library) {
        return manual_covariance<SkipNaN, Masked>(x, y, count, step);
    } else {
        using shape = np::dextents<size_t, 1>;
        using layout = std::conditional_t<Strided, np::layout_stride, np::layout_right>;
        const auto mapping = [&] {
            if constexpr (Strided)
                return typename layout::template mapping<shape>(shape(count),
                                                                std::array<size_t, 1>{step});
            else
                return typename layout::template mapping<shape>(shape(count));
        }();
        const np::tensor_view<const double, shape, layout> a(x, mapping), b(y, mapping);
        if constexpr (SkipNaN)
            return np::nancovariance(a, b);
        else
            return np::covariance(a, b);
    }
}

template <bool Library, bool SkipNaN, bool Strided, bool Masked = false>
void covariance_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto missing = state.range(1);
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<double> x(count * step), y(count * step);
    std::mt19937 engine(389);
    for (size_t i = 0; i != count; ++i) {
        x[i * step] = static_cast<double>(engine() % 10000);
        y[i * step] = static_cast<double>(engine() % 10000);
        if constexpr (SkipNaN)
            if ((missing == 1 && i % 17 == 0) || (missing == 2 && engine() % 2 == 0) ||
                missing == 3)
                x[i * step] = std::numeric_limits<double>::quiet_NaN();
    }
    const auto expected = covariance_call<false, SkipNaN, Strided>(x.data(), y.data(), count);
    const auto actual =
        covariance_call<Library, SkipNaN, Strided, Masked>(x.data(), y.data(), count);
    if (!(expected == actual || (std::isnan(expected) && std::isnan(actual)))) {
        state.SkipWithError("covariance differs from same-order handwritten baseline");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(x.data());
        benchmark::DoNotOptimize(y.data());
        auto value = covariance_call<Library, SkipNaN, Strided, Masked>(x.data(), y.data(), count);
        benchmark::DoNotOptimize(value);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

template <bool SkipNaN>
void covariance_arguments(benchmark::internal::Benchmark *benchmark) {
    for (const int count : {0, 1, 16, 300, 4096})
        for (int missing = 0; missing != (SkipNaN ? 4 : 1); ++missing)
            benchmark->Args({count, missing});
    benchmark->UseRealTime();
}
BENCHMARK_TEMPLATE(covariance_kernel, false, false, false)->Apply(covariance_arguments<false>);
BENCHMARK_TEMPLATE(covariance_kernel, true, false, false)->Apply(covariance_arguments<false>);
BENCHMARK_TEMPLATE(covariance_kernel, false, true, false)->Apply(covariance_arguments<true>);
BENCHMARK_TEMPLATE(covariance_kernel, true, true, false)->Apply(covariance_arguments<true>);
BENCHMARK_TEMPLATE(covariance_kernel, false, false, true)->Apply(covariance_arguments<false>);
BENCHMARK_TEMPLATE(covariance_kernel, true, false, true)->Apply(covariance_arguments<false>);
BENCHMARK_TEMPLATE(covariance_kernel, false, true, true)->Apply(covariance_arguments<true>);
BENCHMARK_TEMPLATE(covariance_kernel, true, true, true)->Apply(covariance_arguments<true>);
BENCHMARK_TEMPLATE(covariance_kernel, false, true, false, true)->Apply(covariance_arguments<true>);
BENCHMARK_TEMPLATE(covariance_kernel, false, true, true, true)->Apply(covariance_arguments<true>);
} // namespace
