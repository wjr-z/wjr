#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_mask.hpp>
#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using shape = np::extents<size_t, 4>;
using leaf = decltype(np::as_expr(std::declval<view<double, shape>>()));
using mask = decltype(std::declval<leaf>() < 0.);

template <class M, class Y, class N>
concept selects = requires(M &&m, Y &&y, N &&n) {
    np::select(std::forward<M>(m), std::forward<Y>(y), std::forward<N>(n));
};
template <class E>
concept negates = requires(E &&e) { np::logical_not(std::forward<E>(e)); };
template <class L, class R>
concept conjunction = requires(L &&left, R &&right) {
    np::logical_and(std::forward<L>(left), std::forward<R>(right));
};
template <class L, class R>
concept scalar_and = requires(L left, R right) { left &&right; };

static_assert(selects<mask, leaf, int> && selects<bool, bool, bool>);
static_assert(!selects<leaf, int, int> && !selects<int, double, double>);
static_assert(!selects<bool, view<double, np::extents<size_t, 3>>, leaf>);
static_assert(!selects<bool, leaf, np::tensor<double, shape> &>);
static_assert(negates<mask> && negates<bool> && !negates<int> && !negates<leaf>);
static_assert(conjunction<mask, bool> && !conjunction<mask, double>);
static_assert(!std::convertible_to<mask, bool> && !scalar_and<mask, mask>);
static_assert(std::same_as<typename mask::value_type, bool>);
using selected = decltype(np::select(std::declval<mask>(), 1.f, std::declval<leaf>()));
static_assert(std::same_as<typename selected::value_type, double>);
static_assert(std::same_as<typename selected::extents_type, shape>);
static_assert(np::detail::expression_function_count<selected> == 0);
static_assert(np::detail::expression_children<selected>::arity == 3);

constexpr bool constant_masks() {
    int input[] = {-2, 0, 2, 5}, output[4]{};
    const auto a = np::as_expr(view<const int, shape>(input));
    const auto valid = np::logical_and(a >= 0, a <= 2);
    np::eval_to(view<int, shape>(output), np::select(valid, a, -1));
    if (output[0] != -1 || output[1] != 0 || output[2] != 2 || output[3] != -1)
        return false;
    bool flags[4]{};
    np::eval_to(view<bool, shape>(flags), np::logical_or(a == 0, 2 == a));
    if (flags[0] || !flags[1] || !flags[2] || flags[3])
        return false;
    np::eval_to(view<bool, shape>(flags), np::logical_not(a != 2));
    if (flags[0] || flags[1] || !flags[2] || flags[3])
        return false;
    np::eval_to(view<int, shape>(output), np::select(a < 0, 0, np::select(a > 2, 2, a)));
    return output[0] == 0 && output[1] == 0 && output[2] == 2 && output[3] == 2 &&
           np::select(true, 1, 2).read(0) == 1 && np::logical_and(true, false).read(0) == false;
}
static_assert(constant_masks());

TEST(NpExpressionMask, ConstantComparisonsAndEagerLogicalNames) {
    EXPECT_TRUE(constant_masks());
    int signed_input[] = {-1};
    auto a = np::as_expr(view<int, np::extents<size_t, 1>>(signed_input));
    EXPECT_FALSE((a < 0u).read(0));
    EXPECT_FALSE((0u > a).read(0));
    EXPECT_TRUE((a >= 0u).read(0));
    auto promoted = np::select(true, -1, 1u);
    static_assert(std::same_as<typename decltype(promoted)::value_type, unsigned int>);
    EXPECT_EQ(promoted.read(0), std::numeric_limits<unsigned int>::max());
    static_assert(
        std::same_as<typename decltype(np::select(false, true, false))::value_type, bool>);
}

TEST(NpExpressionMask, IeeeComparisonsSignedZerosAndNanClip) {
    double input[] = {std::numeric_limits<double>::quiet_NaN(), -0., 0., 2.};
    const auto a = np::as_expr(view<const double, shape>(input));
    auto less = np::eval(a < 0.), equal = np::eval(a == 0.), unequal = np::eval(a != 0.);
    ASSERT_TRUE(less);
    ASSERT_TRUE(equal);
    ASSERT_TRUE(unequal);
    static_assert(std::same_as<std::remove_cvref_t<decltype(equal->view().data_handle())>, bool *>);
    EXPECT_FALSE(less->view()(0));
    EXPECT_FALSE(equal->view()(0));
    EXPECT_TRUE(unequal->view()(0));
    auto nan = np::as_expr(input[0]);
    EXPECT_FALSE((nan <= 0.).read(0));
    EXPECT_FALSE((nan > 0.).read(0));
    EXPECT_FALSE((nan >= 0.).read(0));
    EXPECT_TRUE(equal->view()(1));
    EXPECT_TRUE(equal->view()(2));
    double output[4]{};
    np::eval_to(view<double, shape>(output), np::select(a < 0., 0., np::select(a > 1., 1., a)));
    EXPECT_TRUE(std::isnan(output[0]));
    EXPECT_TRUE(std::signbit(output[1]));
    EXPECT_FALSE(std::signbit(output[2]));
    EXPECT_EQ(output[3], 1.);
    input[1] = 5.;
    EXPECT_TRUE(equal->view()(1));
}

TEST(NpExpressionMask, ThreeChildBroadcastWithStridesAndInPlaceOutput) {
    using matrix_shape = np::extents<size_t, 2, 3>;
    using strided = view<double, matrix_shape, np::layout_stride>;
    double input[] = {-1, 999, 2, 999, 3, 999, 999, -4, 999, 5, 999, 6};
    strided matrix(input, strided::mapping_type({}, std::array<size_t, 2>{7, 2}));
    bool row_mask[] = {true, false};
    double column_bias[] = {10, 20, 30};
    auto e = np::select(view<const bool, np::extents<size_t, 2, 1>>(row_mask), matrix,
                        view<const double, np::extents<size_t, 3>>(column_bias));
    double column_major[6]{};
    np::eval_to(view<double, matrix_shape, np::layout_left>(column_major), e);
    EXPECT_EQ((std::array{column_major[0], column_major[1], column_major[2], column_major[3],
                          column_major[4], column_major[5]}),
              (std::array{-1., 10., 2., 20., 3., 30.}));
    EXPECT_EQ(np::sum(e), 64.);
    np::eval_to(matrix, np::select(np::as_expr(matrix) < 0., -np::as_expr(matrix), matrix));
    EXPECT_EQ(input[0], 1.);
    EXPECT_EQ(input[7], 4.);
    EXPECT_EQ(input[1], 999.);
}

template <int Index>
struct counted_operation {
    static inline int calls = 0;
    static int apply(int value) noexcept {
        ++calls;
        return value;
    }
};
struct counted_boolean {
    static inline int calls = 0;
    static bool apply(bool value) noexcept {
        ++calls;
        return value;
    }
};

TEST(NpExpressionMask, BothBranchesAndLogicalOperandsExecuteWithoutShortCircuit) {
    int input[] = {1, 2, 3, 4}, output[4]{};
    auto a = np::as_expr(view<const int, shape>(input));
    const np::unary_expression<counted_operation<0>, decltype(a)> yes(a);
    const np::unary_expression<counted_operation<1>, decltype(a)> no(a);
    counted_operation<0>::calls = counted_operation<1>::calls = 0;
    np::eval_to(view<int, shape>(output), np::select(false, yes, no));
    EXPECT_EQ(counted_operation<0>::calls, 4);
    EXPECT_EQ(counted_operation<1>::calls, 4);
    bool values[] = {false, true, false, true}, flags[4]{};
    auto b = np::as_expr(view<const bool, shape>(values));
    const np::unary_expression<counted_boolean, decltype(b)> counted(b);
    counted_boolean::calls = 0;
    np::eval_to(view<bool, shape>(flags), np::logical_and(false, counted));
    np::eval_to(view<bool, shape>(flags), np::logical_or(true, counted));
    EXPECT_EQ(counted_boolean::calls, 8);
    for (bool flag : flags)
        EXPECT_TRUE(flag);
}

TEST(NpExpressionMask, EmptyDomainRankZeroAndReturnedValueClosure) {
    using empty = np::extents<size_t, 2, 0>;
    auto a = np::as_expr(view<const int, empty>(nullptr));
    np::eval_to(view<int, empty>(nullptr), np::select(false, a / 0, a / 0));
    np::eval_to(view<bool, empty>(nullptr), a < 0);
    bool scalar_mask = false;
    auto scalar = np::select(view<bool, np::extents<size_t>>(&scalar_mask), 3, 7.5);
    EXPECT_EQ(scalar.read(0), 7.5);
    double values[] = {-2., 0., 2., 5.}, output[4]{};
    const auto returned = [&] {
        auto local = np::as_expr(view<double, shape>(values));
        return np::select(local < 0., 0., np::select(local > 1., 1., local));
    }();
    const auto copied = returned;
    np::eval_to(view<double, shape>(output), copied);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}),
              (std::array{0., 0., 1., 1.}));
    auto movable = np::map(
        [factor = std::make_unique<double>(2.)](double value) noexcept { return value * *factor; },
        np::as_expr(view<double, shape>(values)));
    static_assert(!selects<bool, decltype(movable) &, int>);
    auto moved = np::select(true, std::move(movable), 0);
    static_assert(np::detail::expression_function_count<decltype(moved)> == 1);
    np::eval_to(view<double, shape>(output), moved);
    EXPECT_EQ(output[0], -4.);
    EXPECT_EQ(output[3], 10.);
}

TEST(NpExpressionMask, MapStatesBindIndependentlyAcrossAllThreeChildren) {
    double input[] = {-2., 0., 2., 5.}, output[4]{};
    const auto a = np::as_expr(view<const double, shape>(input));
    const auto predicate =
        np::map([limit = 1.](double value) noexcept { return value > limit; }, a);
    const auto yes = np::map([scale = .5](double value) noexcept { return value * scale; }, a);
    const auto no = np::map([bias = 10.](double value) noexcept { return value + bias; }, a);
    const auto e = np::select(predicate, yes, no);
    using expression_type = std::remove_cv_t<decltype(e)>;
    static_assert(np::detail::expression_function_count<expression_type> == 3);
    np::eval_to(view<double, shape>(output), e);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}),
              (std::array{8., 10., 1., 2.5}));
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_operation {
    static int apply(int) { throw std::runtime_error("unselected branch is still evaluated"); }
};
struct throwing_copy {
    throwing_copy() = default;
    throwing_copy(const throwing_copy &) { throw std::runtime_error("copy failed"); }
    throwing_copy(throwing_copy &&) noexcept = default;
    bool operator()(int) const noexcept { return true; }
};
TEST(NpExpressionMask, BranchAndClosureExceptionsRemainObservable) {
    auto scalar = np::as_expr(1);
    const np::unary_expression<throwing_operation, decltype(scalar)> throwing(scalar);
    const auto e = np::select(false, throwing, 0);
    static_assert(!noexcept(e.read(0)));
    int output = -1;
    EXPECT_THROW(np::eval_to(view<int, np::extents<size_t>>(&output), e), std::runtime_error);
    EXPECT_EQ(output, -1);
    const auto copied = np::map(throwing_copy{}, scalar);
    static_assert(!noexcept(np::select(copied, 1, 0)));
    static_assert(!noexcept(copied == true));
    static_assert(!noexcept(np::logical_not(copied)));
    EXPECT_THROW((void)np::select(copied, 1, 0), std::runtime_error);
    EXPECT_THROW((void)(copied == true), std::runtime_error);
    EXPECT_THROW((void)np::logical_not(copied), std::runtime_error);
    EXPECT_THROW((void)np::logical_and(copied, true), std::runtime_error);
}
#endif
} // namespace
