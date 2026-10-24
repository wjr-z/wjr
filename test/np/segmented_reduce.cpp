#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/segmented_reduce.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class D, class S, class I = const size_t, class Acc = void>
concept has_sum =
    requires(D dst, S src, std::span<I> offsets) { np::segmented_sum_to<Acc>(dst, src, offsets); };
template <class D, class S>
concept has_mean = requires(D dst, S src, std::span<const size_t> offsets) {
    np::segmented_mean_to(dst, src, offsets);
};
template <class Operation>
concept has_reduce =
    requires(view<int> dst, view<const int> src, std::span<const size_t> offsets,
             Operation &operation) { np::segmented_reduce_to(dst, src, offsets, 0, operation); };
static_assert(has_sum<view<int>, view<const bool>>);
static_assert(has_sum<view<double>, view<const float>, uint32_t, double>);
static_assert(!has_sum<view<const int>, view<int>>);
static_assert(!has_sum<view<volatile int>, view<int>>);
static_assert(!has_sum<view<int>, view<volatile int>>);
static_assert(!has_sum<view<int>, view<int>, float>);
static_assert(!has_sum<view<int>, view<int>, bool>);
static_assert(!has_sum<view<int>, view<int>, volatile int>);
static_assert(!has_mean<view<int>, view<int>>);
static_assert(!has_sum<view<int>, np::tensor_view<int, np::extents<size_t, 2, 2>>>);
static_assert(has_reduce<std::plus<>> && !has_reduce<int>);

constexpr bool constant_segments() {
    const int input[] = {99, 1, 2, 3, 4, 99};
    const uint32_t offsets[] = {1, 3, 3, 5};
    int output[3]{};
    np::segmented_sum_to(view<int>(output, 3), view<const int>(input, 6), std::span(offsets));
    return output[0] == 3 && output[1] == 0 && output[2] == 7;
}
static_assert(constant_segments());

TEST(NpSegmentedReduce, SubrangesEmptySegmentsAndMean) {
    EXPECT_TRUE(constant_segments());
    const int input[] = {99, 1, 2, 3, 4, 99};
    const int64_t offsets[] = {1, 3, 3, 5};
    double output[3];
    np::segmented_mean_to(view<double>(output, 3), view<const int>(input, 6), std::span(offsets));
    EXPECT_EQ(output[0], 1.5);
    EXPECT_TRUE(std::isnan(output[1]));
    EXPECT_EQ(output[2], 3.5);
}

TEST(NpSegmentedReduce, NaNsAndAccumulatorConversion) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double input[] = {1, nan, 3, nan, nan, 6};
    const size_t offsets[] = {0, 3, 5, 6};
    const view<const double> source(input, 6);
    double output[3];
    const view<double> destination(output, 3);
    np::segmented_sum_to(destination, source, std::span(offsets));
    EXPECT_TRUE(std::isnan(output[0]) && std::isnan(output[1]));
    np::segmented_nansum_to(destination, source, std::span(offsets));
    EXPECT_EQ(output[0], 4);
    EXPECT_EQ(output[1], 0);
    EXPECT_EQ(output[2], 6);
    np::segmented_nanmean_to(destination, source, std::span(offsets));
    EXPECT_EQ(output[0], 2);
    EXPECT_TRUE(std::isnan(output[1]));
    EXPECT_EQ(output[2], 6);
    const float values[] = {16777216, 1, -16777216};
    const uint32_t one_segment[] = {0, 3};
    np::segmented_sum_to<double>(view<double>(output, 1), view<const float>(values, 3),
                                 std::span(one_segment));
    EXPECT_EQ(output[0], 1);
}

TEST(NpSegmentedReduce, EmptySourceAndDestination) {
    const size_t offsets[] = {0, 0, 0};
    double output[2];
    const view<const double> empty(nullptr, 0);
    np::segmented_sum_to(view<double>(output, 2), empty, std::span(offsets));
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 0);
    np::segmented_mean_to(view<double>(output, 2), empty, std::span(offsets));
    EXPECT_TRUE(std::isnan(output[0]) && std::isnan(output[1]));
    np::segmented_sum_to(view<double>(nullptr, 0), empty, std::span<const size_t>{});
    np::segmented_reduce_to(view<double>(output, 2), empty, std::span(offsets), 7., std::plus<>{});
    EXPECT_EQ(output[0], 7);
    EXPECT_EQ(output[1], 7);
}

TEST(NpSegmentedReduce, BooleanFoldAndNaNFilteringBeforeIntegerConversion) {
    const bool input[] = {true, false, true};
    const uint32_t offsets[] = {0, 2, 3};
    int counts[2];
    np::segmented_sum_to(view<int>(counts, 2), view<const bool>(input, 3), std::span(offsets));
    EXPECT_EQ(counts[0], 1);
    EXPECT_EQ(counts[1], 1);
    bool output[2];
    np::segmented_reduce_to(view<bool>(output, 2), view<const bool>(input, 3), std::span(offsets),
                            true, std::logical_and<>{});
    EXPECT_FALSE(output[0]);
    EXPECT_TRUE(output[1]);
    const double values[] = {1, std::numeric_limits<double>::quiet_NaN(), 3};
    np::segmented_nansum_to<int>(view<int>(counts, 2), view<const double>(values, 3),
                                 std::span(offsets));
    EXPECT_EQ(counts[0], 1);
    EXPECT_EQ(counts[1], 3);
}

struct ordered_operation {
    int *calls;
    explicit ordered_operation(int &count) noexcept : calls(&count) {}
    ordered_operation(const ordered_operation &) = delete;
    ordered_operation(ordered_operation &&) = delete;
    int operator()(int total, int element) noexcept {
        ++*calls;
        return total * 10 + element;
    }
};

TEST(NpSegmentedReduce, BorrowedNoncopyableOperationAndOrder) {
    const int input[] = {1, 2, 3, 4, 5};
    const uint32_t offsets[] = {0, 2, 2, 5};
    int output[3];
    int calls = 0;
    static_assert(noexcept(np::segmented_reduce_to(
        std::declval<view<int>>(), std::declval<view<const int>>(),
        std::declval<std::span<const uint32_t>>(), 0, std::declval<ordered_operation &>())));
    np::segmented_reduce_to(view<int>(output, 3), view<const int>(input, 5), std::span(offsets), 0,
                            ordered_operation(calls));
    EXPECT_EQ(output[0], 12);
    EXPECT_EQ(output[1], 0);
    EXPECT_EQ(output[2], 345);
    EXPECT_EQ(calls, 5);
}

struct throwing_result {
    int value;
    operator int() const {
        if (value == 99)
            throw std::runtime_error("conversion");
        return value;
    }
};
struct converting_operation {
    throwing_result operator()(int total, int element) const noexcept { return {total + element}; }
};

TEST(NpSegmentedReduce, ResultConversionCanThrowWithoutWritingCurrentGroup) {
    const int input[] = {1, 99};
    const size_t offsets[] = {0, 1, 2};
    int output[] = {-1, -1};
    static_assert(!noexcept(np::segmented_reduce_to(
        std::declval<view<int>>(), std::declval<view<const int>>(),
        std::declval<std::span<const size_t>>(), 0, std::declval<converting_operation &>())));
    EXPECT_THROW(np::segmented_reduce_to(view<int>(output, 2), view<const int>(input, 2),
                                         std::span(offsets), 0, converting_operation{}),
                 std::runtime_error);
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], -1);
}

TEST(NpSegmentedReduce, StridedInputAndOutputPreserveHoles) {
    const int input[] = {1, 999, 2, 999, 3, 999, 4};
    int output[] = {-1, -999, -1};
    using input_shape = np::extents<size_t, 4>;
    using output_shape = np::extents<size_t, 2>;
    const np::layout_stride::mapping<input_shape> imap(input_shape{}, std::array<size_t, 1>{2});
    const np::layout_stride::mapping<output_shape> omap(output_shape{}, std::array<size_t, 1>{2});
    const np::tensor_view<const int, input_shape, np::layout_stride> src(input, imap);
    const np::tensor_view<int, output_shape, np::layout_stride> dst(output, omap);
    const uint32_t offsets[] = {0, 1, 4};
    np::segmented_sum_to(dst, src, std::span(offsets));
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], -999);
    EXPECT_EQ(output[2], 9);
}

TEST(NpSegmentedReduce, RandomGroupsAgainstOrderedReference) {
    std::mt19937 engine(701);
    std::vector<double> input(512);
    std::array<uint32_t, 31> offsets{};
    for (auto &value : input)
        value = static_cast<int>(engine() % 1000) - 500;
    for (size_t group = 0; group + 1 != offsets.size(); ++group)
        offsets[group + 1] = offsets[group] + engine() % 15;
    std::array<double, 30> output;
    np::segmented_sum_to(view<double>(output.data(), output.size()),
                         view<const double>(input.data(), input.size()), std::span(offsets));
    for (size_t group = 0; group != output.size(); ++group) {
        double expected = 0;
        for (size_t i = offsets[group]; i != offsets[group + 1]; ++i)
            expected += input[i];
        EXPECT_EQ(output[group], expected);
    }
}
} // namespace
