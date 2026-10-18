#include <array>
#include <cmath>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/expression_cast.hpp>
#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_xsimd.hpp>
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 1>;
template <class T>
using view = np::tensor_view<T, shape>;

template <class T>
void expect_same(T actual, T expected) {
    if (std::isnan(expected)) {
        EXPECT_TRUE(std::isnan(actual));
    } else {
        EXPECT_EQ(actual, expected);
        if (expected == T(0))
            EXPECT_EQ(std::signbit(actual), std::signbit(expected));
    }
}

template <class T, class E>
void compare_expression(const E &expr, size_t count) {
    std::vector<T> actual(count + 1), expected(count + 1);
    np::eval_to(view<T>(expected.data() + 1, count), expr);
    np::eval_to(view<T>(actual.data() + 1, count), expr, np::xsimd_policy<>{});
    for (size_t i = 0; i != count; ++i)
        expect_same(actual[i + 1], expected[i + 1]);
}

template <class T>
void compare_lengths() {
    constexpr auto lanes = xsimd::batch<T>::size;
    for (size_t count = 0; count != 3 * lanes + 3; ++count) {
        std::vector<T> input(count + 1);
        for (size_t i = 0; i != count; ++i)
            input[i + 1] = static_cast<T>(int(i % 7) - 3);
        if (count > 6) {
            input[1] = -T(0);
            input[2] = T(0);
            input[3] = std::numeric_limits<T>::quiet_NaN();
            input[4] = std::numeric_limits<T>::infinity();
            input[5] = -std::numeric_limits<T>::infinity();
        }
        const auto a = np::as_expr(view<const T>(input.data() + 1, count));
        const auto clip = np::select(a < T(-1), T(-1), np::select(a > T(2), T(2), a));
        static_assert(
            np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(clip)>, T>);
        const auto named_minimum = np::minimum(a, T(0));
        const auto named_maximum = np::maximum(a, T(0));
        static_assert(
            np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(named_minimum)>, T>);
        static_assert(
            np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(named_maximum)>, T>);
        const auto named_clip = np::clip(a, T(-1), T(2));
        static_assert(
            np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(named_clip)>, T>);
        compare_expression<T>(named_minimum, count);
        compare_expression<T>(named_maximum, count);
        compare_expression<T>(named_clip, count);
        // All six comparisons produce a numeric batch's matching predicate type.
        compare_expression<T>(clip, count);
        compare_expression<T>(np::select(a <= T(0), T(1), T(0)), count);
        compare_expression<T>(np::select(a >= T(0), T(1), T(0)), count);
        compare_expression<T>(np::select(a == T(0), T(1), T(0)), count);
        compare_expression<T>(np::select(a != T(0), T(1), T(0)), count);
        compare_expression<T>(
            np::select(np::logical_and(true, np::logical_not(np::logical_or(a<T(0), a> T(1)))), a,
                       -a) +
                T(1),
            count);
        // An inner select can choose predicates without materializing bools.
        compare_expression<T>(np::select(np::select(a < T(0), a != T(0), false), -a, a), count);
        const auto previous = input;
        np::eval_to(view<T>(input.data() + 1, count), clip, np::xsimd_policy<>{});
        for (size_t i = 0; i != count; ++i) {
            const T value = previous[i + 1];
            expect_same(input[i + 1], value < T(-1) ? T(-1) : (value > T(2) ? T(2) : value));
        }
    }
    const auto empty = np::as_expr(view<const T>(nullptr, 0));
    np::eval_to(view<T>(nullptr, 0), np::select(empty < T(0), T(0), empty), np::xsimd_policy<>{});
}

TEST(NpPredicateXsimd, ComparisonsLogicSelectTailsAndInPlace) {
    compare_lengths<float>();
    compare_lengths<double>();
}

template <class T>
void compare_rows() {
    constexpr size_t columns = 2 * xsimd::batch<T>::size + 1;
    using matrix = np::extents<size_t, np::dynamic_extent, columns>;
    using single_column = np::extents<size_t, np::dynamic_extent, 1>;
    using row = np::extents<size_t, columns>;
    std::array<T, 3 * columns> input{}, actual{}, expected{};
    std::array<T, columns> limits{};
    T bias[]{T(-1), T(0), T(1)}, scale = T(2);
    for (size_t i = 0; i != input.size(); ++i)
        input[i] = T(int(i % 9) - 4);
    for (size_t i = 0; i != columns; ++i)
        limits[i] = T(i % 3);
    const auto a = np::as_expr(np::tensor_view<const T, matrix>(input.data(), 3));
    const auto b = np::as_expr(np::tensor_view<const T, single_column>(bias, 3));
    const auto limit = np::as_expr(np::tensor_view<const T, row>(limits.data()));
    const auto factor = np::as_expr(np::tensor_view<const T, np::extents<size_t>>(&scale));
    const auto mask = np::logical_or(np::logical_and(a < limit, true), np::logical_not(a >= b));
    const auto expr = np::select(mask, (a + b) * factor, -a);
    static_assert(np::detail::expression_row_load<std::remove_cv_t<decltype(expr)>> ==
                  np::detail::row_load_kind::unit);
    np::eval_to(np::tensor_view<T, matrix>(expected.data(), 3), expr);
    np::eval_to(np::tensor_view<T, matrix>(actual.data(), 3), expr, np::xsimd_policy<>{});
    EXPECT_EQ(actual, expected);
    np::eval_to(np::tensor_view<T, matrix>(input.data(), 3), expr, np::xsimd_policy<>{});
    EXPECT_EQ(input, expected);
}

TEST(NpPredicateXsimd, ProjectedRowsBindUnitLoadsAndPredicateSplats) {
    compare_rows<float>();
    compare_rows<double>();
}

TEST(NpPredicateXsimd, BoolScalarValuesUseMatchingPredicateLanes) {
    std::array<float, 19> input{}, output{};
    for (size_t i = 0; i != input.size(); ++i)
        input[i] = float(i);
    const auto a = np::as_expr(view<const float>(input.data(), input.size()));
    for (bool enabled : {false, true}) {
        np::eval_to(view<float>(output.data(), output.size()), np::select(enabled, a, -a),
                    np::xsimd_policy<>{});
        for (size_t i = 0; i != output.size(); ++i)
            expect_same(output[i], enabled ? input[i] : -input[i]);
        np::eval_to(view<float>(output.data(), output.size()), np::select(enabled, 2.f, -2.f),
                    np::xsimd_policy<>{});
        for (auto value : output)
            EXPECT_EQ(value, enabled ? 2.f : -2.f);
    }
}

template <class T>
void compare_negation() {
    constexpr auto lanes = xsimd::batch<T>::size;
    // Every lane sees both zero signs; the last three values use the scalar tail.
    for (bool reverse : {false, true}) {
        std::array<T, 2 * lanes + 3> input{};
        for (size_t i = 0; i != input.size(); ++i)
            input[i] = ((i % 2 != 0) != reverse) ? -T{0} : T{0};
        input[lanes] = std::numeric_limits<T>::infinity();
        input[lanes + 1] = -std::numeric_limits<T>::infinity();
        const auto values = np::as_expr(view<const T>(input.data(), input.size()));
        compare_expression<T>(-values, input.size());
        compare_expression<T>(np::select(false, values, -values), input.size());
    }
}

TEST(NpPredicateXsimd, UnaryMinusPreservesSignedZerosInBatchesAndTails) {
    compare_negation<float>();
    compare_negation<double>();
}

struct scalar_only_operation {
    static float apply(float value) noexcept { return value * 2.f; }
};

TEST(NpPredicateXsimd, UnsupportedNodesFallBackBeforeBatchInstantiation) {
    std::array<float, 19> input{}, output{};
    std::array<bool, 19> flags{};
    std::array<int, 19> numerator{}, denominator{};
    for (size_t i = 0; i != input.size(); ++i) {
        input[i] = float(int(i) - 8);
        flags[i] = i % 2 != 0;
        numerator[i] = 3;
        denominator[i] = 2;
    }
    const auto a = np::as_expr(view<const float>(input.data(), input.size()));
    const auto mask = a < 0.f;
    const auto mixed = np::select(mask, a, 0.);
    const auto cast = np::select(mask, np::cast<float>(a), -a);
    const auto mapped = np::select(mask, np::map([](float x) noexcept { return x / 2.f; }, a), a);
    const auto math = np::select(mask, np::sqrt(a * a), -a);
    const auto opaque =
        np::select(mask, np::unary_expression<scalar_only_operation, decltype(a)>(a), a);
    const auto stored = np::select(view<const bool>(flags.data(), flags.size()), a, -a);
    using kind = np::detail::xsimd_result_kind;
    static_assert(np::detail::xsimd_expression_result<std::remove_cv_t<decltype(mask)>, float> ==
                  kind::predicate);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(mixed)>, double>);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(cast)>, float>);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(mapped)>, float>);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(math)>, float>);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(opaque)>, float>);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(stored)>, float>);
    compare_expression<float>(mixed, input.size());
    compare_expression<float>(cast, input.size());
    compare_expression<float>(mapped, input.size());
    compare_expression<float>(math, input.size());
    compare_expression<float>(opaque, input.size());
    compare_expression<float>(stored, input.size());
    const auto divided = np::as_expr(view<const int>(numerator.data(), 19)) /
                         np::as_expr(view<const int>(denominator.data(), 19));
    np::eval_to(view<float>(output.data(), 19), np::select(mask, divided, a), np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i)
        EXPECT_EQ(output[i], input[i] < 0.f ? 1.f : input[i]);
    // Bool output never forms xsimd::batch<bool>, even for a numeric comparison.
    np::eval_to(view<bool>(flags.data(), 19), np::logical_not(mask), np::xsimd_policy<>{});
    for (size_t i = 0; i != flags.size(); ++i)
        EXPECT_EQ(flags[i], input[i] >= 0.f);
}

TEST(NpPredicateXsimd, StridedMixedLayoutAndDynamicBroadcastStayCorrect) {
    using matrix = np::dextents<size_t, 2>;
    float input[]{-3, -2, -1, 0, 1, 2}, output[12]{};
    float threshold[]{-1, 1};
    const auto a = np::as_expr(np::tensor_view<const float, matrix>(input, 2, 3));
    const auto b = np::as_expr(np::tensor_view<const float, matrix>(threshold, 2, 1));
    auto expr = np::select(a < b, b, a);
    static_assert(np::detail::expression_row_load<decltype(expr)> ==
                  np::detail::row_load_kind::unsupported);
    np::layout_stride::mapping<matrix> mapping(matrix(2, 3), std::array<size_t, 2>{6, 2});
    np::tensor_view<float, matrix, np::layout_stride> dst(output, mapping);
    np::eval_to(dst, expr, np::xsimd_policy<>{});
    for (size_t r = 0; r != 2; ++r)
        for (size_t c = 0; c != 3; ++c)
            EXPECT_EQ(dst(r, c), input[r * 3 + c] < threshold[r] ? threshold[r] : input[r * 3 + c]);
    np::tensor_view<float, matrix, np::layout_left> left(output, 2, 3);
    np::eval_to(left, expr, np::xsimd_policy<>{});
    for (size_t r = 0; r != 2; ++r)
        for (size_t c = 0; c != 3; ++c)
            EXPECT_EQ(left(r, c),
                      input[r * 3 + c] < threshold[r] ? threshold[r] : input[r * 3 + c]);
}

TEST(NpPredicateXsimd, RankZeroAndEmptyProjectedDomains) {
    float input = -0.f, output = 1.f;
    const auto scalar = np::as_expr(np::tensor_view<const float, np::extents<size_t>>(&input));
    np::eval_to(np::tensor_view<float, np::extents<size_t>>(&output),
                np::select(np::logical_and(true, scalar == 0.f), scalar, -scalar),
                np::xsimd_policy<>{});
    expect_same(output, -0.f);
    using empty = np::extents<size_t, 0, 7>;
    const auto a = np::as_expr(np::tensor_view<const float, empty>(nullptr));
    np::eval_to(np::tensor_view<float, empty>(nullptr), np::select(a < 0.f, 0.f, a),
                np::xsimd_policy<>{});
}
TEST(NpPredicateXsimd, PaddedLeavesAndOutputKeepScalarProjection) {
    using matrix = np::extents<size_t, 3, 7>;
    float input[21]{};
    np::tensor_view<float, matrix> source(input);
    for (size_t r = 0; r != 3; ++r)
        for (size_t c = 0; c != 7; ++c)
            source(r, c) = float(int(c) - 3);
    auto window = np::slice_view(source, np::full_extent, std::pair{1, 6});
    const auto a = np::as_expr(window);
    const auto expr = np::select(a < 0.f, 0.f, a);
    static_assert(
        !np::detail::homogeneous_xsimd_expression<std::remove_cv_t<decltype(expr)>, float>);
    std::array<float, 15> output{};
    np::tensor_view<float, np::extents<size_t, 3, 5>> packed(output.data());
    np::eval_to(packed, expr, np::xsimd_policy<>{});
    np::eval_to(window, expr, np::xsimd_policy<>{});
    for (size_t r = 0; r != 3; ++r) {
        EXPECT_EQ(source(r, 0), -3.f);
        EXPECT_EQ(source(r, 6), 3.f);
        for (size_t c = 0; c != 5; ++c) {
            EXPECT_EQ(window(r, c), float(c < 2 ? 0 : c - 2));
            EXPECT_EQ(packed(r, c), window(r, c));
        }
    }
}
} // namespace
