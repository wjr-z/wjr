#include <concepts>
#include <map>
#include <set>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace {
template <typename Tree, typename Lookup>
concept has_bounds = requires(Tree &tree, const Tree &constant, const Lookup &key) {
    { tree.lower_bound(key) } -> std::same_as<typename Tree::iterator>;
    { tree.upper_bound(key) } -> std::same_as<typename Tree::iterator>;
    { constant.lower_bound(key) } -> std::same_as<typename Tree::const_iterator>;
    { constant.upper_bound(key) } -> std::same_as<typename Tree::const_iterator>;
};

template <typename Tree, typename Lookup>
concept has_map_lookup =
    has_bounds<Tree, Lookup> && requires(Tree &tree, const Tree &constant, const Lookup &key) {
        { constant.count(key) } -> std::same_as<typename Tree::size_type>;
        { tree.at(key) } -> std::same_as<typename Tree::mapped_type &>;
        { constant.at(key) } -> std::same_as<const typename Tree::mapped_type &>;
    };

struct stored_number {
    int value;
};
struct lookup_number {
    int value;
};

struct number_compare {
    using is_transparent = void;
    bool descending = false;
    bool compare(int lhs, int rhs) const noexcept {
        return descending ? lhs / 10 > rhs / 10 : lhs / 10 < rhs / 10;
    }
    bool operator()(stored_number lhs, stored_number rhs) const noexcept {
        return compare(lhs.value, rhs.value);
    }
    bool operator()(stored_number lhs, lookup_number rhs) const noexcept {
        return compare(lhs.value, rhs.value);
    }
    bool operator()(lookup_number lhs, stored_number rhs) const noexcept {
        return compare(lhs.value, rhs.value);
    }
};

struct one_way_compare {
    using is_transparent = void;
    bool operator()(stored_number lhs, stored_number rhs) const noexcept {
        return lhs.value < rhs.value;
    }
    bool operator()(stored_number lhs, lookup_number rhs) const noexcept {
        return lhs.value < rhs.value;
    }
};

struct unannotated_compare {
    using is_transparent = void;
    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs < rhs; }
};

using transparent_map = wjr::btree_map<std::string, int, std::less<>>;
using opaque_map = wjr::btree_map<std::string, int>;
using transparent_set = wjr::btree_set<std::string, std::less<>>;
static_assert(has_map_lookup<transparent_map, std::string_view>);
static_assert(has_map_lookup<transparent_map, std::string>);
static_assert(has_bounds<transparent_set, std::string_view>);
static_assert(!has_map_lookup<opaque_map, std::string_view>);
static_assert(!has_bounds<opaque_map, lookup_number>);
static_assert(!has_bounds<transparent_map, lookup_number>);
static_assert(!has_bounds<wjr::btree_set<stored_number, one_way_compare>, lookup_number>);
static_assert(noexcept(std::declval<transparent_map &>().lower_bound(std::string_view{})));
static_assert(noexcept(std::declval<const transparent_map &>().upper_bound(std::string_view{})));
static_assert(noexcept(std::declval<const transparent_map &>().count(std::string_view{})));
static_assert(!noexcept(std::declval<const transparent_map &>().at(std::string_view{})));
using unannotated_map = wjr::btree_map<std::string, int, unannotated_compare>;
static_assert(has_map_lookup<unannotated_map, std::string_view>);
static_assert(!std::is_nothrow_invocable_v<const unannotated_compare &, const std::string &,
                                           const std::string_view &>);
static_assert(noexcept(std::declval<const unannotated_map &>().count(std::string_view{})));

template <size_t NodeSize>
void check_number_lookup(bool descending) {
    const number_compare comp{descending};
    for (int count : {0, 1, 2, 3, 80}) {
        wjr::btree_map<stored_number, int, number_compare, std::allocator<char>, NodeSize> map(
            comp);
        wjr::btree_set<stored_number, number_compare, std::allocator<char>, NodeSize> set(comp);
        std::map<stored_number, int, number_compare> expected(comp);
        for (int i = 0; i != count; ++i) {
            const stored_number key{i * 20 + 10};
            map.emplace(key, i);
            set.emplace(key);
            expected.emplace(key, i);
        }
        const auto &constant_map = map;
        const auto &constant_set = set;
        for (int i = -1; i <= count * 20 + 21; ++i) {
            const lookup_number key{i};
            const auto first = expected.lower_bound(key);
            const auto last = expected.upper_bound(key);
            EXPECT_EQ(std::distance(map.begin(), map.lower_bound(key)),
                      std::distance(expected.begin(), first));
            EXPECT_EQ(std::distance(map.cbegin(), constant_map.upper_bound(key)),
                      std::distance(expected.begin(), last));
            EXPECT_EQ(std::distance(set.cbegin(), constant_set.lower_bound(key)),
                      std::distance(expected.begin(), first));
            EXPECT_EQ(std::distance(set.begin(), set.upper_bound(key)),
                      std::distance(expected.begin(), last));
            const auto found = expected.find(key);
            EXPECT_EQ(map.count(key), found != expected.end() ? 1u : 0u);
            EXPECT_EQ(set.count(key), found != expected.end() ? 1u : 0u);
            if (found != expected.end()) {
                EXPECT_EQ(&map.at(key), &constant_map.at(key));
                EXPECT_EQ(map.at(key), found->second);
            }
        }
    }
}

TEST(BtreeTransparentLookup, InlineAndIndirectKeysRespectEquivalentQueries) {
    check_number_lookup<4>(false);
    check_number_lookup<4>(true);
    check_number_lookup<16>(false);
    check_number_lookup<16>(true);
    check_number_lookup<32>(false);
    check_number_lookup<32>(true);
}

template <size_t NodeSize, typename Compare>
void check_string_lookup() {
    wjr::btree_map<std::string, int, Compare, std::allocator<char>, NodeSize> map;
    wjr::btree_set<std::string, Compare, std::allocator<char>, NodeSize> set;
    std::map<std::string, int, Compare> expected;
    for (int i = 0; i != 80; ++i) {
        auto key = "route/" + std::string(64, 'x') + "/" + std::to_string(i * 2);
        map.emplace(key, i);
        set.emplace(key);
        expected.emplace(key, i);
    }
    const auto &constant = map;
    for (int i = -1; i != 162; ++i) {
        const auto owner = "route/" + std::string(64, 'x') + "/" + std::to_string(i);
        const std::string_view key(owner);
        const auto first = expected.lower_bound(key);
        const auto last = expected.upper_bound(key);
        EXPECT_EQ(std::distance(map.begin(), map.lower_bound(key)),
                  std::distance(expected.begin(), first));
        EXPECT_EQ(std::distance(set.begin(), set.upper_bound(key)),
                  std::distance(expected.begin(), last));
        const auto found = expected.find(key);
        EXPECT_EQ(map.count(key), found != expected.end() ? 1u : 0u);
        EXPECT_EQ(set.count(owner.c_str()), found != expected.end() ? 1u : 0u);
        if (found != expected.end()) {
            EXPECT_EQ(map.at(key), found->second);
            EXPECT_EQ(constant.at(key), found->second);
        } else {
#if !defined(WJR_DISABLE_EXCEPTIONS)
            EXPECT_THROW(map.at(key), std::out_of_range);
            EXPECT_THROW(constant.at(key), std::out_of_range);
#endif
        }
    }
}

TEST(BtreeTransparentLookup, StringViewsAndCStringQueriesMatchStdMap) {
    check_string_lookup<4, std::less<>>();
    check_string_lookup<16, std::greater<>>();
    check_string_lookup<32, std::less<>>();
}

struct query_allocations {
    static inline size_t calls = 0;
    static inline size_t bytes = 0;
};

template <typename T>
struct query_allocator {
    using value_type = T;
    using is_always_equal = std::true_type;
    query_allocator() noexcept = default;
    template <typename U>
    query_allocator(const query_allocator<U> &) noexcept {}
    T *allocate(size_t count) {
        ++query_allocations::calls;
        query_allocations::bytes += count * sizeof(T);
        return std::allocator<T>{}.allocate(count);
    }
    void deallocate(T *ptr, size_t count) noexcept { std::allocator<T>{}.deallocate(ptr, count); }
    bool operator==(const query_allocator &) const = default;
};

TEST(BtreeTransparentLookup, QueriesDoNotAllocateKeysOrTreeStorage) {
    using key_type = std::basic_string<char, std::char_traits<char>, query_allocator<char>>;
    wjr::btree_map<key_type, int, std::less<>, query_allocator<char>> map;
    wjr::btree_set<key_type, std::less<>, query_allocator<char>> set;
    const std::string owner(256, 'x');
    const std::string absent_owner(257, 'x');
    map.emplace(key_type(owner.data(), owner.size()), 7);
    set.emplace(owner.data(), owner.size());
    const auto calls = query_allocations::calls;
    const auto bytes = query_allocations::bytes;
    const std::string_view key(owner);
    for (int i = 0; i != 100; ++i) {
        EXPECT_EQ(map.count(key), 1u);
        EXPECT_EQ(set.count(key), 1u);
        EXPECT_EQ(map.at(key), 7);
        EXPECT_EQ(std::as_const(map).at(key), 7);
        EXPECT_EQ(map.lower_bound(key), map.begin());
        EXPECT_EQ(std::as_const(map).upper_bound(key), map.end());
        EXPECT_EQ(set.lower_bound(owner.c_str()), set.begin());
        EXPECT_EQ(std::as_const(set).upper_bound(owner.c_str()), set.end());
        EXPECT_EQ(map.count(std::string_view(absent_owner)), 0u);
        EXPECT_EQ(set.count(absent_owner.c_str()), 0u);
    }
    EXPECT_EQ(query_allocations::calls, calls);
    EXPECT_EQ(query_allocations::bytes, bytes);
}

TEST(BtreeTransparentLookup, MultisetBoundsIncludeEveryEquivalentKey) {
    wjr::btree_multiset<std::string, std::less<>, std::allocator<char>, 4> values;
    for (int i = 0; i != 80; ++i) {
        values.emplace("duplicate-key");
    }
    const std::string_view key("duplicate-key");
    EXPECT_EQ(std::distance(values.lower_bound(key), values.upper_bound(key)), 80);
    EXPECT_EQ(std::as_const(values).lower_bound(key), values.begin());
    EXPECT_EQ(std::as_const(values).upper_bound(key), values.end());
}
} // namespace
