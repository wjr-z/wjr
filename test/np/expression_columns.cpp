#include <array>
#include <bit>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
template <class T, size_t Rank, class Layout = np::layout_right>
using view = np::tensor_view<T, np::dextents<size_t, Rank>, Layout>;

template <class Acc, class E, class Read>
void check_columns(const E &expression, size_t rows, size_t columns, Read read) {
    std::vector<double> output(columns * 2 + 1, -123.);
    using output_view = view<double, 1, np::layout_stride>;
    const typename output_view::mapping_type mapping(np::dextents<size_t, 1>(columns),
                                                     std::array<size_t, 1>{2});
    np::sum_to<Acc>(output_view(output.data(), mapping), expression, np::axes<0>);
    for (size_t column = 0; column != columns; ++column) {
        Acc expected{};
        for (size_t row = 0; row != rows; ++row)
            expected += static_cast<Acc>(read(row, column));
        EXPECT_EQ(output[2 * column], static_cast<double>(expected));
        EXPECT_EQ(output[2 * column + 1], -123.);
    }
    EXPECT_EQ(output.back(), -123.);
}

constexpr bool constant_columns() {
    int values[21]{};
    for (int i = 0; i != 21; ++i)
        values[i] = i + 1;
    int output[7]{};
    const auto source = np::as_expr(np::tensor_view<const int, np::extents<size_t, 3, 7>>(values));
    np::sum_to(np::tensor_view<int, np::extents<size_t, 1, 7>>(output), source * source + 1,
               np::axes<0>, np::keepdims);
    for (int column = 0; column != 7; ++column) {
        int expected = 0;
        for (int row = 0; row != 3; ++row) {
            const auto value = values[row * 7 + column];
            expected += value * value + 1;
        }
        if (output[column] != expected)
            return false;
    }
    return true;
}
static_assert(constant_columns());

TEST(NpExpressionColumns, OrderedFoldsAcrossBlocksTailsAndEmptyDomains) {
    EXPECT_TRUE(constant_columns());
    for (size_t rows : {size_t{0}, size_t{1}, size_t{8}, size_t{64}, size_t{2048}}) {
        for (size_t columns : {size_t{0}, size_t{1}, size_t{2}, size_t{3}, size_t{4}, size_t{7},
                               size_t{8}, size_t{15}, size_t{300}}) {
            std::vector<double> values(rows * columns);
            for (size_t i = 0; i != values.size(); ++i)
                values[i] = static_cast<double>(static_cast<int>(i % 31) - 15) / 8;
            const auto source = np::as_expr(view<const double, 2>(values.data(), rows, columns));
            check_columns<double>(source * source + 1.25, rows, columns,
                                  [&](size_t row, size_t column) {
                                      const auto value = values[row * columns + column];
                                      const auto square = value * value;
                                      return square + 1.25;
                                  });
        }
    }
}

struct scale_add {
    static inline int copies = 0, moves = 0;
    double coefficient;
    explicit scale_add(double value) : coefficient(value) {}
    scale_add(const scale_add &other) : coefficient(other.coefficient) { ++copies; }
    scale_add(scale_add &&other) noexcept : coefficient(other.coefficient) { ++moves; }
    double operator()(double value, double bias) const noexcept {
        const auto scaled = coefficient * value;
        return scaled + bias;
    }
};

TEST(NpExpressionColumns, PaddedMappingBroadcastAndCallableBinding) {
    constexpr size_t rows = 8, columns = 7, pitch = 11;
    std::array<double, rows * pitch> values{};
    std::array<double, columns> bias{};
    std::array<double, rows> row_bias{};
    for (size_t i = 0; i != values.size(); ++i)
        values[i] = static_cast<double>(i % 17) / 4;
    for (size_t i = 0; i != columns; ++i)
        bias[i] = static_cast<double>(i) / 2;
    for (size_t i = 0; i != rows; ++i)
        row_bias[i] = static_cast<double>(i) / 8;
    using input_view = view<const double, 2, np::layout_stride>;
    const input_view padded(values.data(),
                            input_view::mapping_type(np::dextents<size_t, 2>(rows, columns),
                                                     std::array<size_t, 2>{pitch, 1}));
    const auto expression =
        np::map(scale_add(1.25), padded, view<const double, 1>(bias.data(), columns));
    const auto copies = scale_add::copies, moves = scale_add::moves;
    check_columns<double>(expression, rows, columns, [&](size_t row, size_t column) {
        const auto scaled = 1.25 * values[row * pitch + column];
        return scaled + bias[column];
    });
    EXPECT_EQ(scale_add::copies, copies);
    EXPECT_EQ(scale_add::moves, moves);
    const auto splat = np::as_expr(view<const double, 2>(row_bias.data(), rows, 1)) +
                       np::as_expr(view<const double, 1>(bias.data(), columns));
    check_columns<double>(splat, rows, columns,
                          [&](size_t row, size_t column) { return row_bias[row] + bias[column]; });
}

TEST(NpExpressionColumns, AccumulatorAndFinalOutputTypeRemainIndependent) {
    constexpr size_t columns = 7;
    float values[3 * columns]{};
    for (size_t column = 0; column != columns; ++column) {
        values[column] = 1e8f;
        values[columns + column] = 1.f;
        values[2 * columns + column] = -1e8f;
    }
    const auto source = np::as_expr(view<const float, 2>(values, 3, columns));
    check_columns<float>(source, 3, columns,
                         [&](size_t row, size_t column) { return values[row * columns + column]; });
    check_columns<double>(source, 3, columns, [&](size_t row, size_t column) {
        return values[row * columns + column];
    });
    double fractional[2 * columns]{};
    for (auto &value : fractional)
        value = .75;
    int output[columns]{};
    np::sum_to(view<int, 1>(output, columns),
               np::as_expr(view<const double, 2>(fractional, 2, columns)), np::axes<0>);
    for (auto value : output)
        EXPECT_EQ(value, 1);
    int numerator[3 * columns]{}, denominator[columns]{};
    for (size_t i = 0; i != 3 * columns; ++i)
        numerator[i] = static_cast<int>(i + 1);
    for (auto &value : denominator)
        value = 3;
    const auto mixed = np::as_expr(view<const int, 2>(numerator, 3, columns)) /
                           np::as_expr(view<const int, 1>(denominator, columns)) +
                       .25f;
    check_columns<double>(mixed, 3, columns, [&](size_t row, size_t column) {
        return numerator[row * columns + column] / 3 + .25f;
    });
}

TEST(NpExpressionColumns, SpecialValuesKeepPerColumnOrder) {
    constexpr size_t columns = 7;
    double values[4 * columns]{};
    for (size_t column = 0; column != columns; ++column) {
        values[column] = 1e16;
        values[columns + column] = 1.;
        values[2 * columns + column] = -1e16;
        values[3 * columns + column] = -0.;
    }
    values[0] = std::numeric_limits<double>::quiet_NaN();
    values[1] = std::numeric_limits<double>::infinity();
    values[2] = -std::numeric_limits<double>::infinity();
    double output[columns]{};
    np::sum_to(view<double, 1>(output, columns),
               np::as_expr(view<const double, 2>(values, 4, columns)), np::axes<0>);
    EXPECT_TRUE(std::isnan(output[0]));
    EXPECT_EQ(output[1], std::numeric_limits<double>::infinity());
    EXPECT_EQ(output[2], -std::numeric_limits<double>::infinity());
    for (size_t column = 3; column != columns; ++column)
        EXPECT_EQ(std::bit_cast<uint64_t>(output[column]), std::bit_cast<uint64_t>(0.));
}

TEST(NpExpressionColumns, NonUnitInputAndKeepdimsOutputUseTheirMappings) {
    double values[48]{};
    for (size_t i = 0; i != 48; ++i)
        values[i] = static_cast<double>(i) / 4;
    using input_view = view<const double, 2, np::layout_stride>;
    const input_view separated(values, input_view::mapping_type(np::dextents<size_t, 2>(3, 7),
                                                                std::array<size_t, 2>{16, 2}));
    check_columns<double>(np::as_expr(separated) + .5, 3, 7, [&](size_t row, size_t column) {
        return values[row * 16 + column * 2] + .5;
    });
    double output[14]{};
    using output_view = view<double, 2, np::layout_stride>;
    const output_view kept(output, output_view::mapping_type(np::dextents<size_t, 2>(1, 7),
                                                             std::array<size_t, 2>{15, 2}));
    const auto packed = np::as_expr(view<const double, 2>(values, 3, 7));
    np::sum_to(kept, packed, np::axes<0>, np::keepdims);
    for (size_t column = 0; column != 7; ++column) {
        EXPECT_EQ(output[2 * column], values[column] + values[7 + column] + values[14 + column]);
        EXPECT_EQ(output[2 * column + 1], 0.);
    }
}

TEST(NpExpressionColumns, StandardMathRetainsLinewiseErrorStateOrder) {
    double values[14];
    for (auto &value : values)
        value = 1.;
    values[3] = -1.;
    values[7] = 0.;
    double expected[7]{}, actual[7]{};
    errno = 0;
    for (size_t column = 0; column != 7; ++column)
        for (size_t row = 0; row != 2; ++row)
            expected[column] += std::log(values[row * 7 + column]);
    const auto expected_errno = errno;
    const auto expression = np::log(view<const double, 2>(values, 2, 7));
    static_assert(!np::detail::interleavable_sum_expression<decltype(expression)>);
    errno = 0;
    np::sum_to(view<double, 1>(actual, 7), expression, np::axes<0>);
    const auto actual_errno = errno;
    EXPECT_EQ(actual_errno, expected_errno);
    for (size_t column = 0; column != 7; ++column) {
        if (std::isnan(expected[column]))
            EXPECT_TRUE(std::isnan(actual[column]));
        else
            EXPECT_EQ(actual[column], expected[column]);
    }
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
template <class E>
WJR_NOINLINE void reduce_throwing(int *output, const E &expression) {
    np::sum_to(view<int, 1>(output, 7), expression, np::axes<0>);
}

TEST(NpExpressionColumns, ThrowingMapStillCommitsOnlyCompletedLines) {
    int values[21]{};
    for (int i = 0; i != 21; ++i)
        values[i] = i + 1;
    values[8] = 99;
    const auto expression = np::map(
        [](int value) {
            if (value == 99)
                throw std::runtime_error("column operation failed");
            return value;
        },
        view<const int, 2>(values, 3, 7));
    static_assert(!noexcept(np::sum_to(view<int, 1>(nullptr, 7), expression, np::axes<0>)));
    int output[] = {-1, -1, -1, -1, -1, -1, -1};
    EXPECT_THROW(reduce_throwing(output, expression), std::runtime_error);
    EXPECT_EQ(output[0], 24);
    for (size_t column = 1; column != 7; ++column)
        EXPECT_EQ(output[column], -1);
}
#endif
} // namespace
