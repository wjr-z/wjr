#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

#include <gtest/gtest.h>

#include <wjr/np/difference.hpp>

namespace {
namespace np = wjr::np;
template <class D, class S, size_t Axis>
concept differences = requires(D dst, S src) { np::diff_to(dst, src, np::axes<Axis>); };
using input_view = np::tensor_view<const int, np::extents<size_t, 2, 3>>;
using rows_view = np::tensor_view<int, np::extents<size_t, 2, 2>>;
static_assert(differences<rows_view, input_view, 1>);
static_assert(!differences<rows_view, input_view, 0>);
static_assert(!differences<rows_view, input_view, 2>);
static_assert(!differences<np::tensor_view<const int, np::extents<size_t, 2, 2>>, input_view, 1>);
static_assert(!differences<rows_view, np::tensor_view<volatile int, np::extents<size_t, 2, 3>>, 1>);
static_assert(!differences<np::tensor_view<int, np::extents<size_t>>,
                           np::tensor_view<int, np::extents<size_t>>, 0>);

template <class Layout, class OutputLayout = Layout>
constexpr bool constant_differences() {
    const int input[] = {1, 3, 6, 10, 15, 21};
    np::tensor_view<const int, np::extents<size_t, 2, 3>, Layout> src(input);
    int row_data[4]{}, column_data[3]{};
    np::tensor_view<int, np::extents<size_t, 2, 2>, OutputLayout> rows(row_data);
    np::tensor_view<int, np::extents<size_t, 1, 3>, OutputLayout> columns(column_data);
    np::diff_to(rows, src, np::axes<1>);
    np::diff_to(columns, src, np::axes<0>);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 2; ++col)
            if (rows(row, col) != src(row, col + 1) - src(row, col))
                return false;
    for (size_t col = 0; col != 3; ++col)
        if (columns(0, col) != src(1, col) - src(0, col))
            return false;
    return true;
}
static_assert(constant_differences<np::layout_right>());
static_assert(constant_differences<np::layout_left>());
static_assert(constant_differences<np::layout_left, np::layout_right>());
static_assert(constant_differences<np::layout_right, np::layout_left>());

TEST(np_difference, PackedLayoutsAndBothAxes) {
    EXPECT_TRUE(constant_differences<np::layout_right>());
    EXPECT_TRUE(constant_differences<np::layout_left>());
}

TEST(np_difference, EmptyAndSingletonAxes) {
    np::tensor_view<const double, np::extents<size_t, 3, 0>> empty(nullptr);
    np::tensor_view<double, np::extents<size_t, 3, 0>> empty_result(nullptr);
    np::diff_to(empty_result, empty, np::axes<1>);
    np::tensor_view<const double, np::dextents<size_t, 2>> dynamic_empty(nullptr, 0, 3);
    np::tensor_view<double, np::dextents<size_t, 2>> dynamic_result(nullptr, 0, 3);
    np::diff_to(dynamic_result, dynamic_empty, np::axes<0>);
    const double one[] = {1, 2, 3};
    np::tensor_view<const double, np::extents<size_t, 3, 1>> singleton(one);
    np::diff_to(empty_result, singleton, np::axes<1>);
}

TEST(np_difference, PositiveStridesAndOutputPadding) {
    using source_shape = np::extents<size_t, 2, 3>;
    using destination_shape = np::extents<size_t, 2, 2>;
    const int input[] = {1, -9, 3, -9, 6, -9, -9, -9, 10, -9, 15, -9, 21};
    const np::layout_stride::mapping<source_shape> input_mapping(source_shape{},
                                                                 std::array<size_t, 2>{8, 2});
    const np::tensor_view<const int, source_shape, np::layout_stride> source(input, input_mapping);
    int output[] = {-1, -1, -1, -1, -1, -1, -1, -1};
    const np::layout_stride::mapping<destination_shape> output_mapping(destination_shape{},
                                                                       std::array<size_t, 2>{5, 2});
    const np::tensor_view<int, destination_shape, np::layout_stride> destination(output,
                                                                                 output_mapping);
    np::diff_to(destination, source, np::axes<1>);
    EXPECT_EQ((std::to_array(output)), (std::array{2, -1, 3, -1, -1, 5, -1, 6}));
}

TEST(np_difference, BoolAndIntegerConversion) {
    const bool flags[] = {false, false, true, true, false};
    bool changes[4]{};
    np::diff_to(np::tensor_view<bool, np::extents<size_t, 4>>(changes),
                np::tensor_view<const bool, np::extents<size_t, 5>>(flags), np::axes<0>);
    EXPECT_EQ((std::to_array(changes)), (std::array{false, true, false, true}));
    const std::uint8_t input[] = {5, 2, 9};
    int wide[2]{};
    np::diff_to(np::tensor_view<int, np::extents<size_t, 2>>(wide),
                np::tensor_view<const std::uint8_t, np::extents<size_t, 3>>(input), np::axes<0>);
    EXPECT_EQ(wide[0], -3);
    EXPECT_EQ(wide[1], 7);
}

TEST(np_difference, RankThreeAndFloatingValues) {
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double input[] = {1, 3, 6, -0.0, 0.0, inf, 2, 4, 8, nan, 1, inf};
    np::tensor_view<const double, np::extents<size_t, 2, 2, 3>> src(input);
    double output[8]{};
    np::tensor_view<double, np::extents<size_t, 2, 2, 2>> dst(output);
    np::diff_to(dst, src, np::axes<2>);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 2; ++j)
            for (size_t k = 0; k != 2; ++k) {
                const auto expected = src(i, j, k + 1) - src(i, j, k);
                if (std::isnan(expected))
                    EXPECT_TRUE(std::isnan(dst(i, j, k)));
                else
                    EXPECT_EQ(dst(i, j, k), expected);
            }
}

TEST(np_difference, PackedInteriorAxisAndNarrowIndices) {
    const int input[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int output[6];
    np::diff_to(np::tensor_view<int, np::extents<size_t, 2, 1, 3>>(output),
                np::tensor_view<const int, np::extents<size_t, 2, 2, 3>>(input), np::axes<1>);
    for (int value : output)
        EXPECT_EQ(value, 3);
    using index_type = std::uint16_t;
    const int small[] = {2, 5, 9};
    np::diff_to(np::tensor_view<int, np::extents<index_type, 2>>(output),
                np::tensor_view<const int, np::extents<index_type, 3>>(small), np::axes<0>);
    EXPECT_EQ(output[0], 3);
    EXPECT_EQ(output[1], 4);
}
} // namespace
