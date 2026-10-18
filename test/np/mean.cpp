#include <array>
#include <cmath>
#include <limits>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/mean.hpp>
#include <wjr/np/transpose.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 2, 3>;
using input_view = np::tensor_view<const int, shape>;
using output_view = np::tensor_view<double, np::extents<size_t, 2>>;

template <class V, class A = void>
concept has_mean = requires(V v) { np::mean<A>(v); };
template <class D, class S, class A = void>
concept has_axis_mean = requires(D d, S s) { np::mean_to<A>(d, s, np::axes<1>); };

static_assert(has_mean<input_view> && has_mean<input_view, float>);
static_assert(!has_mean<input_view, int> && !has_mean<input_view, const double>);
static_assert(!has_mean<np::tensor_view<volatile int, shape>>);
static_assert(has_axis_mean<output_view, input_view>);
static_assert(!has_axis_mean<np::tensor_view<int, np::extents<size_t, 2>>, input_view>);
static_assert(!has_axis_mean<np::tensor_view<const double, np::extents<size_t, 2>>, input_view>);
static_assert(!has_axis_mean<output_view, input_view, int>);
static_assert(!has_axis_mean<np::tensor_view<double, np::extents<size_t, 3>>, input_view>);
static_assert(std::same_as<decltype(np::mean(std::declval<input_view>())), double>);
static_assert(
    std::same_as<decltype(np::mean(std::declval<np::tensor_view<const float, shape>>())), float>);
static_assert(noexcept(np::mean(std::declval<input_view>())));
static_assert(noexcept(np::mean_to(std::declval<output_view>(), std::declval<input_view>(),
                                   np::axes<1>)));

template <class Layout>
constexpr bool constant_mean() {
    const int input[] = {1, 2, 3, 4, 5, 6};
    np::tensor_view<const int, shape, Layout> src(input);
    double row_data[2]{}, column_data[3]{}, kept_data[2]{};
    output_view rows(row_data);
    np::tensor_view<double, np::extents<size_t, 3>> columns(column_data);
    np::tensor_view<double, np::extents<size_t, 2, 1>, np::layout_left> kept(kept_data);
    np::mean_to(rows, src, np::axes<1>);
    np::mean_to(columns, src, np::axes<0>);
    np::mean_to(kept, src, np::axes<1>, np::keepdims);
    for (size_t i = 0; i != 2; ++i) {
        const auto expected = (double(src(i, 0)) + src(i, 1) + src(i, 2)) / 3;
        if (rows(i) != expected || kept(i, 0) != expected)
            return false;
    }
    for (size_t j = 0; j != 3; ++j)
        if (columns(j) != (double(src(0, j)) + src(1, j)) / 2)
            return false;
    return np::mean(src) == 3.5 && np::mean(np::transpose_view(src)) == 3.5;
}
static_assert(constant_mean<np::layout_right>());
static_assert(constant_mean<np::layout_left>());

TEST(np_mean, IntegerBoolAndScalar) {
    EXPECT_TRUE(constant_mean<np::layout_right>());
    EXPECT_TRUE(constant_mean<np::layout_left>());
    const bool flags[] = {true, false, true, true};
    EXPECT_EQ(np::mean(np::tensor_view<const bool, np::extents<size_t, 4>>(flags)), 0.75);
    const int value = 9;
    EXPECT_EQ(np::mean(np::tensor_view<const int, np::extents<size_t>>(&value)), 9.0);
    const int large[] = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    EXPECT_EQ(np::mean(np::tensor_view<const int, np::extents<size_t, 2>>(large)),
              double(std::numeric_limits<int>::max()));
}

TEST(np_mean, AccumulatorIsIndependentOfDestination) {
    const float input[] = {16777216.0f, 1.0f, -16777216.0f};
    np::tensor_view<const float, np::extents<size_t, 1, 3>> src(input);
    float output = -1;
    np::tensor_view<float, np::extents<size_t, 1>> dst(&output);
    EXPECT_EQ(np::mean(src), 0.0f);
    EXPECT_EQ(np::mean<double>(src), 1.0 / 3.0);
    np::mean_to(dst, src, np::axes<1>);
    EXPECT_EQ(output, 0.0f);
    np::mean_to<double>(dst, src, np::axes<1>);
    EXPECT_EQ(output, float(1.0 / 3.0));
}

TEST(np_mean, EmptyInputsAndReducedLines) {
    np::tensor_view<const float, np::dextents<size_t, 1>> empty(nullptr, 0);
    EXPECT_TRUE(std::isnan(np::mean(empty)));
    np::tensor_view<const int, np::dextents<size_t, 2>> no_columns(nullptr, 3, 0);
    double output[] = {1, 2, 3};
    np::tensor_view<double, np::extents<size_t, 3>> dst(output);
    np::mean_to(dst, no_columns, np::axes<1>);
    for (double value : output)
        EXPECT_TRUE(std::isnan(value));
    np::tensor_view<double, np::extents<size_t, 3, 1>> kept(output);
    np::mean_to(kept, no_columns, np::axes<1>, np::keepdims);
    for (double value : output)
        EXPECT_TRUE(std::isnan(value));
    // No output coordinates: no access, even when the reduced axis is nonempty.
    np::tensor_view<const int, np::dextents<size_t, 2>> no_rows(nullptr, 0, 7);
    np::tensor_view<double, np::dextents<size_t, 1>> no_output(nullptr, 0);
    np::mean_to(no_output, no_rows, np::axes<1>);
}

TEST(np_mean, ColumnBlocksAndTails) {
    for (size_t columns = 1; columns != 18; ++columns) {
        std::vector<float> input(7 * columns);
        for (size_t i = 0; i != input.size(); ++i)
            input[i] = float(int(i % 17) - 8);
        np::tensor_view<const float, np::dextents<size_t, 2>> src(input.data(), 7, columns);
        std::vector<double> output(columns, -999);
        np::tensor_view<double, np::dextents<size_t, 1>> dst(output.data(), columns);
        np::mean_to<double>(dst, src, np::axes<0>);
        for (size_t col = 0; col != columns; ++col) {
            double expected = 0;
            for (size_t row = 0; row != 7; ++row)
                expected += double(input[row * columns + col]);
            EXPECT_EQ(output[col], expected / 7.0);
        }
    }
}

TEST(np_mean, StridesAndPaddingAreNotReadOrWritten) {
    using extents = np::extents<size_t, 2, 3>;
    using mapping = np::layout_stride::mapping<extents>;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double input[] = {1, nan, 2, nan, 3, nan, nan, nan, 4, nan, 5, nan, 6};
    const mapping map(extents{}, std::array<size_t, 2>{8, 2});
    np::tensor_view<const double, extents, np::layout_stride> src(input, map);
    EXPECT_EQ(np::mean(src), 3.5);
    double output[] = {-9, -9, -9};
    using dst_extents = np::extents<size_t, 2>;
    const np::layout_stride::mapping<dst_extents> dst_map(dst_extents{}, std::array<size_t, 1>{2});
    np::tensor_view<double, dst_extents, np::layout_stride> dst(output, dst_map);
    np::mean_to(dst, src, np::axes<1>);
    EXPECT_EQ(output[0], 2);
    EXPECT_EQ(output[1], -9);
    EXPECT_EQ(output[2], 5);
    const np::layout_stride::mapping<dst_extents> input_map(dst_extents{},
                                                            std::array<size_t, 1>{8});
    EXPECT_EQ(
        np::mean(np::tensor_view<const double, dst_extents, np::layout_stride>(input, input_map)),
        2.5);
}

TEST(np_mean, RankThreeAxisAndKeepdims) {
    const int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    np::tensor_view<const int, np::extents<size_t, 2, 2, 3>> src(input);
    double output[6]{};
    np::tensor_view<double, np::extents<size_t, 2, 1, 3>> dst(output);
    np::mean_to(dst, src, np::axes<1>, np::keepdims);
    for (size_t outer = 0; outer != 2; ++outer)
        for (size_t col = 0; col != 3; ++col)
            EXPECT_EQ(dst(outer, 0, col), (double(src(outer, 0, col)) + src(outer, 1, col)) / 2);
}

TEST(np_mean, NonfiniteAndOverflowFollowOrderedArithmetic) {
    const double inf = std::numeric_limits<double>::infinity();
    double input[] = {inf, -inf};
    np::tensor_view<double, np::extents<size_t, 2>> src(input);
    EXPECT_TRUE(std::isnan(np::mean(src)));
    input[1] = 1;
    EXPECT_EQ(np::mean(src), inf);
    input[1] = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(std::isnan(np::mean(src)));
    // This is sum/count, not a separate overflow-resistant mean algorithm.
    input[0] = input[1] = std::numeric_limits<double>::max();
    EXPECT_EQ(np::mean(src), inf);
    input[0] = input[1] = -0.0;
    EXPECT_FALSE(std::signbit(np::mean(src)));
}
} // namespace
