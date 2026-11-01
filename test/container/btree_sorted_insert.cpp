#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

#include <algorithm>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <string>

#include <gtest/gtest.h>

namespace {
template <size_t N, typename Compare>
void check_string_edge() {
    wjr::btree_map<std::string, int, Compare, std::allocator<char>, N> values;
    std::map<std::string, int, Compare> reference;
    std::map<std::string, const int *> addresses;
    const bool reverse = Compare{}(std::string("b"), std::string("a"));
    for (int i = 0; i < 1500; ++i) {
        const int number = reverse ? 20000 - i : 10000 + i;
        const auto key = std::string(48, 'x') + std::to_string(number);
        auto [position, inserted] = values.try_emplace(key, number);
        EXPECT_TRUE(inserted);
        EXPECT_EQ(position->first, key);
        reference.emplace(key, number);
        addresses.emplace(key, &position->second);
    }
    EXPECT_TRUE(std::equal(values.begin(), values.end(), reference.begin(), reference.end()));
    for (const auto &[key, address] : addresses) {
        EXPECT_EQ(&values.at(key), address);
        EXPECT_EQ(values.find(key), values.lower_bound(key));
    }
    auto copied = values;
    EXPECT_EQ(copied, values);
    std::vector<std::string> keys;
    for (const auto &[key, value] : reference)
        keys.push_back(key);
    std::mt19937 random(73);
    std::shuffle(keys.begin(), keys.end(), random);
    for (const auto &key : keys)
        EXPECT_EQ(values.erase(key), reference.erase(key));
    EXPECT_TRUE(values.empty());
}

TEST(BtreeSortedInsert, IndirectKeysAndValueAddressesWithBothOrders) {
    check_string_edge<4, std::less<>>();
    check_string_edge<5, std::greater<>>();
    check_string_edge<16, std::less<>>();
    check_string_edge<16, std::greater<>>();
    check_string_edge<64, std::less<>>();
}

TEST(BtreeSortedInsert, EqualKeysCrossLeafRedistributionBoundaries) {
    wjr::btree_multiset<int> values;
    std::multiset<int> reference;
    for (int i = 0; i < 4096; ++i) {
        values.insert(i / 19);
        reference.insert(i / 19);
    }
    EXPECT_TRUE(std::equal(values.begin(), values.end(), reference.begin(), reference.end()));
    for (int key = 0; key < 216; ++key) {
        EXPECT_EQ(values.count(key), reference.count(key));
        EXPECT_EQ(values.erase(key), reference.erase(key));
    }
    EXPECT_TRUE(values.empty());
}
} // namespace
