#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>

#include <gtest/gtest.h>

#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_xsimd.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class T>
void expect_same(T actual, T expected) {
    if (std::isnan(expected))
        EXPECT_TRUE(std::isnan(actual));
    else {
        using bits = std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>;
        EXPECT_EQ(std::bit_cast<bits>(actual), std::bit_cast<bits>(expected));
    }
}

template <bool Minimum, class T>
T oracle(T left, T right) {
    if (std::isnan(left))
        return left;
    if (std::isnan(right))
        return right;
    if (left == right)
        return left;
    if constexpr (Minimum)
        return left < right ? left : right;
    else
        return left > right ? left : right;
}

template <class T>
void cartesian_values() {
    constexpr auto infinity = std::numeric_limits<T>::infinity();
    constexpr auto nan = std::numeric_limits<T>::quiet_NaN();
    constexpr auto tiny = std::numeric_limits<T>::denorm_min();
    const std::array<T, 10> values{-infinity, -1, -tiny, -T(0), T(0), tiny, 1, infinity, nan, -nan};
    std::array<T, 100> left{}, right{};
    std::array<T, 102> output{};
    size_t index = 0;
    for (T a : values)
        for (T b : values) {
            left[index] = a;
            right[index++] = b;
        }
    auto check = [&](size_t count) {
        const auto a = view<const T>(left.data(), count);
        const auto b = view<const T>(right.data(), count);
        auto minimum = np::minimum(a, b);
        auto maximum = np::maximum(a, b);
        static_assert(np::detail::homogeneous_xsimd_expression<decltype(minimum), T>);
        static_assert(np::detail::homogeneous_xsimd_expression<decltype(maximum), T>);
        auto run = [&](const auto &expression, auto reference) {
            output.fill(T(123));
            np::eval_to(view<T>(output.data() + 1, count), expression, np::xsimd_policy<>{});
            EXPECT_EQ(output.front(), T(123));
            EXPECT_EQ(output[count + 1], T(123));
            for (size_t i = 0; i != count; ++i)
                expect_same(output[i + 1], reference(left[i], right[i]));
        };
        run(minimum, oracle<true, T>);
        run(maximum, oracle<false, T>);
    };
    for (size_t count = 0; count != 3 * xsimd::batch<T>::size + 3; ++count)
        check(count);
    check(left.size());
}

TEST(NpMinmaxXsimd, CartesianExceptionalValuesUnalignedAndTails) {
    cartesian_values<float>();
    cartesian_values<double>();
}

template <class T>
void broadcast_in_place() {
    constexpr size_t columns = 2 * xsimd::batch<T>::size + 1;
    using matrix = np::tensor_view<T, np::extents<size_t, 3, columns>>;
    using row = np::tensor_view<const T, np::extents<size_t, columns>>;
    std::array<T, 3 * columns> input{}, expected{};
    std::array<T, columns> lower{}, upper{};
    for (size_t i = 0; i != input.size(); ++i)
        input[i] = T(int(i % 9) - 4);
    for (size_t i = 0; i != columns; ++i) {
        lower[i] = i & 1 ? -T(0) : T(-1);
        upper[i] = i & 1 ? T(0) : T(2);
    }
    input[0] = std::numeric_limits<T>::quiet_NaN();
    input[columns] = -T(0);
    lower[2] = std::numeric_limits<T>::quiet_NaN();
    upper[3] = std::numeric_limits<T>::quiet_NaN();
    for (size_t i = 0; i != input.size(); ++i)
        expected[i] = oracle<true>(oracle<false>(input[i], lower[i % columns]), upper[i % columns]);
    auto expression = np::clip(matrix(input.data()), row(lower.data()), row(upper.data()));
    static_assert(np::detail::homogeneous_xsimd_expression<decltype(expression), T>);
    np::eval_to(matrix(input.data()), expression, np::xsimd_policy<>{});
    for (size_t i = 0; i != input.size(); ++i)
        expect_same(input[i], expected[i]);
}

TEST(NpMinmaxXsimd, BroadcastClipRetainsLeftTiesInPlace) {
    broadcast_in_place<float>();
    broadcast_in_place<double>();
}

TEST(NpMinmaxXsimd, MixedTypesAndNonunitStridesKeepFallback) {
    using shape = np::extents<size_t, 5>;
    using mapping = np::layout_stride::mapping<shape>;
    const float input[]{-2, 99, -0.f, 99, 1, 99, std::numeric_limits<float>::quiet_NaN(), 99, 3};
    const np::tensor_view<const float, shape, np::layout_stride> source(
        input, mapping(shape{}, std::array<size_t, 1>{2}));
    std::array<float, 5> output{};
    auto expression = np::minimum(source, 0);
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(expression), float>);
    np::eval_to(np::tensor_view<float, shape>(output.data()), expression, np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i)
        expect_same(output[i], oracle<true>(input[2 * i], 0.f));
    auto homogeneous = np::maximum(source, 0.f);
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(homogeneous), float>);
    np::eval_to(np::tensor_view<float, shape>(output.data()), homogeneous, np::xsimd_policy<>{});
    for (size_t i = 0; i != output.size(); ++i)
        expect_same(output[i], oracle<false>(input[2 * i], 0.f));
}
} // namespace
