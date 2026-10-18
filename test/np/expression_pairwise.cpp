#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_pairwise.hpp>
#include <wjr/np/tensor.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using line = np::dextents<size_t, 1>;
using matrix = np::extents<size_t, 2, 3>;

template <class E, class Acc = void>
concept pairwise = requires(const E &expression) { np::pairwise_sum<Acc>(expression); };
template <class V>
using leaf = decltype(np::as_expr(std::declval<V>()));
using float_leaf = leaf<view<const float, line>>;
using int_leaf = leaf<view<const int, line>>;
using bool_leaf = leaf<view<const bool, line>>;
using broadcast = decltype(np::as_expr(std::declval<view<float, matrix>>()) +
                           np::as_expr(std::declval<view<float, np::extents<size_t, 1, 3>>>()));
using rank_broadcast = decltype(np::as_expr(std::declval<view<float, matrix>>()) +
                                np::as_expr(std::declval<view<float, np::extents<size_t, 3>>>()));
static_assert(pairwise<float_leaf> && pairwise<int_leaf, double> && pairwise<bool_leaf, double>);
static_assert(!pairwise<int_leaf> && !pairwise<bool_leaf>);
static_assert(!pairwise<float_leaf, int> && !pairwise<float_leaf, const double>);
static_assert(!pairwise<broadcast> && !pairwise<rank_broadcast>);
static_assert(!pairwise<leaf<view<float, matrix, np::layout_left>>>);
static_assert(!pairwise<leaf<view<float, line, np::layout_stride>>>);
static_assert(pairwise<leaf<view<float, line, np::layout_left>>>);
static_assert(!pairwise<np::tensor<float, matrix>>);
static_assert(std::same_as<decltype(np::pairwise_sum(std::declval<const float_leaf &>())), float>);
static_assert(
    std::same_as<decltype(np::pairwise_sum<double>(std::declval<const int_leaf &>())), double>);
static_assert(noexcept(np::pairwise_sum(std::declval<const float_leaf &>())));
using bound_values = decltype(np::detail::bind_expression_values(std::declval<const float_leaf &>(),
                                                                 std::make_index_sequence<1>{}));
using cursor = np::detail::pairwise_expression_cursor<float_leaf, bound_values>;
static_assert(sizeof(cursor) == sizeof(const bound_values *) + sizeof(size_t));
static_assert(std::is_trivially_copyable_v<cursor>);

struct throwing_function {
    float operator()(float value) const noexcept(false) { return value; }
};
struct unknown_operation {
    static float apply(float value) noexcept { return value; }
};
using throwing_expression = decltype(np::map(throwing_function{}, std::declval<float_leaf>()));
static_assert(!pairwise<throwing_expression>);
static_assert(!pairwise<np::unary_expression<unknown_operation, float_leaf>>);
static_assert(!pairwise<decltype(np::log(std::declval<float_leaf>()))>);
static_assert(!pairwise<decltype(np::cast<double>(std::declval<float_leaf>()))>);

constexpr bool constant_cases() {
    const float values[]{1, 2, 3, 4, 5, 6};
    const auto a = np::as_expr(view<const float, matrix>(values));
    if (np::pairwise_sum(a * 2.f - 1.f) != 36.f)
        return false;
    const int integers[]{3, 5, 7, -3};
    const auto b = np::as_expr(view<const int, np::extents<size_t, 4>>(integers));
    if (np::pairwise_sum<double>(b / 2) != 5.)
        return false;
    std::array<float, 257> ones{};
    ones.fill(1.f);
    const auto c = np::as_expr(view<const float, np::extents<size_t, 257>>(ones.data()));
    return np::pairwise_sum(c + 1.f) == 514.f && np::pairwise_sum(np::as_expr(-0.f)) == 0.f;
}
static_assert(constant_cases());
static_assert(std::bit_cast<uint32_t>(np::pairwise_sum(np::as_expr(-0.f))) == 0x80000000u);

// Independent schedule: fold each lane in isolation, then merge adjacent lanes.
template <class Acc, class T>
Acc grouping_oracle(const T *input, size_t count) {
    if (count == 0)
        return Acc{0};
    if (count > 128) {
        const auto split = (count / 16) * 8;
        const auto left = grouping_oracle<Acc>(input, split);
        const auto right = grouping_oracle<Acc>(input + split, count - split);
        return left + right;
    }
    if (count < 8) {
        Acc result = -Acc{0};
        for (size_t i = 0; i != count; ++i)
            result += static_cast<Acc>(input[i]);
        return result;
    }
    std::array<Acc, 8> lanes{};
    const auto bound = count - count % 8;
    for (size_t lane = 0; lane != 8; ++lane) {
        lanes[lane] = static_cast<Acc>(input[lane]);
        for (size_t i = lane + 8; i < bound; i += 8)
            lanes[lane] += static_cast<Acc>(input[i]);
    }
    for (size_t width = 1; width != 8; width *= 2)
        for (size_t first = 0; first != 8; first += 2 * width)
            lanes[first] += lanes[first + width];
    auto result = lanes[0];
    for (size_t i = bound; i != count; ++i)
        result += static_cast<Acc>(input[i]);
    return result;
}

template <class Acc, class T>
void check_grouping() {
    auto check = [](size_t count) {
        std::vector<T> a(count), b(count), materialized(count);
        for (size_t i = 0; i != count; ++i) {
            const auto magnitude = i % 17 == 0 ? T(1e10) : T(int(i % 11) - 5) / T(8);
            a[i] = i % 3 == 0 ? -magnitude : magnitude;
            b[i] = T(int(i % 7) - 3) / T(4);
            const T scaled = a[i] * T(.5);
            const T shifted = b[i] - T(.25);
            materialized[i] = scaled + shifted;
        }
        const auto x = np::as_expr(view<const T, line>(a.data(), count));
        const auto y = np::as_expr(view<const T, line>(b.data(), count));
        const auto actual = np::pairwise_sum<Acc>(x * T(.5) + (y - T(.25)));
        const auto expected = grouping_oracle<Acc>(materialized.data(), count);
        EXPECT_EQ(actual, expected);
        EXPECT_EQ(actual, np::pairwise_sum<Acc>(view<const T, line>(materialized.data(), count)));
    };
    for (size_t count = 0; count != 520; ++count)
        check(count);
    for (size_t count : {1024u, 4095u, 4096u, 4097u, 8193u})
        check(count);
}

TEST(NpExpressionPairwise, ExactGroupingAcrossLeafTailAndRecursiveBoundaries) {
    check_grouping<float, float>();
    check_grouping<double, float>();
    check_grouping<double, double>();
}

TEST(NpExpressionPairwise, DtypeIsAppliedBeforeAccumulatorConversion) {
    EXPECT_TRUE(constant_cases());
    const float value = 16777216.f;
    const auto a = np::as_expr(view<const float, np::extents<size_t>>(&value));
    EXPECT_EQ(np::pairwise_sum<double>(a + 1.f), 16777216.);
    const bool values[]{true, false, true};
    EXPECT_EQ(
        np::pairwise_sum<double>(np::as_expr(view<const bool, np::extents<size_t, 3>>(values))),
        2.);
}

TEST(NpExpressionPairwise, PackedDomainsRankZeroAndScalarOperands) {
    const double values[]{1, 2, 3, 4, 5, 6};
    const auto a = np::as_expr(view<const double, np::dextents<size_t, 2>>(values, 2, 3));
    const auto b = np::as_expr(view<const double, matrix>(values));
    EXPECT_EQ(np::pairwise_sum(np::maximum(a, b) + 1.), 27.);
    const auto c = np::as_expr(view<const double, line, np::layout_left>(values, 6));
    EXPECT_EQ(np::pairwise_sum(c * 2.), 42.);
    const auto scalar =
        np::as_expr(view<const double, np::extents<size_t>, np::layout_left>(values));
    EXPECT_EQ(np::pairwise_sum(scalar + 1.), 2.);
}

TEST(NpExpressionPairwise, EmptySignedZerosNanAndInfinity) {
    const auto empty = np::as_expr(view<const float, line>(nullptr, 0));
    const auto zero = np::pairwise_sum(empty * 2.f);
    EXPECT_EQ(zero, 0.f);
    EXPECT_FALSE(std::signbit(zero));
    const auto empty_matrix = np::as_expr(view<const float, np::extents<size_t, 2, 0>>(nullptr));
    EXPECT_EQ(np::pairwise_sum(empty_matrix + 1.f), 0.f);
    std::array<float, 257> zeros{};
    zeros.fill(-0.f);
    const auto a = np::as_expr(view<const float, line>(zeros.data(), zeros.size()));
    EXPECT_TRUE(std::signbit(np::pairwise_sum(a)));
    EXPECT_TRUE(std::signbit(np::pairwise_sum(np::as_expr(-0.f))));
    zeros[1] = 0.f;
    EXPECT_FALSE(std::signbit(np::pairwise_sum(a)));
    zeros[0] = std::numeric_limits<float>::infinity();
    EXPECT_EQ(np::pairwise_sum(a + 1.f), std::numeric_limits<float>::infinity());
    zeros[128] = -std::numeric_limits<float>::infinity();
    EXPECT_TRUE(std::isnan(np::pairwise_sum(a + 1.f)));
    zeros[128] = std::numeric_limits<float>::quiet_NaN();
    EXPECT_TRUE(std::isnan(np::pairwise_sum(a + 1.f)));
}

struct move_only_scale {
    float scale;
    int *moves;
    move_only_scale(float value, int *counter) noexcept : scale(value), moves(counter) {}
    move_only_scale(const move_only_scale &) = delete;
    move_only_scale(move_only_scale &&other) noexcept : scale(other.scale), moves(other.moves) {
        ++*moves;
    }
    float operator()(float value) const noexcept { return value * scale; }
};
struct never_called {
    float operator()(float) const noexcept { std::abort(); }
};

TEST(NpExpressionPairwise, PureCallableIsBorrowedAndEmptyDoesNotInvokeIt) {
    std::array<float, 257> values{};
    values.fill(1.f);
    int moves = 0;
    const auto expression = np::map(move_only_scale(2.f, &moves),
                                    view<const float, np::extents<size_t, 257>>(values.data()));
    moves = 0;
    EXPECT_EQ(np::pairwise_sum(expression), 514.f);
    EXPECT_EQ(np::pairwise_sum(expression), 514.f);
    EXPECT_EQ(moves, 0);
    const auto empty = np::map(never_called{}, view<const float, line>(nullptr, 0));
    EXPECT_EQ(np::pairwise_sum(empty), 0.f);
}
} // namespace
