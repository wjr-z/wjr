#include <wjr/container/btree_set.hpp>

#include <algorithm>
#include <limits>
#include <random>
#include <set>
#include <vector>

#include <gtest/gtest.h>

namespace {
TEST(BtreeIntegralSearch, EveryPartialNodeAndInsertionPosition) {
    for (unsigned size = 1; size <= 32; ++size) {
        for (unsigned first = 0; first <= size; ++first) {
            bool in_bounds = true;
            const auto result =
                wjr::btree_detail::integral_set_searcher32::search<1>(size, [&](unsigned index) {
                    in_bounds &= index < size;
                    return index >= first;
                });
            EXPECT_TRUE(in_bounds) << size << ':' << first;
            EXPECT_EQ(result, first) << size << ':' << first;
        }
    }
}

template <typename Key, typename Compare, bool Multi>
void check_mutations() {
    using tree_type =
        std::conditional_t<Multi, wjr::btree_multiset<Key, Compare, std::allocator<char>, 32>,
                           wjr::btree_set<Key, Compare, std::allocator<char>, 32>>;
    using reference_type =
        std::conditional_t<Multi, std::multiset<Key, Compare>, std::set<Key, Compare>>;
    tree_type actual;
    reference_type expected;
    std::mt19937_64 random(17062021);
    std::vector<Key> keys{std::numeric_limits<Key>::min(), std::numeric_limits<Key>::max()};
    for (unsigned i = 0; i != 1536; ++i) {
        keys.push_back(static_cast<Key>(random() % 512) - static_cast<Key>(128));
    }
    auto check = [&](Key key) {
        EXPECT_EQ(actual.size(), expected.size());
        EXPECT_EQ(actual.count(key), expected.count(key));
        const auto compare_position = [&](auto lhs, auto rhs) {
            ASSERT_EQ(lhs == actual.end(), rhs == expected.end());
            if (rhs != expected.end()) {
                EXPECT_EQ(*lhs, *rhs);
            }
        };
        compare_position(actual.lower_bound(key), expected.lower_bound(key));
        compare_position(actual.upper_bound(key), expected.upper_bound(key));
    };
    for (auto key : keys) {
        actual.insert(key);
        expected.insert(key);
        check(key);
    }
    EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    for (size_t i = 0; i < keys.size(); i += 3) {
        EXPECT_EQ(actual.erase(keys[i]), expected.erase(keys[i]));
        check(keys[i]);
    }
    EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    while (!expected.empty()) {
        actual.erase(actual.begin());
        expected.erase(expected.begin());
        if (!expected.empty()) {
            check(*expected.begin());
        }
    }
    actual.insert(Key{7});
    actual.insert(Key{3});
    actual.insert(Key{9});
    EXPECT_EQ(*actual.lower_bound(Key{4}), Key{7});
    actual.clear();
    EXPECT_TRUE(actual.empty());
}

TEST(BtreeIntegralSearch, TypedAndTransparentUniqueSetMutation) {
    check_mutations<uint64_t, std::less<uint64_t>, false>();
    check_mutations<int64_t, std::less<>, false>();
}

TEST(BtreeIntegralSearch, TypedAndTransparentDuplicateSetMutation) {
    check_mutations<int64_t, std::less<int64_t>, true>();
    check_mutations<uint64_t, std::less<>, true>();
}
} // namespace
