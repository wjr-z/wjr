#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/expression_cast.hpp>
#include <wjr/np/expression_mask.hpp>
#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_scan.hpp>
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using matrix = np::extents<size_t, 2, 3>;
using leaf = decltype(np::as_expr(std::declval<view<const int, matrix>>()));

template <class Dst, class E, class Axes, class Acc = void>
concept scans =
    requires(Dst dst, const E &expr, Axes axes) { np::cumsum_to<Acc>(dst, expr, axes); };
static_assert(scans<view<int, matrix>, leaf, np::axes_t<0>>);
static_assert(scans<view<float, matrix>, leaf, np::axes_t<1>, long long>);
static_assert(!scans<view<const int, matrix>, leaf, np::axes_t<0>>);
static_assert(!scans<view<volatile int, matrix>, leaf, np::axes_t<0>>);
static_assert(!scans<view<int, matrix>, leaf, np::axes_t<>>);
static_assert(!scans<view<int, matrix>, leaf, np::axes_t<0, 1>>);
static_assert(!scans<view<int, matrix>, leaf, np::axes_t<2>>);
static_assert(!scans<view<int, matrix>, leaf, np::axes_t<1>, bool>);
static_assert(!scans<view<int, np::extents<size_t, 6>>, leaf, np::axes_t<0>>);
static_assert(!scans<view<int, np::extents<size_t, 3, 2>>, leaf, np::axes_t<1>>);
static_assert(!scans<view<int, np::extents<size_t>>, decltype(np::as_expr(2)), np::axes_t<0>>);
static_assert(noexcept(np::cumsum_to(std::declval<view<int, matrix>>(),
                                     std::declval<const leaf &>(), np::axes<0>)));

template <class SrcLayout, class DstLayout, size_t Axis>
constexpr bool constant_scan() {
    int data[]{1, 2, 3, 4, 5, 6}, output[6]{};
    view<const int, matrix, SrcLayout> src(data);
    view<int, matrix, DstLayout> dst(output);
    const auto expr = 2 * np::as_expr(src) + 1;
    np::cumsum_to(dst, expr, np::axes<Axis>);
    for (size_t row = 0; row != 2; ++row) {
        for (size_t col = 0; col != 3; ++col) {
            int expected = 0;
            if constexpr (Axis == 0)
                for (size_t i = 0; i <= row; ++i)
                    expected += 2 * src(i, col) + 1;
            else
                for (size_t i = 0; i <= col; ++i)
                    expected += 2 * src(row, i) + 1;
            if (dst(row, col) != expected)
                return false;
        }
    }
    return true;
}
static_assert(constant_scan<np::layout_right, np::layout_right, 1>());
static_assert(constant_scan<np::layout_left, np::layout_right, 0>());
static_assert(constant_scan<np::layout_right, np::layout_left, 0>());
static_assert(constant_scan<np::layout_left, np::layout_left, 1>());

constexpr bool constant_strided_inplace() {
    using shape = np::extents<size_t, 3>;
    int data[]{1, 0, 2, 0, 3, 0};
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array{2});
    view<const int, shape, np::layout_stride> src(data, mapping);
    view<int, shape, np::layout_stride> dst(data + 1, mapping);
    np::cumsum_to(dst, np::as_expr(src) * 2, np::axes<0>);
    if (data[1] != 2 || data[3] != 6 || data[5] != 12)
        return false;
    np::cumsum_to(dst, np::as_expr(dst) + 1, np::axes<0>);
    return data[1] == 3 && data[3] == 10 && data[5] == 23 && data[4] == 3;
}
static_assert(constant_strided_inplace());

TEST(NpExpressionScan, ConstantEvaluationAndMixedLayouts) {
    EXPECT_TRUE((constant_scan<np::layout_right, np::layout_right, 1>()));
    EXPECT_TRUE((constant_scan<np::layout_left, np::layout_right, 0>()));
    EXPECT_TRUE((constant_scan<np::layout_right, np::layout_left, 0>()));
    EXPECT_TRUE((constant_scan<np::layout_left, np::layout_left, 1>()));
    EXPECT_TRUE(constant_strided_inplace());
}

TEST(NpExpressionScan, BroadcastMapSelectMathAndNoCallableCopies) {
    using dimensions = np::extents<size_t, 2, 5>;
    int input[]{-2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
    double bias[]{1, 2};
    const auto a = np::as_expr(view<const int, dimensions>(input));
    const auto b = np::as_expr(view<const double, np::extents<size_t, 2, 1>>(bias));
    const auto expr = np::sqrt(np::select(a < 0, -a, a) + b);
    double output[10]{};
    view<double, dimensions> dst(output);
    np::cumsum_to(dst, expr, np::axes<1>);
    for (size_t row = 0; row != 2; ++row) {
        double state = 0;
        for (size_t col = 0; col != 5; ++col) {
            state += std::sqrt(std::abs(input[row * 5 + col]) + bias[row]);
            EXPECT_EQ(dst(row, col), state);
        }
    }
    struct scale_operation {
        int *copies;
        explicit scale_operation(int &count) : copies(&count) {}
        scale_operation(const scale_operation &other) : copies(other.copies) { ++*copies; }
        double operator()(int x) const noexcept { return x * 0.5; }
    };
    int copies = 0;
    const auto mapped = np::map(scale_operation(copies), a);
    copies = 0;
    np::cumsum_to(dst, mapped, np::axes<0>);
    EXPECT_EQ(copies, 0);
    for (size_t col = 0; col != 5; ++col) {
        EXPECT_EQ(dst(0, col), input[col] * 0.5);
        EXPECT_EQ(dst(1, col), (input[col] + input[5 + col]) * 0.5);
    }
}

TEST(NpExpressionScan, EachPrefixUsesIndependentPromotedAccumulator) {
    const uint8_t bytes[]{200, 100, 100};
    uint8_t narrow[3]{};
    using three = np::extents<size_t, 3>;
    np::cumsum_to(view<uint8_t, three>(narrow), np::as_expr(view<const uint8_t, three>(bytes)),
                  np::axes<0>);
    EXPECT_EQ((std::array{narrow[0], narrow[1], narrow[2]}),
              (std::array<uint8_t, 3>{200, 44, 144}));
    const int integers[]{16777217, 1, 1};
    float floating[3]{};
    np::cumsum_to(view<float, three>(floating), np::as_expr(view<const int, three>(integers)),
                  np::axes<0>);
    EXPECT_EQ(floating[0], float(16777217));
    EXPECT_EQ(floating[1], float(16777218));
    EXPECT_EQ(floating[2], float(16777219));
    const int large[]{std::numeric_limits<int>::max(), 1, 1};
    long long wide[3]{};
    np::cumsum_to<long long>(view<long long, three>(wide),
                             np::as_expr(view<const int, three>(large)), np::axes<0>);
    EXPECT_EQ(wide[2], static_cast<long long>(std::numeric_limits<int>::max()) + 2);
    bool flags[]{true, false, true};
    int counts[3]{};
    np::cumsum_to(view<int, three>(counts), np::as_expr(view<const bool, three>(flags)),
                  np::axes<0>);
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2]}), (std::array{1, 1, 2}));
}

TEST(NpExpressionScan, FloatingPointOrderAndExactInPlace) {
    using dimensions = np::extents<size_t, 3, 5>;
    double input[15]{};
    for (size_t col = 0; col != 5; ++col) {
        input[col] = 1e20;
        input[5 + col] = 1;
        input[10 + col] = -1e20;
    }
    view<double, dimensions> dst(input);
    np::cumsum_to(dst, np::as_expr(dst) + 0., np::axes<0>);
    for (size_t col = 0; col != 5; ++col) {
        EXPECT_EQ(dst(0, col), 1e20);
        EXPECT_EQ(dst(1, col), 1e20);
        EXPECT_EQ(dst(2, col), 0.);
    }
}

TEST(NpExpressionScan, AxisZeroBlocksTailsAndBroadcastLeaves) {
    using dimensions = np::dextents<size_t, 2>;
    for (size_t rows = 0; rows != 6; ++rows) {
        for (size_t cols = 0; cols != 10; ++cols) {
            std::vector<int> input(rows * cols), expected(input.size()), output(input.size());
            std::vector<int> row_bias(rows), column_bias(cols);
            for (size_t i = 0; i != input.size(); ++i)
                input[i] = int(i % 13);
            for (size_t i = 0; i != rows; ++i)
                row_bias[i] = int(i);
            for (size_t i = 0; i != cols; ++i)
                column_bias[i] = int(i % 3);
            for (size_t col = 0; col != cols; ++col) {
                int state = 0;
                for (size_t row = 0; row != rows; ++row) {
                    state += input[row * cols + col] * 2 + row_bias[row] + column_bias[col];
                    expected[row * cols + col] = state;
                }
            }
            view<int, dimensions> dst(output.data(), rows, cols), inplace(input.data(), rows, cols);
            const auto a = np::as_expr(inplace);
            const auto b = np::as_expr(view<const int, dimensions>(row_bias.data(), rows, 1));
            const auto c =
                np::as_expr(view<const int, np::dextents<size_t, 1>>(column_bias.data(), cols));
            const auto expr = a * 2 + b + c;
            np::cumsum_to(dst, expr, np::axes<0>);
            EXPECT_EQ(output, expected);
            np::cumsum_to(inplace, expr, np::axes<0>);
            EXPECT_EQ(input, expected);
        }
    }
    using shape = np::extents<size_t, 3, 5>;
    int integers[15];
    for (size_t i = 0; i != 15; ++i)
        integers[i] = i < 5 ? 16777217 : 1;
    float output[15];
    np::cumsum_to(view<float, shape>(output), np::as_expr(view<const int, shape>(integers)),
                  np::axes<0>);
    for (size_t col = 0; col != 5; ++col) {
        EXPECT_EQ(output[col], float(16777217));
        EXPECT_EQ(output[5 + col], float(16777218));
        EXPECT_EQ(output[10 + col], float(16777219));
    }
}

struct traced_operation {
    static inline std::array<int, 15> reads{};
    static inline size_t count = 0;
    static int apply(int value) noexcept {
        reads[count++] = value;
        return value;
    }
};

TEST(NpExpressionScan, UnknownNothrowOperationsRetainLineOrder) {
    using shape = np::extents<size_t, 3, 5>;
    int input[15], output[15];
    for (size_t i = 0; i != 15; ++i)
        input[i] = int(i);
    auto a = np::as_expr(view<const int, shape>(input));
    const np::unary_expression<traced_operation, decltype(a)> expr(a);
    traced_operation::count = 0;
    np::cumsum_to(view<int, shape>(output), expr, np::axes<0>);
    EXPECT_EQ(traced_operation::count, 15u);
    for (size_t col = 0; col != 5; ++col)
        for (size_t row = 0; row != 3; ++row)
            EXPECT_EQ(traced_operation::reads[col * 3 + row], input[row * 5 + col]);
    auto empty = np::as_expr(view<const int, np::extents<size_t, 0, 5>>(nullptr));
    const np::unary_expression<traced_operation, decltype(empty)> empty_expr(empty);
    traced_operation::count = 0;
    np::cumsum_to(view<int, np::extents<size_t, 0, 5>>(nullptr), empty_expr, np::axes<0>);
    EXPECT_EQ(traced_operation::count, 0u);
}

TEST(NpExpressionScan, PaddedSlicesTouchOnlyLogicalElements) {
    using dimensions = np::extents<size_t, 3, 7>;
    int input[21], output[21];
    for (size_t i = 0; i != 21; ++i) {
        input[i] = int(i);
        output[i] = -9;
    }
    view<int, dimensions> src(input), dst(output);
    const auto window = np::slice_view(src, np::full_extent, std::pair{1, 6});
    const auto out = np::slice_view(dst, np::full_extent, std::pair{1, 6});
    np::cumsum_to(out, np::as_expr(window) * 2 + 1, np::axes<0>);
    for (size_t col = 0; col != 5; ++col) {
        int expected = 0;
        for (size_t row = 0; row != 3; ++row) {
            expected += window(row, col) * 2 + 1;
            EXPECT_EQ(out(row, col), expected);
        }
    }
    for (size_t row = 0; row != 3; ++row) {
        EXPECT_EQ(dst(row, 0), -9);
        EXPECT_EQ(dst(row, 6), -9);
    }
}

TEST(NpExpressionScan, RankThreeMiddleAxisAndEmptySkipReads) {
    int data[]{1, 2, 3, 4, 5, 6}, factors[]{1, 2, 3, 4}, output[24]{};
    auto a = np::as_expr(view<const int, np::extents<size_t, 2, 1, 3>>(data));
    auto b = np::as_expr(view<const int, np::extents<size_t, 1, 4, 1>>(factors));
    view<int, np::extents<size_t, 2, 4, 3>, np::layout_left> dst(output);
    np::cumsum_to(dst, a * b, np::axes<1>);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col) {
            int expected = 0;
            for (size_t axis = 0; axis != 4; ++axis) {
                expected += data[row * 3 + col] * factors[axis];
                EXPECT_EQ(dst(row, axis, col), expected);
            }
        }
    using empty_shape = np::dextents<size_t, 2>;
    const auto empty = np::as_expr(view<const double, empty_shape>(nullptr, 0, 1));
    double bias[]{1, 2, 3};
    const auto mapped_empty = np::map([](double x) noexcept { return x * x; }, empty);
    const auto empty_expr =
        mapped_empty + np::as_expr(view<const double, np::extents<size_t, 1, 3>>(bias));
    np::cumsum_to(view<double, empty_shape>(nullptr, 0, 3), empty_expr, np::axes<0>);
    np::cumsum_to(view<double, empty_shape>(nullptr, 0, 3), empty_expr, np::axes<1>);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_operation {
    static int apply(int value) {
        if (value == 5)
            throw std::runtime_error("scan read failed");
        return value;
    }
};
using throwing_leaf =
    decltype(np::as_expr(std::declval<view<const int, np::extents<size_t, 3, 4>>>()));
using throwing_expression = np::unary_expression<throwing_operation, throwing_leaf>;
static_assert(!noexcept(np::cumsum_to(std::declval<view<int, np::extents<size_t, 3, 4>>>(),
                                      std::declval<const throwing_expression &>(), np::axes<0>)));

template <size_t Axis>
WJR_NOINLINE void scan_throwing(view<int, np::extents<size_t, 3, 4>> dst,
                                const throwing_expression &expr) {
    np::cumsum_to(dst, expr, np::axes<Axis>);
}

TEST(NpExpressionScan, ThrowingReadsCommitOnlyCompletedPrefixesInLineOrder) {
    using dimensions = np::extents<size_t, 3, 4>;
    int input[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    std::array<int, 12> output;
    const auto source = np::as_expr(view<const int, dimensions>(input));
    throwing_expression expr(source);
    output.fill(-1);
    EXPECT_THROW(scan_throwing<0>(view<int, dimensions>(output.data()), expr), std::runtime_error);
    EXPECT_EQ(output[0], 1);
    for (size_t i = 1; i != output.size(); ++i)
        EXPECT_EQ(output[i], -1);
    output.fill(-1);
    EXPECT_THROW(scan_throwing<1>(view<int, dimensions>(output.data()), expr), std::runtime_error);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}), (std::array{1, 3, 6, 10}));
    for (size_t i = 4; i != output.size(); ++i)
        EXPECT_EQ(output[i], -1);
}
#endif
} // namespace
