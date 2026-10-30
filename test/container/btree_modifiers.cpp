#include <array>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

namespace {
template <typename Tree>
void upsert() {
    Tree values;
    std::map<int, int> reference;
    for (int i = 0; i != 500; ++i) {
        const int key = i * 37 % 137;
        const int source = i * 3;
        auto [position, inserted] = values.insert_or_assign(key, source);
        const auto expected = reference.insert_or_assign(key, source);
        EXPECT_EQ(inserted, expected.second);
        EXPECT_EQ(position->first, expected.first->first);
        EXPECT_EQ(position->second, expected.first->second);
    }
    for (int i = 0; i != 200; ++i)
        EXPECT_EQ(values[int(i)], reference[int(i)]);
    ASSERT_EQ(values.size(), reference.size());
    EXPECT_TRUE(std::equal(values.begin(), values.end(), reference.begin()));
    const int &aliased = values.begin()->second;
    const int expected = aliased;
    values.insert_or_assign(1000, aliased);
    EXPECT_EQ(values.at(1000), expected);
}
TEST(BtreeModifiers, UpsertAndRvalueIndexMatchMap) {
    upsert<wjr::btree_map<int, int>>();
    upsert<wjr::experimental::compact_btree_map<int, int>>();
}

struct key_counter {
    static inline int copies = 0;
    static inline int moves = 0;
    int value = 0;
    explicit key_counter(int n) noexcept : value(n) {}
    key_counter(const key_counter &other) noexcept : value(other.value) { ++copies; }
    key_counter(key_counter &&other) noexcept : value(other.value) {
        ++moves;
        other.value = -1;
    }
    friend bool operator<(const key_counter &lhs, const key_counter &rhs) noexcept {
        return lhs.value < rhs.value;
    }
};
TEST(BtreeModifiers, MovedKeysAndMoveOnlyMappedSourcesAreConsumedOnlyWhenNeeded) {
    wjr::btree_map<key_counter, int> values;
    key_counter first(1);
    key_counter::copies = key_counter::moves = 0;
    values[std::move(first)] = 7;
    EXPECT_EQ(first.value, -1);
    EXPECT_EQ(key_counter::copies, 0);
    EXPECT_EQ(key_counter::moves, 1);
    key_counter existing(1);
    values.insert_or_assign(std::move(existing), 8);
    EXPECT_EQ(existing.value, 1);
    EXPECT_EQ(key_counter::moves, 1);
    EXPECT_EQ(values.at(existing), 8);
    wjr::experimental::compact_btree_map<int, std::unique_ptr<int>> compact;
    auto first_value = std::make_unique<int>(7);
    auto inserted = compact.insert_or_assign(1, std::move(first_value));
    EXPECT_TRUE(inserted.second);
    EXPECT_FALSE(first_value);
    auto second_value = std::make_unique<int>(9);
    auto assigned = compact.insert_or_assign(1, std::move(second_value));
    EXPECT_FALSE(assigned.second);
    EXPECT_FALSE(second_value);
    ASSERT_TRUE(assigned.first->second);
    EXPECT_EQ(*assigned.first->second, 9);
}

struct explicit_source {
    int value;
};
struct unusual_assignment {
    int value;
    explicit unusual_assignment(explicit_source source) noexcept : value(source.value) {}
    void operator=(explicit_source source) noexcept { value = source.value; }
};
template <typename Tree, typename Source>
concept can_upsert = requires(Tree &tree, Source &&source) {
    tree.insert_or_assign(1, std::forward<Source>(source));
};
static_assert(!std::assignable_from<unusual_assignment &, explicit_source>);
static_assert(can_upsert<wjr::btree_map<int, unusual_assignment>, explicit_source>);
static_assert(!can_upsert<wjr::btree_map<int, int>, std::string>);
TEST(BtreeModifiers, UpsertDoesNotRequireCommonReferenceOrAssignmentReturnType) {
    wjr::btree_map<int, unusual_assignment> values;
    EXPECT_TRUE(values.insert_or_assign(1, explicit_source{3}).second);
    EXPECT_FALSE(values.insert_or_assign(1, explicit_source{5}).second);
    EXPECT_EQ(values.at(1).value, 5);
}

struct throwing_assignment {
    int value;
    explicit throwing_assignment(int n) noexcept : value(n) {}
    throwing_assignment &operator=(int) { throw std::runtime_error("assignment"); }
};
TEST(BtreeModifiers, AssignmentExceptionsPropagateWithoutDestroyingTheEntry) {
    wjr::btree_map<int, throwing_assignment> values;
    values.try_emplace(1, 4);
    EXPECT_THROW(values.insert_or_assign(1, 7), std::runtime_error);
    EXPECT_EQ(values.at(1).value, 4);
    EXPECT_EQ(values.size(), 1u);
}

template <typename Tree, typename Reference>
void single_pass() {
    std::istringstream input("5 1 7 5 3 1 9 8");
    Tree values;
    values.insert(std::istream_iterator<int>(input), std::istream_iterator<int>());
    const Reference reference{5, 1, 7, 5, 3, 1, 9, 8};
    EXPECT_EQ(values.size(), reference.size());
    EXPECT_TRUE(std::equal(values.begin(), values.end(), reference.begin()));
    values.insert({2, 2, 5});
    auto expected = reference;
    expected.insert({2, 2, 5});
    EXPECT_EQ(values.size(), expected.size());
    EXPECT_TRUE(std::equal(values.begin(), values.end(), expected.begin()));
    const int more[] = {20, 10, 30, 20};
    values.insert(std::counted_iterator(more, 4), std::default_sentinel);
    expected.insert(std::begin(more), std::end(more));
    EXPECT_EQ(values.size(), expected.size());
    EXPECT_TRUE(std::equal(values.begin(), values.end(), expected.begin()));
}
TEST(BtreeModifiers, RangeInsertionAcceptsSinglePassAndNoncommonSentinel) {
    single_pass<wjr::btree_set<int>, std::set<int>>();
    single_pass<wjr::btree_multiset<int>, std::multiset<int>>();
}

template <typename Tree>
void map_ranges() {
    const std::pair<short, short> input[] = {{4, 40}, {1, 10}, {4, 99}, {2, 20}};
    Tree values;
    values.insert(std::begin(input), std::end(input));
    values.insert({{3, 30}, {2, 99}});
    EXPECT_EQ(values.size(), 4u);
    EXPECT_EQ(values.at(4), 40);
    EXPECT_EQ(values.at(2), 20);
    auto next = values.insert(typename Tree::value_type{5, 50});
    EXPECT_TRUE(next.second);
    EXPECT_EQ(next.first->second, 50);
}
template <typename Tree>
void moved_range() {
    std::vector<std::pair<int, std::unique_ptr<int>>> input;
    for (int i = 0; i != 100; ++i)
        input.emplace_back(i, std::make_unique<int>(i));
    Tree values;
    values.insert(std::make_move_iterator(input.begin()), std::make_move_iterator(input.end()));
    ASSERT_EQ(values.size(), 100u);
    for (const auto &[key, value] : values) {
        ASSERT_TRUE(value);
        EXPECT_EQ(*value, key);
    }
    for (const auto &[key, value] : input)
        EXPECT_FALSE(value);
}
TEST(BtreeModifiers, MapRangeConversionAndMoveOnlyValuesUseExistingInsertion) {
    map_ranges<wjr::btree_map<int, int>>();
    map_ranges<wjr::experimental::compact_btree_map<int, int>>();
    moved_range<wjr::btree_map<int, std::unique_ptr<int>>>();
    moved_range<wjr::experimental::compact_btree_map<int, std::unique_ptr<int>>>();
}

struct throwing_increment {
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;
    const int *current = nullptr;
    const int *stop = nullptr;
    int operator*() const { return *current; }
    throwing_increment &operator++() {
        if (current == stop)
            throw std::runtime_error("input increment");
        ++current;
        return *this;
    }
    void operator++(int) { ++*this; }
    friend bool operator==(const throwing_increment &lhs, const throwing_increment &rhs) {
        return lhs.current == rhs.current;
    }
};
static_assert(std::input_iterator<throwing_increment>);
TEST(BtreeModifiers, InputIteratorFailureLeavesTheInsertedPrefix) {
    const int input[] = {1, 2, 3};
    wjr::btree_set<int> values;
    const throwing_increment first{input, input + 1}, last{input + 3, input + 1};
    EXPECT_THROW(values.insert(first, last), std::runtime_error);
    ASSERT_EQ(values.size(), 2u);
    EXPECT_EQ(values.count(1), 1u);
    EXPECT_EQ(values.count(2), 1u);
    EXPECT_EQ(values.count(3), 0u);
}

template <typename Tree, typename Iter, typename Sentinel>
concept accepts_range =
    requires(Tree &tree, Iter first, Sentinel last) { tree.insert(first, last); };
static_assert(accepts_range<wjr::btree_set<int>, const int *, const int *>);
static_assert(!accepts_range<wjr::btree_set<int>, const std::string *, const std::string *>);
static_assert(!accepts_range<wjr::btree_set<int>, int, int>);
static_assert(!accepts_range<wjr::btree_set<int>, const int *, std::default_sentinel_t>);
} // namespace
