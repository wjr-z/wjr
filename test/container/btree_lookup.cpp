#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

#include <map>

#include <gtest/gtest.h>

namespace {
struct bucket_compare {
    bool descending;
    bool operator()(int lhs, int rhs) const noexcept {
        return descending ? lhs / 10 > rhs / 10 : lhs / 10 < rhs / 10;
    }
};

template <size_t NodeSize, typename Compare>
void check_lookup(const Compare &comp) {
    SCOPED_TRACE(NodeSize);
    for (int count : {0, 1, 2, 3, 80}) {
        SCOPED_TRACE(count);
        wjr::btree_map<int, int, Compare, std::allocator<char>, NodeSize> map(comp);
        wjr::btree_set<int, Compare, std::allocator<char>, NodeSize> set(comp);
        std::map<int, int, Compare> expected(comp);
        for (int i = 0; i != count; ++i) {
            map.emplace(i * 20 + 10, i);
            set.emplace(i * 20 + 10);
            expected.emplace(i * 20 + 10, i);
        }

        const auto &const_map = map;
        const auto &const_set = set;
        for (int key = -1; key <= count * 20 + 21; ++key) {
            SCOPED_TRACE(key);
            const auto iter = expected.find(key);
            const bool present = iter != expected.end();
            EXPECT_EQ(map.count(key), present ? 1u : 0u);
            EXPECT_EQ(const_map.count(key), present ? 1u : 0u);
            EXPECT_EQ(set.count(key), present ? 1u : 0u);
            EXPECT_EQ(const_set.count(key), present ? 1u : 0u);
            if (present) {
                EXPECT_EQ(map.at(key), iter->second);
                EXPECT_EQ(const_map.at(key), iter->second);
                EXPECT_EQ(&map.at(key), &const_map.at(key));
            } else {
#if !defined(WJR_DISABLE_EXCEPTIONS)
                EXPECT_THROW(map.at(key), std::out_of_range);
                EXPECT_THROW(const_map.at(key), std::out_of_range);
#endif
            }
        }
    }
}

template <size_t NodeSize>
void check_orders() {
    check_lookup<NodeSize>(std::less<int>{});
    check_lookup<NodeSize>(std::greater<int>{});
    check_lookup<NodeSize>(bucket_compare{false});
    check_lookup<NodeSize>(bucket_compare{true});
}

TEST(BtreeLookup, ExactAndEquivalentKeysAcrossStorageAndSearchModes) {
    check_orders<4>();
    check_orders<16>();
    check_orders<32>();
}
} // namespace
