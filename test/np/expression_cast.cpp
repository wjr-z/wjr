#include <array>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_cast.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using shape = np::extents<size_t, 3>;
using leaf = decltype(np::as_expr(std::declval<view<int, shape>>()));
using identity = decltype(np::cast<int>(std::declval<const leaf &>()));

template <class To, class E>
concept casts = requires(E &&value) { np::cast<To>(std::forward<E>(value)); };

static_assert(casts<double, leaf> && casts<double, const leaf &> && casts<bool, int>);
static_assert(casts<float, view<int, shape>>);
static_assert(!casts<const int, leaf> && !casts<volatile int, leaf> && !casts<int &, leaf>);
static_assert(!casts<void, leaf> && !casts<int, int *>);
static_assert(!casts<int, np::tensor<int, shape> &>);
static_assert(!casts<int, view<volatile int, shape>>);
static_assert(std::same_as<typename identity::value_type, int>);
static_assert(std::same_as<typename identity::extents_type, shape>);
static_assert(sizeof(identity) == sizeof(leaf) && alignof(identity) == alignof(leaf));
static_assert(std::is_trivially_copyable_v<identity>);

struct custom_value {
    explicit operator int() const { return 1; }
};
struct custom_operation {
    static custom_value apply(int) noexcept { return {}; }
};
using custom_expression = np::unary_expression<custom_operation, leaf>;
static_assert(!casts<int, custom_expression>);

constexpr bool constant_cast() {
    int numerator[] = {3, 7, 8}, denominator[] = {2, 2, 3};
    double before[3]{}, after[3]{};
    const auto a = np::as_expr(view<const int, shape>(numerator));
    const auto b = np::as_expr(view<const int, shape>(denominator));
    np::eval_to(view<double, shape>(before), np::cast<double>(a) / b);
    np::eval_to(view<double, shape>(after), np::cast<double>(a / b));
    int scalar = 0;
    np::eval_to(view<int, np::extents<size_t>>(&scalar), np::cast<int>(3.75));
    return before[0] == 1.5 && before[1] == 3.5 && before[2] == 8. / 3. && after[0] == 1. &&
           after[1] == 3. && after[2] == 2. && scalar == 3;
}
static_assert(constant_cast());

TEST(NpExpressionCast, ConstantEvaluationAndConversionPosition) {
    EXPECT_TRUE(constant_cast());
    int values[] = {1, 2, 3}, output[3]{};
    auto returned = [&] {
        auto local = np::as_expr(view<int, shape>(values)) + 0;
        return np::cast<int>(std::move(local));
    }();
    np::eval_to(view<int, shape>(output), returned);
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{1, 2, 3}));
}

TEST(NpExpressionCast, BroadcastStridesAndColumnMajorLayout) {
    using matrix_shape = np::extents<size_t, 2, 3>;
    using strided = view<const int, matrix_shape, np::layout_stride>;
    int input[] = {1, 999, 2, 999, 3, 999, 999, 4, 999, 5, 999, 6};
    strided source(input, strided::mapping_type({}, std::array<size_t, 2>{7, 2}));
    int offsets[] = {1, 2, 3};
    const auto converted = np::cast<double>(source);
    double output[6]{};
    np::eval_to(view<double, matrix_shape, np::layout_left>(output),
                converted / 2. + np::cast<double>(view<const int, shape>(offsets)));
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4], output[5]}),
              (std::array{1.5, 3., 3., 4.5, 4.5, 6.}));
    using column_leaf =
        decltype(np::cast<double>(view<int, matrix_shape, np::layout_left>(nullptr)));
    static_assert(std::same_as<typename column_leaf::layout_type, np::layout_left>);
}

TEST(NpExpressionCast, EmptyRankZeroAndBooleanOwnership) {
    using empty = np::extents<size_t, 2, 0>;
    np::eval_to(view<int, empty>(nullptr), np::cast<int>(view<const double, empty>(nullptr)) / 0);
    int scalar = -7;
    double converted = 0.;
    np::eval_to(view<double, np::extents<size_t>>(&converted),
                np::cast<double>(view<int, np::extents<size_t>>(&scalar)));
    EXPECT_EQ(converted, -7.);
    double values[] = {0., -0., std::numeric_limits<double>::quiet_NaN(), -2.};
    auto result = np::eval(np::cast<bool>(view<double, np::extents<size_t, 4>>(values)));
    ASSERT_TRUE(result);
    static_assert(
        std::same_as<std::remove_cvref_t<decltype(result->view().data_handle())>, bool *>);
    EXPECT_FALSE(result->view()(0));
    EXPECT_FALSE(result->view()(1));
    EXPECT_TRUE(result->view()(2));
    EXPECT_TRUE(result->view()(3));
    values[2] = 0.;
    EXPECT_TRUE(result->view()(2));
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_operation {
    static int apply(int) { throw std::runtime_error("child failed"); }
};
TEST(NpExpressionCast, ConstructionDoesNotReadAndChildExceptionPropagates) {
    const np::unary_expression<throwing_operation, np::scalar_expression<int>> child(
        np::as_expr(1));
    const auto expression = np::cast<double>(child);
    static_assert(noexcept(np::cast<double>(child)));
    static_assert(!noexcept(expression.read(0)));
    EXPECT_THROW((void)expression.read(0), std::runtime_error);
}
#endif
} // namespace
