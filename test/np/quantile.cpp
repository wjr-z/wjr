#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/quantile.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class D, class T, class Q>
concept quantiles =
    requires(view<D> dst, view<T> src, view<Q> probabilities, std::span<std::remove_cv_t<T>> data,
             std::span<size_t> ranks) { np::quantile_to(dst, src, probabilities, data, ranks); };
static_assert(quantiles<double, const int, const float>);
static_assert(!quantiles<const double, int, double>);
static_assert(!quantiles<volatile double, int, double>);
static_assert(!quantiles<double, volatile int, double>);
static_assert(!quantiles<int, int, double>);
static_assert(!quantiles<double, int, int>);

constexpr bool constant_quantiles() {
    double values[] = {9, 1, 7, 3, 5};
    const double probabilities[] = {.125, .5, .875};
    double output[3];
    size_t ranks[6];
    np::quantile_inplace_to(view<double>(output, 3), view<double>(values, 5),
                            view<const double>(probabilities, 3), std::span(ranks));
    np::quantile_inplace_to<np::quantile_nan_policy::propagate, np::partition_method::shared_pivot>(
        view<double>(output, 3), view<double>(values, 5), view<const double>(probabilities, 3),
        std::span(ranks));
    return output[0] == 2 && output[1] == 5 && output[2] == 8;
}
static_assert(constant_quantiles());
constexpr auto large_last = static_cast<size_t>(PTRDIFF_MAX) - 1;
static_assert(np::detail::locate_quantile<double>(1., large_last).lower == large_last);

TEST(NpQuantile, UnsortedRepeatedProbabilitiesAndSourcePreservation) {
    EXPECT_TRUE(constant_quantiles());
    const int source[] = {9, 1, 7, 3, 5};
    const double probabilities[] = {1, .25, 0, .5, .25, .125};
    double output[6];
    int workspace[5];
    size_t ranks[12];
    np::quantile_to(view<double>(output, 6), view<const int>(source, 5),
                    view<const double>(probabilities, 6), std::span(workspace), std::span(ranks));
    const double expected[] = {9, 3, 1, 5, 3, 2};
    EXPECT_TRUE(std::equal(std::begin(output), std::end(output), std::begin(expected)));
    EXPECT_EQ(source[0], 9);
    EXPECT_EQ(source[1], 1);
    int mutable_values[] = {9, 1, 7, 3, 5};
    np::quantile_inplace_to(view<double>(output, 6), view<int>(mutable_values, 5),
                            view<const double>(probabilities, 6), std::span(ranks));
    EXPECT_TRUE(std::equal(std::begin(output), std::end(output), std::begin(expected)));
}

TEST(NpQuantile, NaNPropagationOmissionAndEmptyInputs) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double source[] = {4, nan, 1, 3, 2};
    const double probabilities[] = {0, .5, 1};
    double output[3], workspace[5];
    size_t ranks[6];
    const view<double> dst(output, 3);
    const view<const double> src(source, 5), query(probabilities, 3);
    np::quantile_to(dst, src, query, std::span(workspace), std::span(ranks));
    for (double value : output)
        EXPECT_TRUE(std::isnan(value));
    np::quantile_to<np::quantile_nan_policy::omit>(dst, src, query, std::span(workspace),
                                                   std::span(ranks));
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], 2.5);
    EXPECT_EQ(output[2], 4);
    double inplace[] = {nan, 4, nan, 1, 3, 2};
    np::quantile_inplace_to<np::quantile_nan_policy::omit>(dst, view<double>(inplace, 6), query,
                                                           std::span(ranks));
    EXPECT_EQ(output[1], 2.5);
    const double missing[] = {nan, nan};
    np::quantile_to<np::quantile_nan_policy::omit>(dst, view<const double>(missing, 2), query,
                                                   std::span(workspace), std::span<size_t>{});
    EXPECT_TRUE(std::isnan(output[1]));
    np::quantile_to(dst, view<const double>(nullptr, 0), query, std::span<double>{},
                    std::span<size_t>{});
    EXPECT_TRUE(std::isnan(output[0]));
    np::quantile_to(view<double>(nullptr, 0), src, view<const double>(nullptr, 0),
                    std::span<double>{}, std::span<size_t>{});
}

TEST(NpQuantile, SingletonBooleansAndLargeIntegerOrdering) {
    const bool bits[] = {true, false};
    const double probabilities[] = {0, .5, 1};
    double output[3];
    bool workspace[2];
    size_t ranks[6];
    np::quantile_to(view<double>(output, 3), view<const bool>(bits, 2),
                    view<const double>(probabilities, 3), std::span(workspace), std::span(ranks));
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], .5);
    EXPECT_EQ(output[2], 1);
    int single[] = {42};
    np::quantile_inplace_to(view<double>(output, 3), view<int>(single, 1),
                            view<const double>(probabilities, 3), std::span(ranks));
    for (double value : output)
        EXPECT_EQ(value, 42);
    uint64_t integers[] = {UINT64_MAX, (uint64_t{1} << 63) + 1, 0, (uint64_t{1} << 63)};
    const double middle = .5;
    np::quantile_inplace_to(view<double>(output, 1), view<uint64_t>(integers, 4),
                            view<const double>(&middle, 1), std::span(ranks));
    EXPECT_EQ(integers[1], (uint64_t{1} << 63));
    EXPECT_EQ(integers[2], (uint64_t{1} << 63) + 1);
}

TEST(NpQuantile, FiniteInterpolationDoesNotOverflowAndInfinityPolicyIsExplicit) {
    const double max = std::numeric_limits<double>::max(),
                 inf = std::numeric_limits<double>::infinity();
    const double probabilities[] = {0, .5, 1};
    double output[3];
    size_t ranks[6];
    auto run = [&](double a, double b) {
        double values[] = {a, b};
        np::quantile_inplace_to(view<double>(output, 3), view<double>(values, 2),
                                view<const double>(probabilities, 3), std::span(ranks));
    };
    run(-max, max);
    EXPECT_EQ(output[0], -max);
    EXPECT_EQ(output[1], 0);
    EXPECT_EQ(output[2], max);
    run(inf, inf);
    EXPECT_EQ(output[1], inf);
    run(0, inf);
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], inf);
    run(-inf, inf);
    EXPECT_TRUE(std::isnan(output[1]));
    EXPECT_EQ(output[0], -inf);
    EXPECT_EQ(output[2], inf);
}

TEST(NpQuantile, StridedInputQueriesAndOutputPreservePadding) {
    const int values[] = {9, -999, 1, -999, 5, -999, 3, -999, 7};
    const double probabilities[] = {0, -999, .5, -999, 1};
    double output[] = {-1, -999, -1, -999, -1};
    int workspace[5];
    size_t ranks[6];
    using source_shape = np::extents<size_t, 5>;
    using query_shape = np::extents<size_t, 3>;
    const np::layout_stride::mapping<source_shape> smap(source_shape{}, std::array<size_t, 1>{2});
    const np::layout_stride::mapping<query_shape> qmap(query_shape{}, std::array<size_t, 1>{2});
    np::quantile_to(
        np::tensor_view<double, query_shape, np::layout_stride>(output, qmap),
        np::tensor_view<const int, source_shape, np::layout_stride>(values, smap),
        np::tensor_view<const double, query_shape, np::layout_stride>(probabilities, qmap),
        std::span(workspace), std::span(ranks));
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[2], 5);
    EXPECT_EQ(output[4], 9);
    EXPECT_EQ(output[1], -999);
    EXPECT_EQ(output[3], -999);
}

TEST(NpQuantile, RandomQuantilesAgainstFullSort) {
    std::mt19937 engine(1053);
    for (size_t count : {1, 2, 3, 17, 300}) {
        std::vector<double> input(count), workspace(count), probabilities(31), output(31);
        std::vector<size_t> ranks(probabilities.size() * 2);
        for (auto &value : input)
            value = static_cast<int>(engine() % 5000) - 2500;
        for (auto &q : probabilities)
            q = static_cast<double>(engine() % 33) / 32;
        np::quantile_to(view<double>(output.data(), output.size()),
                        view<const double>(input.data(), count),
                        view<const double>(probabilities.data(), probabilities.size()),
                        std::span(workspace), std::span(ranks));
        std::sort(input.begin(), input.end());
        const auto ordinary = output;
        np::quantile_to<np::quantile_nan_policy::propagate, np::partition_method::shared_pivot>(
            view<double>(output.data(), output.size()), view<const double>(input.data(), count),
            view<const double>(probabilities.data(), probabilities.size()), std::span(workspace),
            std::span(ranks));
        EXPECT_EQ(output, ordinary);
        for (size_t i = 0; i != probabilities.size(); ++i) {
            const double position = probabilities[i] * (count - 1);
            const auto low = static_cast<size_t>(position);
            const auto high = std::min(low + 1, count - 1);
            const auto expected = input[low] + (input[high] - input[low]) * (position - low);
            EXPECT_DOUBLE_EQ(output[i], expected);
        }
    }
}
} // namespace
