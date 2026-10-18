#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/pairwise.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using line = np::dextents<size_t, 1>;
using matrix = np::extents<size_t, 2, 3>;

template <class V, class Acc = void>
concept pairwise = requires(V source) { np::pairwise_sum<Acc>(source); };
struct custom_accessor : np::default_accessor<float> {};
static_assert(pairwise<view<const float, matrix>>);
static_assert(pairwise<view<const double, matrix>, long double>);
static_assert(pairwise<view<const int, matrix>, double>);
static_assert(!pairwise<view<const int, matrix>>);
static_assert(!pairwise<view<const float, matrix>, int>);
static_assert(!pairwise<view<const float, matrix>, const double>);
static_assert(!pairwise<view<volatile float, matrix>>);
static_assert(!pairwise<np::tensor_view<float, matrix, np::layout_right, custom_accessor>>);
static_assert(!pairwise<view<float, matrix, np::layout_left>>);
static_assert(!pairwise<view<float, matrix, np::layout_stride>>);
static_assert(pairwise<view<float, line, np::layout_left>>);
static_assert(pairwise<view<float, line, np::layout_stride>>);
static_assert(std::same_as<decltype(np::pairwise_sum(std::declval<view<float, line>>())), float>);
static_assert(
    std::same_as<decltype(np::pairwise_sum<double>(std::declval<view<int, line>>())), double>);
static_assert(noexcept(np::pairwise_sum(std::declval<view<double, line>>())));

// Independent grouping oracle: accumulate each lane separately, then merge
// slots level by level without using the production kernel.
template <class Acc, class T>
Acc grouping_oracle(const T *input, size_t count, size_t step = 1) {
    if (count == 0)
        return Acc{0};
    if (count > 128) {
        const auto split = (count / 16) * 8;
        const Acc left = grouping_oracle<Acc>(input, split, step);
        const Acc right = grouping_oracle<Acc>(input + split * step, count - split, step);
        return left + right;
    }
    if (count < 8) {
        Acc result = -Acc{0};
        for (size_t i = 0; i != count; ++i)
            result += static_cast<Acc>(input[i * step]);
        return result;
    }
    std::array<Acc, 8> lanes;
    for (size_t lane = 0; lane != 8; ++lane) {
        lanes[lane] = static_cast<Acc>(input[lane * step]);
        for (size_t i = lane + 8; i < count - count % 8; i += 8)
            lanes[lane] += static_cast<Acc>(input[i * step]);
    }
    for (size_t width = 1; width != 8; width *= 2)
        for (size_t first = 0; first != 8; first += 2 * width)
            lanes[first] = lanes[first] + lanes[first + width];
    auto result = lanes[0];
    for (size_t i = count - count % 8; i != count; ++i)
        result += static_cast<Acc>(input[i * step]);
    return result;
}

constexpr bool constant_cases() {
    const double input[]{1, 2, 3, 4, 5, 6};
    if (np::pairwise_sum(view<const double, matrix>(input)) != 21.)
        return false;
    const np::layout_stride::mapping<np::extents<size_t, 3>> mapping(np::extents<size_t, 3>{},
                                                                     std::array{2});
    if (np::pairwise_sum(
            view<const double, np::extents<size_t, 3>, np::layout_stride>(input, mapping)) != 9.)
        return false;
    int integers[]{1, 2, 3};
    if (np::pairwise_sum<double>(view<const int, np::extents<size_t, 3>>(integers)) != 6.)
        return false;
    std::array<float, 257> ones;
    ones.fill(1.f);
    return np::pairwise_sum(view<const float, np::extents<size_t, 257>>(ones.data())) == 257.f &&
           np::pairwise_sum(view<const float, line>(nullptr, 0)) == 0.f;
}
static_assert(constant_cases());

TEST(NpPairwiseSum, ConstantEvaluationAndLayouts) {
    EXPECT_TRUE(constant_cases());
    const double value = 3.5;
    EXPECT_EQ(np::pairwise_sum(view<const double, np::extents<size_t>>(&value)), 3.5);
    EXPECT_EQ(np::pairwise_sum(view<const double, np::extents<size_t>, np::layout_left>(&value)),
              3.5);
    const np::layout_stride::mapping<np::extents<size_t>> scalar_mapping(np::extents<size_t>{},
                                                                         std::array<size_t, 0>{});
    EXPECT_EQ(np::pairwise_sum(view<const double, np::extents<size_t>, np::layout_stride>(
                  &value, scalar_mapping)),
              3.5);
    double values[]{1, 2, 3, 4};
    EXPECT_EQ(np::pairwise_sum(view<const double, line, np::layout_left>(values, 4)), 10.);
    EXPECT_EQ(np::pairwise_sum(view<const double, np::extents<size_t, 1, 2, 2>>(values)), 10.);
    EXPECT_EQ(np::pairwise_sum(view<const double, np::dextents<size_t, 3>>(nullptr, 2, 0, 4)), 0.);
}

template <class T>
void check_grouping() {
    for (size_t count = 0; count != 520; ++count) {
        std::vector<T> storage(2 * count + 1);
        for (size_t i = 0; i != count; ++i) {
            const auto value = i % 17 == 0 ? T(1e10) : T(int(i % 11) - 5) / T(8);
            storage[i * 2 + 1] = i % 3 == 0 ? -value : value;
        }
        const auto *input = storage.data() + 1;
        const np::layout_stride::mapping<line> mapping(line(count), std::array<size_t, 1>{2});
        EXPECT_EQ(np::pairwise_sum(view<const T, line, np::layout_stride>(input, mapping)),
                  grouping_oracle<T>(input, count, 2));
        std::vector<T> packed(count + 1);
        for (size_t i = 0; i != count; ++i)
            packed[i + 1] = storage[2 * i + 1];
        EXPECT_EQ(np::pairwise_sum(view<const T, line>(packed.data() + 1, count)),
                  grouping_oracle<T>(packed.data() + 1, count));
    }
}

TEST(NpPairwiseSum, ExactGroupingAcrossLeafSplitAndStrideBoundaries) {
    check_grouping<float>();
    check_grouping<double>();
}

TEST(NpPairwiseSum, ExactIntegerOraclesForCancellationAndScale) {
    constexpr size_t count = 4096;
    std::vector<float> values(count, 1.f);
    values.front() = 16777216.f;
    values.back() = -16777216.f;
    const auto source = view<const float, line>(values.data(), count);
    // Every input is an exact integer. The int64 sum is an exact oracle,
    // including on MSVC where long double is not wider than double.
    std::int64_t exact = 0;
    for (auto value : values)
        exact += static_cast<std::int64_t>(value);
    const auto ordered = np::sum(source);
    const auto paired = np::pairwise_sum(source);
    EXPECT_TRUE(std::abs(double(paired) - double(exact)) <
                std::abs(double(ordered) - double(exact)));
    EXPECT_EQ(np::pairwise_sum<double>(source), static_cast<double>(exact));
    std::vector<double> large(1024, 1.);
    large.front() = 9007199254740992.;
    const auto expected = std::int64_t{9007199254740992} + 1023;
    const auto result = np::pairwise_sum(view<const double, line>(large.data(), large.size()));
    const auto direct = np::sum(view<const double, line>(large.data(), large.size()));
    // The exact answer lies halfway between adjacent doubles; compare errors
    // in integer units instead of using an already-rounded double oracle.
    EXPECT_TRUE(std::abs(static_cast<std::int64_t>(result) - expected) <
                std::abs(static_cast<std::int64_t>(direct) - expected));
}

TEST(NpPairwiseSum, EmptySignedZeroNaNAndInfinity) {
    const auto empty = np::pairwise_sum(view<const double, line>(nullptr, 0));
    EXPECT_EQ(empty, 0.);
    EXPECT_FALSE(std::signbit(empty));
    for (size_t count :
         {size_t{1}, size_t{7}, size_t{8}, size_t{9}, size_t{128}, size_t{129}, size_t{1024}}) {
        std::vector<double> values(count, -0.);
        EXPECT_TRUE(std::signbit(np::pairwise_sum(view<const double, line>(values.data(), count))));
        values.back() = 0.;
        EXPECT_FALSE(
            std::signbit(np::pairwise_sum(view<const double, line>(values.data(), count))));
        values.front() = std::numeric_limits<double>::infinity();
        EXPECT_EQ(np::pairwise_sum(view<const double, line>(values.data(), count)),
                  std::numeric_limits<double>::infinity());
        values.back() = std::numeric_limits<double>::quiet_NaN();
        EXPECT_TRUE(std::isnan(np::pairwise_sum(view<const double, line>(values.data(), count))));
    }
    const double infinity = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    double values[]{1., infinity, 2.};
    EXPECT_EQ(np::pairwise_sum(view<const double, np::extents<size_t, 3>>(values)), infinity);
    values[0] = -infinity;
    EXPECT_TRUE(std::isnan(np::pairwise_sum(view<const double, np::extents<size_t, 3>>(values))));
    values[0] = nan;
    EXPECT_TRUE(std::isnan(np::pairwise_sum(view<const double, np::extents<size_t, 3>>(values))));
    const double minus_zero = -0.;
    EXPECT_TRUE(
        std::signbit(np::pairwise_sum(view<const double, np::extents<size_t>>(&minus_zero))));
}

TEST(NpPairwiseSum, ExplicitFloatingAccumulatorConvertsEachInputBeforeAdding) {
    int values[]{std::numeric_limits<int>::max(), 1, 2};
    EXPECT_EQ(np::pairwise_sum<double>(view<const int, np::extents<size_t, 3>>(values)),
              double(std::numeric_limits<int>::max()) + 3.);
    float input[]{1.f, 0x1p-24f, 0x1p-24f};
    EXPECT_EQ(np::pairwise_sum<double>(view<const float, np::extents<size_t, 3>>(input)),
              1. + 0x1p-23);
    bool flags[]{true, false, true};
    EXPECT_EQ(np::pairwise_sum<double>(view<const bool, np::extents<size_t, 3>>(flags)), 2.);
}
} // namespace
