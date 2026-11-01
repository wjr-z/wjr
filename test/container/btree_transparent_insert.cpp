#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

#include <memory_resource>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace {
class resource : public std::pmr::memory_resource {
    void *do_allocate(size_t bytes, size_t alignment) override {
        ++allocations;
        live_bytes += bytes;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }
    void do_deallocate(void *p, size_t bytes, size_t alignment) override {
        live_bytes -= bytes;
        std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    size_t allocations = 0, live_bytes = 0;
};
struct default_resource_scope {
    std::pmr::memory_resource *previous;
    explicit default_resource_scope(std::pmr::memory_resource *next)
        : previous(std::pmr::set_default_resource(next)) {}
    ~default_resource_scope() { std::pmr::set_default_resource(previous); }
};
using allocator = std::pmr::polymorphic_allocator<char>;
using pmr_set = wjr::btree_set<std::pmr::string, std::less<>, allocator, 5>;

TEST(BtreeTransparentInsert, DuplicateRegistryKeysNeedNoOwnedTemporaryOrAllocation) {
    resource defaults, owner;
    default_resource_scope scope(&defaults);
    {
        pmr_set enabled(std::less<>{}, allocator(&owner));
        const char *path = "/api/v1/warehouses/inventory/availability/by-location/central";
        const std::string_view view(path);
        EXPECT_TRUE(enabled.insert(view).second);
        EXPECT_EQ(enabled.begin()->get_allocator().resource(), &owner);
        const auto allocations = owner.allocations;
        for (int i = 0; i < 100; ++i) {
            EXPECT_FALSE(enabled.insert(path).second);
            EXPECT_FALSE(enabled.insert(view).second);
            EXPECT_EQ(enabled.insert(enabled.cbegin(), path), enabled.begin());
            EXPECT_EQ(enabled.insert(enabled.cend(), view), enabled.begin());
        }
        EXPECT_EQ(owner.allocations, allocations);
        EXPECT_EQ(defaults.allocations, 0u);
        EXPECT_EQ(enabled.size(), 1u);
    }
    EXPECT_EQ(owner.live_bytes, 0u);
    EXPECT_EQ(defaults.live_bytes, 0u);
}

template <typename Compare>
void check_order() {
    resource owner;
    {
        wjr::btree_set<std::pmr::string, Compare, allocator, 5> tree(Compare{}, allocator(&owner));
        std::set<std::string, Compare> expected;
        for (int i = 0; i < 91; ++i) {
            const auto key = std::string(70, 'x') + std::to_string((i * 37) % 97);
            const std::string_view view(key);
            if (i % 3 == 0)
                EXPECT_TRUE(tree.insert(view).second);
            else if (i % 3 == 1)
                EXPECT_EQ(*tree.insert(tree.cbegin(), view), view);
            else
                EXPECT_EQ(*tree.insert(tree.cend(), view), view);
            expected.insert(key);
            ASSERT_EQ(tree.size(), expected.size());
            auto reference = expected.begin();
            for (const auto &stored : tree) {
                EXPECT_EQ(std::string_view(stored), std::string_view(*reference));
                EXPECT_EQ(stored.get_allocator().resource(), &owner);
                ++reference;
            }
            const auto allocations = owner.allocations;
            EXPECT_FALSE(tree.insert(view).second);
            EXPECT_EQ(owner.allocations, allocations);
        }
    }
    EXPECT_EQ(owner.live_bytes, 0u);
}
TEST(BtreeTransparentInsert, BothOrdersAndCorrectOrWrongHintsPreserveCrossLeafInvariants) {
    check_order<std::less<>>();
    check_order<std::greater<>>();
}

TEST(BtreeTransparentInsert, ViewLengthAndEmptyKeysArePreserved) {
    wjr::btree_set<std::string, std::less<>> tree;
    const std::string_view embedded("a\0b", 3);
    tree.insert(embedded);
    tree.insert(tree.cend(), "a");
    tree.insert(std::string_view{});
    ASSERT_EQ(tree.size(), 3u);
    EXPECT_EQ(tree.find(embedded)->size(), 3u);
    EXPECT_TRUE(tree.contains("a"));
    EXPECT_TRUE(tree.begin()->empty());
    const char terminated_before_end[] = {'a', '\0', 'z'};
    EXPECT_FALSE(tree.insert(terminated_before_end).second);
    EXPECT_EQ(tree.find(terminated_before_end)->size(), 1u);
}

struct comparison_counts {
    int c_strings = 0, views = 0;
};
struct observed_order {
    using is_transparent = void;
    comparison_counts *counts;
    bool operator()(const std::string &a, const std::string &b) const noexcept { return a < b; }
    bool operator()(const std::string &a, const char *b) const noexcept {
        ++counts->c_strings;
        return a < b;
    }
    bool operator()(const char *a, const std::string &b) const noexcept {
        ++counts->c_strings;
        return a < b;
    }
    bool operator()(const std::string &a, std::string_view b) const noexcept {
        ++counts->views;
        return a < b;
    }
    bool operator()(std::string_view a, const std::string &b) const noexcept {
        ++counts->views;
        return a < b;
    }
};
struct observed_traits : std::char_traits<char> {
    static inline int lengths = 0;
    static size_t length(const char *key) noexcept {
        ++lengths;
        return std::char_traits<char>::length(key);
    }
};
TEST(BtreeTransparentInsert, CustomComparatorTraitsAndQueryTypesAreNotNormalized) {
    comparison_counts counts;
    wjr::btree_set<std::string, observed_order> tree(observed_order{&counts});
    tree.emplace("entry");
    EXPECT_FALSE(tree.insert("entry").second);
    EXPECT_TRUE(tree.contains("entry"));
    EXPECT_GT(counts.c_strings, 0);
    EXPECT_EQ(counts.views, 0);
    EXPECT_TRUE(tree.contains(std::string_view("entry")));
    EXPECT_GT(counts.views, 0);
    using custom_string = std::basic_string<char, observed_traits>;
    static_assert(
        !wjr::btree_detail::cached_c_string_lookup<std::less<>, custom_string, const char *>);
    static_assert(
        !wjr::btree_detail::cached_c_string_lookup<std::less<>, std::string, char *volatile>);
    static_assert(
        wjr::btree_detail::cached_c_string_lookup<std::less<>, std::string, const char[8]>);
    wjr::btree_set<custom_string, std::less<>> custom;
    custom.emplace("entry");
    observed_traits::lengths = 0;
    EXPECT_FALSE(custom.insert("entry").second);
    EXPECT_GT(observed_traits::lengths, 0);
}

TEST(BtreeTransparentInsert, CachedLookupServesMapAndMultisetBoundsAndAliasedErase) {
    wjr::btree_map<std::string, int, std::less<>, std::allocator<char>, 5> values;
    wjr::btree_multiset<std::string, std::less<>, std::allocator<char>, 5> names;
    for (int i = 0; i < 80; ++i) {
        const auto key = std::string(70, 'x') + std::to_string(i);
        values.try_emplace(key, i);
        names.emplace(key);
        names.emplace(key);
    }
    for (int i = 0; i < 80; ++i) {
        const auto key = std::string(70, 'x') + std::to_string(i);
        EXPECT_EQ(values.find(key.c_str())->second, i);
        EXPECT_EQ(values.count(key.c_str()), 1u);
        EXPECT_EQ(values.at(key.c_str()), i);
        EXPECT_EQ(values.lower_bound(key.c_str())->first, key);
        EXPECT_EQ(std::distance(names.lower_bound(key.c_str()), names.upper_bound(key.c_str())), 2);
        const auto range = names.equal_range(key.c_str());
        EXPECT_EQ(std::distance(range.first, range.second), 2);
        EXPECT_EQ(names.count(key.c_str()), 2u);
    }
    const char *alias = values.begin()->first.c_str();
    EXPECT_EQ(values.erase(alias), 1u);
    const char *duplicate_alias = names.begin()->c_str();
    EXPECT_EQ(names.erase(duplicate_alias), 2u);
    EXPECT_EQ(values.size(), 79u);
    EXPECT_EQ(names.size(), 158u);
}

struct borrowed_key {
    std::string_view text;
    int *conversions;
    borrowed_key(std::string_view text, int &count) : text(text), conversions(&count) {}
    borrowed_key(const borrowed_key &) = delete;
    explicit operator std::string() && {
        ++*conversions;
        return std::string(text);
    }
};
struct borrowed_order {
    using is_transparent = void;
    bool operator()(const std::string &a, const std::string &b) const noexcept { return a < b; }
    bool operator()(const std::string &a, const borrowed_key &b) const noexcept {
        return a < b.text;
    }
    bool operator()(const borrowed_key &a, const std::string &b) const noexcept {
        return a.text < b;
    }
};
TEST(BtreeTransparentInsert, MoveOnlyBorrowedKeyIsConvertedOnlyOnInsertion) {
    wjr::btree_set<std::string, borrowed_order> tree;
    int conversions = 0;
    tree.insert(borrowed_key("entry", conversions));
    tree.insert(tree.cend(), borrowed_key("entry", conversions));
    EXPECT_EQ(conversions, 1);
    tree.insert(tree.cbegin(), borrowed_key("another", conversions));
    EXPECT_EQ(conversions, 2);
    EXPECT_EQ(*tree.begin(), "another");
}

struct alignas(64) allocated_key {
    using allocator_type = allocator;
    std::pmr::memory_resource *owner;
    int number;
    allocated_key(std::allocator_arg_t, allocator alloc, int n) noexcept
        : owner(alloc.resource()), number(n) {}
    allocated_key(const allocated_key &) = delete;
    allocated_key(allocated_key &&) = delete;
};
struct allocated_order {
    using is_transparent = void;
    bool operator()(const allocated_key &a, const allocated_key &b) const noexcept {
        return a.number < b.number;
    }
    bool operator()(const allocated_key &a, int b) const noexcept { return a.number < b; }
    bool operator()(int a, const allocated_key &b) const noexcept { return a < b.number; }
};
TEST(BtreeTransparentInsert, AllocatorOnlyImmovableKeysUseTheirTypedOwner) {
    resource owner;
    {
        wjr::btree_set<allocated_key, allocated_order, allocator, 5> tree(allocated_order{},
                                                                          allocator(&owner));
        for (int i = 0; i < 80; ++i) {
            tree.insert(i);
            const auto allocations = owner.allocations;
            tree.insert(tree.cend(), i);
            EXPECT_EQ(owner.allocations, allocations);
            EXPECT_EQ(tree.find(i)->owner, &owner);
            EXPECT_EQ(reinterpret_cast<std::uintptr_t>(std::addressof(*tree.find(i))) % 64, 0u);
        }
        tree.erase(40);
        EXPECT_FALSE(tree.contains(40));
    }
    EXPECT_EQ(owner.live_bytes, 0u);
}

template <typename Tree, typename Query>
concept accepts_query = requires(Tree &tree, Query &&query) {
    tree.insert(std::forward<Query>(query));
    tree.insert(tree.cend(), std::forward<Query>(query));
};
struct one_way {
    using is_transparent = void;
    bool operator()(const std::string &, const std::string &) const;
    bool operator()(const std::string &, std::string_view) const;
};
struct comparison_only {};
struct comparison_order {
    using is_transparent = void;
    bool operator()(const std::string &, const std::string &) const;
    bool operator()(const std::string &, comparison_only) const;
    bool operator()(comparison_only, const std::string &) const;
};
TEST(BtreeTransparentInsert, UnsupportedQueriesRemainQuietAndOldKeySignaturesRemainUsable) {
    static_assert(accepts_query<pmr_set, std::string_view>);
    static_assert(!accepts_query<wjr::btree_set<std::string>, std::string_view>);
    static_assert(!accepts_query<wjr::btree_set<std::string, one_way>, std::string_view>);
    static_assert(!accepts_query<wjr::btree_set<std::string, comparison_order>, comparison_only>);
    static_assert(!accepts_query<pmr_set, pmr_set::iterator>);
    using tree = wjr::btree_set<std::string, std::less<>>;
    using member = std::pair<tree::iterator, bool> (tree::*)(const std::string &) noexcept;
    member original = &tree::insert;
    tree values;
    const std::string key = "same-key";
    EXPECT_TRUE((values.*original)(key).second);
    EXPECT_FALSE(values.insert(key).second);
    const std::string more[] = {"one", "two"};
    values.insert(std::begin(more), std::end(more));
    EXPECT_EQ(values.size(), 3u);
}

struct implicit_query {
    int number;
    bool fail;
    operator int() const {
        if (fail)
            throw std::runtime_error("key conversion");
        return number;
    }
};
struct implicit_order {
    using is_transparent = void;
    bool operator()(int a, int b) const noexcept { return a < b; }
    bool operator()(int a, const implicit_query &b) const noexcept { return a < b.number; }
    bool operator()(const implicit_query &a, int b) const noexcept { return a.number < b; }
};
TEST(BtreeTransparentInsert, ImplicitConversionExceptionsKeepTheirExistingBoundary) {
    wjr::btree_set<int, implicit_order> tree;
    tree.insert(1);
    const implicit_query hit{1, true}, miss{2, true};
    EXPECT_FALSE(tree.insert(hit).second);
    EXPECT_EQ(tree.insert(tree.cend(), hit), tree.begin());
    EXPECT_THROW(tree.insert(miss), std::runtime_error);
    EXPECT_THROW(tree.insert(tree.cbegin(), miss), std::runtime_error);
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_EQ(*tree.begin(), 1);
}
} // namespace
