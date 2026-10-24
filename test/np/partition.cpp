#include <algorithm>
#include <array>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/partition.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class V, class I>
concept partitionable = requires(V data, std::span<I> ranks) { np::partition_at(data, ranks); };
static_assert(partitionable<view<int>, const unsigned>);
static_assert(!partitionable<view<const int>, size_t>);
static_assert(!partitionable<view<volatile int>, size_t>);
static_assert(!partitionable<view<int>, bool>);
static_assert(!partitionable<view<int>, volatile int>);
static_assert(!partitionable<view<int>, double>);
static_assert(!partitionable<np::tensor_view<int, np::extents<size_t, 2, 2>>, size_t>);

constexpr bool select_constants() {
    int data[] = {8, 1, 5, 3, 9, 2};
    const unsigned ranks[] = {0, 2, 2, 5};
    np::partition_at(view<int>(data, 6), std::span(ranks));
    return data[0] == 1 && data[2] == 3 && data[5] == 9;
}
static_assert(select_constants());

TEST(NpPartition, EmptyDuplicatesAndConstantEvaluation) {
    EXPECT_TRUE(select_constants());
    np::partition_at(view<int>(nullptr, 0), std::span<const size_t>{});
    int untouched[] = {3, 2, 1};
    np::partition_at(view<int>(untouched, 3), std::span<const size_t>{});
    EXPECT_EQ(untouched[0], 3);
    EXPECT_EQ(untouched[2], 1);
    bool bits[] = {true, false, true, false};
    const int ranks[] = {1, 2};
    np::partition_at(view<bool>(bits, 4), std::span(ranks));
    EXPECT_FALSE(bits[1]);
    EXPECT_TRUE(bits[2]);
}

TEST(NpPartition, NaNsSortLastAndInfinitiesParticipate) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    double data[] = {nan, inf, 2, -0., nan, -inf, 0., 1};
    const size_t ranks[] = {0, 1, 2, 3, 4, 5, 6, 7};
    np::partition_at(view<double>(data, 8), std::span(ranks));
    EXPECT_EQ(data[0], -inf);
    EXPECT_EQ(data[1], 0);
    EXPECT_EQ(data[2], 0);
    EXPECT_EQ(data[3], 1);
    EXPECT_EQ(data[4], 2);
    EXPECT_EQ(data[5], inf);
    EXPECT_NE(data[6], data[6]);
    EXPECT_NE(data[7], data[7]);
}

TEST(NpPartition, RandomAndAdversarialInputsPreserveSelectedBoundaries) {
    std::mt19937 engine(1051);
    for (size_t count : {1, 2, 3, 17, 300}) {
        for (int distribution = 0; distribution != 5; ++distribution) {
            std::vector<int> values(count);
            for (size_t i = 0; i != count; ++i)
                values[i] = distribution == 0   ? static_cast<int>(engine() % 100)
                            : distribution == 1 ? static_cast<int>(i)
                            : distribution == 2 ? -static_cast<int>(i)
                            : distribution == 3 ? 7
                                                : static_cast<int>(i % 2);
            auto sorted = values;
            std::sort(sorted.begin(), sorted.end());
            std::vector<uint32_t> ranks{0, static_cast<uint32_t>(count / 2),
                                        static_cast<uint32_t>(count - 1)};
            for (int i = 0; i != 12; ++i)
                ranks.push_back(engine() % count);
            std::sort(ranks.begin(), ranks.end());
            np::partition_at(view<int>(values.data(), count), std::span(ranks));
            for (size_t rank : ranks) {
                ASSERT_EQ(values[rank], sorted[rank]);
                for (size_t i = 0; i < rank; ++i)
                    EXPECT_LE(values[i], values[rank]);
                for (size_t i = rank + 1; i < count; ++i)
                    EXPECT_GE(values[i], values[rank]);
            }
            std::sort(values.begin(), values.end());
            EXPECT_EQ(values, sorted);
        }
    }
}

TEST(NpPartition, OneSidedRequestsAndEquivalentBlocks) {
    std::vector<double> original(512);
    for (size_t i = 0; i != original.size(); ++i)
        original[i] = i % 7 == 0    ? std::numeric_limits<double>::quiet_NaN()
                      : i % 11 == 0 ? std::numeric_limits<double>::infinity()
                      : i % 13 == 0 ? -std::numeric_limits<double>::infinity()
                      : i % 5 == 0  ? -0.
                                    : static_cast<double>(i % 3);
    const np::detail::partition_less less;
    auto expected = original;
    std::sort(expected.begin(), expected.end(), less);
    for (const auto &ranks :
         {std::vector<size_t>{0, 1, 2}, std::vector<size_t>{509, 510, 511},
          std::vector<size_t>{3, 80, 150, 200, 300, 400, 510}, std::vector<size_t>(100, 200)}) {
        auto values = original;
        np::partition_at(view<double>(values.data(), values.size()), std::span(ranks));
        for (auto rank : ranks) {
            EXPECT_FALSE(less(values[rank], expected[rank]) || less(expected[rank], values[rank]));
            for (size_t i = 0; i < rank; ++i)
                EXPECT_FALSE(less(values[rank], values[i]));
            for (size_t i = rank + 1; i < values.size(); ++i)
                EXPECT_FALSE(less(values[i], values[rank]));
        }
    }
    std::vector<int> equal(512, 7);
    const size_t ranks[] = {0, 1, 200, 400, 511};
    np::partition_at(view<int>(equal.data(), equal.size()), std::span(ranks));
    EXPECT_TRUE(std::all_of(equal.begin(), equal.end(), [](int value) { return value == 7; }));
    std::vector<int> fallback(512);
    std::mt19937 engine(1057);
    for (auto &value : fallback)
        value = static_cast<int>(engine() % 1000);
    np::detail::select_ranks<std::less<>>(fallback.data(), 0, fallback.size(),
                                          std::span<const size_t>(ranks), 0);
    EXPECT_TRUE(std::is_sorted(fallback.begin(), fallback.end()));
}
} // namespace
