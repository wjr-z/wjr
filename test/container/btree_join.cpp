#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/btree_join.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <algorithm>
#include <functional>
#include <map>
#include <memory_resource>
#include <random>
#include <set>
#include <string>

#include <gtest/gtest.h>

namespace {
template <typename Destination, typename Source>
concept accepts_append = requires(Destination &destination, Source &source) {
    { wjr::experimental::append_btree_disjoint(destination, source) } -> std::same_as<void>;
};
struct custom_less {
    bool operator()(int lhs, int rhs) const noexcept { return lhs < rhs; }
};
using legacy = wjr::btree_map<int, int>;
using compact = wjr::experimental::compact_btree_map<int, int>;
static_assert(accepts_append<legacy, legacy>);
static_assert(accepts_append<compact, compact>);
static_assert(!accepts_append<legacy, const legacy>);
static_assert(!accepts_append<legacy, compact>);
static_assert(!accepts_append<legacy, wjr::btree_map<int, int, std::greater<>>>);
static_assert(
    !accepts_append<wjr::btree_map<int, int, custom_less>, wjr::btree_map<int, int, custom_less>>);

template <typename Tree>
void check_shapes() {
    const bool reverse = !typename Tree::key_compare{}(0, 1);
    for (int left : {0, 1, 2, 3, 8, 9, 16, 17, 18, 31, 32, 33, 80, 150, 272, 273, 4096}) {
        for (int right : {0, 1, 2, 3, 8, 9, 16, 17, 18, 31, 32, 33, 80, 150, 272, 273, 4096}) {
            Tree destination, source;
            std::map<int, int, typename Tree::key_compare> reference;
            for (int i = 0; i < left; ++i) {
                const int key = i + (reverse ? 10000 : 0);
                destination.try_emplace(key, key * 3);
                reference.emplace(key, key * 3);
            }
            for (int i = 0; i < right; ++i) {
                const int key = i + (reverse ? 0 : 10000);
                source.try_emplace(key, key * 3);
                reference.emplace(key, key * 3);
            }
            wjr::experimental::append_btree_disjoint(destination, source);
            EXPECT_TRUE(source.empty());
            EXPECT_TRUE(std::equal(destination.begin(), destination.end(), reference.begin(),
                                   reference.end()));
            auto copy = destination;
            EXPECT_EQ(copy, destination);
            Tree moved(std::move(copy));
            EXPECT_TRUE(copy.empty());
            destination.swap(moved);
            wjr::experimental::append_btree_disjoint(destination, destination);
            EXPECT_EQ(destination.size(), reference.size());
            std::vector<int> keys;
            for (const auto &value : reference)
                keys.push_back(value.first);
            std::mt19937 random(42);
            std::shuffle(keys.begin(), keys.end(), random);
            for (int key : keys)
                EXPECT_EQ(destination.erase(key), 1);
            EXPECT_TRUE(destination.empty());
            source.try_emplace(42, 42);
            EXPECT_EQ(source.size(), 1);
        }
    }
}

TEST(BtreeJoin, AllBoundaryShapesAndPostJoinMutations) {
    check_shapes<legacy>();
    check_shapes<compact>();
    check_shapes<
        wjr::experimental::compact_btree_map<int, int, std::less<>, std::allocator<char>, 5>>();
    check_shapes<wjr::btree_map<int, int, std::greater<>>>();
}

struct owner {
    std::map<void *, std::pair<size_t, size_t>> blocks;
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t value_constructions = 0;
    size_t value_destructions = 0;
    std::function<void()> observe;
};
template <typename T>
struct owner_allocator {
    using value_type = T;
    owner *state = nullptr;
    int identity = 0;
    owner_allocator() = default;
    owner_allocator(owner *state, int identity) : state(state), identity(identity) {}
    template <typename U>
    owner_allocator(const owner_allocator<U> &other) noexcept
        : state(other.state), identity(other.identity) {}
    T *allocate(size_t count) {
        auto *ptr = std::allocator<T>{}.allocate(count);
        EXPECT_TRUE(state->blocks.emplace(ptr, std::pair{count, alignof(T)}).second);
        ++state->allocations;
        if (state->observe)
            state->observe();
        return ptr;
    }
    void deallocate(T *ptr, size_t count) noexcept {
        const auto found = state->blocks.find(ptr);
        EXPECT_NE(found, state->blocks.end());
        if (found == state->blocks.end())
            std::abort();
        EXPECT_EQ(found->second, (std::pair{count, alignof(T)}));
        if (state->observe)
            state->observe();
        state->blocks.erase(found);
        ++state->deallocations;
        std::allocator<T>{}.deallocate(ptr, count);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        std::construct_at(ptr, std::forward<Args>(args)...);
        if constexpr (requires(U &value) {
                          value.first;
                          value.second;
                      })
            ++state->value_constructions;
        if (state->observe)
            state->observe();
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        std::destroy_at(ptr);
        if constexpr (requires(U &value) {
                          value.first;
                          value.second;
                      })
            ++state->value_destructions;
        if (state->observe)
            state->observe();
    }
    template <typename U>
    bool operator==(const owner_allocator<U> &other) const noexcept {
        return state == other.state;
    }
};

struct alignas(64) payload {
    static inline int live = 0;
    static inline int moves = 0;
    const payload *self = this;
    int value;
    explicit payload(int value) noexcept : value(value) { ++live; }
    payload(const payload &) = delete;
    payload(payload &&other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++moves;
        ++live;
    }
    ~payload() noexcept {
        EXPECT_EQ(self, this);
        --live;
    }
};

template <bool Compact, size_t N>
void check_graph_ownership(int count) {
    using allocator = owner_allocator<char>;
    using legacy_tree = wjr::btree_map<int, payload, std::less<>, allocator, N>;
    using compact_tree =
        wjr::experimental::compact_btree_map<int, payload, std::less<>, allocator, N>;
    using tree = std::conditional_t<Compact, compact_tree, legacy_tree>;
    owner state;
    {
        tree destination(std::less<>{}, allocator(&state, 11));
        tree source(std::less<>{}, allocator(&state, 22));
        std::map<int, const payload *> addresses;
        for (int i = 0; i < count; ++i) {
            destination.try_emplace(i, i);
            source.try_emplace(i + 10000, i + 10000);
        }
        for (const auto &value : destination)
            addresses.emplace(value.first, &value.second);
        for (const auto &value : source)
            addresses.emplace(value.first, &value.second);
        const auto allocations = state.allocations;
        const auto deallocations = state.deallocations;
        const auto constructions = state.value_constructions;
        const auto destructions = state.value_destructions;
        const auto moves = payload::moves;
        state.observe = [&] {
            EXPECT_EQ(destination.size() + source.size(), static_cast<size_t>(count * 2));
            std::set<int> observed;
            for (const auto &value : destination) {
                EXPECT_TRUE(observed.insert(value.first).second);
                EXPECT_EQ(&value.second, addresses.at(value.first));
            }
            for (const auto &value : source) {
                EXPECT_TRUE(observed.insert(value.first).second);
                EXPECT_EQ(&value.second, addresses.at(value.first));
            }
            EXPECT_EQ(observed.size(), static_cast<size_t>(count * 2));
        };
        wjr::experimental::append_btree_disjoint(destination, source);
        state.observe = {};
        EXPECT_TRUE(source.empty());
        EXPECT_LE(state.allocations - allocations, 1u);
        EXPECT_LE(state.deallocations - deallocations, 1u);
        EXPECT_EQ(state.value_constructions, constructions);
        EXPECT_EQ(state.value_destructions, destructions);
        EXPECT_EQ(payload::moves, moves);
        EXPECT_EQ(destination.get_allocator().identity, 11);
        EXPECT_EQ(source.get_allocator().identity, 22);
        for (const auto &value : destination) {
            EXPECT_EQ(&value.second, addresses.at(value.first));
            EXPECT_EQ(value.second.value, value.first);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(&value.second) % alignof(payload), 0);
        }
        source.try_emplace(-1, -1);
        EXPECT_EQ(source.at(-1).value, -1);
    }
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.value_constructions, state.value_destructions);
    EXPECT_EQ(payload::live, 0);
}

TEST(BtreeJoin, ExistingLifetimesAllocatorIdentityAndHookObservability) {
    for (int count : {9, 17, 80, 150, 4096}) {
        check_graph_ownership<false, 16>(count);
        check_graph_ownership<true, 16>(count);
    }
    check_graph_ownership<true, 4>(9);
    check_graph_ownership<true, 5>(16);
    check_graph_ownership<true, 64>(33);
    check_graph_ownership<true, 64>(1000);
    check_graph_ownership<true, 64>(1500);
}

TEST(BtreeJoin, IndirectStringSeparatorsAndMultisetGroups) {
    wjr::btree_set<std::string> destination, source;
    std::map<std::string, const std::string *> addresses;
    for (int i = 1000; i < 1300; ++i) {
        destination.insert("a" + std::to_string(i) + std::string(40, 'x'));
        source.insert("z" + std::to_string(i) + std::string(40, 'x'));
    }
    for (const auto &key : destination)
        addresses.emplace(key, &key);
    for (const auto &key : source)
        addresses.emplace(key, &key);
    wjr::experimental::append_btree_disjoint(destination, source);
    EXPECT_TRUE(source.empty());
    for (const auto &key : destination)
        EXPECT_EQ(&key, addresses.at(key));
    while (!destination.empty())
        destination.erase(destination.begin());
    wjr::btree_multiset<int> left, right;
    std::multiset<int> reference;
    for (int i = 0; i < 400; ++i) {
        left.insert(i % 7);
        right.insert(100 + i % 11);
        reference.insert(i % 7);
        reference.insert(100 + i % 11);
    }
    wjr::experimental::append_btree_disjoint(left, right);
    EXPECT_TRUE(right.empty());
    EXPECT_TRUE(std::equal(left.begin(), left.end(), reference.begin(), reference.end()));
    EXPECT_EQ(left.erase(102), reference.erase(102));
    EXPECT_EQ(left.size(), reference.size());
}

TEST(BtreeJoin, PmrSourceCanBeDestroyedAfterAppend) {
    std::pmr::unsynchronized_pool_resource resource;
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree =
        wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<>, allocator>;
    tree destination(std::less<>{}, allocator(&resource));
    for (int i = 0; i < 150; ++i)
        destination.try_emplace(i, 80, 'a');
    {
        tree source(std::less<>{}, allocator(&resource));
        for (int i = 1000; i < 1150; ++i)
            source.try_emplace(i, 80, 'b');
        wjr::experimental::append_btree_disjoint(destination, source);
        EXPECT_TRUE(source.empty());
    }
    EXPECT_EQ(destination.size(), 300);
    for (const auto &[key, value] : destination) {
        EXPECT_EQ(value.get_allocator().resource(), &resource);
        EXPECT_EQ(value, std::pmr::string(80, key < 1000 ? 'a' : 'b'));
    }
}
} // namespace
