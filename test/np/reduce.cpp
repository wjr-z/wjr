#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/reduce.hpp>
#include <wjr/np/subview.hpp>
#include <wjr/np/transform.hpp>
#include <wjr/np/transpose.hpp>

namespace {
namespace np = wjr::np;
using matrix_shape = np::extents<size_t, 2, 3>;
using matrix_view = np::tensor_view<const int, matrix_shape>;
using rows_view = np::tensor_view<int, np::extents<size_t, 2>>;

template <class View, class Acc = void>
concept sums = requires(View source) { np::sum<Acc>(source); };
template <class Dst, class Src, class Axes>
concept reduces = requires(Dst dst, Src src, Axes axes) { np::sum_to(dst, src, axes); };
template <class Dst, class Src, class Axes>
concept reduces_keepdims =
    requires(Dst dst, Src src, Axes axes) { np::sum_to(dst, src, axes, np::keepdims); };

struct custom_accessor : np::default_accessor<int> {};
struct object {
    int value;
};
static_assert(sums<matrix_view> && sums<matrix_view, double>);
static_assert(!sums<matrix_view, bool> && !sums<matrix_view, unsigned char>);
static_assert(!sums<matrix_view, const double>);
static_assert(!sums<np::tensor_view<volatile int, matrix_shape>>);
static_assert(!sums<np::tensor_view<object, matrix_shape>>);
static_assert(!sums<np::tensor_view<int, matrix_shape, np::layout_right, custom_accessor>>);
static_assert(reduces<rows_view, matrix_view, np::axes_t<1>>);
static_assert(!reduces<rows_view, matrix_view, np::axes_t<>>);
static_assert(!reduces<rows_view, matrix_view, np::axes_t<0, 1>>);
static_assert(!reduces<rows_view, matrix_view, np::axes_t<2>>);
static_assert(!reduces<rows_view, matrix_view, np::axes_t<0>>);
static_assert(
    !reduces<np::tensor_view<const int, np::extents<size_t, 2>>, matrix_view, np::axes_t<1>>);
static_assert(!reduces<np::tensor_view<int, np::extents<size_t>>,
                       np::tensor_view<int, np::extents<size_t>>, np::axes_t<0>>);
static_assert(
    reduces_keepdims<np::tensor_view<int, np::extents<size_t, 2, 1>>, matrix_view, np::axes_t<1>>);
static_assert(
    !reduces_keepdims<np::tensor_view<int, np::extents<size_t, 2, 2>>, matrix_view, np::axes_t<1>>);
static_assert(
    std::same_as<decltype(np::sum(
                     std::declval<np::tensor_view<const std::uint8_t, np::extents<size_t, 3>>>())),
                 int>);
static_assert(
    std::same_as<decltype(np::sum(std::declval<np::tensor_view<bool, np::extents<size_t, 3>>>())),
                 int>);
static_assert(
    std::same_as<decltype(np::sum(std::declval<np::tensor_view<float, np::extents<size_t, 3>>>())),
                 float>);
static_assert(noexcept(np::sum(std::declval<matrix_view>())));
static_assert(noexcept(np::sum_to(std::declval<rows_view>(), std::declval<matrix_view>(),
                                  np::axes<1>)));

template <class Layout>
constexpr bool constant_sums() {
    const int data[] = {1, 2, 3, 4, 5, 6};
    np::tensor_view<const int, matrix_shape, Layout> src(data);
    int row_data[2]{}, column_data[3]{}, kept_data[2]{};
    np::tensor_view<int, np::extents<size_t, 2>> rows(row_data);
    np::tensor_view<int, np::extents<size_t, 3>> columns(column_data);
    np::tensor_view<int, np::extents<size_t, 2, 1>, np::layout_left> kept(kept_data);
    np::sum_to(rows, src, np::axes<1>);
    np::sum_to(columns, src, np::axes<0>);
    np::sum_to(kept, src, np::axes<1>, np::keepdims);
    for (size_t row = 0; row != 2; ++row) {
        int expected = 0;
        for (size_t col = 0; col != 3; ++col)
            expected += src(row, col);
        if (rows(row) != expected || kept(row, 0) != expected)
            return false;
    }
    for (size_t col = 0; col != 3; ++col)
        if (columns(col) != src(0, col) + src(1, col))
            return false;
    const auto transposed = np::transpose_view(src);
    return np::sum(src) == 21 && np::sum(transposed) == 21 && np::sum<long long>(src) == 21;
}
static_assert(constant_sums<np::layout_right>());
static_assert(constant_sums<np::layout_left>());

constexpr bool constant_scalar_and_empty() {
    const int data = 7;
    np::tensor_view<const int, np::extents<size_t>> scalar(&data);
    if (np::sum(scalar) != 7)
        return false;
    np::tensor_view<const int, np::dextents<size_t, 2>, np::layout_left> empty(nullptr, 3, 0);
    int zeros[] = {9, 9, 9};
    np::tensor_view<int, np::extents<size_t, 3>> rows(zeros);
    np::sum_to(rows, empty, np::axes<1>);
    if (np::sum(empty) != 0 || zeros[0] != 0 || zeros[1] != 0 || zeros[2] != 0)
        return false;
    np::tensor_view<int, np::extents<size_t, 0>> no_rows(nullptr);
    np::sum_to(no_rows, empty, np::axes<0>);
    const int values[] = {1, 2, 3};
    int result = -1;
    np::tensor_view<int, np::extents<size_t>> dst(&result);
    np::sum_to(dst, np::tensor_view<const int, np::extents<size_t, 3>>(values), np::axes<0>);
    if (result != 6)
        return false;
    np::sum_to(dst, np::tensor_view<const int, np::extents<size_t, 0>>(nullptr), np::axes<0>);
    np::tensor_view<const int, np::extents<size_t>, np::layout_stride> strided_scalar(
        &data, np::layout_stride::mapping<np::extents<size_t>>(np::extents<size_t>{},
                                                               std::array<size_t, 0>{}));
    return result == 0 && np::sum(strided_scalar) == 7;
}
static_assert(constant_scalar_and_empty());

constexpr bool constant_disjoint_columns() {
    int data[] = {1, 2, 0, 4, 5, 0};
    using src_shape = np::extents<size_t, 2, 2>;
    using dst_shape = np::extents<size_t, 2>;
    np::tensor_view<const int, src_shape, np::layout_stride> src(
        data, np::layout_stride::mapping<src_shape>(src_shape{}, std::array{3, 1}));
    np::tensor_view<int, dst_shape, np::layout_stride> dst(
        data + 2, np::layout_stride::mapping<dst_shape>(dst_shape{}, std::array{3}));
    np::sum_to(dst, src, np::axes<1>);
    return data[2] == 3 && data[5] == 9 && np::sum(src) == 12;
}
static_assert(constant_disjoint_columns());

TEST(np_reduce, constexpr_rows_columns_scalar_and_empty) {
    EXPECT_TRUE(constant_sums<np::layout_right>());
    EXPECT_TRUE(constant_sums<np::layout_left>());
    EXPECT_TRUE(constant_scalar_and_empty());
    EXPECT_TRUE(constant_disjoint_columns());
}

TEST(np_reduce, accumulator_promotion_is_independent_of_output) {
    const std::uint8_t small[] = {200, 200, 200};
    EXPECT_EQ(np::sum(np::tensor_view<const std::uint8_t, np::extents<std::uint8_t, 3>>(small)),
              600);
    const bool bits[] = {true, false, true};
    EXPECT_EQ(np::sum(np::tensor_view<const bool, np::extents<size_t, 3>>(bits)), 2);
    const float input[] = {100000000, 1, -100000000};
    np::tensor_view<const float, np::extents<size_t, 1, 3>> src(input);
    double default_value = -1, wide_value = -1;
    np::sum_to(np::tensor_view<double, np::extents<size_t, 1>>(&default_value), src, np::axes<1>);
    np::sum_to<double>(np::tensor_view<double, np::extents<size_t, 1>>(&wide_value), src,
                       np::axes<1>);
    EXPECT_EQ(np::sum(src), 0);
    EXPECT_EQ(np::sum<double>(src), 1);
    EXPECT_EQ(default_value, 0);
    EXPECT_EQ(wide_value, 1);
}

TEST(np_reduce, logical_order_is_independent_of_layout) {
    using square = np::extents<size_t, 2, 2>;
    const double row_data[] = {1e16, 1, -1e16, 1};
    const double column_data[] = {1e16, -1e16, 1, 1};
    np::tensor_view<const double, square> row(row_data);
    np::tensor_view<const double, square, np::layout_left> column(column_data);
    EXPECT_EQ(np::sum(row), 1);
    EXPECT_EQ(np::sum(column), 1);
    const double physical =
        ((0.0 + column_data[0]) + column_data[1]) + column_data[2] + column_data[3];
    EXPECT_EQ(physical, 2);
}

TEST(np_reduce, rank_three_middle_axis_and_output_padding) {
    using cube_shape = np::extents<int, 2, 3, 2>;
    using result_shape = np::extents<size_t, 2, 2>;
    int source[32];
    int output[8];
    std::fill_n(source, 32, -100);
    std::fill_n(output, 8, -99);
    np::tensor_view<int, cube_shape, np::layout_stride> src(
        source, np::layout_stride::mapping<cube_shape>(cube_shape{}, std::array{20, 5, 1}));
    for (int i = 0; i != 2; ++i)
        for (int j = 0; j != 3; ++j)
            for (int k = 0; k != 2; ++k)
                src(i, j, k) = i * 6 + j * 2 + k;
    np::tensor_view<int, result_shape, np::layout_stride> dst(
        output, np::layout_stride::mapping<result_shape>(result_shape{}, std::array{5, 2}));
    np::sum_to(dst, src, np::axes<1>);
    EXPECT_EQ(np::sum(src), 66);
    EXPECT_EQ(output[0], 6);
    EXPECT_EQ(output[2], 9);
    EXPECT_EQ(output[5], 24);
    EXPECT_EQ(output[7], 27);
    for (auto i : {1, 3, 4, 6})
        EXPECT_EQ(output[i], -99);
}

TEST(np_reduce, dynamic_extent_keepdims_and_strided_column) {
    const double data[] = {1, 2, 3, 4, 5, 6};
    np::tensor_view<const double, np::dextents<size_t, 2>> src(data, 2, 3);
    double storage[3]{};
    np::tensor_view<double, np::dextents<int, 2>, np::layout_left> dst(storage, 1, 3);
    np::sum_to(dst, src, np::axes<0>, np::keepdims);
    EXPECT_EQ(dst(0, 0), 5);
    EXPECT_EQ(dst(0, 2), 9);
    const auto column = np::try_column_view(src, 1);
    ASSERT_TRUE(column);
    EXPECT_EQ(np::sum(*column), 7);
}

TEST(np_reduce, row_centering_and_squared_residuals) {
    const double data[] = {1, 2, 3, 4, 6, 8};
    double rows[2], centered[6];
    np::tensor_view<const double, matrix_shape> src(data);
    np::tensor_view<double, np::extents<size_t, 2, 1>> totals(rows);
    np::tensor_view<double, matrix_shape> out(centered);
    np::sum_to<double>(totals, src, np::axes<1>, np::keepdims);
    for (size_t row = 0; row != 2; ++row) {
        const double mean = totals(row, 0) / 3;
        np::transform_to(*np::try_row_view(out, row), *np::try_row_view(src, row),
                         [mean](double x) noexcept { return x - mean; });
    }
    EXPECT_EQ(np::sum(out), 0);
    np::transform_to(out, out, [](double x) noexcept { return x * x; });
    EXPECT_EQ(np::sum(out), 10);
}

TEST(np_reduce, ieee_values_follow_ordered_scalar_addition) {
    using single = np::extents<size_t, 1>;
    const double negative_zero = -0.0;
    const double infinity = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_FALSE(std::signbit(np::sum(np::tensor_view<const double, single>(&negative_zero))));
    EXPECT_TRUE(std::isinf(np::sum(np::tensor_view<const double, single>(&infinity))));
    EXPECT_TRUE(std::isnan(np::sum(np::tensor_view<const double, single>(&nan))));
}

template <class Input, class Accumulator, class Output>
void check_column_blocks() {
    using input_shape = np::dextents<size_t, 2>;
    using output_shape = np::dextents<size_t, 1>;
    for (size_t rows : {0, 1, 3, 5}) {
        for (size_t columns : {0, 1, 3, 4, 5, 7, 300}) {
            std::vector<Input> input(rows * columns);
            std::vector<Output> actual(columns, Output{9}), expected(columns), kept(columns);
            for (size_t row = 0; row != rows; ++row)
                for (size_t column = 0; column != columns; ++column)
                    input[row * columns + column] =
                        static_cast<Input>((static_cast<int>(row * 3 + column) % 17) - 8);
            for (size_t column = 0; column != columns; ++column) {
                Accumulator value{};
                for (size_t row = 0; row != rows; ++row)
                    value += static_cast<Accumulator>(input[row * columns + column]);
                expected[column] = static_cast<Output>(value);
            }
            const np::tensor_view<const Input, input_shape> src(input.data(), rows, columns);
            np::sum_to<Accumulator>(
                np::tensor_view<Output, output_shape, np::layout_left>(actual.data(), columns), src,
                np::axes<0>);
            np::sum_to<Accumulator>(np::tensor_view<Output, input_shape>(kept.data(), 1, columns),
                                    src, np::axes<0>, np::keepdims);
            EXPECT_EQ(actual, expected);
            EXPECT_EQ(kept, expected);
        }
    }
}

TEST(np_reduce, adjacent_column_blocks_and_remainders) {
    check_column_blocks<double, double, double>();
    check_column_blocks<float, double, float>();
    check_column_blocks<std::uint8_t, int, std::uint8_t>();
}

TEST(np_reduce, column_blocks_keep_order_and_wide_accumulator) {
    using input_shape = np::extents<size_t, 3, 5>;
    using output_shape = np::extents<size_t, 5>;
    const float input[] = {1e8f, 1e8f, 1e8f,  1e8f,  1e8f,  1,     1,    1,
                           1,    1,    -1e8f, -1e8f, -1e8f, -1e8f, -1e8f};
    float defaults[5]{};
    double wide[5]{};
    const np::tensor_view<const float, input_shape> src(input);
    np::sum_to(np::tensor_view<float, output_shape>(defaults), src, np::axes<0>);
    np::sum_to<double>(np::tensor_view<double, output_shape>(wide), src, np::axes<0>);
    for (size_t column = 0; column != 5; ++column) {
        EXPECT_EQ(defaults[column], 0);
        EXPECT_EQ(wide[column], 1);
    }
}
} // namespace
