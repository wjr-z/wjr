#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <map>
#include <memory_resource>
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
    size_t allocations = 0;
    size_t live_bytes = 0;
};
struct default_resource_scope {
    std::pmr::memory_resource *previous;
    explicit default_resource_scope(std::pmr::memory_resource *next)
        : previous(std::pmr::set_default_resource(next)) {}
    ~default_resource_scope() { std::pmr::set_default_resource(previous); }
};
using allocator = std::pmr::polymorphic_allocator<char>;
using string_tree = wjr::btree_map<std::pmr::string, int, std::less<>, allocator, 5>;

TEST(BtreeTransparentUpdate, LongBorrowedKeysOnlyAllocateWhenMissing) {
    resource defaults, owner;
    default_resource_scope scope(&defaults);
    {
        string_tree tree(std::less<>{}, allocator(&owner));
        const char *path = "/api/v1/warehouses/inventory/availability/by-location/central";
        const std::string_view view(path);
        EXPECT_TRUE(tree.try_emplace(view, 10).second);
        EXPECT_EQ(tree.begin()->first.get_allocator().resource(), &owner);
        const auto before = owner.allocations;
        for (int i = 0; i < 100; ++i) {
            EXPECT_FALSE(tree.try_emplace(path, 0).second);
            EXPECT_FALSE(tree.try_emplace(view, 0).second);
            EXPECT_FALSE(tree.insert_or_assign(view, i).second);
            ++tree[path];
            ++tree[view];
            tree.try_emplace(tree.cend(), view, 0);
            tree.insert_or_assign(tree.cend(), path, i);
        }
        EXPECT_EQ(owner.allocations, before);
        EXPECT_EQ(defaults.allocations, 0u);
        EXPECT_EQ(tree.at(view), 99);
    }
    EXPECT_EQ(owner.live_bytes, 0u);
    EXPECT_EQ(defaults.live_bytes, 0u);
}

template <typename Compare>
void check_string_order() {
    resource owner;
    {
        using tree_type = wjr::btree_map<std::pmr::string, int, Compare, allocator, 5>;
        tree_type tree(Compare{}, allocator(&owner));
        std::map<std::string, int, Compare> expected;
        for (int i = 0; i < 100; ++i) {
            const auto key = std::string(70, 'x') + std::to_string((i * 37) % 101);
            const std::string_view view(key);
            switch (i % 5) {
            case 0:
                tree.try_emplace(view, i);
                expected.try_emplace(key, i);
                break;
            case 1:
                tree.try_emplace(tree.cbegin(), view, i);
                expected.try_emplace(key, i);
                break;
            case 2:
                tree.insert_or_assign(view, i);
                expected.insert_or_assign(key, i);
                break;
            case 3:
                tree.insert_or_assign(tree.cend(), view, i);
                expected.insert_or_assign(key, i);
                break;
            default:
                tree[view] += i;
                expected[key] += i;
                break;
            }
            EXPECT_EQ(tree.find(view)->first.get_allocator().resource(), &owner);
            ASSERT_EQ(tree.size(), expected.size());
            auto reference = expected.begin();
            for (const auto &[stored, number] : tree) {
                EXPECT_EQ(std::string_view(stored), std::string_view(reference->first));
                EXPECT_EQ(number, reference->second);
                ++reference;
            }
        }
    }
    EXPECT_EQ(owner.live_bytes, 0u);
}

TEST(BtreeTransparentUpdate, AscendingAndDescendingQueriesPreserveCrossLeafOrdering) {
    check_string_order<std::less<>>();
    check_string_order<std::greater<>>();
}

struct mapped {
    int number;
    static inline int constructions = 0;
    explicit mapped(int n) noexcept : number(n) { ++constructions; }
    mapped(mapped &&other) noexcept : number(other.number) { ++constructions; }
    mapped &operator=(mapped &&other) noexcept {
        number = other.number;
        other.number = -1;
        return *this;
    }
};
template <typename Tree>
void check_scalar() {
    Tree tree;
    std::map<int, int> expected;
    for (int i = 0; i < 80; ++i) {
        const short key = (i * 37) % 83;
        const auto result = tree.try_emplace(key, i);
        EXPECT_TRUE(result.second);
        expected.emplace(key, i);
        const auto constructions = mapped::constructions;
        tree.try_emplace(tree.cbegin(), key, 1000);
        EXPECT_EQ(mapped::constructions, constructions);
        mapped replacement(i + 1);
        tree.insert_or_assign(tree.cend(), key, std::move(replacement));
        expected.insert_or_assign(key, i + 1);
        EXPECT_EQ(replacement.number, -1);
    }
    auto reference = expected.begin();
    for (const auto &[key, value] : tree) {
        EXPECT_EQ(key, reference->first);
        EXPECT_EQ(value.number, reference->second);
        ++reference;
    }
}

TEST(BtreeTransparentUpdate, ScalarQueriesKeepLegacyAndCompactMoveOnlyMappedSemantics) {
    check_scalar<wjr::btree_map<int, mapped, std::less<>, std::allocator<char>, 5>>();
    check_scalar<
        wjr::experimental::compact_btree_map<int, mapped, std::less<>, std::allocator<char>, 5>>();
    wjr::experimental::compact_btree_map<int, int, std::less<>> counts;
    ++counts[short(3)];
    EXPECT_EQ(counts.at(3), 1);
}

struct borrowed_key {
    std::string_view text;
    int *conversions;
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

TEST(BtreeTransparentUpdate, RvalueBorrowedKeyIsConvertedOnlyForMissingEntry) {
    wjr::btree_map<std::string, int, borrowed_order> tree;
    int conversions = 0;
    tree.try_emplace(borrowed_key{"entry", &conversions}, 1);
    EXPECT_EQ(conversions, 1);
    tree.try_emplace(borrowed_key{"entry", &conversions}, 2);
    tree.insert_or_assign(tree.cbegin(), borrowed_key{"entry", &conversions}, 3);
    EXPECT_EQ(conversions, 1);
    EXPECT_EQ(tree.begin()->second, 3);
    tree[borrowed_key{"next", &conversions}] = 4;
    EXPECT_EQ(conversions, 2);
}

TEST(BtreeTransparentUpdate, StringViewLengthIsPreservedIncludingEmbeddedZero) {
    wjr::btree_map<std::string, int, std::less<>> tree;
    const std::string_view query("a\0b", 3);
    tree.try_emplace(query, 7);
    tree["a"] = 2;
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.find(query)->first.size(), 3u);
    EXPECT_EQ(tree.at(query), 7);
    EXPECT_EQ(tree.at("a"), 2);
}

struct allocator_mapped {
    using allocator_type = allocator;
    std::pmr::memory_resource *owner;
    int number;
    allocator_mapped(std::allocator_arg_t, allocator alloc, int n) noexcept
        : owner(alloc.resource()), number(n) {}
    allocator_mapped(std::allocator_arg_t, allocator alloc, allocator_mapped &&other) noexcept
        : owner(alloc.resource()), number(other.number) {}
    allocator_mapped(allocator_mapped &&) noexcept = default;
    allocator_mapped &operator=(int n) noexcept {
        number = n;
        return *this;
    }
};

TEST(BtreeTransparentUpdate, BorrowedUpdatesRetainAllocatorOnlyMappedConstruction) {
    resource owner;
    {
        wjr::btree_map<std::pmr::string, allocator_mapped, std::less<>, allocator> tree(
            std::less<>{}, allocator(&owner));
        const std::string_view query = "allocator-aware-value";
        tree.insert_or_assign(query, 7);
        tree.insert_or_assign(tree.cend(), query, 9);
        EXPECT_EQ(tree.at(query).number, 9);
        EXPECT_EQ(tree.at(query).owner, &owner);
        wjr::experimental::compact_btree_map<int, allocator_mapped, std::less<>, allocator> compact(
            std::less<>{}, allocator(&owner));
        compact.insert_or_assign(short(3), 5);
        compact.insert_or_assign(compact.cbegin(), short(3), 6);
        EXPECT_EQ(compact.at(3).number, 6);
        EXPECT_EQ(compact.at(3).owner, &owner);
    }
    EXPECT_EQ(owner.live_bytes, 0u);
}

template <typename Tree, typename Query>
concept accepts_query = requires(Tree &tree, Query &&query) {
    tree.try_emplace(std::forward<Query>(query), 0);
    tree.try_emplace(tree.cend(), std::forward<Query>(query), 0);
    tree.insert_or_assign(std::forward<Query>(query), 0);
    tree.insert_or_assign(tree.cend(), std::forward<Query>(query), 0);
    tree[std::forward<Query>(query)];
};
struct one_way {
    using is_transparent = void;
    bool operator()(const std::string &, const std::string &) const;
    bool operator()(const std::string &, std::string_view) const;
};
struct comparison_only {};
struct unconstructible_order {
    using is_transparent = void;
    bool operator()(const std::string &, const std::string &) const;
    bool operator()(const std::string &, const comparison_only &) const;
    bool operator()(const comparison_only &, const std::string &) const;
};
TEST(BtreeTransparentUpdate, ConstraintsPreserveKeyOverloadsAndRejectUnsupportedQueries) {
    static_assert(accepts_query<string_tree, std::string_view>);
    static_assert(accepts_query<string_tree, const char *>);
    static_assert(!accepts_query<wjr::btree_map<std::string, int>, std::string_view>);
    static_assert(!accepts_query<wjr::btree_map<std::string, int, one_way>, std::string_view>);
    static_assert(
        !accepts_query<wjr::btree_map<std::string, int, unconstructible_order>, comparison_only>);
    static_assert(!wjr::btree_detail::transparent_update<string_tree::iterator, std::less<>,
                                                         string_tree::key_type, allocator,
                                                         string_tree::const_iterator>);
    wjr::btree_map<std::string, int, std::less<>> tree;
    /// Explicitly typed member addresses retain the original const-Key overload.
    using member =
        std::pair<decltype(tree)::iterator, bool> (decltype(tree)::*)(const std::string &, int &&);
    member insert = &decltype(tree)::template try_emplace<int>;
    std::string key = "same-key";
    EXPECT_TRUE((tree.*insert)(key, 7).second);
    EXPECT_EQ(key, "same-key");
}
} // namespace
