#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/expression_xsimd.hpp>

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 1>;

template <class T>
using view = np::tensor_view<T, shape>;

struct scalar_only_operation {
    static constexpr double apply(double value) noexcept { return value * 2.0; }
};

template <class T>
void compare_lengths() {
    constexpr size_t lanes = xsimd::batch<T>::size;
    for (size_t count = 0; count != 3 * lanes + 3; ++count) {
        std::vector<T> x(count + 1), y(count + 1), expected(count + 1), actual(count + 1);
        for (size_t i = 0; i != count + 1; ++i) {
            x[i] = static_cast<T>(i % 7) / T(4);
            y[i] = static_cast<T>(i % 5) / T(8);
        }
        // Offset handles exercise unaligned loads and exact allocation ends.
        view<T> a(x.data() + 1, count), b(y.data() + 1, count);
        const auto first = np::as_expr(a), second = np::as_expr(b);
        const auto expression = T(2) * (+first) - second / T(2) + T(1);
        np::eval_to(view<T>(expected.data() + 1, count), expression);
        np::eval_to(view<T>(actual.data() + 1, count), expression, np::xsimd_policy<>{});
        EXPECT_EQ(actual, expected);
        np::eval_to(a, expression, np::xsimd_policy<>{});
        EXPECT_EQ(x, expected);
    }
}

TEST(NpExpressionXsimd, BatchesTailsAndPointwiseAlias) {
    compare_lengths<float>();
    compare_lengths<double>();
}

TEST(NpExpressionXsimd, MixedIntegerDivisionKeepsNodeSemantics) {
    std::array<int32_t, 19> numerator{}, denominator{};
    std::array<float, 19> bias{}, output{};
    numerator.fill(3);
    denominator.fill(2);
    const auto expression = np::as_expr(view<int32_t>(numerator.data(), 19)) /
                                np::as_expr(view<int32_t>(denominator.data(), 19)) +
                            np::as_expr(view<float>(bias.data(), 19));
    np::eval_to(view<float>(output.data(), 19), expression, np::xsimd_policy<>{});
    for (auto value : output)
        EXPECT_EQ(value, 1.0f);
}

TEST(NpExpressionXsimd, WiderScalarPreservesIntermediatePrecision) {
    std::array<float, 19> source{}, output{}, expected{};
    source.fill(1.0f);
    const auto expression = (np::as_expr(view<float>(source.data(), 19)) + 0x1p-30) - 1.0;
    np::eval_to(view<float>(expected.data(), 19), expression);
    np::eval_to(view<float>(output.data(), 19), expression, np::xsimd_policy<>{});
    EXPECT_EQ(output, expected);
    EXPECT_EQ(output.front(), 0x1p-30f);
}

TEST(NpExpressionXsimd, ScalarOnlyAndIntegerOutputFallback) {
    std::array<int, 19> output{};
    np::eval_to(view<int>(output.data(), output.size()), np::as_expr(3) / 2, np::xsimd_policy<>{});
    for (auto value : output)
        EXPECT_EQ(value, 1);
    std::array<double, 19> floating{};
    np::eval_to(view<double>(floating.data(), floating.size()), np::as_expr(3.0) / 2.0,
                np::xsimd_policy<>{});
    for (auto value : floating)
        EXPECT_EQ(value, 1.5);
}

TEST(NpExpressionXsimd, ColumnMajorPackedTraversal) {
    using extents = np::extents<size_t, 2, 3>;
    using matrix = np::tensor_view<double, extents, np::layout_left>;
    std::array<double, 6> source{1, 2, 3, 4, 5, 6}, output{};
    np::eval_to(matrix(output.data()), np::as_expr(matrix(source.data())) * 2.0,
                np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i)
        EXPECT_EQ(output[i], 2.0 * source[i]);
}

TEST(NpExpressionXsimd, UnknownOperationUsesScalarFallback) {
    std::array<double, 19> source{}, output{};
    source.fill(3.0);
    const auto leaf = np::as_expr(view<double>(source.data(), source.size()));
    const np::unary_expression<scalar_only_operation, decltype(leaf)> expression(leaf);
    np::eval_to(view<double>(output.data(), output.size()), expression, np::xsimd_policy<>{});
    for (auto value : output)
        EXPECT_EQ(value, 6.0);
}

TEST(NpExpressionXsimd, BroadcastAndMixedLayoutUseCoordinateFallback) {
    using matrix = np::extents<size_t, 2, 3>;
    const double source[] = {1, 2, 3, 4, 5, 6};
    const double bias[] = {10, 20, 30};
    const double columns[] = {2, 4};
    const double left[] = {1, 4, 2, 5, 3, 6};
    double result[6]{};
    const auto expression =
        np::as_expr(np::tensor_view<const double, matrix>(source)) +
        np::as_expr(np::tensor_view<const double, np::extents<size_t, 3>>(bias)) *
            np::as_expr(np::tensor_view<const double, np::extents<size_t, 2, 1>>(columns)) +
        np::as_expr(np::tensor_view<const double, matrix, np::layout_left>(left));
    np::eval_to(np::tensor_view<double, matrix>(result), expression, np::xsimd_policy<>{});
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            EXPECT_EQ(result[i * 3 + j], 2 * source[i * 3 + j] + bias[j] * columns[i]);

    const np::layout_stride::mapping<matrix> mapping(matrix{}, std::array<size_t, 2>{4, 1});
    const double padded[] = {1, 2, 3, -99, 4, 5, 6};
    const auto strided =
        np::as_expr(np::tensor_view<const double, matrix, np::layout_stride>(padded, mapping));
    np::eval_to(np::tensor_view<double, matrix>(result), strided * 2.0, np::xsimd_policy<>{});
    for (size_t i = 0; i != 6; ++i)
        EXPECT_EQ(result[i], 2 * source[i]);
}

TEST(NpExpressionXsimd, NullEmptyAndRankZero) {
    view<double> empty(nullptr, 0);
    np::eval_to(empty, np::as_expr(empty) * 2.0, np::xsimd_policy<>{});
    double source = 3.0, output = 0.0;
    using scalar = np::tensor_view<double, np::extents<size_t>>;
    np::eval_to(scalar(&output), -np::as_expr(scalar(&source)) + 0.5, np::xsimd_policy<>{});
    EXPECT_EQ(output, -2.5);
}

TEST(NpExpressionXsimd, ExceptionalFloatingPointValues) {
    constexpr size_t count = 19;
    std::array<double, count> source{}, actual{}, expected{};
    source.fill(2.0);
    source[0] = std::numeric_limits<double>::infinity();
    source[1] = -std::numeric_limits<double>::infinity();
    source[2] = std::numeric_limits<double>::quiet_NaN();
    source[3] = -0.0;
    source[18] = -0.0;
    const auto expression = -np::as_expr(view<double>(source.data(), count)) / 2.0;
    np::eval_to(view<double>(expected.data(), count), expression);
    np::eval_to(view<double>(actual.data(), count), expression, np::xsimd_policy<>{});
    for (size_t i = 0; i != count; ++i) {
        if (std::isnan(expected[i])) {
            EXPECT_TRUE(std::isnan(actual[i]));
        } else {
            EXPECT_EQ(actual[i], expected[i]);
            EXPECT_EQ(std::signbit(actual[i]), std::signbit(expected[i]));
        }
    }
}

} // namespace
