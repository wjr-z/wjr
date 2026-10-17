#include <array>
#include <memory>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/expression.hpp>
#include <wjr/np/tensor.hpp>
#include <wjr/vector.hpp>

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 1>;

enum class storage { raw_default, std_vector, wjr_default, tensor };

template <storage Storage>
void np_materialize(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    std::vector<double> x(count), y(count);
    for (size_t i = 0; i != count; ++i) {
        x[i] = static_cast<double>(i % 17) / 4.0;
        y[i] = static_cast<double>(i % 23) / 8.0;
    }
    const double *first = x.data(), *second = y.data();
    benchmark::DoNotOptimize(first);
    benchmark::DoNotOptimize(second);
    using input = np::tensor_view<const double, shape>;
    const auto expression =
        1.5 * np::as_expr(input(first, count)) - 0.5 * np::as_expr(input(second, count));
    auto compute = [&](double *dst) {
        np::eval_to(np::tensor_view<double, shape>(dst, count), expression);
        benchmark::DoNotOptimize(dst);
        benchmark::ClobberMemory();
    };
    for (auto _ : state) {
        if constexpr (Storage == storage::raw_default) {
            // Default initialization starts scalar lifetimes without a zero pass.
            std::unique_ptr<double[]> result(new double[count]);
            compute(result.get());
        } else if constexpr (Storage == storage::std_vector) {
            std::vector<double> result(count);
            compute(result.data());
        } else if constexpr (Storage == storage::wjr_default) {
            wjr::vector<double> result(count, wjr::default_construct);
            compute(result.data());
        } else {
            auto result = np::make_tensor<np::tensor<double, shape>>(std::array{count});
            if (!result) {
                state.SkipWithError("tensor shape could not be materialized");
                break;
            }
            compute(result->view().data_handle());
        }
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
    if constexpr (Storage == storage::raw_default)
        state.counters["owner_bytes"] = sizeof(std::unique_ptr<double[]>);
    else if constexpr (Storage == storage::std_vector)
        state.counters["owner_bytes"] = sizeof(std::vector<double>);
    else if constexpr (Storage == storage::wjr_default)
        state.counters["owner_bytes"] = sizeof(wjr::vector<double>);
    else
        state.counters["owner_bytes"] = sizeof(np::tensor<double, shape>);
}

BENCHMARK_TEMPLATE(np_materialize, storage::raw_default)
    ->Arg(300)
    ->Arg(4096)
    ->Arg(262144)
    ->UseRealTime();
BENCHMARK_TEMPLATE(np_materialize, storage::std_vector)
    ->Arg(300)
    ->Arg(4096)
    ->Arg(262144)
    ->UseRealTime();
BENCHMARK_TEMPLATE(np_materialize, storage::wjr_default)
    ->Arg(300)
    ->Arg(4096)
    ->Arg(262144)
    ->UseRealTime();
BENCHMARK_TEMPLATE(np_materialize, storage::tensor)
    ->Arg(300)
    ->Arg(4096)
    ->Arg(262144)
    ->UseRealTime();

} // namespace
