#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace {
template <typename Iter>
concept exposes_leaf = requires(Iter iter) { iter.get_leaf(); };

template <typename Tree>
constexpr bool check_iterator_contract() {
    using iterator = typename Tree::iterator;
    using const_iterator = typename Tree::const_iterator;
    static_assert(std::bidirectional_iterator<iterator>);
    static_assert(std::bidirectional_iterator<const_iterator>);
    static_assert(std::bidirectional_iterator<typename Tree::reverse_iterator>);
    static_assert(std::bidirectional_iterator<typename Tree::const_reverse_iterator>);
    static_assert(std::sentinel_for<const_iterator, iterator>);
    static_assert(std::sentinel_for<iterator, const_iterator>);
    static_assert(std::convertible_to<iterator, const_iterator>);
    static_assert(!std::constructible_from<iterator, const_iterator>);
    static_assert(!std::assignable_from<iterator &, const_iterator>);
    static_assert(!exposes_leaf<iterator>);
    static_assert(!exposes_leaf<const_iterator>);
    static_assert(sizeof(iterator) == sizeof(const_iterator));
    static_assert(sizeof(iterator) == 2 * sizeof(void *));
    return true;
}

using map_type = wjr::btree_map<int, int>;
using set_type = wjr::btree_set<int>;
using multiset_type = wjr::btree_multiset<int>;
static_assert(check_iterator_contract<map_type>());
static_assert(check_iterator_contract<set_type>());
static_assert(check_iterator_contract<multiset_type>());
static_assert(std::is_same_v<std::iter_reference_t<map_type::iterator>, map_type::value_type &>);
static_assert(std::is_const_v<map_type::value_type::first_type>);
static_assert(
    std::is_same_v<std::iter_reference_t<map_type::const_iterator>, const map_type::value_type &>);
static_assert(std::is_same_v<std::iter_reference_t<set_type::iterator>, const int &>);
static_assert(std::is_same_v<std::iter_reference_t<multiset_type::iterator>, const int &>);
static_assert(std::is_same_v<set_type::iterator::pointer, const int *>);
static_assert(std::is_same_v<set_type::reference, const int &>);
static_assert(std::is_same_v<multiset_type::reference, const int &>);
static_assert(!std::indirectly_writable<set_type::iterator, int>);
static_assert(!std::indirectly_writable<multiset_type::iterator, int>);

template <typename Tree>
void check_iteration() {
    for (int count : {0, 1, 2, 3, 80}) {
        Tree tree;
        for (int i = 0; i != count; ++i) {
            if constexpr (std::is_void_v<typename Tree::mapped_type>) {
                tree.emplace(i);
            } else {
                tree.emplace(i, i);
            }
        }
        const auto &const_tree = tree;
        EXPECT_EQ(std::ranges::distance(tree), count);
        EXPECT_EQ(std::ranges::distance(const_tree), count);
        EXPECT_EQ(std::ranges::distance(std::ranges::subrange(tree.begin(), const_tree.end())),
                  count);
        EXPECT_EQ(std::ranges::distance(std::ranges::subrange(const_tree.begin(), tree.end())),
                  count);
        auto mutable_it = tree.begin();
        auto const_it = const_tree.begin();
        for (int i = 0; i != count; ++i) {
            EXPECT_TRUE(mutable_it == const_it);
            EXPECT_TRUE(const_it == mutable_it);
            EXPECT_FALSE(mutable_it != const_it);
            EXPECT_FALSE(const_it != mutable_it);
            if constexpr (std::is_void_v<typename Tree::mapped_type>) {
                EXPECT_EQ(*mutable_it, i);
            } else {
                mutable_it->second += 10;
                EXPECT_EQ(const_it->second, i + 10);
            }
            ++mutable_it;
            ++const_it;
        }
        EXPECT_EQ(mutable_it, const_tree.end());
        EXPECT_EQ(const_it, tree.end());
        while (!tree.empty()) {
            auto next = tree.erase(tree.cbegin());
            EXPECT_EQ(next, tree.begin());
        }
        EXPECT_EQ(tree.begin(), const_tree.end());
    }
}

TEST(BtreeIterator, ConstAndMutableRangesAndMutationResults) {
    check_iteration<map_type>();
    check_iteration<set_type>();
}
} // namespace
