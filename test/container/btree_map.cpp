#include "random.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/vector.hpp>

using namespace wjr;

// Test fixture for typed tests with different key-value types
template <typename TestType>
class BtreeMapTest {};

#define REGISTER_TYPES()                                                                           \
    using key_type = tp_at_t<TestType, 0>;                                                         \
    using value_type = tp_at_t<TestType, 1>

// Test: Default constructor creates empty map
TEMPLATE_TEST_CASE("BtreeMapTest - DefaultConstructor", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    btree_map<key_type, value_type> map;
    CHECK(map.empty());
    CHECK_EQ(map.size(), 0u);
    CHECK_EQ(map.begin(), map.end());
    CHECK_EQ(map.rbegin(), map.rend());
}

// Test: Range constructor with various sizes
TEMPLATE_TEST_CASE("BtreeMapTest - RangeConstructor", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 2, 3, 7, 15, 31, 63, 127, 255, 511, 1023}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());

        CHECK_EQ(map.size(), std_map.size());
        CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Copy constructor
TEMPLATE_TEST_CASE("BtreeMapTest - CopyConstructor", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 10, 50, 100, 500}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        btree_map<key_type, value_type> map_copy(map);

        CHECK_EQ(map.size(), map_copy.size());
        CHECK(std::equal(map.begin(), map.end(), map_copy.begin(), map_copy.end()));
    }
}

// Test: Move constructor
TEMPLATE_TEST_CASE("BtreeMapTest - MoveConstructor", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 10, 50, 100, 500}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());

        btree_map<key_type, value_type> map_moved(std::move(map));

        CHECK(map.empty());
        CHECK_EQ(map_moved.size(), std_map.size());
        CHECK(std::equal(map_moved.begin(), map_moved.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Copy assignment
TEMPLATE_TEST_CASE("BtreeMapTest - CopyAssignment", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 10, 50, 100, 500}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        btree_map<key_type, value_type> map2;

        map2 = map;

        CHECK_EQ(map.size(), map2.size());
        CHECK(std::equal(map.begin(), map.end(), map2.begin(), map2.end()));
    }
}

// Test: Move assignment
TEMPLATE_TEST_CASE("BtreeMapTest - MoveAssignment", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 10, 50, 100, 500}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());
        btree_map<key_type, value_type> map2;

        map2 = std::move(map);

        CHECK(map.empty());
        CHECK_EQ(map2.size(), std_map.size());
        CHECK(std::equal(map2.begin(), map2.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Emplace with pair
TEMPLATE_TEST_CASE("BtreeMapTest - EmplaceWithPair", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map;
        std::map<key_type, value_type> std_map;

        for (const auto &pr : vec) {
            auto [it1, inserted1] = map.emplace(pr);
            auto [it2, inserted2] = std_map.emplace(pr);

            CHECK_EQ(inserted1, inserted2) /* Emplace insertion result mismatch */;
        }

        CHECK_EQ(map.size(), std_map.size());
        CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Emplace with key and value
TEMPLATE_TEST_CASE("BtreeMapTest - EmplaceWithKeyValue", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map;
        std::map<key_type, value_type> std_map;

        for (const auto &pr : vec) {
            auto [it1, inserted1] = map.emplace(pr.first, pr.second);
            auto [it2, inserted2] = std_map.emplace(pr.first, pr.second);

            CHECK_EQ(inserted1, inserted2) /* Emplace insertion result mismatch */;
        }

        CHECK_EQ(map.size(), std_map.size());
        CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Insert duplicate keys (should not increase size)
TEMPLATE_TEST_CASE("BtreeMapTest - InsertDuplicates", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    btree_map<key_type, value_type> map;

    key_type key = trandom<key_type>();
    value_type val1 = trandom<value_type>();
    value_type val2 = trandom<value_type>();

    auto [it1, inserted1] = map.emplace(key, val1);
    CHECK(inserted1);
    CHECK_EQ(map.size(), 1u);

    auto [it2, inserted2] = map.emplace(key, val2);
    CHECK_FALSE(inserted2);
    CHECK_EQ(map.size(), 1u);
    CHECK_EQ(it1, it2);
}

// Test: lower_bound
TEMPLATE_TEST_CASE("BtreeMapTest - LowerBound", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());

        for (const auto &[key, value] : vec) {
            auto it1 = map.lower_bound(key);
            auto it2 = std_map.lower_bound(key);

            if (it1 == map.end()) {
                CHECK_EQ(it2, std_map.end());
            } else {
                CHECK_NE(it2, std_map.end());
                CHECK_EQ(*it1, *it2);
            }
        }
    }
}

// Test: upper_bound
TEMPLATE_TEST_CASE("BtreeMapTest - UpperBound", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());

        for (const auto &[key, value] : vec) {
            auto it1 = map.upper_bound(key);
            auto it2 = std_map.upper_bound(key);

            if (it1 == map.end()) {
                CHECK_EQ(it2, std_map.end());
            } else {
                CHECK_NE(it2, std_map.end());
                CHECK_EQ(*it1, *it2);
            }
        }
    }
}

// Test: Lookup with lower_bound and upper_bound
TEMPLATE_TEST_CASE("BtreeMapTest - Lookup", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 10, 50, 100}) {
        btree_map<key_type, value_type> map;
        std::map<key_type, value_type> std_map;
        vector<key_type> keys;

        // Insert elements
        for (int i = 0; i < n; ++i) {
            key_type key = trandom<key_type>();
            value_type val = trandom<value_type>();
            map.emplace(key, val);
            std_map.emplace(key, val);
            keys.push_back(key);
        }

        // Lookup existing keys using lower_bound
        for (const auto &key : keys) {
            auto it1 = map.lower_bound(key);
            auto it2 = std_map.lower_bound(key);

            if (it1 == map.end()) {
                CHECK_EQ(it2, std_map.end());
            } else {
                CHECK_NE(it2, std_map.end());
                CHECK_EQ(it1->first, it2->first);
                CHECK_EQ(it1->second, it2->second);
            }
        }
    }
}

// Test: Erase by iterator
TEMPLATE_TEST_CASE("BtreeMapTest - EraseByIterator", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {1, 3, 7, 15, 31, 63, 127, 255, 511}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());

        // Erase first half of elements
        int to_erase = n / 2;
        for (int i = 0; i < to_erase; ++i) {
            auto it1 = map.begin();
            auto it2 = std_map.begin();

            CHECK_EQ(it1->first, it2->first);

            map.erase(it1);
            std_map.erase(it2);
        }

        CHECK_EQ(map.size(), std_map.size());
        CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Erase by finding key first
TEMPLATE_TEST_CASE("BtreeMapTest - EraseByKey", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {10, 50, 100, 500}) {
        btree_map<key_type, value_type> map;
        std::map<key_type, value_type> std_map;
        vector<key_type> keys;

        // Insert elements
        for (int i = 0; i < n; ++i) {
            key_type key = trandom<key_type>();
            value_type val = trandom<value_type>();
            map.emplace(key, val);
            std_map.emplace(key, val);
            keys.push_back(key);
        }

        // Erase half by finding and erasing
        int to_erase = keys.size() / 2;
        for (int i = 0; i < to_erase; ++i) {
            auto it1 = map.lower_bound(keys[i]);
            if (it1 != map.end() && it1->first == keys[i]) {
                map.erase(it1);
            }
            std_map.erase(keys[i]);
        }

        CHECK_EQ(map.size(), std_map.size());
        CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
    }
}

// Test: Erase with node_size=4 (critical for reproducing past bugs)
TEMPLATE_TEST_CASE("BtreeMapTest - EraseWithSmallNodeSize", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    btree_map<key_type, value_type, std::less<>, std::allocator<char>, 4> map;
    std::map<key_type, value_type> std_map;

    // Insert many elements
    for (int i = 0; i < 1000; ++i) {
        key_type key = trandom<key_type>();
        value_type val = trandom<value_type>();
        map.emplace(key, val);
        std_map.emplace(key, val);
    }

    // Erase many elements randomly
    vector<key_type> keys;
    for (const auto &[key, val] : std_map) {
        keys.push_back(key);
    }

    // Shuffle and erase first half
    std::shuffle(keys.begin(), keys.end(), __mt_rand);
    size_t to_erase = keys.size() / 2;

    for (size_t i = 0; i < to_erase; ++i) {
        auto it = map.lower_bound(keys[i]);
        if (it != map.end() && it->first == keys[i]) {
            map.erase(it);
            std_map.erase(keys[i]);
        }
    }

    CHECK_EQ(map.size(), std_map.size());
    CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
}

// Test: Clear
TEMPLATE_TEST_CASE("BtreeMapTest - Clear", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 10, 50, 100}) {
        std::map<key_type, value_type> std_map;
        for (int i = 0; i < n; ++i) {
            std_map.emplace(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(std_map.begin(), std_map.end());
        CHECK_EQ(map.size(), std_map.size());

        map.clear();

        CHECK(map.empty());
        CHECK_EQ(map.size(), 0u);
        CHECK_EQ(map.begin(), map.end());
    }
}

// Test: Swap
TEMPLATE_TEST_CASE("BtreeMapTest - Swap", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 3, 7, 15, 31, 63, 127, 255}) {
        vector<std::pair<key_type, value_type>> vec;
        for (int i = 0; i < n; ++i) {
            vec.emplace_back(trandom<key_type>(), trandom<value_type>());
        }

        btree_map<key_type, value_type> map(vec.begin(), vec.end());
        std::map<key_type, value_type> std_map(vec.begin(), vec.end());

        for (int m : {0, 1, 3, 7, 15, 31, 63, 127, 255}) {
            vector<std::pair<key_type, value_type>> vec2;
            for (int i = 0; i < m; ++i) {
                vec2.emplace_back(trandom<key_type>(), trandom<value_type>());
            }

            btree_map<key_type, value_type> map2(vec2.begin(), vec2.end());
            std::map<key_type, value_type> std_map2(vec2.begin(), vec2.end());

            // Swap
            map.swap(map2);
            CHECK(std::equal(map.begin(), map.end(), std_map2.begin(), std_map2.end()));
            CHECK(std::equal(map2.begin(), map2.end(), std_map.begin(), std_map.end()));

            // Swap back
            map.swap(map2);
            CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
            CHECK(std::equal(map2.begin(), map2.end(), std_map2.begin(), std_map2.end()));
        }
    }
}

// Test: Iterator increment and decrement
TEMPLATE_TEST_CASE("BtreeMapTest - IteratorTraversal", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    for (int n : {0, 1, 10, 50, 100}) {
        btree_map<key_type, value_type> map;
        std::map<key_type, value_type> std_map;

        // Insert elements to both maps
        for (int i = 0; i < n; ++i) {
            key_type key = trandom<key_type>();
            value_type val = trandom<value_type>();
            map.emplace(key, val);
            std_map.emplace(key, val);
        }

        // Verify sizes match
        REQUIRE_EQ(map.size(), std_map.size());

        // Forward iteration
        auto it1 = map.begin();
        auto it2 = std_map.begin();
        while (it1 != map.end() && it2 != std_map.end()) {
            CHECK_EQ(it1->first, it2->first) /* Forward iteration key mismatch */;
            ++it1;
            ++it2;
        }
        CHECK_EQ(it1, map.end());
        CHECK_EQ(it2, std_map.end());

        // Reverse iteration
        auto rit1 = map.rbegin();
        auto rit2 = std_map.rbegin();
        while (rit1 != map.rend() && rit2 != std_map.rend()) {
            CHECK_EQ(rit1->first, rit2->first) /* Reverse iteration key mismatch */;
            ++rit1;
            ++rit2;
        }
        CHECK_EQ(rit1, map.rend());
        CHECK_EQ(rit2, std_map.rend());
    }
}

// Test: const iterator
TEMPLATE_TEST_CASE("BtreeMapTest - ConstIterator", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    btree_map<key_type, value_type> map;
    std::map<key_type, value_type> std_map;

    for (int i = 0; i < 50; ++i) {
        key_type key = trandom<key_type>();
        value_type val = trandom<value_type>();
        map.emplace(key, val);
        std_map.emplace(key, val);
    }

    const btree_map<key_type, value_type> &cmap = map;
    const std::map<key_type, value_type> &cstd_map = std_map;

    CHECK_EQ(cmap.size(), cstd_map.size());
    CHECK(std::equal(cmap.begin(), cmap.end(), cstd_map.begin(), cstd_map.end()));
    CHECK(std::equal(cmap.cbegin(), cmap.cend(), cstd_map.cbegin(), cstd_map.cend()));
}

// Test: Random operations stress test
TEMPLATE_TEST_CASE("BtreeMapTest - RandomOperationsStressTest", "", (tp_list<int, int>),
                   (tp_list<std::string, std::string>)) {
    REGISTER_TYPES();

    btree_map<key_type, value_type> map;
    std::map<key_type, value_type> std_map;

    // Random insertions and deletions
    for (int i = 0; i < 500; ++i) {
        if (__mt_rand() % 2 == 0 || map.empty()) {
            // Insert
            key_type key = trandom<key_type>();
            value_type val = trandom<value_type>();
            map.emplace(key, val);
            std_map.emplace(key, val);
        } else {
            // Delete
            auto it = map.begin();
            std::advance(it, __mt_rand() % map.size());
            key_type key = it->first;
            map.erase(it);
            std_map.erase(key);
        }

        // Verify every 50 operations
        if (i % 50 == 49) {
            CHECK_EQ(map.size(), std_map.size());
            CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
        }
    }

    CHECK_EQ(map.size(), std_map.size());
    CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
}

// Test: Different node sizes
TEST_CASE("BtreeMapNodeSizeTest - DifferentNodeSizes") {
    // Test with node_size = 4
    {
        btree_map<int, int, std::less<>, std::allocator<char>, 4> map;
        for (int i = 0; i < 100; ++i) {
            map.emplace(i, i * 2);
        }
        CHECK_EQ(map.size(), 100u);

        for (int i = 0; i < 50; ++i) {
            auto it = map.lower_bound(i);
            if (it != map.end())
                map.erase(it);
        }
        CHECK_EQ(map.size(), 50u);
    }

    // Test with node_size = 8
    {
        btree_map<int, int, std::less<>, std::allocator<char>, 8> map;
        for (int i = 0; i < 100; ++i) {
            map.emplace(i, i * 2);
        }
        CHECK_EQ(map.size(), 100u);

        for (int i = 0; i < 50; ++i) {
            auto it = map.lower_bound(i);
            if (it != map.end())
                map.erase(it);
        }
        CHECK_EQ(map.size(), 50u);
    }

    // Test with node_size = 16
    {
        btree_map<int, int, std::less<>, std::allocator<char>, 16> map;
        for (int i = 0; i < 100; ++i) {
            map.emplace(i, i * 2);
        }
        CHECK_EQ(map.size(), 100u);

        for (int i = 0; i < 50; ++i) {
            auto it = map.lower_bound(i);
            if (it != map.end())
                map.erase(it);
        }
        CHECK_EQ(map.size(), 50u);
    }
}

// Test: Large scale operations
TEST_CASE("BtreeMapScaleTest - LargeScale") {
    btree_map<int, std::string> map;
    std::map<int, std::string> std_map;

    // Insert 10000 elements
    for (int i = 0; i < 10000; ++i) {
        std::string val = "value_" + std::to_string(i);
        map.emplace(i, val);
        std_map.emplace(i, val);
    }

    CHECK_EQ(map.size(), 10000u);
    CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));

    // Erase 5000 random elements
    vector<int> keys;
    for (int i = 0; i < 10000; ++i) {
        keys.push_back(i);
    }
    std::shuffle(keys.begin(), keys.end(), __mt_rand);

    for (int i = 0; i < 5000; ++i) {
        auto it = map.lower_bound(keys[i]);
        if (it != map.end() && it->first == keys[i]) {
            map.erase(it);
        }
        std_map.erase(keys[i]);
    }

    CHECK_EQ(map.size(), 5000u);
    CHECK(std::equal(map.begin(), map.end(), std_map.begin(), std_map.end()));
}