// Design experiment, not a public expression API.
#include "detail/expression.hpp"

extern "C" {
void handwritten_axpby(double *dst, const double *x, const double *y, std::size_t n, double alpha,
                       double beta) noexcept {
    for (std::size_t i = 0; i != n; ++i)
        dst[i] = alpha * x[i] + beta * y[i];
}

void expression_axpby(double *dst, const double *x, const double *y, std::size_t n, double alpha,
                      double beta) noexcept {
    using namespace expression_experiment;
    const leaf<> a{std::span<const double>(x, n)};
    const leaf<> b{std::span<const double>(y, n)};
    evaluate(dst, n, alpha * a + beta * b);
}

void handwritten_reuse(double *dst, const double *x, const double *y, std::size_t n) noexcept {
    for (std::size_t i = 0; i != n; ++i) {
        const auto sum = x[i] + y[i];
        dst[i] = sum * sum;
    }
}

void expression_reuse(double *dst, const double *x, const double *y, std::size_t n) noexcept {
    expression_experiment::evaluate(dst, n, expression_experiment::returned_expression(x, y, n));
}

void handwritten_static(double *dst, const double *x, const double *y) noexcept {
    for (std::size_t i = 0; i != 4; ++i)
        dst[i] = (x[i] - y[i]) * (x[i] + y[i]);
}

void expression_static(double *dst, const double *x, const double *y) noexcept {
    using namespace expression_experiment;
    const leaf<4> a{std::span<const double, 4>(x, 4)};
    const leaf<4> b{std::span<const double, 4>(y, 4)};
    evaluate(dst, 4, (a - b) * (a + b));
}

void handwritten_rows(double *dst, const double *x, const double *weights, std::size_t rows,
                      std::size_t columns, double scale) noexcept {
    for (std::size_t row = 0; row != rows; ++row)
        for (std::size_t col = 0; col != columns; ++col)
            dst[row * columns + col] = x[row * columns + col] + weights[col] * scale;
}

void expression_rows(double *dst, const double *x, const double *weights, std::size_t rows,
                     std::size_t columns, double scale) noexcept {
    using namespace expression_experiment;
    const matrix_leaf matrix{x, columns};
    const leaf<> row_weights{std::span<const double>(weights, columns)};
    evaluate_rows(dst, rows, columns, matrix + row_weights * scale);
}

#if defined(WJR_EXPR_XSIMD)
void handwritten_simd_axpby(double *dst, const double *x, const double *y, std::size_t n,
                            double alpha, double beta) noexcept {
    using batch = xsimd::batch<double>;
    const batch a(alpha), b(beta);
    std::size_t i = 0;
    for (; n - i >= batch::size; i += batch::size) {
        (a * batch::load_unaligned(x + i) + b * batch::load_unaligned(y + i))
            .store_unaligned(dst + i);
    }
    for (; i != n; ++i)
        dst[i] = alpha * x[i] + beta * y[i];
}

void expression_simd_axpby(double *dst, const double *x, const double *y, std::size_t n,
                           double alpha, double beta) noexcept {
    using namespace expression_experiment;
    const leaf<> a{std::span<const double>(x, n)};
    const leaf<> b{std::span<const double>(y, n)};
    evaluate_simd(dst, n, alpha * a + beta * b);
}

void handwritten_simd_reuse(double *dst, const double *x, const double *y, std::size_t n) noexcept {
    using batch = xsimd::batch<double>;
    std::size_t i = 0;
    for (; n - i >= batch::size; i += batch::size) {
        const auto sum = batch::load_unaligned(x + i) + batch::load_unaligned(y + i);
        (sum * sum).store_unaligned(dst + i);
    }
    for (; i != n; ++i) {
        const auto sum = x[i] + y[i];
        dst[i] = sum * sum;
    }
}

void expression_simd_reuse(double *dst, const double *x, const double *y, std::size_t n) noexcept {
    expression_experiment::evaluate_simd(dst, n,
                                         expression_experiment::returned_expression(x, y, n));
}
#endif
}

#if defined(WJR_EXPR_TEST)
    #include <array>
    #include <cstdio>
    #include <cstdlib>
    #include <new>
    #include <vector>

    #include <wjr/memory/asan.hpp>

namespace {
bool recording_allocations = false;
std::size_t allocations = 0;
} // namespace

    #if !defined(WJR_HAS_ADDRESS_SANITIZER)
void *operator new(std::size_t count) {
    if (recording_allocations)
        ++allocations;
    if (void *ptr = std::malloc(count == 0 ? 1 : count))
        return ptr;
    throw std::bad_alloc{};
}
void operator delete(void *ptr) noexcept { std::free(ptr); }
void operator delete(void *ptr, std::size_t) noexcept { std::free(ptr); }
    #endif

int main() {
    std::array<double, 1024> x{}, y{}, actual{}, expected{};
    for (std::size_t i = 0; i != x.size(); ++i) {
        x[i] = static_cast<double>(i % 17) - 8.0;
        y[i] = static_cast<double>(i % 23) / 4.0;
    }
    recording_allocations = true;
    for (std::size_t n = 0; n != 513; ++n) {
        handwritten_axpby(expected.data(), x.data(), y.data(), n, 2.0, -3.0);
        expression_axpby(actual.data(), x.data(), y.data(), n, 2.0, -3.0);
        if (actual != expected)
            return 1;
        handwritten_reuse(expected.data(), x.data(), y.data(), n);
        expression_reuse(actual.data(), x.data(), y.data(), n);
        if (actual != expected)
            return 2;
    #if defined(WJR_EXPR_XSIMD)
        handwritten_simd_axpby(expected.data(), x.data(), y.data(), n, 2.0, -3.0);
        expression_simd_axpby(actual.data(), x.data(), y.data(), n, 2.0, -3.0);
        if (actual != expected)
            return 6;
        handwritten_simd_reuse(expected.data(), x.data(), y.data(), n);
        expression_simd_reuse(actual.data(), x.data(), y.data(), n);
        if (actual != expected)
            return 7;
    #endif
    }
    for (std::size_t rows = 0; rows != 8; ++rows) {
        for (std::size_t columns = 0; columns != 33; ++columns) {
            handwritten_rows(expected.data(), x.data(), y.data(), rows, columns, 0.5);
            expression_rows(actual.data(), x.data(), y.data(), rows, columns, 0.5);
            if (actual != expected)
                return 3;
        }
    }
    handwritten_static(expected.data(), x.data(), y.data());
    expression_static(actual.data(), x.data(), y.data());
    if (actual != expected)
        return 4;
    expression_axpby(nullptr, nullptr, nullptr, 0, 1.0, 2.0);
    expression_rows(nullptr, nullptr, nullptr, 7, 0, 2.0);
    recording_allocations = false;
    for (std::size_t n = 0; n != 33; ++n) {
        std::vector<double> first(n, 1.25), second(n, -0.75), reference(n), result(n);
        recording_allocations = true;
        handwritten_axpby(reference.data(), first.data(), second.data(), n, 2.0, -3.0);
        expression_axpby(result.data(), first.data(), second.data(), n, 2.0, -3.0);
        if (result != reference)
            return 8;
    #if defined(WJR_EXPR_XSIMD)
        expression_simd_axpby(result.data(), first.data(), second.data(), n, 2.0, -3.0);
        if (result != reference)
            return 9;
    #endif
        recording_allocations = false;
    }
    using namespace expression_experiment;
    const leaf<> a{std::span<const double>(x)};
    const leaf<> b{std::span<const double>(y)};
    const auto tree = 2.0 * a + 3.0 * b;
    #if defined(WJR_HAS_ADDRESS_SANITIZER)
    std::puts("allocation counter disabled for sanitizer runtime; measured separately");
    #endif
    std::printf("counted_allocations=%zu leaf_static=%zu leaf_dynamic=%zu axpby_tree=%zu\n",
                allocations, sizeof(leaf<4>), sizeof(leaf<>), sizeof(tree));
    return allocations == 0 ? 0 : 5;
}
#endif

#if defined(WJR_EXPR_BENCHMARK)
    #include <array>
    #include <vector>

    #include <benchmark/benchmark.h>

namespace {
template <auto Kernel, bool Mixed = false>
void benchmark_axpby(benchmark::State &state) {
    constexpr std::array<std::size_t, 8> sizes{0, 1, 7, 31, 300, 511, 2048, 4096};
    const auto count = Mixed ? sizes.back() : static_cast<std::size_t>(state.range(0));
    std::vector<double> x(count, 1.25), y(count, -0.75), output(count);
    auto *dst = output.data();
    auto *first = x.data();
    auto *second = y.data();
    benchmark::DoNotOptimize(dst);
    benchmark::DoNotOptimize(first);
    benchmark::DoNotOptimize(second);
    std::size_t iteration = 0;
    for (auto _ : state) {
        const auto n = Mixed ? sizes[iteration++ & 7] : count;
        Kernel(dst, first, second, n, 1.5, -0.5);
        benchmark::ClobberMemory();
    }
}

template <auto Kernel>
void benchmark_reuse(benchmark::State &state) {
    const auto count = static_cast<std::size_t>(state.range(0));
    std::vector<double> x(count, 1.25), y(count, -0.75), output(count);
    auto *dst = output.data();
    auto *first = x.data();
    auto *second = y.data();
    benchmark::DoNotOptimize(dst);
    benchmark::DoNotOptimize(first);
    benchmark::DoNotOptimize(second);
    for (auto _ : state) {
        Kernel(dst, first, second, count);
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(benchmark_axpby, handwritten_axpby)
    ->Arg(4)
    ->Arg(300)
    ->Arg(262144)
    ->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_axpby, expression_axpby)->Arg(4)->Arg(300)->Arg(262144)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_axpby, handwritten_axpby, true)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_axpby, expression_axpby, true)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_reuse, handwritten_reuse)->Arg(300)->Arg(262144)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_reuse, expression_reuse)->Arg(300)->Arg(262144)->UseRealTime();
    #if defined(WJR_EXPR_XSIMD)
BENCHMARK_TEMPLATE(benchmark_axpby, handwritten_simd_axpby)
    ->Arg(4)
    ->Arg(300)
    ->Arg(262144)
    ->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_axpby, expression_simd_axpby)
    ->Arg(4)
    ->Arg(300)
    ->Arg(262144)
    ->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_axpby, handwritten_simd_axpby, true)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_axpby, expression_simd_axpby, true)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_reuse, handwritten_simd_reuse)->Arg(300)->Arg(262144)->UseRealTime();
BENCHMARK_TEMPLATE(benchmark_reuse, expression_simd_reuse)->Arg(300)->Arg(262144)->UseRealTime();
    #endif
} // namespace

BENCHMARK_MAIN();
#endif
