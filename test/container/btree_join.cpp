#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/btree_join.hpp>
#include <wjr/container/experimental/btree_join_range.hpp>
#include <wjr/container/experimental/btree_repack.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <algorithm>
#include <functional>
#include <map>
#include <memory_resource>
#include <random>
#include <set>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace {
template <typename T>
const void *construction_tag() noexcept {
    static char marker;
    return &marker;
}

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
    size_t live_bytes = 0;
    size_t peak_bytes = 0;
    size_t value_constructions = 0;
    size_t value_destructions = 0;
    size_t fail_allocation = 0;
    const void *fail_construct_tag = nullptr;
    size_t matching_constructions = 0;
    size_t fail_matching_construction = 0;
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
        if (state->fail_allocation != 0 && state->allocations + 1 == state->fail_allocation)
            throw std::bad_alloc();
        auto *ptr = std::allocator<T>{}.allocate(count);
        EXPECT_TRUE(state->blocks.emplace(ptr, std::pair{count, alignof(T)}).second);
        ++state->allocations;
        state->live_bytes += count * sizeof(T);
        state->peak_bytes = std::max(state->peak_bytes, state->live_bytes);
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
        state->live_bytes -= count * sizeof(T);
        std::allocator<T>{}.deallocate(ptr, count);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        if (state->fail_construct_tag == construction_tag<U>()) {
            ++state->matching_constructions;
            if (state->matching_constructions == state->fail_matching_construction)
                throw std::runtime_error("metadata construction");
        }
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
    static inline std::function<void()> moving;
    const payload *self = this;
    int value;
    explicit payload(int value) noexcept : value(value) { ++live; }
    payload(const payload &) = delete;
    payload(payload &&other) noexcept : value(other.value) {
        if (moving)
            moving();
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

template <typename Destination, typename Iter, typename Sentinel = Iter>
concept accepts_shard_range = requires(Destination &destination, Iter first, Sentinel last) {
    wjr::experimental::append_btree_disjoint(destination, first, last);
};
static_assert(accepts_shard_range<legacy, std::vector<legacy>::iterator>);
static_assert(!accepts_shard_range<legacy, std::vector<legacy>::const_iterator>);
static_assert(!accepts_shard_range<legacy, std::vector<compact>::iterator>);
static_assert(!accepts_shard_range<legacy, std::move_iterator<std::vector<legacy>::iterator>>);

template <bool Compact, size_t N>
void check_shard_range(int shards, int elements, int prefix = 0) {
    using allocator = owner_allocator<char>;
    using legacy_tree = wjr::btree_map<int, payload, std::less<>, allocator, N>;
    using compact_tree =
        wjr::experimental::compact_btree_map<int, payload, std::less<>, allocator, N>;
    using tree = std::conditional_t<Compact, compact_tree, legacy_tree>;
    owner state;
    {
        tree destination(std::less<>{}, allocator(&state, 11));
        std::vector<tree> sources;
        sources.reserve(shards);
        for (int s = 0; s < shards; ++s) {
            sources.emplace_back(std::less<>{}, allocator(&state, 100 + s));
            for (int i = 0; i < elements; ++i)
                sources.back().try_emplace((s + 1) * 4096 + i, (s + 1) * 4096 + i);
        }
        for (int i = 0; i < prefix; ++i)
            destination.try_emplace(i, i);
        std::map<int, const payload *> addresses;
        for (const auto &entry : destination)
            addresses.emplace(entry.first, &entry.second);
        for (const auto &source : sources)
            for (const auto &entry : source)
                addresses.emplace(entry.first, &entry.second);
        const auto constructions = state.value_constructions;
        const auto destructions = state.value_destructions;
        const auto moves = payload::moves;
        const auto inspect = [&](const tree &current) {
            EXPECT_EQ(static_cast<size_t>(std::distance(current.begin(), current.end())),
                      current.size());
            if (current.empty())
                return;
            const auto first = current.begin();
            const auto middle = std::next(first, current.size() / 2);
            const auto last = std::prev(current.end());
            for (auto position : {first, middle, last}) {
                EXPECT_EQ(current.find(position->first), position);
                EXPECT_EQ(&position->second, addresses.at(position->first));
            }
            EXPECT_EQ(current.lower_bound(first->first), first);
            EXPECT_EQ(current.upper_bound(last->first), current.end());
        };
        state.observe = [&] {
            size_t count = destination.size();
            for (const auto &source : sources)
                count += source.size();
            EXPECT_EQ(count, addresses.size());
            inspect(destination);
            for (const auto &source : sources)
                inspect(source);
        };
        wjr::experimental::append_btree_disjoint(destination, sources.begin(), sources.end());
        state.observe = {};
        EXPECT_EQ(destination.size(), addresses.size());
        EXPECT_EQ(state.value_constructions, constructions);
        EXPECT_EQ(state.value_destructions, destructions);
        EXPECT_EQ(payload::moves, moves);
        EXPECT_EQ(destination.get_allocator().identity, 11);
        for (int s = 0; s < shards; ++s) {
            EXPECT_TRUE(sources[s].empty());
            EXPECT_EQ(sources[s].get_allocator().identity, 100 + s);
            sources[s].try_emplace(-1, -1);
        }
        for (const auto &entry : destination) {
            EXPECT_EQ(&entry.second, addresses.at(entry.first));
            EXPECT_EQ(entry.second.value, entry.first);
        }
        while (!destination.empty())
            destination.erase(destination.begin());
    }
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.value_constructions, state.value_destructions);
    EXPECT_EQ(payload::live, 0);
}

TEST(BtreeJoinRange, NormalizesManyRootsWithoutMovingValues) {
    check_shard_range<false, 16>(8, 512);
    check_shard_range<true, 16>(8, 512);
    check_shard_range<true, 16>(8, 512, 512);
    check_shard_range<true, 16>(20, 150);
    check_shard_range<true, 4>(12, 9);
    check_shard_range<true, 5>(12, 16);
    check_shard_range<true, 64>(8, 1000);
}

TEST(BtreeJoinRange, EmptySingleReferenceAndMixedShapeInputs) {
    legacy destination;
    std::vector<legacy> empty(4);
    wjr::experimental::append_btree_disjoint(destination, empty.begin(), empty.end());
    EXPECT_TRUE(destination.empty());
    empty[2].try_emplace(7, 42);
    wjr::experimental::append_btree_disjoint(destination, empty.begin(), empty.end());
    EXPECT_EQ(destination.at(7), 42);
    std::vector<legacy> sources(4);
    std::vector<std::reference_wrapper<legacy>> references;
    for (int s = 0; s < 4; ++s) {
        for (int i = 0; i < (s == 0 ? 3 : s == 1 ? 16 : 512); ++i)
            sources[s].try_emplace(10000 + s * 4096 + i, i);
        references.push_back(sources[s]);
    }
    const auto expected =
        1u + sources[0].size() + sources[1].size() + sources[2].size() + sources[3].size();
    wjr::experimental::append_btree_disjoint(destination, references.begin(), references.end());
    EXPECT_EQ(destination.size(), expected);
    for (const auto &source : sources)
        EXPECT_TRUE(source.empty());
    std::vector<legacy> counted(3);
    for (int s = 0; s < 3; ++s)
        for (int i = 0; i < 150; ++i)
            counted[s].try_emplace(40000 + s * 4096 + i, i);
    const auto before = destination.size();
    wjr::experimental::append_btree_disjoint(
        destination, std::counted_iterator(counted.begin(), ptrdiff_t(counted.size())),
        std::default_sentinel);
    EXPECT_EQ(destination.size(), before + 450);
}

TEST(BtreeJoinRange, StringSeparatorsAndReverseOrdering) {
    std::vector<wjr::btree_set<std::string>> shards(8);
    std::map<std::string, const std::string *> addresses;
    for (int s = 0; s < 8; ++s) {
        for (int i = 1000; i < 1150; ++i)
            shards[s].insert(std::to_string(s) + std::to_string(i) + std::string(40, 'x'));
        for (const auto &key : shards[s])
            addresses.emplace(key, &key);
    }
    wjr::btree_set<std::string> result;
    wjr::experimental::append_btree_disjoint(result, shards.begin(), shards.end());
    EXPECT_EQ(result.size(), addresses.size());
    for (const auto &key : result)
        EXPECT_EQ(&key, addresses.at(key));
    using reverse_tree = wjr::experimental::compact_btree_map<int, int, std::greater<>>;
    std::vector<reverse_tree> reverse(8);
    for (int s = 0; s < 8; ++s)
        for (int i = 0; i < 150; ++i)
            reverse[s].try_emplace((8 - s) * 4096 + i, i);
    reverse_tree descending;
    wjr::experimental::append_btree_disjoint(descending, reverse.begin(), reverse.end());
    EXPECT_EQ(descending.size(), 1200);
    int previous = 100000;
    for (const auto &entry : descending) {
        EXPECT_LT(entry.first, previous);
        previous = entry.first;
    }
    while (!descending.empty())
        descending.erase(descending.begin());
}

TEST(BtreeJoinRange, DuplicateGroupsAndImmovableLegacyValues) {
    std::vector<wjr::btree_multiset<int>> shards(8);
    std::multiset<int> expected;
    for (int s = 0; s < 8; ++s) {
        for (int i = 0; i < 512; ++i) {
            shards[s].insert(s * 100 + i % 7);
            expected.insert(s * 100 + i % 7);
        }
    }
    wjr::btree_multiset<int> result;
    wjr::experimental::append_btree_disjoint(result, shards.begin(), shards.end());
    EXPECT_TRUE(std::equal(result.begin(), result.end(), expected.begin(), expected.end()));
    EXPECT_EQ(result.erase(203), expected.erase(203));
    EXPECT_EQ(result.size(), expected.size());
    struct immovable {
        int value;
        explicit immovable(int value) noexcept : value(value) {}
        immovable(const immovable &) = delete;
        immovable(immovable &&) = delete;
    };
    using tree = wjr::btree_map<int, immovable>;
    std::vector<tree> owners(8);
    std::map<int, const immovable *> addresses;
    for (int s = 0; s < 8; ++s) {
        for (int i = 0; i < 150; ++i)
            owners[s].try_emplace(s * 4096 + i, i);
        for (const auto &entry : owners[s])
            addresses.emplace(entry.first, &entry.second);
    }
    tree index;
    wjr::experimental::append_btree_disjoint(index, owners.begin(), owners.end());
    for (const auto &entry : index)
        EXPECT_EQ(&entry.second, addresses.at(entry.first));
}

template <typename Tree>
struct fault_iterator {
    using value_type = Tree;
    using difference_type = ptrdiff_t;
    using iterator_concept = std::forward_iterator_tag;
    Tree *position = nullptr;
    size_t *increments = nullptr;
    size_t fail_at = 0;
    Tree &operator*() const { return *position; }
    fault_iterator &operator++() {
        if (++*increments == fail_at)
            throw std::runtime_error("owner iterator increment");
        ++position;
        return *this;
    }
    fault_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }
    bool operator==(const fault_iterator &other) const { return position == other.position; }
};
static_assert(std::forward_iterator<fault_iterator<legacy>>);

TEST(BtreeJoinRange, ThrowingForwardIterationFinishesBeforeMutation) {
    for (size_t failure : {1, 5, 8}) {
        legacy destination;
        std::vector<legacy> sources(4);
        for (int s = 0; s < 4; ++s)
            for (int i = 0; i < 150; ++i)
                sources[s].try_emplace(s * 4096 + i, i);
        size_t increments = 0;
        fault_iterator<legacy> first{sources.data(), &increments, failure};
        fault_iterator<legacy> last{sources.data() + sources.size(), &increments, failure};
        EXPECT_THROW(wjr::experimental::append_btree_disjoint(destination, first, last),
                     std::runtime_error);
        EXPECT_TRUE(destination.empty());
        for (const auto &source : sources)
            EXPECT_EQ(source.size(), 150);
    }
}

TEST(BtreeJoinRange, PreparationFailuresLeaveEverySourceUnchanged) {
    using allocator = owner_allocator<char>;
    using traits = wjr::btree_detail::compact_map_traits<int, payload, std::less<>, allocator, 16>;
    using tree = wjr::experimental::compact_btree_map<int, payload, std::less<>, allocator, 16>;
    for (int failure = 0; failure < 5; ++failure) {
        owner state;
        {
            tree destination(std::less<>{}, allocator(&state, 11));
            std::vector<tree> sources;
            sources.reserve(20);
            for (int s = 0; s < 20; ++s) {
                sources.emplace_back(std::less<>{}, allocator(&state, 100 + s));
                for (int i = 0; i < 150; ++i)
                    sources.back().try_emplace((s + 1) * 4096 + i, i);
            }
            const auto blocks = state.blocks;
            const auto constructions = state.value_constructions;
            const auto destructions = state.value_destructions;
            const auto moves = payload::moves;
            if (failure < 3) {
                state.fail_allocation = state.allocations + failure + 1;
                EXPECT_THROW(wjr::experimental::append_btree_disjoint(destination, sources.begin(),
                                                                      sources.end()),
                             std::bad_alloc);
            } else {
                state.fail_construct_tag = construction_tag<typename traits::inner_node_type *>();
                state.fail_matching_construction = 20 + failure - 2;
                EXPECT_THROW(wjr::experimental::append_btree_disjoint(destination, sources.begin(),
                                                                      sources.end()),
                             std::runtime_error);
            }
            state.fail_allocation = 0;
            state.fail_construct_tag = nullptr;
            EXPECT_TRUE(destination.empty());
            EXPECT_EQ(state.blocks, blocks);
            EXPECT_EQ(state.value_constructions, constructions);
            EXPECT_EQ(state.value_destructions, destructions);
            EXPECT_EQ(payload::moves, moves);
            for (const auto &source : sources) {
                EXPECT_EQ(source.size(), 150);
                for (const auto &entry : source)
                    EXPECT_EQ(entry.second.self, &entry.second);
            }
        }
        EXPECT_TRUE(state.blocks.empty());
        EXPECT_EQ(payload::live, 0);
    }
}

template <size_t N>
void check_repack(int count) {
    using allocator = owner_allocator<char>;
    using tree = wjr::experimental::compact_btree_map<int, payload, std::less<>, allocator, N>;
    owner state;
    {
        tree data(std::less<>{}, allocator(&state, 31));
        for (int i = 0; i < count; ++i)
            data.try_emplace(i, i);
        const auto moves = payload::moves;
        const auto constructions = state.value_constructions;
        const auto destructions = state.value_destructions;
        const auto allocations = state.allocations;
        const auto before = state.live_bytes;
        state.peak_bytes = before;
        state.observe = [&] {
            EXPECT_TRUE(data.empty() || data.size() == static_cast<size_t>(count));
            if (!data.empty()) {
                EXPECT_EQ(data.begin()->first, 0);
                EXPECT_EQ(data.upper_bound(count - 1), data.end());
                EXPECT_EQ(data.at(count / 2).value, count / 2);
            }
        };
        payload::moving = [&] { EXPECT_TRUE(data.empty()); };
        const bool changed = wjr::experimental::repack_compact_btree(data);
        payload::moving = {};
        state.observe = {};
        EXPECT_EQ(data.size(), static_cast<size_t>(count));
        EXPECT_EQ(data.get_allocator().identity, 31);
        EXPECT_EQ(payload::moves - moves, changed ? count : 0);
        EXPECT_EQ(state.value_constructions - constructions, changed ? count : 0);
        EXPECT_EQ(state.value_destructions - destructions, changed ? count : 0);
        if (changed) {
            EXPECT_LT(state.live_bytes, before);
            EXPECT_LT(state.peak_bytes, before * 2);
        } else {
            EXPECT_EQ(state.allocations, allocations);
        }
        for (int i = 0; i < count; ++i) {
            const auto &value = data.at(i);
            EXPECT_EQ(value.self, &value);
            EXPECT_EQ(value.value, i);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(&value) % alignof(payload), 0);
        }
        const auto next_allocations = state.allocations;
        EXPECT_FALSE(wjr::experimental::repack_compact_btree(data));
        EXPECT_EQ(state.allocations, next_allocations);
        while (!data.empty())
            data.erase(data.begin());
    }
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(state.live_bytes, 0);
    EXPECT_EQ(payload::live, 0);
}

TEST(BtreeRepack, TypedLifetimesMinimumLeavesAndPublicEmptyState) {
    for (int count : {0, 1, 2, 3, 16, 17, 32, 33, 64, 150, 512, 4096})
        check_repack<16>(count);
    check_repack<4>(150);
    check_repack<5>(150);
    check_repack<64>(1000);
}

TEST(BtreeRepack, WorkspaceFailurePrecedesMutation) {
    using allocator = owner_allocator<char>;
    using traits = wjr::btree_detail::compact_map_traits<int, payload, std::less<>, allocator, 16>;
    using tree = wjr::experimental::compact_btree_map<int, payload, std::less<>, allocator, 16>;
    for (int failure = 0; failure < 4; ++failure) {
        owner state;
        {
            tree data(std::less<>{}, allocator(&state, 31));
            for (int i = 0; i < 512; ++i)
                data.try_emplace(i, i);
            const auto blocks = state.blocks;
            const auto moves = payload::moves;
            const auto constructions = state.value_constructions;
            const auto destructions = state.value_destructions;
            if (failure == 0) {
                state.fail_allocation = state.allocations + 1;
                EXPECT_THROW(wjr::experimental::repack_compact_btree(data), std::bad_alloc);
            } else {
                state.fail_construct_tag = construction_tag<typename traits::node_type *>();
                state.fail_matching_construction = failure == 1 ? 1 : failure == 2 ? 16 : 32;
                EXPECT_THROW(wjr::experimental::repack_compact_btree(data), std::runtime_error);
            }
            state.fail_allocation = 0;
            state.fail_construct_tag = nullptr;
            EXPECT_EQ(data.size(), 512);
            EXPECT_EQ(state.blocks, blocks);
            EXPECT_EQ(payload::moves, moves);
            EXPECT_EQ(state.value_constructions, constructions);
            EXPECT_EQ(state.value_destructions, destructions);
            for (int i = 0; i < 512; ++i)
                EXPECT_EQ(data.at(i).value, i);
        }
        EXPECT_TRUE(state.blocks.empty());
        EXPECT_EQ(payload::live, 0);
    }
}

TEST(BtreeRepack, ComparatorInstanceAndMappedResourceArePreserved) {
    struct comparison {
        bool reverse;
        mutable int calls = 0;
        bool operator()(int lhs, int rhs) const noexcept {
            ++calls;
            return reverse ? lhs > rhs : lhs < rhs;
        }
    };
    std::pmr::unsynchronized_pool_resource resource;
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree = wjr::experimental::compact_btree_map<int, std::pmr::string, comparison, allocator>;
    tree data(comparison{true}, allocator(&resource));
    for (int i = 0; i < 512; ++i)
        data.try_emplace(i, 96, static_cast<char>('a' + i % 20));
    const auto *instance = &data.key_comp();
    const auto calls = data.key_comp().calls;
    EXPECT_TRUE(wjr::experimental::repack_compact_btree(data));
    EXPECT_EQ(&data.key_comp(), instance);
    EXPECT_EQ(data.key_comp().calls, calls);
    EXPECT_TRUE(data.key_comp().reverse);
    EXPECT_EQ(data.get_allocator().resource(), &resource);
    for (int i = 0; i < 512; ++i) {
        EXPECT_EQ(data.at(i).get_allocator().resource(), &resource);
        EXPECT_EQ(data.at(i), std::pmr::string(96, static_cast<char>('a' + i % 20)));
    }
    int previous = 512;
    for (const auto &value : data) {
        EXPECT_LT(value.first, previous);
        previous = value.first;
    }
}
} // namespace
