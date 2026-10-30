#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

namespace {
template <class Value>
const auto &key_of(const Value &value) {
    if constexpr (requires { value.first; })
        return value.first;
    else
        return value;
}
template <class Tree>
auto hinted(Tree &tree, typename Tree::const_iterator hint, int key) {
    if constexpr (requires(typename Tree::value_type value) { value.first; })
        return tree.emplace_hint(hint, key, key * 7);
    else
        return tree.emplace_hint(hint, key);
}
template <class Tree, class Reference>
void same_contents(const Tree &tree, const Reference &reference) {
    ASSERT_EQ(tree.size(), reference.size());
    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), reference.begin()));
    for (const auto &value : reference) {
        auto actual = tree.find(key_of(value));
        ASSERT_NE(actual, tree.end());
        EXPECT_EQ(key_of(*actual), key_of(*reference.find(key_of(value))));
    }
}

struct group_order {
    int width = 1;
    bool reverse = false;
    bool operator()(int lhs, int rhs) const noexcept {
        return reverse ? lhs / width > rhs / width : lhs / width < rhs / width;
    }
};
template <class Tree, class Reference>
void varied_hints(group_order order) {
    Tree tree(order);
    Reference reference(order);
    std::mt19937 random(191);
    for (int i = 0; i != 650; ++i) {
        const int key = random() % 900;
        auto actual = tree.cbegin();
        auto expected = reference.cbegin();
        const int policy = i % 5;
        if (policy == 0) {
            actual = tree.lower_bound(key);
            expected = reference.lower_bound(key);
        } else if (policy == 1) {
            actual = tree.lower_bound(key);
            expected = reference.lower_bound(key);
            if (actual != tree.cbegin()) {
                --actual;
                --expected;
            }
        } else {
            const size_t offset = policy == 2   ? tree.size()
                                  : policy == 3 ? 0
                                                : random() % (tree.size() + 1);
            std::advance(actual, offset);
            std::advance(expected, offset);
        }
        const auto result = hinted(tree, actual, key);
        const auto standard = hinted(reference, expected, key);
        EXPECT_EQ(key_of(*result), key_of(*standard));
        if (i % 37 == 0)
            same_contents(tree, reference);
    }
    same_contents(tree, reference);
}
TEST(BtreeHints, ValidAdjacentAndWrongHintsMatchStandardContainers) {
    for (group_order order : {group_order{1, false}, {1, true}, {10, false}, {10, true}}) {
        varied_hints<wjr::btree_map<int, int, group_order, std::allocator<char>, 4>,
                     std::map<int, int, group_order>>(order);
        varied_hints<wjr::btree_set<int, group_order, std::allocator<char>, 5>,
                     std::set<int, group_order>>(order);
        varied_hints<wjr::btree_multiset<int, group_order, std::allocator<char>, 4>,
                     std::multiset<int, group_order>>(order);
        varied_hints<wjr::experimental::compact_btree_map<int, int, group_order>,
                     std::map<int, int, group_order>>(order);
    }
}

TEST(BtreeHints, CrossLeafInsertionPreservesIndirectSeparatorLifetimes) {
    wjr::btree_set<std::string, std::less<>, std::allocator<char>, 4> tree;
    std::set<std::string, std::less<>> reference;
    for (int i = 0; i != 150; ++i) {
        const auto key = std::to_string(100000 + i * 10);
        tree.emplace(key);
        reference.emplace(key);
    }
    for (int i = 0; i != 150; ++i) {
        const auto key = std::to_string(100000 + i * 10 - 1);
        tree.emplace_hint(tree.lower_bound(key), key);
        reference.emplace(key);
    }
    for (int i = 0; i != 150; ++i) {
        const auto key = std::to_string(100000 + i * 10);
        EXPECT_EQ(tree.erase(key), reference.erase(key));
        if (i % 19 == 0)
            same_contents(tree, reference);
    }
    same_contents(tree, reference);
}

struct counted_less {
    size_t *calls;
    bool operator()(int lhs, int rhs) const noexcept {
        ++*calls;
        return lhs < rhs;
    }
};
template <class Tree>
void linear_hint_comparisons() {
    size_t calls = 0;
    Tree tree(counted_less{&calls});
    auto hint = tree.cend();
    for (int i = 0; i != 4096; ++i)
        hint = hinted(tree, hint, i);
    EXPECT_LE(calls, 3u * tree.size());
}
TEST(BtreeHints, CorrectHintsAndSortedRangesAvoidRepeatedRootSearch) {
    linear_hint_comparisons<wjr::btree_set<int, counted_less>>();
    linear_hint_comparisons<wjr::btree_map<int, int, counted_less>>();
    linear_hint_comparisons<wjr::experimental::compact_btree_map<int, int, counted_less>>();
    std::vector<std::pair<int, int>> input;
    for (int i = 0; i != 4096; ++i)
        input.emplace_back(i, i * 7);
    size_t calls = 0;
    wjr::btree_map<int, int, counted_less> values(counted_less{&calls});
    values.insert_with_hint(input.begin(), input.end());
    EXPECT_LE(calls, 3u * values.size());
    calls = 0;
    values.insert_with_hint(input.begin(), input.end());
    EXPECT_LE(calls, 4u * values.size());
}

TEST(BtreeHints, MultiRangePreservesInputAndExistingEquivalentOrder) {
    using tree = wjr::btree_multiset<int, group_order, std::allocator<char>, 4>;
    const group_order order{10, false};
    tree values(order);
    std::multiset<int, group_order> reference(order);
    const int first[] = {11, 19, 20, 31, 39, 40, 12, 18};
    values.insert_with_hint(std::begin(first), std::end(first));
    reference.insert(std::begin(first), std::end(first));
    const int second[] = {1, 17, 16, 35, 34, 22, 15, 14, 2, 3, 13};
    values.insert_with_hint(std::begin(second), std::end(second));
    reference.insert(std::begin(second), std::end(second));
    same_contents(values, reference);
    values = {11, 19, 12, 18, 31, 39, 32, 38};
    reference = {11, 19, 12, 18, 31, 39, 32, 38};
    same_contents(values, reference);
}

struct lazy_value {
    static inline int constructions = 0;
    int value;
    explicit lazy_value(int n) noexcept : value(n) { ++constructions; }
    lazy_value &operator=(int n) noexcept {
        value = n;
        return *this;
    }
};
template <class Tree>
void hinted_updates() {
    Tree values;
    lazy_value::constructions = 0;
    auto first = values.try_emplace(values.cend(), 1, 10);
    EXPECT_EQ(first->second.value, 10);
    const int before = lazy_value::constructions;
    auto duplicate = values.try_emplace(values.cbegin(), 1, 99);
    EXPECT_EQ(first, duplicate);
    EXPECT_EQ(lazy_value::constructions, before);
    auto assigned = values.insert_or_assign(values.cend(), 1, 20);
    EXPECT_EQ(assigned->second.value, 20);
    EXPECT_EQ(lazy_value::constructions, before);
    auto inserted = values.insert_or_assign(values.cbegin(), 2, 30);
    EXPECT_EQ(inserted->second.value, 30);
    EXPECT_EQ(values.size(), 2u);
}
TEST(BtreeHints, HintedTryEmplaceAndUpsertRetainLazyConstruction) {
    hinted_updates<wjr::btree_map<int, lazy_value>>();
    hinted_updates<wjr::experimental::compact_btree_map<int, lazy_value>>();
    wjr::btree_map<std::string, std::unique_ptr<int>> values;
    const std::string text(128, 'x');
    values.try_emplace(values.end(), text, std::make_unique<int>(7));
    std::string key = text;
    auto source = std::make_unique<int>(9);
    values.try_emplace(values.begin(), std::move(key), std::move(source));
    EXPECT_EQ(key, text);
    ASSERT_TRUE(source);
    values.insert_or_assign(values.begin(), std::move(key), std::move(source));
    EXPECT_EQ(key, text);
    EXPECT_FALSE(source);
    EXPECT_EQ(*values.begin()->second, 9);
}

struct tracked_value {
    static inline int live = 0;
    const tracked_value *self = this;
    int value;
    explicit tracked_value(int n) noexcept : value(n) { ++live; }
    tracked_value(const tracked_value &other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++live;
    }
    tracked_value(tracked_value &&other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++live;
    }
    ~tracked_value() noexcept {
        EXPECT_EQ(self, this);
        --live;
    }
};
TEST(BtreeHints, CompactHintsStageAliasedValuesAndPreserveLifetime) {
    {
        wjr::experimental::compact_btree_map<int, tracked_value, std::less<int>,
                                             std::allocator<char>, 4>
            values;
        for (int i = 0; i != 200; ++i)
            values.try_emplace(values.cend(), i * 2, i);
        const auto &source = values.begin()->second;
        const int expected = source.value;
        auto inserted = values.emplace_hint(values.cbegin(), -1, source);
        EXPECT_EQ(inserted->second.value, expected);
        auto duplicate = values.emplace_hint(values.cend(), std::piecewise_construct,
                                             std::forward_as_tuple(-1), std::forward_as_tuple(99));
        EXPECT_EQ(duplicate->second.value, expected);
        EXPECT_EQ(tracked_value::live, values.size());
        while (!values.empty())
            values.erase(values.begin());
    }
    EXPECT_EQ(tracked_value::live, 0);
    wjr::btree_set<int> unique;
    const int value = 5;
    EXPECT_EQ(*unique.insert(unique.cend(), value), 5);
    EXPECT_EQ(*unique.insert(unique.cbegin(), 7), 7);
    wjr::btree_multiset<int> repeated;
    repeated.insert(repeated.cend(), value);
    repeated.insert(repeated.cbegin(), 5);
    EXPECT_EQ(repeated.size(), 2u);
}
} // namespace
