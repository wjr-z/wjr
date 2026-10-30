#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/btree_set.hpp>

namespace {
template <typename Key>
Key make_key(int i) {
    if constexpr (std::is_same_v<Key, int>) {
        return i;
    } else {
        return std::to_string(i);
    }
}

template <typename Key, typename Compare, size_t NodeSize>
void check_multiset() {
    using tree_type = wjr::btree_multiset<Key, Compare, std::allocator<char>, NodeSize>;
    using iterator = typename tree_type::iterator;
    static_assert(
        std::is_same_v<decltype(std::declval<tree_type &>().insert(std::declval<const Key &>())),
                       iterator>);
    static_assert(
        std::is_same_v<decltype(std::declval<tree_type &>().insert(std::declval<Key &&>())),
                       iterator>);
    static_assert(std::is_same_v<decltype(std::declval<tree_type &>().emplace(std::declval<Key>())),
                                 iterator>);
    tree_type actual;
    std::multiset<Key, Compare> expected;
    for (int i = 0; i != 240; ++i) {
        const auto key = make_key<Key>((i * 7) % 17);
        auto iter = [&]() {
            if (i % 3 == 0) {
                return actual.insert(key);
            }
            if (i % 3 == 1) {
                auto moved_key = key;
                return actual.insert(std::move(moved_key));
            }
            return actual.emplace(key);
        }();
        ASSERT_NE(iter, actual.end());
        EXPECT_EQ(*iter, key);
        expected.insert(key);
        EXPECT_EQ(actual.size(), expected.size());
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }
    for (int i = -1; i <= 18; ++i) {
        const auto key = make_key<Key>(i);
        const auto first = actual.lower_bound(key);
        const auto last = actual.upper_bound(key);
        const auto expected_first = expected.lower_bound(key);
        const auto expected_last = expected.upper_bound(key);
        EXPECT_EQ(std::distance(actual.begin(), first),
                  std::distance(expected.begin(), expected_first));
        EXPECT_EQ(std::distance(actual.begin(), last),
                  std::distance(expected.begin(), expected_last));
        EXPECT_EQ(std::distance(first, last), expected.count(key));
    }
    int step = 0;
    while (!expected.empty()) {
        const auto offset = static_cast<std::ptrdiff_t>((step++ * 13) % expected.size());
        const auto next = actual.erase(std::next(actual.begin(), offset));
        const auto expected_next = expected.erase(std::next(expected.begin(), offset));
        EXPECT_EQ(actual.size(), expected.size());
        EXPECT_EQ(next == actual.end(), expected_next == expected.end());
        if (next != actual.end() && expected_next != expected.end()) {
            EXPECT_EQ(*next, *expected_next);
        }
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }
    EXPECT_TRUE(actual.empty());
    actual.emplace(make_key<Key>(5));
    actual.emplace(make_key<Key>(5));
    EXPECT_EQ(actual.size(), 2u);
    actual.clear();
    EXPECT_TRUE(actual.empty());
}

TEST(BtreeMultiset, InsertEmplaceRetainDuplicatesAndReturnPosition) {
    check_multiset<int, std::less<int>, 4>();
    check_multiset<int, std::greater<int>, 16>();
    check_multiset<int, std::less<int>, 32>();
    check_multiset<std::string, std::less<std::string>, 4>();
    check_multiset<std::string, std::greater<std::string>, 16>();
    check_multiset<std::string, std::less<std::string>, 32>();
}
} // namespace
