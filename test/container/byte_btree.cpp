#include <algorithm>
#include <cstdint>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/experimental/byte_btree.hpp>

namespace {
using namespace wjr::experimental;
using key_type = std::uint64_t;
using layout = byte_btree_layout<key_type>;
static_assert(layout::leaf_slots == 28);
static_assert(layout::leaf_allocation_bytes == 256);
static_assert(layout::inner_allocation_bytes > 256);
static_assert(layout::budget_satisfied);
static_assert(wjr::btree_traits<key_type, void, false>::node_size == 16);
static_assert(
    std::is_same_v<byte_btree_set<key_type>,
                   wjr::btree_set<key_type, std::less<key_type>, std::allocator<char>, 28>>);
static_assert(std::is_same_v<
              byte_btree_map<key_type, key_type>,
              wjr::btree_map<key_type, key_type, std::less<key_type>, std::allocator<char>, 28>>);
static_assert(sizeof(byte_btree_set<key_type>) == sizeof(wjr::btree_set<key_type>));
static_assert(sizeof(byte_btree_map<key_type, key_type>) ==
              sizeof(wjr::btree_map<key_type, key_type>));

struct incomplete;
template <typename Key, typename Value, typename Budget>
concept has_layout = requires { typename byte_btree_layout<Key, Value, Budget>; };
template <typename Value>
concept has_map = requires { typename byte_btree_map<int, Value>; };
static_assert(!has_layout<incomplete, void, leaf_bytes<256>>);
static_assert(!has_layout<void, void, leaf_bytes<256>>);
static_assert(!has_layout<const int, void, leaf_bytes<256>>);
static_assert(!has_layout<int, int &, leaf_bytes<256>>);
static_assert(!has_layout<int, int, std::integral_constant<size_t, 256>>);
static_assert(!has_map<void>);
static_assert(!has_map<const int>);

TEST(BtreeByteBudget, BoundariesPaddingAndClamp) {
    using small = byte_btree_layout<key_type, void, leaf_bytes<0>>;
    using minimum = byte_btree_layout<key_type, void, leaf_bytes<64>>;
    using before = byte_btree_layout<key_type, void, leaf_bytes<255>>;
    using maximum = byte_btree_layout<key_type, void, leaf_bytes<SIZE_MAX>>;
    EXPECT_EQ(small::leaf_slots, 4u);
    EXPECT_FALSE(small::budget_satisfied);
    EXPECT_EQ(minimum::leaf_slots, 4u);
    EXPECT_EQ(minimum::leaf_allocation_bytes, 64u);
    EXPECT_TRUE(minimum::budget_satisfied);
    EXPECT_EQ(before::leaf_slots, 26u);
    EXPECT_EQ(before::leaf_allocation_bytes, 240u);
    EXPECT_EQ(maximum::leaf_slots, 64u);
    EXPECT_TRUE(maximum::budget_satisfied);

    using padded = byte_btree_layout<unsigned char, void, leaf_bytes<47>>;
    using exact = byte_btree_layout<unsigned char, void, leaf_bytes<48>>;
    EXPECT_EQ(padded::leaf_slots, 4u);
    EXPECT_EQ(padded::leaf_allocation_bytes, 48u);
    EXPECT_FALSE(padded::budget_satisfied);
    EXPECT_EQ(exact::leaf_slots, 16u);
    EXPECT_EQ(exact::leaf_allocation_bytes, 48u);
    EXPECT_TRUE(exact::budget_satisfied);
}

struct allocation_state {
    static inline std::vector<std::pair<size_t, size_t>> requests;
    static inline size_t live = 0;
};

template <typename T>
struct counting_allocator {
    using value_type = T;
    counting_allocator() = default;
    template <typename U>
    counting_allocator(const counting_allocator<U> &) noexcept {}
    T *allocate(size_t count) {
        allocation_state::requests.emplace_back(sizeof(T) * count, alignof(T));
        ++allocation_state::live;
        return std::allocator<T>{}.allocate(count);
    }
    void deallocate(T *ptr, size_t count) noexcept {
        --allocation_state::live;
        std::allocator<T>{}.deallocate(ptr, count);
    }
    bool operator==(const counting_allocator &) const = default;
};

/** @brief Compare diagnostics with the requests made by the real typed allocator. */
template <typename Key, typename Value, typename Budget>
void check_allocations() {
    using allocator = counting_allocator<char>;
    using info = byte_btree_layout<Key, Value, Budget, allocator>;
    using tree_type =
        std::conditional_t<std::is_void_v<Value>,
                           byte_btree_set<Key, Budget, std::less<Key>, allocator>,
                           wjr::btree_map<Key, Value, std::less<Key>, allocator, info::leaf_slots>>;
    allocation_state::requests.clear();
    ASSERT_EQ(allocation_state::live, 0u);
    {
        tree_type tree;
        EXPECT_TRUE(allocation_state::requests.empty());
        for (int i = 0; i != 3; ++i) {
            if constexpr (std::is_void_v<Value>) {
                tree.emplace(static_cast<Key>(i));
            } else {
                tree.try_emplace(static_cast<Key>(i), i);
            }
            const auto values = wjr::btree_detail::is_inline_key_v<typename tree_type::value_type>
                                    ? 0u
                                    : static_cast<unsigned>(i + 1);
            EXPECT_EQ(allocation_state::requests.size(), values + (i == 2 ? 1u : 0u));
        }
        ASSERT_FALSE(allocation_state::requests.empty());
        auto found_leaf = std::find_if(
            allocation_state::requests.begin(), allocation_state::requests.end(),
            [](const auto &request) { return request.first == info::leaf_allocation_bytes; });
        ASSERT_NE(found_leaf, allocation_state::requests.end());
        EXPECT_EQ(found_leaf->second, 16u);
        for (int i = 3; i != 120; ++i) {
            if constexpr (std::is_void_v<Value>) {
                tree.emplace(static_cast<Key>(i));
            } else {
                tree.try_emplace(static_cast<Key>(i), i);
            }
        }
        EXPECT_TRUE(std::any_of(
            allocation_state::requests.begin(), allocation_state::requests.end(),
            [](const auto &request) { return request.first == info::inner_allocation_bytes; }));
        tree.clear();
        EXPECT_EQ(allocation_state::live, 0u);
    }
    EXPECT_EQ(allocation_state::live, 0u);
}

TEST(BtreeByteBudget, DescriptorMatchesRealAllocationsAndSso) {
    check_allocations<key_type, void, leaf_bytes<256>>();
    check_allocations<key_type, key_type, leaf_bytes<256>>();
    check_allocations<unsigned char, void, leaf_bytes<48>>();
    check_allocations<key_type, void, leaf_bytes<0>>();
}

struct equivalent_less {
    bool operator()(key_type lhs, key_type rhs) const noexcept { return lhs / 3 < rhs / 3; }
};

/** @brief Exercise N28 split, rebalance, root collapse and comparator equivalence. */
template <typename Compare>
void check_mutations() {
    byte_btree_map<key_type, key_type, leaf_bytes<256>, Compare> map;
    byte_btree_set<key_type, leaf_bytes<256>, Compare> set;
    std::map<key_type, key_type, Compare> reference_map;
    std::set<key_type, Compare> reference_set;
    std::vector<key_type> keys(2400);
    std::iota(keys.begin(), keys.end(), key_type{0});
    std::mt19937 random(78432);
    std::shuffle(keys.begin(), keys.end(), random);
    const auto compare_contents = [&] {
        ASSERT_EQ(map.size(), reference_map.size());
        ASSERT_EQ(set.size(), reference_set.size());
        EXPECT_TRUE(std::equal(map.begin(), map.end(), reference_map.begin(), reference_map.end()));
        EXPECT_TRUE(std::equal(set.begin(), set.end(), reference_set.begin(), reference_set.end()));
    };
    for (const auto key : keys) {
        const auto actual_map = map.try_emplace(key, key + 7);
        const auto expected_map = reference_map.try_emplace(key, key + 7);
        EXPECT_EQ(actual_map.second, expected_map.second);
        EXPECT_EQ(actual_map.first->first, expected_map.first->first);
        EXPECT_EQ(set.emplace(key).second, reference_set.emplace(key).second);
        if (key % 79 == 0) {
            compare_contents();
        }
    }
    compare_contents();
    for (key_type key = 0; key != 2403; ++key) {
        const auto actual = map.lower_bound(key);
        const auto expected = reference_map.lower_bound(key);
        ASSERT_EQ(actual == map.end(), expected == reference_map.end());
        if (actual != map.end()) {
            EXPECT_EQ(actual->first, expected->first);
        }
        EXPECT_EQ(set.count(key), reference_set.count(key));
    }
    std::shuffle(keys.begin(), keys.end(), random);
    for (const auto key : keys) {
        const auto map_iter = map.lower_bound(key);
        const auto set_iter = set.lower_bound(key);
        const bool has_map = map_iter != map.end() && !Compare{}(key, map_iter->first);
        const bool has_set = set_iter != set.end() && !Compare{}(key, *set_iter);
        EXPECT_EQ(has_map, reference_map.erase(key) != 0);
        EXPECT_EQ(has_set, reference_set.erase(key) != 0);
        if (has_map) {
            map.erase(map_iter);
        }
        if (has_set) {
            set.erase(set_iter);
        }
        if (key % 79 == 0) {
            compare_contents();
        }
    }
    compare_contents();
    EXPECT_TRUE(map.empty());
    EXPECT_TRUE(set.empty());
    map.try_emplace(5, 12);
    set.emplace(5);
    EXPECT_EQ(map.begin()->second, 12u);
    EXPECT_EQ(*set.begin(), 5u);
}

TEST(BtreeByteBudget, N28Ascending) { check_mutations<std::less<key_type>>(); }
TEST(BtreeByteBudget, N28Descending) { check_mutations<std::greater<key_type>>(); }
TEST(BtreeByteBudget, N28EquivalentKeys) { check_mutations<equivalent_less>(); }
} // namespace
