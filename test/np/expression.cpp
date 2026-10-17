#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/expression.hpp>
#include <wjr/np/tensor.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 2, 3>;
using view = np::tensor_view<double, shape>;
using node = decltype(np::as_expr(std::declval<view>()));

template <class T>
concept can_capture = requires(T &&value) { np::as_expr(std::forward<T>(value)); };
template <class L, class R>
concept can_add = requires(L &&left, R &&right) { std::forward<L>(left) + std::forward<R>(right); };
template <class Dst, class E>
concept can_evaluate = requires(Dst dst, const E &expression) { np::eval_to(dst, expression); };

struct custom_accessor : np::default_accessor<double> {};
using owner = np::tensor<double, shape>;
static_assert(can_capture<view> && can_capture<const node &> && can_capture<int>);
static_assert(!can_capture<owner &> && !can_capture<owner>);
static_assert(can_capture<np::tensor_view<double, shape, np::layout_stride>>);
static_assert(!can_capture<np::tensor_view<volatile double, shape>>);
static_assert(!can_capture<np::tensor_view<double, shape, np::layout_right, custom_accessor>>);
static_assert(!can_add<view, view> && !can_add<node, owner &>);
using mismatched =
    decltype(np::as_expr(std::declval<np::tensor_view<double, np::extents<size_t, 2, 4>>>()));
using wrong_rank =
    decltype(np::as_expr(std::declval<np::tensor_view<double, np::extents<size_t, 6>>>()));
using left_node =
    decltype(np::as_expr(std::declval<np::tensor_view<double, shape, np::layout_left>>()));
static_assert(!can_add<node, mismatched> && !can_add<node, wrong_rank>);
static_assert(can_add<node, left_node>);
static_assert(!can_evaluate<np::tensor_view<const double, shape>, node>);
static_assert(can_evaluate<np::tensor_view<double, shape, np::layout_stride>, node>);
static_assert(can_evaluate<np::tensor_view<double, shape, np::layout_left>, node>);
static_assert(!can_evaluate<np::tensor_view<double, np::extents<size_t, 3, 2>>, node>);
static_assert(!can_evaluate<view, wrong_rank>);
static_assert(std::same_as<decltype(std::declval<const node &>().read(0)), const double &>);
static_assert(std::same_as<typename node::extents_type, shape>);
static_assert(std::same_as<typename node::layout_type, np::layout_right>);
static_assert(sizeof(node) == sizeof(view));
static_assert(alignof(node) == alignof(view));
static_assert(std::is_trivially_copyable_v<node>);

template <class View>
constexpr auto returned_square(View input) {
    auto local = np::as_expr(input);
    auto difference = local - 1;
    return difference * difference;
}

constexpr bool constant_expression() {
    int first[] = {1, 2, 3, 4, 5, 6};
    const int second[] = {6, 5, 4, 3, 2, 1};
    int output[6]{};
    const auto a = np::as_expr(np::tensor_view<int, shape>(first));
    const auto b = np::as_expr(np::tensor_view<const int, shape>(second));
    const auto expression = -(a - b) * +a / 2 + 1;
    np::eval_to(np::tensor_view<int, shape>(output), expression);
    for (size_t i = 0; i != 6; ++i) {
        if (output[i] != -(first[i] - second[i]) * first[i] / 2 + 1)
            return false;
    }
    np::eval_to(np::tensor_view<int, shape>(first), a + a);
    return first[0] == 2 && first[5] == 12;
}
static_assert(constant_expression());

constexpr bool constant_scalar_and_empty() {
    int value = 3;
    auto scalar = np::as_expr(np::tensor_view<int, np::extents<size_t>>(&value));
    np::eval_to(np::tensor_view<int, np::extents<size_t>>(&value), scalar * 2);
    if (value != 6)
        return false;
    using empty = np::extents<size_t, 2, 0>;
    auto empty_array = np::as_expr(np::tensor_view<const int, empty>(nullptr));
    np::eval_to(np::tensor_view<int, empty>(nullptr), empty_array / 0);
    np::eval_to(np::tensor_view<int, empty>(nullptr), np::as_expr(1) / 0);
    int output[6]{};
    np::eval_to(np::tensor_view<int, shape>(output), np::as_expr(2) + 3);
    return output[0] == 5 && output[5] == 5;
}
static_assert(constant_scalar_and_empty());
} // namespace

TEST(np_expression, arithmetic_adl_and_constant_evaluation) {
    EXPECT_TRUE(constant_expression());
    EXPECT_TRUE(constant_scalar_and_empty());
    static_assert(noexcept(np::eval_to(std::declval<view>(), std::declval<const node &>())));
}

TEST(np_expression, values_and_descriptors_are_snapshots) {
    double input[] = {1, 2, 3, 4, 5, 6};
    double replacement[6]{};
    double output[6]{};
    view descriptor(input);
    double coefficient = 2;
    auto expression = np::as_expr(descriptor) * coefficient;
    descriptor = view(replacement);
    coefficient = 9;
    input[0] = 7;
    np::eval_to(view(output), expression);
    EXPECT_EQ(output[0], 14);
    EXPECT_EQ(output[5], 12);
    auto square = returned_square(view(input));
    np::eval_to(view(output), square);
    EXPECT_EQ(output[0], 36);
    EXPECT_EQ(output[5], 25);
}

TEST(np_expression, per_node_integer_promotion_and_assignment_conversion) {
    const int numerator[] = {3, -3, 7, -7, 9, -9, 1, 5, -5};
    const int denominator[] = {2, 2, 3, 3, 4, 4, 2, 2, 2};
    const float offset[9]{};
    using line = np::extents<size_t, 9>;
    auto a = np::as_expr(np::tensor_view<const int, line>(numerator));
    auto b = np::as_expr(np::tensor_view<const int, line>(denominator));
    auto c = np::as_expr(np::tensor_view<const float, line>(offset));
    auto expression = a / b + c;
    static_assert(std::same_as<typename decltype(a / b)::value_type, int>);
    static_assert(std::same_as<typename decltype(expression)::value_type, float>);
    float output[9]{};
    np::eval_to(np::tensor_view<float, line>(output), expression);
    for (size_t i = 0; i != 9; ++i) {
        EXPECT_EQ(output[i], numerator[i] / denominator[i]);
    }
    std::uint8_t bytes[] = {255, 3};
    int promoted[2]{};
    using pair = np::extents<size_t, 2>;
    auto byte_expression = np::as_expr(np::tensor_view<std::uint8_t, pair>(bytes));
    auto half = (byte_expression + std::uint8_t{1}) / 2;
    static_assert(std::same_as<typename decltype(half)::value_type, int>);
    static_assert(std::same_as<typename decltype(-byte_expression)::value_type, int>);
    np::eval_to(np::tensor_view<int, pair>(promoted), half);
    EXPECT_EQ(promoted[0], 128);
    EXPECT_EQ(promoted[1], 2);
    int truncated[9]{};
    np::eval_to(np::tensor_view<int, line>(truncated), c + 2.75);
    EXPECT_EQ(truncated[0], 2);
}

TEST(np_expression, mixed_dynamic_extents_preserve_static_information) {
    using dynamic_shape = np::extents<int, np::dynamic_extent, 3>;
    const double first[] = {1, 2, 3, 4, 5, 6};
    const double second[] = {6, 5, 4, 3, 2, 1};
    double output[6]{};
    auto a = np::as_expr(np::tensor_view<const double, dynamic_shape>(first, 2));
    auto b = np::as_expr(np::tensor_view<const double, shape>(second));
    auto expression = a + b;
    static_assert(decltype(expression)::rank() == 2);
    static_assert(decltype(expression)::static_extent(0) == 2);
    static_assert(decltype(expression)::static_extent(1) == 3);
    np::eval_to(view(output), expression);
    for (double value : output) {
        EXPECT_EQ(value, 7);
    }
    using dynamic_line = np::dextents<size_t, 1>;
    auto left = np::as_expr(np::tensor_view<const double, dynamic_line, np::layout_left>(first, 6));
    auto right = np::as_expr(np::tensor_view<const double, dynamic_line>(second, 6));
    np::eval_to(np::tensor_view<double, dynamic_line>(output, 6), left + right);
    EXPECT_EQ(output[5], 7);
}

TEST(np_expression, left_layout_and_pointwise_in_place) {
    double first[] = {1, 2, 3, 4, 5, 6};
    const double second[] = {6, 5, 4, 3, 2, 1};
    using left_view = np::tensor_view<double, shape, np::layout_left>;
    auto a = np::as_expr(left_view(first));
    auto b = np::as_expr(np::tensor_view<const double, shape, np::layout_left>(second));
    np::eval_to(left_view(first), a + a + b);
    EXPECT_EQ(first[0], 8);
    EXPECT_EQ(first[5], 13);
    EXPECT_EQ(left_view(first)(1, 2), 13);
}

TEST(np_expression, owner_lvalue_real_bool_and_scalar_domain) {
    owner storage;
    np::eval_to(storage.view(), np::as_expr(3.0));
    auto source = np::as_expr(std::as_const(storage).view());
    np::eval_to(storage.view(), source + 2);
    EXPECT_EQ(storage.view()(1, 2), 5);
    using pair = np::extents<size_t, 2>;
    bool flags[] = {false, true};
    int output[2]{};
    auto values = np::as_expr(np::tensor_view<bool, pair>(flags));
    np::eval_to(np::tensor_view<int, pair>(output), values + 1);
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], 2);
}

TEST(np_expression, floating_edge_cases_match_scalar_operations) {
    using line = np::extents<size_t, 7>;
    const double values[] = {std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::infinity(),
                             -std::numeric_limits<double>::infinity(),
                             0.0,
                             -0.0,
                             0.25,
                             -0.5};
    double output[7]{};
    auto expression = -np::as_expr(np::tensor_view<const double, line>(values));
    np::eval_to(np::tensor_view<double, line>(output), expression);
    for (size_t i = 0; i != 7; ++i) {
        const double expected = -values[i];
        if (std::isnan(expected)) {
            EXPECT_TRUE(std::isnan(output[i]));
        } else {
            EXPECT_EQ(output[i], expected);
            if (expected == 0)
                EXPECT_EQ(std::signbit(output[i]), std::signbit(expected));
        }
    }
}

TEST(np_expression, dynamic_buffers_end_at_the_logical_boundary) {
    using line = np::dextents<size_t, 1>;
    for (size_t count = 0; count != 130; ++count) {
        auto source = std::make_unique<double[]>(count);
        auto output = std::make_unique<double[]>(count);
        const double *input = count ? source.get() : nullptr;
        double *result = count ? output.get() : nullptr;
        auto expression = np::as_expr(np::tensor_view<const double, line>(input, count)) + 1;
        np::eval_to(np::tensor_view<double, line>(result, count), expression);
        for (size_t i = 0; i != count; ++i) {
            EXPECT_EQ(result[i], 1);
        }
    }
}
