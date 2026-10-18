#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/copy.hpp>
#include <wjr/np/expression_cast.hpp>
#include <wjr/np/expression_mask.hpp>
#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_xsimd.hpp>
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 1>;

TEST(NpExpressionXsimd, MappedSubtreesUseScalarFallback) {
    double source[]{1, 2, 3, 4, 5}, output[5]{};
    const auto mapped =
        np::map([](double value) noexcept { return value > 2 ? value * value : -value; },
                np::tensor_view<double, shape>(source, 5));
    np::eval_to(np::tensor_view<double, shape>(output, 5), -mapped + 1.0, np::xsimd_policy<>{});
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4]}),
              (std::array{2.0, 3.0, -8.0, -15.0, -24.0}));
    using row = np::extents<size_t, 1, 5>;
    double expanded[10]{};
    np::eval_to(
        np::tensor_view<double, np::extents<size_t, 2, 5>>(expanded),
        np::map([](double x) noexcept { return x + 2; }, np::tensor_view<double, row>(source)),
        np::xsimd_policy<>{});
    EXPECT_EQ(expanded[0], 3.0);
    EXPECT_EQ(expanded[9], 7.0);
}

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

TEST(NpExpressionXsimd, ExplicitCastUsesScalarFallback) {
    std::array<float, 19> input{}, output{};
    input.fill(3.75f);
    auto cast = np::cast<int>(view<float>(input.data(), input.size()));
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(cast), float>);
    np::eval_to(view<float>(output.data(), output.size()), cast / 2, np::xsimd_policy<>{});
    for (auto value : output)
        EXPECT_EQ(value, 1.f);
    auto identity = np::cast<float>(view<float>(input.data(), input.size()));
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(identity), float>);
    np::eval_to(view<float>(output.data(), output.size()), identity, np::xsimd_policy<>{});
    EXPECT_EQ(output, input);
}

TEST(NpExpressionXsimd, NumericSelectAndStoredBoolUseScalarFallback) {
    std::array<float, 19> input{};
    std::array<double, 19> output{};
    std::array<bool, 19> stored{};
    for (size_t i = 0; i != input.size(); ++i) {
        input[i] = static_cast<float>(i);
        stored[i] = i % 2 != 0;
    }
    const auto a = np::as_expr(view<float>(input.data(), input.size()));
    auto selected = np::select(a > 2.f, a / 2.f, 0.);
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(selected), double>);
    np::eval_to(view<double>(output.data(), output.size()), selected + 1., np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i)
        EXPECT_EQ(output[i], (input[i] > 2.f ? input[i] / 2.f : 0.) + 1.);
    np::eval_to(view<double>(output.data(), output.size()),
                np::select(view<const bool>(stored.data(), stored.size()), a, -a),
                np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i)
        EXPECT_EQ(output[i], stored[i] ? input[i] : -input[i]);
    np::eval_to(view<bool>(stored.data(), stored.size()), np::select(a > 2.f, true, false),
                np::xsimd_policy<>{});
    for (size_t i = 0; i != stored.size(); ++i)
        EXPECT_EQ(stored[i], input[i] > 2.f);
}

TEST(NpExpressionXsimd, StdMathAndNumericOuterRootUseScalarFallback) {
    std::array<float, 19> input{}, output{};
    for (size_t i = 0; i != input.size(); ++i)
        input[i] = static_cast<float>(i) / 4.f;
    const auto a = np::as_expr(view<float>(input.data(), input.size()));
    const auto expression = np::sqrt(a) + np::log(np::abs(a) + 1.f) + np::exp(-np::abs(a));
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(expression), float>);
    np::eval_to(view<float>(output.data(), output.size()), expression, np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i) {
        const float first = std::sqrt(input[i]) + std::log(std::abs(input[i]) + 1.f);
        EXPECT_EQ(output[i], first + std::exp(-std::abs(input[i])));
    }
    double scalar = 0.;
    np::eval_to(np::tensor_view<double, np::extents<size_t>>(&scalar), np::sqrt(true),
                np::xsimd_policy<>{});
    EXPECT_EQ(scalar, 1.);
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

template <class T, size_t Columns>
void compare_broadcast_rows() {
    using matrix_shape = np::extents<size_t, np::dynamic_extent, Columns>;
    using column_shape = np::extents<size_t, np::dynamic_extent, 1>;
    using row_shape = np::extents<size_t, Columns>;
    using matrix = np::tensor_view<T, matrix_shape>;
    using column = np::tensor_view<T, column_shape>;
    using row = np::tensor_view<T, row_shape>;
    for (size_t rows = 0; rows != 7; ++rows) {
        const auto count = rows * Columns;
        std::vector<T> x(count + 1), bias(Columns + 1), scale(rows + 1);
        std::vector<T> actual(count + 1), expected(count + 1);
        for (size_t i = 0; i != count + 1; ++i)
            x[i] = static_cast<T>(i % 17) / T(4);
        for (size_t i = 0; i != Columns + 1; ++i)
            bias[i] = static_cast<T>(i % 7) / T(8);
        for (size_t i = 0; i != rows + 1; ++i)
            scale[i] = static_cast<T>(i % 3) / T(4);
        const auto a = np::as_expr(matrix(x.data() + 1, rows));
        const auto b = np::as_expr(row(bias.data() + 1));
        const auto c = np::as_expr(column(scale.data() + 1, rows));
        const auto expression = -(a + b * T(.5)) / (c + T(1));
        np::eval_to(matrix(expected.data() + 1, rows), expression);
        np::eval_to(matrix(actual.data() + 1, rows), expression, np::xsimd_policy<>{});
        EXPECT_EQ(actual, expected);
        np::eval_to(matrix(x.data() + 1, rows), expression, np::xsimd_policy<>{});
        EXPECT_EQ(x, expected);
    }
}

TEST(NpExpressionXsimd, StaticInnerBroadcastBatchesTailsAndExactAlias) {
    compare_broadcast_rows<float, 0>();
    compare_broadcast_rows<float, 1>();
    compare_broadcast_rows<float, 3>();
    compare_broadcast_rows<float, xsimd::batch<float>::size + 1>();
    compare_broadcast_rows<float, 300>();
    compare_broadcast_rows<double, 0>();
    compare_broadcast_rows<double, 1>();
    compare_broadcast_rows<double, 3>();
    compare_broadcast_rows<double, xsimd::batch<double>::size + 1>();
    compare_broadcast_rows<double, 300>();
}

TEST(NpExpressionXsimd, BroadcastRowCanAliasOneRowOutputExactly) {
    using matrix_shape = np::extents<size_t, 1, 7>;
    using row_shape = np::extents<size_t, 7>;
    std::array<double, 7> source{1, 2, 3, 4, 5, 6, 7};
    std::array<double, 7> output{7, 6, 5, 4, 3, 2, 1};
    const auto x = np::as_expr(np::tensor_view<const double, matrix_shape>(source.data()));
    const auto bias = np::as_expr(np::tensor_view<const double, row_shape>(output.data()));
    np::eval_to(np::tensor_view<double, matrix_shape>(output.data()), x + bias,
                np::xsimd_policy<>{});
    for (auto value : output)
        EXPECT_EQ(value, 8.);
}

TEST(NpExpressionXsimd, BroadcastsMissingAxesAndRankZeroViews) {
    using cube_shape = np::extents<size_t, 2, 1, 7>;
    using row_shape = np::extents<size_t, 7>;
    std::array<float, 14> source{}, actual{}, expected{};
    std::array<float, 7> bias{1, 2, 3, 4, 5, 6, 7};
    float center = .5f;
    const auto x = np::as_expr(np::tensor_view<const float, cube_shape>(source.data()));
    const auto b = np::as_expr(np::tensor_view<const float, row_shape>(bias.data()));
    const auto scalar = np::as_expr(np::tensor_view<const float, np::extents<size_t>>(&center));
    const auto expression = x + b * scalar;
    np::eval_to(np::tensor_view<float, cube_shape>(expected.data()), expression);
    np::eval_to(np::tensor_view<float, cube_shape>(actual.data()), expression,
                np::xsimd_policy<>{});
    EXPECT_EQ(actual, expected);
}

TEST(NpExpressionXsimd, AssignmentBroadcastUsesDestinationInnerExtent) {
    using destination_shape = np::extents<size_t, 2, 3, 7>;
    using row_shape = np::extents<size_t, 7>;
    using singleton_shape = np::extents<size_t, 1>;
    std::array<double, 42> actual{}, expected{};
    std::array<double, 7> bias{1, 2, 3, 4, 5, 6, 7};
    const auto row = np::as_expr(np::tensor_view<const double, row_shape>(bias.data()));
    np::eval_to(np::tensor_view<double, destination_shape>(expected.data()), row + 1.);
    np::eval_to(np::tensor_view<double, destination_shape>(actual.data()), row + 1.,
                np::xsimd_policy<>{});
    EXPECT_EQ(actual, expected);

    const auto singleton = np::as_expr(np::tensor_view<const double, singleton_shape>(bias.data()));
    np::eval_to(np::tensor_view<double, destination_shape>(expected.data()), singleton + 1.);
    np::eval_to(np::tensor_view<double, destination_shape>(actual.data()), singleton + 1.,
                np::xsimd_policy<>{});
    EXPECT_EQ(actual, expected);
}

TEST(NpExpressionXsimd, DynamicSingletonAndUnitInputsKeepScalarProjection) {
    using matrix_shape = np::extents<size_t, 2, 7>;
    std::array<double, 14> source{}, actual{}, expected{};
    std::array<double, 7> row{1, 2, 3, 4, 5, 6, 7};
    const auto x = np::as_expr(np::tensor_view<const double, matrix_shape>(source.data()));
    for (auto length : {size_t{1}, size_t{7}}) {
        const auto b = np::as_expr(view<const double>(row.data(), length));
        const auto expression = x + b;
        np::eval_to(np::tensor_view<double, matrix_shape>(expected.data()), expression);
        np::eval_to(np::tensor_view<double, matrix_shape>(actual.data()), expression,
                    np::xsimd_policy<>{});
        EXPECT_EQ(actual, expected);
    }
}

TEST(NpExpressionXsimd, BroadcastWideIntermediateKeepsItsPrecision) {
    using matrix_shape = np::extents<size_t, 2, 7>;
    using row_shape = np::extents<size_t, 7>;
    std::array<float, 14> source{}, actual{};
    std::array<double, 7> bias{};
    source.fill(1.f);
    bias.fill(0x1p-30);
    const auto expression =
        (np::as_expr(np::tensor_view<const float, matrix_shape>(source.data())) +
         np::as_expr(np::tensor_view<const double, row_shape>(bias.data()))) -
        1.;
    np::eval_to(np::tensor_view<float, matrix_shape>(actual.data()), expression,
                np::xsimd_policy<>{});
    for (auto value : actual)
        EXPECT_EQ(value, 0x1p-30f);
}

TEST(NpExpressionXsimd, NonunitOutputStrideRetainsScalarProjection) {
    using matrix_shape = np::extents<size_t, 2, 7>;
    using row_shape = np::extents<size_t, 7>;
    std::array<double, 14> source{};
    std::array<double, 7> bias{1, 2, 3, 4, 5, 6, 7};
    std::array<double, 31> actual{}, expected{};
    actual.fill(-99.);
    expected = actual;
    using strided = np::tensor_view<double, matrix_shape, np::layout_stride>;
    const np::layout_stride::mapping<matrix_shape> mapping(matrix_shape{},
                                                           std::array<size_t, 2>{16, 2});
    const auto expression =
        np::as_expr(np::tensor_view<const double, matrix_shape>(source.data())) +
        np::as_expr(np::tensor_view<const double, row_shape>(bias.data()));
    np::eval_to(strided(expected.data(), mapping), expression);
    np::eval_to(strided(actual.data(), mapping), expression, np::xsimd_policy<>{});
    EXPECT_EQ(actual, expected);
}

TEST(NpExpressionXsimd, BroadcastExceptionalValuesKeepScalarResults) {
    using matrix_shape = np::extents<size_t, 2, 7>;
    using row_shape = np::extents<size_t, 7>;
    std::array<double, 14> source{}, actual{}, expected{};
    std::array<double, 7> bias{std::numeric_limits<double>::infinity(),
                               -std::numeric_limits<double>::infinity(),
                               std::numeric_limits<double>::quiet_NaN(),
                               -0.,
                               0.,
                               1.,
                               -1.};
    const auto expression =
        -(np::as_expr(np::tensor_view<const double, matrix_shape>(source.data())) +
          np::as_expr(np::tensor_view<const double, row_shape>(bias.data())));
    np::eval_to(np::tensor_view<double, matrix_shape>(expected.data()), expression);
    np::eval_to(np::tensor_view<double, matrix_shape>(actual.data()), expression,
                np::xsimd_policy<>{});
    for (size_t i = 0; i != actual.size(); ++i) {
        if (std::isnan(expected[i]))
            EXPECT_TRUE(std::isnan(actual[i]));
        else {
            EXPECT_EQ(actual[i], expected[i]);
            EXPECT_EQ(std::signbit(actual[i]), std::signbit(expected[i]));
        }
    }
}

} // namespace

namespace {
namespace np = wjr::np;
using slice_shape = np::extents<size_t, 4, 6>;
TEST(np_slice, optional_simd_uses_correct_padded_fallback) {
    double values[24]{};
    np::tensor_view<double, slice_shape> source(values);
    auto window = np::slice_view(source, np::full_extent, std::pair{1, 5});
    np::fill_to(window, 2.0);
    const auto expression = np::as_expr(window) * 3.0;
    double output[16]{};
    np::tensor_view<double, np::extents<size_t, 4, 4>> dense(output);
    np::eval_to(dense, expression, np::xsimd_policy<>{});
    np::eval_to(window, expression, np::xsimd_policy<>{});
    for (size_t row = 0; row != 4; ++row) {
        EXPECT_EQ(source(row, 0), 0);
        EXPECT_EQ(source(row, 5), 0);
        for (size_t col = 0; col != 4; ++col) {
            EXPECT_EQ(window(row, col), 6);
            EXPECT_EQ(dense(row, col), 6);
        }
    }
}
} // namespace
