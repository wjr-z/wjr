#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/scan.hpp>
#include <wjr/np/subview.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 2, 3>;
using input_view = np::tensor_view<const int, shape>;
using output_view = np::tensor_view<int, shape>;

template <class Dst, class Src, class Axes, class Acc = void>
concept scans = requires(Dst dst, Src src, Axes axes) { np::cumsum_to<Acc>(dst, src, axes); };
struct custom_accessor : np::default_accessor<int> {};
static_assert(scans<output_view, input_view, np::axes_t<0>>);
static_assert(scans<output_view, input_view, np::axes_t<1>, long long>);
static_assert(!scans<output_view, input_view, np::axes_t<>>);
static_assert(!scans<output_view, input_view, np::axes_t<0, 1>>);
static_assert(!scans<output_view, input_view, np::axes_t<2>>);
static_assert(!scans<output_view, input_view, np::axes_t<1>, bool>);
static_assert(!scans<input_view, input_view, np::axes_t<1>>);
static_assert(!scans<np::tensor_view<int, np::extents<size_t, 3, 2>>, input_view, np::axes_t<1>>);
static_assert(!scans<np::tensor_view<int, np::extents<size_t, 6>>, input_view, np::axes_t<0>>);
static_assert(!scans<np::tensor_view<int, np::extents<size_t>>,
                     np::tensor_view<int, np::extents<size_t>>, np::axes_t<0>>);
static_assert(!scans<np::tensor_view<volatile int, shape>, input_view, np::axes_t<0>>);
static_assert(!scans<output_view, np::tensor_view<volatile int, shape>, np::axes_t<0>>);
static_assert(!scans<np::tensor_view<int, shape, np::layout_right, custom_accessor>, input_view,
                     np::axes_t<0>>);
static_assert(noexcept(np::cumsum_to(std::declval<output_view>(), std::declval<input_view>(),
                                     np::axes<1>)));

template <class SrcLayout, class DstLayout, size_t Axis>
constexpr bool constant_matrix_scan() {
    int input[] = {1, 2, 3, 4, 5, 6};
    int result[6]{};
    np::tensor_view<const int, shape, SrcLayout> src(input);
    np::tensor_view<int, shape, DstLayout> dst(result);
    np::cumsum_to(dst, src, np::axes<Axis>);
    for (size_t row = 0; row != 2; ++row) {
        for (size_t col = 0; col != 3; ++col) {
            int expected = 0;
            if constexpr (Axis == 0) {
                for (size_t i = 0; i <= row; ++i)
                    expected += src(i, col);
            } else {
                for (size_t i = 0; i <= col; ++i)
                    expected += src(row, i);
            }
            if (dst(row, col) != expected)
                return false;
        }
    }
    return true;
}
static_assert(constant_matrix_scan<np::layout_right, np::layout_right, 1>());
static_assert(constant_matrix_scan<np::layout_left, np::layout_right, 0>());
static_assert(constant_matrix_scan<np::layout_right, np::layout_left, 1>());
static_assert(constant_matrix_scan<np::layout_left, np::layout_left, 0>());

constexpr bool constant_disjoint_and_inplace() {
    int data[] = {1, 0, 2, 0, 3, 0};
    using line_shape = np::extents<size_t, 3>;
    const np::layout_stride::mapping<line_shape> mapping(line_shape{}, std::array{2});
    np::tensor_view<const int, line_shape, np::layout_stride> src(data, mapping);
    np::tensor_view<int, line_shape, np::layout_stride> dst(data + 1, mapping);
    np::cumsum_to(dst, src, np::axes<0>);
    if (data[1] != 1 || data[3] != 3 || data[5] != 6)
        return false;
    np::cumsum_to(dst, dst, np::axes<0>);
    return data[1] == 1 && data[3] == 4 && data[5] == 10 && data[2] == 2;
}
static_assert(constant_disjoint_and_inplace());

constexpr bool constant_empty_and_singleton() {
    using empty_shape = np::dextents<size_t, 3>;
    np::tensor_view<int, empty_shape, np::layout_left> dst(nullptr, 2, 0, 4);
    np::tensor_view<const int, empty_shape> src(nullptr, 2, 0, 4);
    np::cumsum_to(dst, src, np::axes<0>);
    np::cumsum_to(dst, src, np::axes<1>);
    np::cumsum_to(dst, src, np::axes<2>);
    int value = 7;
    np::tensor_view<int, np::extents<size_t, 1>> single(&value);
    np::cumsum_to(single, single, np::axes<0>);
    return value == 7;
}
static_assert(constant_empty_and_singleton());

constexpr bool constant_column_block() {
    using dimensions = np::extents<size_t, 3, 5>;
    int source[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int output[15]{};
    np::cumsum_to(np::tensor_view<int, dimensions>(output),
                  np::tensor_view<const int, dimensions>(source), np::axes<0>);
    for (size_t column = 0; column != 5; ++column) {
        int state = 0;
        for (size_t row = 0; row != 3; ++row) {
            state += source[row * 5 + column];
            if (output[row * 5 + column] != state)
                return false;
        }
    }
    return true;
}
static_assert(constant_column_block());

TEST(np_scan, constant_evaluation_and_empty_domains) {
    EXPECT_TRUE((constant_matrix_scan<np::layout_right, np::layout_right, 1>()));
    EXPECT_TRUE((constant_matrix_scan<np::layout_left, np::layout_right, 0>()));
    EXPECT_TRUE((constant_matrix_scan<np::layout_right, np::layout_left, 1>()));
    EXPECT_TRUE((constant_matrix_scan<np::layout_left, np::layout_left, 0>()));
    EXPECT_TRUE(constant_disjoint_and_inplace());
    EXPECT_TRUE(constant_empty_and_singleton());
    EXPECT_TRUE(constant_column_block());
}

TEST(np_scan, output_conversion_does_not_replace_accumulator) {
    const int integers[] = {16777217, 1, 1};
    float floating[3]{};
    np::cumsum_to(np::tensor_view<float, np::extents<size_t, 3>>(floating),
                  np::tensor_view<const int, np::extents<size_t, 3>>(integers), np::axes<0>);
    EXPECT_EQ(floating[0], static_cast<float>(16777217));
    EXPECT_EQ(floating[1], static_cast<float>(16777218));
    EXPECT_EQ(floating[2], static_cast<float>(16777219));

    const double fractions[] = {.75, .75, .75, .75};
    int truncated[4]{};
    np::cumsum_to(np::tensor_view<int, np::extents<size_t, 4>>(truncated),
                  np::tensor_view<const double, np::extents<size_t, 4>>(fractions), np::axes<0>);
    EXPECT_EQ(truncated[0], 0);
    EXPECT_EQ(truncated[1], 1);
    EXPECT_EQ(truncated[2], 2);
    EXPECT_EQ(truncated[3], 3);
}

TEST(np_scan, explicit_wide_accumulator_and_small_integer_promotion) {
    const float source[] = {100000000, 1, -100000000};
    double defaults[3]{}, wide[3]{};
    const auto src = np::tensor_view<const float, np::extents<size_t, 3>>(source);
    np::cumsum_to(np::tensor_view<double, np::extents<size_t, 3>>(defaults), src, np::axes<0>);
    np::cumsum_to<double>(np::tensor_view<double, np::extents<size_t, 3>>(wide), src, np::axes<0>);
    EXPECT_EQ(defaults[2], 0);
    EXPECT_EQ(wide[2], 1);
    std::uint8_t small[] = {200, 100, 100};
    np::tensor_view<std::uint8_t, np::extents<std::uint8_t, 3>> in_place(small);
    np::cumsum_to(in_place, in_place, np::axes<0>);
    EXPECT_EQ(small[0], 200);
    EXPECT_EQ(small[1], 44);
    EXPECT_EQ(small[2], 144);
}

TEST(np_scan, dynamic_row_and_column_lines_are_independent) {
    const double data[] = {1, 2, 3, 4, 5, 6};
    double rows[6], columns[6];
    using dynamic_shape = np::dextents<size_t, 2>;
    np::tensor_view<const double, dynamic_shape> src(data, 2, 3);
    np::tensor_view<double, dynamic_shape> row_result(rows, 2, 3);
    np::tensor_view<double, dynamic_shape, np::layout_left> column_result(columns, 2, 3);
    np::cumsum_to(row_result, src, np::axes<1>);
    np::cumsum_to(column_result, src, np::axes<0>);
    EXPECT_EQ(row_result(0, 2), 6);
    EXPECT_EQ(row_result(1, 0), 4);
    EXPECT_EQ(row_result(1, 2), 15);
    EXPECT_EQ(column_result(1, 0), 5);
    EXPECT_EQ(column_result(0, 2), 3);
    EXPECT_EQ(column_result(1, 2), 9);
}

TEST(np_scan, rank_three_middle_axis_and_padding) {
    using cube_shape = np::extents<int, 2, 3, 2>;
    int input[32], output[32];
    std::fill_n(input, 32, -100);
    std::fill_n(output, 32, -99);
    const np::layout_stride::mapping<cube_shape> mapping(cube_shape{}, std::array{20, 5, 1});
    np::tensor_view<int, cube_shape, np::layout_stride> src(input, mapping), dst(output, mapping);
    for (int i = 0; i != 2; ++i)
        for (int j = 0; j != 3; ++j)
            for (int k = 0; k != 2; ++k)
                src(i, j, k) = i * 6 + j * 2 + k;
    np::cumsum_to(dst, src, np::axes<1>);
    EXPECT_EQ(dst(0, 0, 1), 1);
    EXPECT_EQ(dst(0, 2, 0), 6);
    EXPECT_EQ(dst(1, 2, 1), 27);
    EXPECT_EQ(output[2], -99);
    EXPECT_EQ(output[19], -99);
    EXPECT_EQ(output[32 - 1], 27);
}

TEST(np_scan, real_bool_and_nan_prefixes) {
    const bool input[] = {true, false, true};
    int counts[3]{};
    np::cumsum_to(np::tensor_view<int, np::extents<size_t, 3>>(counts),
                  np::tensor_view<const bool, np::extents<size_t, 3>>(input), np::axes<0>);
    EXPECT_EQ(counts[2], 2);
    const double source[] = {1, std::numeric_limits<double>::quiet_NaN(), 3};
    double output[3];
    np::cumsum_to(np::tensor_view<double, np::extents<size_t, 3>>(output),
                  np::tensor_view<const double, np::extents<size_t, 3>>(source), np::axes<0>);
    EXPECT_EQ(output[0], 1);
    EXPECT_TRUE(std::isnan(output[1]));
    EXPECT_TRUE(std::isnan(output[2]));
}

template <class Input, class Accumulator, class Output>
void check_column_scan_blocks() {
    using dimensions = np::dextents<size_t, 2>;
    for (size_t rows : {0, 1, 3, 5}) {
        for (size_t columns : {0, 1, 3, 4, 5, 7, 300}) {
            std::vector<Input> input(rows * columns);
            std::vector<Output> actual(rows * columns), expected(rows * columns);
            for (size_t row = 0; row != rows; ++row)
                for (size_t column = 0; column != columns; ++column)
                    input[row * columns + column] =
                        static_cast<Input>((static_cast<int>(row * 3 + column) % 17) - 8);
            for (size_t column = 0; column != columns; ++column) {
                Accumulator state{};
                for (size_t row = 0; row != rows; ++row) {
                    state += static_cast<Accumulator>(input[row * columns + column]);
                    expected[row * columns + column] = static_cast<Output>(state);
                }
            }
            const np::tensor_view<const Input, dimensions> src(input.data(), rows, columns);
            np::cumsum_to<Accumulator>(
                np::tensor_view<Output, dimensions>(actual.data(), rows, columns), src,
                np::axes<0>);
            EXPECT_EQ(actual, expected);
            if constexpr (std::same_as<Input, Output>) {
                np::tensor_view<Input, dimensions> inplace(input.data(), rows, columns);
                np::cumsum_to<Accumulator>(inplace, inplace, np::axes<0>);
                EXPECT_EQ(input, expected);
            }
        }
    }
}

TEST(np_scan, adjacent_column_blocks_and_remainders) {
    check_column_scan_blocks<double, double, double>();
    check_column_scan_blocks<float, double, float>();
    check_column_scan_blocks<std::uint8_t, int, std::uint8_t>();
    check_column_scan_blocks<int, int, float>();
}

TEST(np_scan, column_blocks_keep_order_and_accumulator_between_output_conversions) {
    using dimensions = np::extents<size_t, 3, 5>;
    const float input[] = {1e8f, 1e8f, 1e8f,  1e8f,  1e8f,  1,     1,    1,
                           1,    1,    -1e8f, -1e8f, -1e8f, -1e8f, -1e8f};
    float actual[15]{};
    np::cumsum_to<double>(np::tensor_view<float, dimensions>(actual),
                          np::tensor_view<const float, dimensions>(input), np::axes<0>);
    for (size_t column = 0; column != 5; ++column)
        EXPECT_EQ(actual[10 + column], 1);
}
} // namespace
