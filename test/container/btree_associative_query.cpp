#include <algorithm>
#include <map>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

namespace {
template <typename T>
const auto &key_of(const T &value) {
    if constexpr (requires { value.first; })
        return value.first;
    else
        return value;
}

template <typename Tree>
void insert_key(Tree &tree, int key) {
    if constexpr (requires { tree.try_emplace(key, key * 7); })
        tree.try_emplace(key, key * 7);
    else
        tree.emplace(key);
}

template <typename Tree, typename Reference>
void check_contents(const Tree &tree, const Reference &reference) {
    ASSERT_EQ(tree.size(), reference.size());
    auto position = tree.begin();
    for (const auto &value : reference) {
        ASSERT_NE(position, tree.end());
        EXPECT_EQ(key_of(*position), key_of(value));
        ++position;
    }
    EXPECT_EQ(position, tree.end());
}

struct grouped_order {
    int width = 10;
    bool descending = true;
    bool operator()(int lhs, int rhs) const noexcept {
        return descending ? lhs / width > rhs / width : lhs / width < rhs / width;
    }
};

template <typename Tree, typename Reference>
void check_queries(Tree &tree, Reference &reference) {
    static_assert(std::same_as<decltype(tree.find(1)), typename Tree::iterator>);
    static_assert(
        std::same_as<decltype(std::as_const(tree).find(1)), typename Tree::const_iterator>);
    for (int key = 0; key != 240; ++key) {
        const auto &constant = tree;
        const auto actual = tree.find(key);
        const auto expected = reference.find(key);
        ASSERT_EQ(actual == tree.end(), expected == reference.end());
        EXPECT_EQ(constant.find(key), actual);
        EXPECT_EQ(constant.contains(key), reference.contains(key));
        EXPECT_EQ(constant.count(key), reference.count(key));
        if (actual != tree.end())
            EXPECT_EQ(key_of(*actual), key_of(*expected));
        const auto [first, last] = tree.equal_range(key);
        const auto [ref_first, ref_last] = reference.equal_range(key);
        const auto constant_range = constant.equal_range(key);
        EXPECT_EQ(first, constant_range.first);
        EXPECT_EQ(last, constant_range.second);
        EXPECT_EQ(std::distance(first, last), std::distance(ref_first, ref_last));
        ASSERT_EQ(first == tree.end(), ref_first == reference.end());
        ASSERT_EQ(last == tree.end(), ref_last == reference.end());
        if (first != tree.end())
            EXPECT_EQ(key_of(*first), key_of(*ref_first));
        if (last != tree.end())
            EXPECT_EQ(key_of(*last), key_of(*ref_last));
    }
    check_contents(tree, reference);
    EXPECT_TRUE(std::is_sorted(tree.begin(), tree.end(), tree.value_comp()));
}

template <typename Tree, typename Reference>
void ordered_queries_and_key_erase() {
    Tree tree(grouped_order{3, true});
    Reference reference(grouped_order{3, true});
    check_queries(tree, reference);
    for (int key : {30, 5, 7, 1, 80, 81, 83, 90, 11, 200, 201}) {
        insert_key(tree, key);
        insert_key(reference, key);
    }
    check_queries(tree, reference);
    for (int key : {82, 0, 31, 80, 5, 201, 11, 90}) {
        EXPECT_EQ(tree.erase(key), reference.erase(key));
        check_queries(tree, reference);
    }
}

TEST(BtreeAssociativeQuery, ComparatorEquivalenceConstnessAndAllAdapters) {
    ordered_queries_and_key_erase<wjr::btree_map<int, int, grouped_order>,
                                  std::map<int, int, grouped_order>>();
    ordered_queries_and_key_erase<wjr::experimental::compact_btree_map<int, int, grouped_order>,
                                  std::map<int, int, grouped_order>>();
    ordered_queries_and_key_erase<wjr::btree_set<int, grouped_order>,
                                  std::set<int, grouped_order>>();
    ordered_queries_and_key_erase<wjr::btree_multiset<int, grouped_order>,
                                  std::multiset<int, grouped_order>>();
}

template <typename Tree, typename Reference>
void range_erase() {
    std::mt19937 random(81);
    for (int size : {0, 1, 2, 3, 4, 5, 16, 63, 200}) {
        for (int trial = 0; trial != 12; ++trial) {
            Tree tree;
            Reference reference;
            for (int i = 0; i != size; ++i) {
                const int key = random() % 500;
                insert_key(tree, key);
                insert_key(reference, key);
            }
            if (trial == 0) {
                const auto after_clear = tree.erase(tree.cbegin(), tree.cend());
                EXPECT_EQ(after_clear, tree.end());
                reference.clear();
                check_contents(tree, reference);
            }
            while (!tree.empty()) {
                const auto start = random() % (tree.size() + 1);
                const auto length = random() % (tree.size() - start + 1);
                auto first = tree.cbegin(), last = tree.cbegin();
                std::advance(first, start);
                std::advance(last, start + length);
                auto ref_first = reference.cbegin(), ref_last = reference.cbegin();
                std::advance(ref_first, start);
                std::advance(ref_last, start + length);
                const auto next = tree.erase(first, last);
                const auto expected = reference.erase(ref_first, ref_last);
                ASSERT_EQ(next == tree.end(), expected == reference.end());
                if (next != tree.end())
                    EXPECT_EQ(key_of(*next), key_of(*expected));
                check_contents(tree, reference);
                if (length == 0 && !tree.empty()) {
                    EXPECT_EQ(tree.erase(key_of(*tree.begin())),
                              reference.erase(key_of(*reference.begin())));
                }
            }
            const auto after_empty = tree.erase(tree.cbegin(), tree.cend());
            EXPECT_EQ(after_empty, tree.end());
        }
    }
}

TEST(BtreeAssociativeQuery, RangeEraseSurvivesLeafAndEndpointInvalidation) {
    range_erase<wjr::btree_map<int, int, std::less<>, std::allocator<char>, 4>,
                std::map<int, int>>();
    range_erase<wjr::btree_set<int>, std::set<int>>();
    range_erase<wjr::btree_multiset<int, std::less<>, std::allocator<char>, 5>,
                std::multiset<int>>();
    range_erase<wjr::experimental::compact_btree_map<int, int>, std::map<int, int>>();
}

template <typename Tree>
concept view_lookup = requires(Tree &tree, const Tree &constant, std::string_view query) {
    tree.find(query);
    constant.find(query);
    constant.contains(query);
    tree.equal_range(query);
    constant.equal_range(query);
    tree.erase(query);
};
static_assert(view_lookup<wjr::btree_map<std::string, int, std::less<>>>);
static_assert(!view_lookup<wjr::btree_map<std::string, int, std::less<std::string>>>);
static_assert(view_lookup<wjr::btree_set<std::string, std::less<>>>);
static_assert(view_lookup<wjr::btree_multiset<std::string, std::less<>>>);

TEST(BtreeAssociativeQuery, TransparentLookupAndAliasedKeyErase) {
    wjr::btree_map<std::string, int, std::less<>> map;
    map.emplace("alpha", 1);
    map.emplace("beta", 2);
    map.emplace("gamma", 3);
    const auto &constant = map;
    EXPECT_EQ(constant.find(std::string_view("beta"))->second, 2);
    EXPECT_TRUE(constant.contains(std::string_view("gamma")));
    EXPECT_FALSE(constant.contains(std::string_view("delta")));
    EXPECT_EQ(map.equal_range(std::string_view("aardvark")).first, map.begin());
    EXPECT_EQ(map.erase(std::string_view("beta")), 1u);
    EXPECT_EQ(map.erase(map.begin()->first), 1u);
    EXPECT_EQ(map.erase(std::string_view("missing")), 0u);
    wjr::btree_multiset<std::string, std::less<>, std::allocator<char>, 4> values;
    for (int i = 0; i != 70; ++i)
        values.emplace(i % 3 == 0 ? "alpha" : "beta");
    EXPECT_EQ(values.count(std::string_view("alpha")), 24u);
    EXPECT_EQ(values.erase(*values.begin()), 24u);
    EXPECT_EQ(values.erase(std::string_view("beta")), 46u);
    EXPECT_TRUE(values.empty());
}

struct broad_compare {
    using is_transparent = void;
    template <typename L, typename R>
    bool operator()(const L &, const R &) const noexcept {
        return false;
    }
};
using broad_tree = wjr::btree_set<int, broad_compare>;
static_assert(
    std::same_as<decltype(std::declval<broad_tree &>().erase(std::declval<broad_tree::iterator>())),
                 broad_tree::iterator>);
static_assert(std::same_as<wjr::btree_set<int, grouped_order>::value_compare, grouped_order>);

TEST(BtreeAssociativeQuery, ValueComparatorRetainsStateAndIgnoresMappedValues) {
    wjr::btree_map<int, int, grouped_order> values(grouped_order{10, true});
    auto compare = values.value_comp();
    EXPECT_FALSE(compare({21, 1}, {29, 100}));
    EXPECT_TRUE(compare({31, -100}, {20, 100}));
    values.key_comp().descending = false;
    EXPECT_TRUE(compare({31, -100}, {20, 100}));
    EXPECT_FALSE(values.value_comp()({31, -100}, {20, 100}));
    static_assert(sizeof(wjr::btree_map<int, int>) == 48);
}

struct live_mapped {
    static inline int live = 0;
    int value;
    explicit live_mapped(int n) noexcept : value(n) { ++live; }
    live_mapped(live_mapped &&other) noexcept : value(other.value) { ++live; }
    ~live_mapped() noexcept { --live; }
};

TEST(BtreeAssociativeQuery, CompactRangeDeletionDestroysOnlyRemovedMappedValues) {
    using tree = wjr::experimental::compact_btree_map<int, live_mapped, std::less<int>,
                                                      std::allocator<char>, 4>;
    {
        tree values;
        for (int i = 0; i != 200; ++i)
            values.try_emplace(i, i);
        auto next = values.erase(std::next(values.cbegin(), 3), std::next(values.cbegin(), 193));
        ASSERT_NE(next, values.end());
        EXPECT_EQ(next->first, 193);
        EXPECT_EQ(values.size(), 10u);
        EXPECT_EQ(live_mapped::live, 10);
        EXPECT_EQ(values.erase(values.begin()->first), 1u);
        EXPECT_EQ(live_mapped::live, 9);
        for (const auto &[key, value] : values)
            EXPECT_EQ(key, value.value);
        const auto after_clear = values.erase(values.cbegin(), values.cend());
        EXPECT_EQ(after_clear, values.end());
        EXPECT_EQ(live_mapped::live, 0);
    }
    EXPECT_EQ(live_mapped::live, 0);
}
} // namespace
