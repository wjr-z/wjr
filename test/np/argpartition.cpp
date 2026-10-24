#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/argpartition.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class D, class S, class R>
concept selectable =
    requires(D dst, S src, std::span<R> ranks) { np::argpartition_to(dst, src, ranks); };
static_assert(selectable<view<unsigned>, view<const double>, const size_t>);
static_assert(!selectable<view<const unsigned>, view<double>, size_t>);
static_assert(!selectable<view<volatile unsigned>, view<double>, size_t>);
static_assert(!selectable<view<bool>, view<int>, size_t>);
static_assert(!selectable<view<double>, view<int>, size_t>);
static_assert(!selectable<view<int>, view<volatile int>, size_t>);
static_assert(!selectable<view<int>, view<int>, bool>);
static_assert(!selectable<view<int>, view<int>, volatile int>);
static_assert(!selectable<view<int>, view<int>, double>);
static_assert(!selectable<np::tensor_view<int, np::extents<size_t, 3>>,
                          np::tensor_view<const int, np::extents<size_t, 4>>, size_t>);

template <np::partition_method Method>
constexpr bool select_constants() {
    const int source[] = {7, 1, 5, 3, 9, 2};
    unsigned output[6]{};
    const unsigned ranks[] = {0, 2, 2, 5};
    np::argpartition_to<np::sort_order::ascending, Method>(
        view<unsigned>(output, 6), view<const int>(source, 6), std::span(ranks));
    return source[output[0]] == 1 && source[output[2]] == 3 && source[output[5]] == 9;
}
static_assert(select_constants<np::partition_method::nth_element>());
static_assert(select_constants<np::partition_method::shared_pivot>());

TEST(NpArgpartition, EmptyRequestsInitializePermutation) {
    np::argpartition_to(view<unsigned>(nullptr, 0), view<const int>(nullptr, 0),
                        std::span<const size_t>{});
    const int source[] = {4, 2, 1};
    int output[3] = {-1, -1, -1};
    np::argpartition_to(view<int>(output, 3), view<const int>(source, 3), std::span<const int>{});
    EXPECT_EQ(std::to_array(output), (std::array{0, 1, 2}));
    EXPECT_TRUE(select_constants<np::partition_method::nth_element>());
    EXPECT_TRUE(select_constants<np::partition_method::shared_pivot>());
}

template <np::sort_order Order, class T>
constexpr bool less_value(T a, T b) {
    if constexpr (std::is_floating_point_v<T>) {
        if (std::isnan(a))
            return false;
        if (std::isnan(b))
            return true;
    }
    if constexpr (Order == np::sort_order::ascending)
        return a < b;
    else
        return a > b;
}

template <np::sort_order Order, np::partition_method Method, class T>
void compare_reference(const std::vector<T> &source, const std::vector<size_t> &ranks) {
    auto sorted = source;
    std::sort(sorted.begin(), sorted.end(), less_value<Order, T>);
    std::vector<size_t> output(source.size());
    np::argpartition_to<Order, Method>(view<size_t>(output.data(), output.size()),
                                       view<const T>(source.data(), source.size()),
                                       std::span(ranks));
    for (const auto rank : ranks) {
        const auto actual = source[output[rank]];
        EXPECT_FALSE(less_value<Order>(actual, sorted[rank]));
        EXPECT_FALSE(less_value<Order>(sorted[rank], actual));
        for (size_t i = 0; i != rank; ++i)
            EXPECT_FALSE(less_value<Order>(actual, source[output[i]]));
        for (size_t i = rank + 1; i != source.size(); ++i)
            EXPECT_FALSE(less_value<Order>(source[output[i]], actual));
    }
    std::sort(output.begin(), output.end());
    for (size_t i = 0; i != output.size(); ++i)
        EXPECT_EQ(output[i], i);
}

template <np::partition_method Method>
void check_special_values() {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const std::vector<double> source{nan, 4, -0., inf, nan, 2, 0., -inf, 2};
    const std::vector<size_t> ranks{0, 1, 1, 4, 7, 8};
    compare_reference<np::sort_order::ascending, Method>(source, ranks);
    compare_reference<np::sort_order::descending, Method>(source, ranks);
}

TEST(NpArgpartition, NaNsZerosInfinitiesAndDuplicateRanks) {
    check_special_values<np::partition_method::nth_element>();
    check_special_values<np::partition_method::shared_pivot>();
}

TEST(NpArgpartition, RandomRanksBothAlgorithmsAndDirections) {
    std::mt19937 engine(911);
    for (size_t count : {1, 16, 17, 65, 257}) {
        std::vector<int> source(count);
        for (auto &value : source)
            value = static_cast<int>(engine() % 11);
        for (auto ranks : {std::vector<size_t>{0, 0}, std::vector<size_t>{count - 1, count - 1},
                           std::vector<size_t>{0, count / 2, count / 2, count - 1}}) {
            compare_reference<np::sort_order::ascending, np::partition_method::nth_element>(source,
                                                                                            ranks);
            compare_reference<np::sort_order::descending, np::partition_method::nth_element>(source,
                                                                                             ranks);
            compare_reference<np::sort_order::ascending, np::partition_method::shared_pivot>(source,
                                                                                             ranks);
            compare_reference<np::sort_order::descending, np::partition_method::shared_pivot>(
                source, ranks);
        }
    }
}

TEST(NpArgpartition, SharedPivotLargePartitionsAndEqualBlocks) {
    std::vector<double> source(2048);
    std::mt19937 engine(199);
    const std::vector<size_t> ranks{1, 1, 3, 2046};
    for (auto &value : source)
        value = static_cast<double>(engine() % 31);
    compare_reference<np::sort_order::ascending, np::partition_method::shared_pivot>(source, ranks);
    compare_reference<np::sort_order::descending, np::partition_method::shared_pivot>(source,
                                                                                      ranks);
    std::fill(source.begin(), source.end(), std::numeric_limits<double>::quiet_NaN());
    compare_reference<np::sort_order::ascending, np::partition_method::shared_pivot>(source, ranks);
}

TEST(NpArgpartition, StridedAndPaddedSourcesAreNotModified) {
    using shape = np::extents<size_t, 4>;
    const int data[] = {8, -99, 2, -99, 7, -99, 1};
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 1>{2});
    const np::tensor_view<const int, shape, np::layout_stride> source(data, mapping);
    unsigned output[4];
    const unsigned ranks[] = {0, 2};
    np::argpartition_to(np::tensor_view<unsigned, shape>(output), source, std::span(ranks));
    EXPECT_EQ(source(output[0]), 1);
    EXPECT_EQ(source(output[2]), 7);
    EXPECT_EQ(std::to_array(data), (std::array{8, -99, 2, -99, 7, -99, 1}));
    using padded = Kokkos::Experimental::layout_right_padded<8>;
    const int packed[] = {8, 2, 7, 1};
    np::argpartition_to<np::sort_order::descending>(
        np::tensor_view<unsigned, shape>(output), np::tensor_view<const int, shape, padded>(packed),
        std::span(ranks));
    EXPECT_EQ(packed[output[0]], 8);
    EXPECT_EQ(packed[output[2]], 2);
}

TEST(NpArgpartition, SharedPivotStridedNaNsAndEquivalentBlocks) {
    constexpr size_t count = 257, step = 3;
    std::array<double, count * step> storage;
    storage.fill(-99);
    for (size_t i = 0; i != count; ++i)
        storage[i * step] = i % 7 == 0 ? std::numeric_limits<double>::quiet_NaN()
                                       : static_cast<double>((i * 13) % 19);
    using shape = np::extents<size_t, count>;
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 1>{step});
    const np::tensor_view<const double, shape, np::layout_stride> source(storage.data(), mapping);
    const size_t ranks[] = {0, 2, 2, count / 2, count - 2, count - 1};
    const auto check = [&]<np::sort_order Order>() {
        std::array<size_t, count> output;
        std::array<double, count> expected;
        for (size_t i = 0; i != count; ++i)
            expected[i] = source(i);
        std::sort(expected.begin(), expected.end(), less_value<Order, double>);
        np::argpartition_to<Order, np::partition_method::shared_pivot>(
            np::tensor_view<size_t, shape>(output.data()), source, std::span(ranks));
        for (auto rank : ranks) {
            const auto actual = source(output[rank]);
            EXPECT_FALSE(less_value<Order>(actual, expected[rank]));
            EXPECT_FALSE(less_value<Order>(expected[rank], actual));
            for (size_t i = 0; i != rank; ++i)
                EXPECT_FALSE(less_value<Order>(actual, source(output[i])));
            for (size_t i = rank + 1; i != count; ++i)
                EXPECT_FALSE(less_value<Order>(source(output[i]), actual));
        }
        std::sort(output.begin(), output.end());
        for (size_t i = 0; i != count; ++i)
            EXPECT_EQ(output[i], i);
    };
    check.template operator()<np::sort_order::ascending>();
    check.template operator()<np::sort_order::descending>();
    for (size_t i = 0; i != count; ++i) {
        EXPECT_EQ(storage[i * step + 1], -99);
        EXPECT_EQ(storage[i * step + 2], -99);
        storage[i * step] = 7;
    }
    check.template operator()<np::sort_order::ascending>();
    check.template operator()<np::sort_order::descending>();
}

TEST(NpArgpartition, FullIntegerPrecisionBoolAndNarrowOutput) {
    constexpr auto max = std::numeric_limits<uint64_t>::max();
    const std::vector<uint64_t> values{max, max - 1, 0, max - 2, max};
    const std::vector<size_t> ranks{0, 1, 3, 4};
    compare_reference<np::sort_order::ascending, np::partition_method::nth_element>(values, ranks);
    compare_reference<np::sort_order::descending, np::partition_method::shared_pivot>(values,
                                                                                      ranks);
    std::array<int, 256> source;
    std::iota(source.rbegin(), source.rend(), 0);
    uint8_t output[256];
    const unsigned targets[] = {0, 255};
    np::argpartition_to(view<uint8_t>(output, 256), view<const int>(source.data(), 256),
                        std::span(targets));
    EXPECT_EQ(source[output[0]], 0);
    EXPECT_EQ(source[output[255]], 255);
    const bool bits[] = {true, false};
    int indices[2];
    const int rank = 0;
    np::argpartition_to(view<int>(indices, 2), view<const bool>(bits, 2),
                        std::span<const int>(&rank, 1));
    EXPECT_FALSE(bits[indices[0]]);
}
} // namespace
