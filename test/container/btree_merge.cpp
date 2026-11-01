#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <algorithm>
#include <map>
#include <memory_resource>
#include <set>
#include <string>

#include <gtest/gtest.h>

namespace {
template <typename Destination, typename Source>
concept accepts_merge = requires(Destination &destination, Source &source) {
    { destination.merge(source) } -> std::same_as<void>;
};

using ordinary_map = wjr::btree_map<int, int>;
using compact_map = wjr::experimental::compact_btree_map<int, int>;
static_assert(
    accepts_merge<ordinary_map, wjr::btree_map<int, int, std::greater<>, std::allocator<char>, 4>>);
static_assert(accepts_merge<wjr::btree_set<int>, wjr::btree_multiset<int>>);
static_assert(accepts_merge<wjr::btree_multiset<int>, wjr::btree_set<int>>);
static_assert(!accepts_merge<ordinary_map, compact_map>);
static_assert(!accepts_merge<compact_map, ordinary_map>);
static_assert(!accepts_merge<ordinary_map, wjr::btree_map<int, long>>);
static_assert(!accepts_merge<ordinary_map, const ordinary_map>);
static_assert(
    !accepts_merge<ordinary_map,
                   wjr::btree_map<int, int, std::less<>, std::pmr::polymorphic_allocator<char>>>);

struct alignas(64) payload {
    static inline int live = 0;
    static inline int moves = 0;
    int value;
    const payload *self = this;
    explicit payload(int value) noexcept : value(value) { ++live; }
    payload(const payload &) = delete;
    payload(payload &&other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        other.value = -1;
        ++live;
        ++moves;
    }
    ~payload() noexcept {
        EXPECT_EQ(self, this);
        --live;
    }
};

using tracked_value = std::pair<const int, payload>;
struct merge_owner {
    std::map<void *, std::pair<size_t, size_t>> blocks;
    size_t value_allocations = 0;
    size_t value_constructions = 0;
    size_t value_destructions = 0;
    size_t scalar_constructions = 0;
    size_t scalar_destructions = 0;
};

template <typename T>
struct merge_allocator {
    using value_type = T;
    using is_always_equal = std::false_type;
    merge_owner *owner = nullptr;
    merge_allocator() = default;
    explicit merge_allocator(merge_owner *owner) : owner(owner) {}
    template <typename U>
    merge_allocator(const merge_allocator<U> &other) noexcept : owner(other.owner) {}
    T *allocate(size_t n) {
        T *ptr = std::allocator<T>{}.allocate(n);
        owner->blocks.emplace(ptr, std::pair{n * sizeof(T), alignof(T)});
        if constexpr (std::same_as<T, tracked_value>)
            ++owner->value_allocations;
        return ptr;
    }
    void deallocate(T *ptr, size_t n) noexcept {
        const auto entry = owner->blocks.find(ptr);
        EXPECT_NE(entry, owner->blocks.end());
        if (entry == owner->blocks.end())
            std::abort();
        EXPECT_EQ(entry->second, (std::pair{n * sizeof(T), alignof(T)}));
        owner->blocks.erase(entry);
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        std::construct_at(ptr, std::forward<Args>(args)...);
        if constexpr (std::same_as<U, tracked_value>)
            ++owner->value_constructions;
        if constexpr (std::same_as<U, int>)
            ++owner->scalar_constructions;
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        std::destroy_at(ptr);
        if constexpr (std::same_as<U, tracked_value>)
            ++owner->value_destructions;
        if constexpr (std::same_as<U, int>)
            ++owner->scalar_destructions;
    }
    template <typename U>
    bool operator==(const merge_allocator<U> &other) const noexcept {
        return owner == other.owner;
    }
};

TEST(BtreeMerge, TransfersIndirectOwnersWithoutValueConstructionOrAllocation) {
    using allocator = merge_allocator<char>;
    using destination = wjr::btree_map<int, payload, std::less<>, allocator, 16>;
    using source = wjr::btree_map<int, payload, std::greater<>, allocator, 4>;
    for (int count : {0, 1, 2, 3, 16, 80, 512}) {
        merge_owner owner;
        {
            destination target(std::less<>{}, allocator(&owner));
            source incoming(std::greater<>{}, allocator(&owner));
            std::map<int, const payload *> addresses;
            for (int key = 0; key < count; ++key) {
                incoming.try_emplace(key, key);
                addresses.emplace(key, &incoming.find(key)->second);
                if (key % 3 == 0)
                    target.try_emplace(key, key + 1000);
            }
            const auto allocations = owner.value_allocations;
            const auto constructions = owner.value_constructions;
            const auto destructions = owner.value_destructions;
            payload::moves = 0;
            target.merge(incoming);
            EXPECT_EQ(owner.value_allocations, allocations);
            EXPECT_EQ(owner.value_constructions, constructions);
            EXPECT_EQ(owner.value_destructions, destructions);
            EXPECT_EQ(payload::moves, 0);
            EXPECT_EQ(target.size(), static_cast<size_t>(count));
            for (int key = 0; key < count; ++key) {
                if (key % 3 == 0) {
                    EXPECT_EQ(incoming.find(key)->second.value, key);
                    EXPECT_EQ(&incoming.find(key)->second, addresses.at(key));
                    EXPECT_EQ(target.find(key)->second.value, key + 1000);
                } else {
                    EXPECT_FALSE(incoming.contains(key));
                    EXPECT_EQ(&target.find(key)->second, addresses.at(key));
                    EXPECT_EQ(target.find(key)->second.value, key);
                }
            }
            target.merge(std::move(incoming));
            const auto size = target.size();
            target.merge(target);
            target.merge(std::move(target));
            EXPECT_EQ(target.size(), size);
        }
        EXPECT_TRUE(owner.blocks.empty());
        EXPECT_EQ(owner.value_constructions, owner.value_destructions);
        EXPECT_EQ(payload::live, 0);
    }
}

struct immovable {
    int value;
    explicit immovable(int value) : value(value) {}
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
};

TEST(BtreeMerge, ImmovableMappedValuesAndIndirectStringKeys) {
    wjr::btree_map<std::string, immovable> target;
    wjr::btree_map<std::string, immovable, std::greater<>, std::allocator<char>, 5> source;
    std::map<std::string, const immovable *> addresses;
    for (int i = 0; i != 256; ++i) {
        const auto key = std::string(80, 'x') + std::to_string(i);
        source.try_emplace(key, i);
        addresses[key] = &source.find(key)->second;
    }
    target.merge(source);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(target.size(), 256u);
    for (const auto &[key, value] : target)
        EXPECT_EQ(&value, addresses.at(key));
}

struct group_compare {
    int width;
    bool operator()(int lhs, int rhs) const noexcept { return lhs / width < rhs / width; }
};

TEST(BtreeMerge, DestinationEquivalenceAndMultiToUniqueMatchStandardContainers) {
    wjr::btree_map<int, int, group_compare> target(group_compare{10});
    wjr::btree_map<int, int, group_compare, std::allocator<char>, 4> source(group_compare{5});
    std::map<int, int, group_compare> ref_target(group_compare{10}), ref_source(group_compare{5});
    for (int i = 0; i != 120; ++i) {
        source.try_emplace(i, i);
        ref_source.try_emplace(i, i);
    }
    target.merge(source);
    ref_target.merge(ref_source);
    EXPECT_TRUE(std::equal(target.begin(), target.end(), ref_target.begin(), ref_target.end()));
    EXPECT_TRUE(std::equal(source.begin(), source.end(), ref_source.begin(), ref_source.end()));

    wjr::btree_set<int> unique;
    wjr::btree_multiset<int, std::greater<>, std::allocator<char>, 4> duplicate;
    for (int i = 0; i != 256; ++i)
        duplicate.insert(i % 31);
    unique.merge(duplicate);
    EXPECT_EQ(unique.size(), 31u);
    EXPECT_EQ(duplicate.size(), 225u);
    wjr::btree_multiset<int> all;
    all.merge(unique);
    all.merge(duplicate);
    EXPECT_TRUE(unique.empty());
    EXPECT_TRUE(duplicate.empty());
    EXPECT_EQ(all.size(), 256u);
}

TEST(BtreeMerge, InlineValuesRetainAllocatorConstructionAndDestructionHooks) {
    merge_owner owner;
    {
        using tree = wjr::btree_set<int, std::less<>, merge_allocator<char>, 4>;
        tree target(std::less<>{}, merge_allocator<char>(&owner));
        tree source(std::less<>{}, merge_allocator<char>(&owner));
        for (int i = 0; i != 80; ++i)
            source.insert(i);
        const auto constructions = owner.scalar_constructions;
        const auto destructions = owner.scalar_destructions;
        target.merge(source);
        EXPECT_TRUE(source.empty());
        EXPECT_EQ(target.size(), 80u);
        EXPECT_EQ(owner.scalar_constructions - constructions, 80u);
        EXPECT_EQ(owner.scalar_destructions - destructions, 80u);
    }
    EXPECT_TRUE(owner.blocks.empty());
    EXPECT_EQ(owner.scalar_constructions, owner.scalar_destructions);
}

TEST(BtreeMerge, CompactMoveOnlyValuesAndSharedPmrResource) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    std::pmr::unsynchronized_pool_resource resource;
    using tree =
        wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<>, allocator, 4>;
    tree target(std::less<>{}, allocator(&resource));
    tree source(std::less<>{}, allocator(&resource));
    for (int i = 0; i != 160; ++i) {
        source.try_emplace(i, 96, static_cast<char>('a' + i % 20));
        if (i % 4 == 0)
            target.try_emplace(i, "existing");
    }
    target.merge(source);
    EXPECT_EQ(target.size(), 160u);
    EXPECT_EQ(source.size(), 40u);
    for (const auto &[key, value] : target) {
        EXPECT_EQ(value.get_allocator().resource(), &resource);
        EXPECT_EQ(value, key % 4 == 0 ? std::pmr::string("existing")
                                      : std::pmr::string(96, static_cast<char>('a' + key % 20)));
    }
    for (const auto &[key, value] : source) {
        EXPECT_EQ(key % 4, 0);
        EXPECT_EQ(value.size(), 96u);
    }
    {
        using move_tree = wjr::experimental::compact_btree_map<int, payload>;
        move_tree destination, incoming;
        for (int i = 0; i != 80; ++i)
            incoming.try_emplace(i, i);
        destination.merge(incoming);
        EXPECT_TRUE(incoming.empty());
        for (const auto &[key, value] : destination) {
            EXPECT_EQ(value.value, key);
            EXPECT_EQ(value.self, &value);
        }
    }
    EXPECT_EQ(payload::live, 0);
}
} // namespace
