#include <algorithm>
#include <array>
#include <iterator>
#include <ranges>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/bitset.hpp>

namespace {

using bits = wjr::bitset<130>;
static_assert(std::ranges::random_access_range<bits>);
static_assert(std::ranges::random_access_range<const bits>);
static_assert(std::ranges::sized_range<bits>);
static_assert(std::ranges::common_range<bits>);
static_assert(std::output_iterator<bits::iterator, bool>);
static_assert(std::indirectly_copyable<bits::const_iterator, bits::iterator>);
static_assert(std::permutable<bits::iterator>);
static_assert(std::sortable<bits::iterator>);
static_assert(!std::output_iterator<bits::const_iterator, bool>);
static_assert(!std::ranges::contiguous_range<bits>);
static_assert(std::same_as<std::iter_rvalue_reference_t<bits::iterator>, bool>);
static_assert(sizeof(bits) == 3 * sizeof(uint64_t));

constexpr bool constant_range_operations() {
    wjr::bitset<9> value(0b100001001);
    std::ranges::reverse(value);
    if (value[0] != true || value[5] != true || value[8] != true) {
        return false;
    }
    std::ranges::fill(value, false);
    value.begin()[4] = true;
    return std::ranges::count(value, true) == 1 && *std::next(value.cbegin(), 4);
}
static_assert(constant_range_operations());

TEST(bitset_ranges, random_access_and_const_traversal) {
    bits value;
    value.set(0).set(32).set(64).set(129);
    EXPECT_EQ(value.end() - value.begin(), 130);
    EXPECT_EQ(value.begin() - value.end(), -130);
    EXPECT_EQ(value.cend() - value.begin(), 130);
    EXPECT_EQ(value.end() - value.cbegin(), 130);
    EXPECT_EQ(std::ranges::distance(value), 130);
    EXPECT_EQ(std::ranges::count(std::as_const(value), true), 4);
    EXPECT_TRUE(*value.rbegin());
    EXPECT_TRUE(*value.crbegin());
    EXPECT_EQ(value.crend() - value.crbegin(), 130);
    EXPECT_TRUE(*(64 + value.begin()));
}

TEST(bitset_ranges, copy_and_fill_exclude_padding) {
    bits value;
    std::array<bool, 130> expected{};
    for (size_t index = 0; index < expected.size(); ++index) {
        expected[index] = index % 3 == 1;
    }
    std::ranges::copy(expected, value.begin());
    EXPECT_TRUE(std::ranges::equal(expected, value));
    std::array<bool, 130> copied{};
    std::ranges::copy(std::as_const(value), copied.begin());
    EXPECT_EQ(copied, expected);
    std::ranges::fill(value, true);
    EXPECT_EQ(value.count(), value.size());
    EXPECT_EQ(value.data()[2], 3);
    std::ranges::fill(value, false);
    EXPECT_TRUE(value.none());
}

TEST(bitset_ranges, proxy_exchange_preserves_bindings) {
    bits lhs;
    bits rhs;
    lhs.set(1).set(65);
    auto first = lhs[1];
    auto second = lhs[2];
    using std::swap;
    swap(first, second);
    EXPECT_FALSE(lhs[1]);
    EXPECT_TRUE(lhs[2]);
    first = true;
    EXPECT_TRUE(lhs[1]);
    std::ranges::iter_swap(lhs.begin() + 65, rhs.begin() + 129);
    EXPECT_FALSE(lhs[65]);
    EXPECT_TRUE(rhs[129]);
    std::ranges::iter_swap(rhs.begin() + 129, rhs.begin() + 129);
    EXPECT_TRUE(rhs[129]);
    const auto proxy = rhs[129];
    proxy = false;
    EXPECT_FALSE(rhs[129]);
    proxy = lhs[1];
    EXPECT_TRUE(rhs[129]);
}

TEST(bitset_ranges, permutation_algorithms_across_words) {
    bits value;
    std::array<bool, 130> expected{};
    for (size_t index = 0; index < expected.size(); ++index) {
        expected[index] = index % 5 == 0;
    }
    std::ranges::copy(expected, value.begin());
    std::ranges::reverse(value);
    std::ranges::reverse(expected);
    EXPECT_TRUE(std::ranges::equal(value, expected));
    std::ranges::rotate(value, value.begin() + 33);
    std::ranges::rotate(expected, expected.begin() + 33);
    EXPECT_TRUE(std::ranges::equal(value, expected));
    std::ranges::sort(value);
    std::ranges::sort(expected);
    EXPECT_TRUE(std::ranges::equal(value, expected));
    EXPECT_EQ(value.count(), 26);
    EXPECT_EQ(value.data()[2] & ~uint64_t(3), 0);
}

TEST(bitset_ranges, single_bit_and_word_boundary_ranges) {
    wjr::bitset<1> one;
    std::ranges::fill(one, true);
    EXPECT_TRUE(one[0]);
    EXPECT_EQ(one.end() - one.begin(), 1);
    wjr::bitset<64> word;
    std::ranges::fill(word, true);
    EXPECT_EQ(word.count(), 64);
    std::ranges::copy(one, word.begin());
    EXPECT_EQ(word.count(), 64);
}

} // namespace
