#include <wjr/container/experimental/btree_bulk.hpp>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory_resource>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace {
template <typename Tree, typename Iter, typename Sentinel = Iter>
concept accepts_packed = requires(Iter first, Sentinel last) {
    { wjr::experimental::make_btree_from_sorted_unique<Tree>(first, last) } -> std::same_as<Tree>;
};
static_assert(accepts_packed<wjr::btree_set<int>, const int *>);
static_assert(!accepts_packed<wjr::btree_set<int>, std::istream_iterator<int>>);
static_assert(accepts_packed<wjr::btree_set<int>, std::counted_iterator<std::istream_iterator<int>>,
                             std::default_sentinel_t>);
static_assert(!accepts_packed<std::set<int>, const int *>);

struct allocation_state {
    std::map<void *, std::pair<size_t, size_t>> blocks;
    size_t current = 0;
    size_t peak = 0;
    size_t requests = 0;
    size_t pointer_requests = 0;
    size_t fail_pointer_request = 0;
    size_t value_constructions = 0;
    size_t value_destructions = 0;
};

template <typename T>
struct bulk_allocator {
    using value_type = T;
    allocation_state *state = nullptr;
    bulk_allocator() = default;
    explicit bulk_allocator(allocation_state *state) : state(state) {}
    template <typename U>
    bulk_allocator(const bulk_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(size_t count) {
        if constexpr (std::is_pointer_v<T>) {
            ++state->pointer_requests;
            if (state->pointer_requests == state->fail_pointer_request)
                throw std::bad_alloc();
        }
        auto *ptr = std::allocator<T>{}.allocate(count);
        EXPECT_TRUE(state->blocks.emplace(ptr, std::pair{count, alignof(T)}).second);
        state->current += count * sizeof(T);
        state->peak = std::max(state->peak, state->current);
        ++state->requests;
        return ptr;
    }
    void deallocate(T *ptr, size_t count) noexcept {
        const auto found = state->blocks.find(ptr);
        ASSERT_NE(found, state->blocks.end());
        EXPECT_EQ(found->second, (std::pair{count, alignof(T)}));
        state->blocks.erase(found);
        state->current -= count * sizeof(T);
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
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        std::destroy_at(ptr);
        if constexpr (requires(U &value) {
                          value.first;
                          value.second;
                      })
            ++state->value_destructions;
    }
    template <typename U>
    bool operator==(const bulk_allocator<U> &other) const noexcept {
        return state == other.state;
    }
};

struct alignas(64) tracked {
    static inline int live = 0;
    const tracked *self = this;
    int value;
    explicit tracked(int value) noexcept : value(value) { ++live; }
    tracked(const tracked &other) noexcept : value(other.value) { ++live; }
    tracked(tracked &&other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++live;
        other.value = -1;
    }
    ~tracked() noexcept {
        EXPECT_EQ(self, this);
        --live;
    }
};

template <typename Tree>
void check_shapes() {
    for (int count :
         {0, 1, 2, 3, 4, 5, 16, 17, 31, 32, 33, 64, 255, 256, 257, 272, 273, 274, 4096, 4353}) {
        std::vector<std::pair<int, int>> input;
        for (int i = 0; i < count; ++i)
            input.emplace_back(i * 2, i * 3);
        auto tree =
            wjr::experimental::make_btree_from_sorted_unique<Tree>(input.begin(), input.end());
        EXPECT_EQ(tree.size(), input.size());
        for (int i = 0; i < count; ++i) {
            EXPECT_EQ(tree.at(i * 2), i * 3);
            EXPECT_FALSE(tree.contains(i * 2 + 1));
            EXPECT_EQ(tree.lower_bound(i * 2)->first, i * 2);
        }
        auto copy = tree;
        EXPECT_EQ(copy, tree);
        Tree moved(std::move(copy));
        EXPECT_TRUE(copy.empty());
        tree.swap(moved);
        std::vector<int> keys;
        for (int i = 0; i < count; ++i)
            keys.push_back(i * 2);
        std::mt19937 random(42);
        std::shuffle(keys.begin(), keys.end(), random);
        for (int key : keys)
            EXPECT_EQ(tree.erase(key), 1);
        EXPECT_TRUE(tree.empty());
        for (int i = 0; i < count; ++i)
            tree.try_emplace(i * 2 + 1, i);
        auto first = moved.begin();
        std::advance(first, count / 3);
        auto last = first;
        std::advance(last, count / 3);
        moved.erase(first, last);
        EXPECT_EQ(moved.size(), static_cast<size_t>(count - count / 3));
    }
}

TEST(BtreeBulk, BalancedShapesRemainMutable) {
    check_shapes<wjr::btree_map<int, int>>();
    check_shapes<wjr::experimental::compact_btree_map<int, int>>();
    check_shapes<wjr::btree_map<int, int, std::less<>, std::allocator<char>, 4>>();
    check_shapes<
        wjr::experimental::compact_btree_map<int, int, std::less<>, std::allocator<char>, 5>>();
}

TEST(BtreeBulk, ComparatorStateAndIndirectSeparators) {
    std::vector<std::string> input;
    for (int i = 300; i >= 100; --i)
        input.push_back(std::to_string(i) + std::string(40, 'x'));
    using tree_type = wjr::btree_set<std::string, std::greater<>>;
    auto tree =
        wjr::experimental::make_btree_from_sorted_unique<tree_type>(input.begin(), input.end());
    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), input.begin(), input.end()));
    for (const auto &key : input)
        EXPECT_TRUE(tree.contains(key));
    tree.erase(std::next(tree.begin(), 20), std::next(tree.begin(), 180));
    EXPECT_EQ(tree.size(), input.size() - 160);
    std::vector<int> integers{1, 2, 3, 4, 5};
    auto multi = wjr::experimental::make_btree_from_sorted_unique<wjr::btree_multiset<int>>(
        integers.begin(), integers.end());
    multi.insert(3);
    EXPECT_EQ(multi.count(3), 2);
    struct stateful_compare {
        bool reverse;
        bool operator()(int lhs, int rhs) const noexcept { return reverse ? lhs > rhs : lhs < rhs; }
    };
    std::reverse(integers.begin(), integers.end());
    using stateful_tree = wjr::btree_set<int, stateful_compare>;
    auto stateful = wjr::experimental::make_btree_from_sorted_unique<stateful_tree>(
        integers.begin(), integers.end(), stateful_compare{true}, std::allocator<char>{});
    EXPECT_TRUE(stateful.key_comp().reverse);
    EXPECT_TRUE(std::equal(stateful.begin(), stateful.end(), integers.begin(), integers.end()));
}

TEST(BtreeBulk, SizedSinglePassAndMoveOnlyInput) {
    std::istringstream source("1 2 3 4 5 6 7");
    auto first = std::counted_iterator(std::istream_iterator<int>(source), 7);
    auto tree = wjr::experimental::make_btree_from_sorted_unique<wjr::btree_set<int>>(
        first, std::default_sentinel);
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(*tree.rbegin(), 7);
    std::vector<std::pair<int, std::unique_ptr<int>>> input;
    for (int i = 0; i < 100; ++i)
        input.emplace_back(i, std::make_unique<int>(i * 2));
    using compact = wjr::experimental::compact_btree_map<int, std::unique_ptr<int>>;
    auto moved = wjr::experimental::make_btree_from_sorted_unique<compact>(
        std::make_move_iterator(input.begin()), std::make_move_iterator(input.end()));
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(*moved.at(i), i * 2);
        EXPECT_FALSE(input[i].second);
    }
}

template <bool Compact>
void check_allocator() {
    using alloc = bulk_allocator<char>;
    using legacy = wjr::btree_map<int, tracked, std::less<>, alloc>;
    using compact = wjr::experimental::compact_btree_map<int, tracked, std::less<>, alloc>;
    using tree_type = std::conditional_t<Compact, compact, legacy>;
    for (int count : {0, 1, 2, 3, 17, 273, 4096}) {
        allocation_state state;
        {
            std::vector<std::pair<int, tracked>> input;
            for (int i = 0; i < count; ++i)
                input.emplace_back(i, tracked(i));
            const auto live = tracked::live;
            {
                auto tree = wjr::experimental::make_btree_from_sorted_unique<tree_type>(
                    input.begin(), input.end(), std::less<>{}, alloc(&state));
                EXPECT_EQ(tree.get_allocator().state, &state);
                EXPECT_EQ(tracked::live, live + count);
                EXPECT_EQ(state.value_constructions, static_cast<size_t>(count));
                for (const auto &entry : tree) {
                    EXPECT_EQ(reinterpret_cast<uintptr_t>(&entry.second) % alignof(tracked), 0);
                    EXPECT_EQ(entry.second.self, &entry.second);
                }
            }
            EXPECT_EQ(tracked::live, live);
            EXPECT_EQ(state.value_destructions, static_cast<size_t>(count));
        }
        EXPECT_EQ(tracked::live, 0);
        EXPECT_TRUE(state.blocks.empty());
        EXPECT_EQ(state.current, 0);
    }
}

TEST(BtreeBulk, ReboundOwnerAndOveralignedLifetimes) {
    check_allocator<false>();
    check_allocator<true>();
    std::pmr::unsynchronized_pool_resource resource;
    using tree_type =
        wjr::btree_map<int, std::pmr::string, std::less<>, std::pmr::polymorphic_allocator<char>>;
    std::vector<std::pair<int, std::string>> input;
    for (int i = 0; i < 100; ++i)
        input.emplace_back(i, std::string(80, 'a'));
    auto tree = wjr::experimental::make_btree_from_sorted_unique<tree_type>(
        input.begin(), input.end(), std::less<>{}, tree_type::allocator_type(&resource));
    for (const auto &entry : tree)
        EXPECT_EQ(entry.second.get_allocator().resource(), &resource);
    using compact = wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<>,
                                                         std::pmr::polymorphic_allocator<char>>;
    auto packed = wjr::experimental::make_btree_from_sorted_unique<compact>(
        input.begin(), input.end(), std::less<>{}, compact::allocator_type(&resource));
    for (const auto &entry : packed)
        EXPECT_EQ(entry.second.get_allocator().resource(), &resource);
}

struct throwing_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<int, tracked>;
    using difference_type = ptrdiff_t;
    value_type *position = nullptr;
    value_type *failure = nullptr;
    bool fail_increment = false;
    value_type &operator*() const {
        if (!fail_increment && position == failure)
            throw std::runtime_error("input dereference");
        return *position;
    }
    throwing_iterator &operator++() {
        if (fail_increment && position == failure)
            throw std::runtime_error("input increment");
        ++position;
        return *this;
    }
    throwing_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }
    bool operator==(const throwing_iterator &other) const { return position == other.position; }
    difference_type operator-(const throwing_iterator &other) const {
        return position - other.position;
    }
};
static_assert(std::forward_iterator<throwing_iterator>);

TEST(BtreeBulk, InputAndWorkspaceFailuresReleasePartialForest) {
    using alloc = bulk_allocator<char>;
    using tree_type = wjr::experimental::compact_btree_map<int, tracked, std::less<>, alloc>;
    std::vector<std::pair<int, tracked>> input;
    for (int i = 0; i < 4096; ++i)
        input.emplace_back(i, tracked(i));
    const auto live = tracked::live;
    for (int count : {1, 2, 3, 17, 4096}) {
        for (int failure : {0, count / 2, count - 1}) {
            for (bool increment : {false, true}) {
                allocation_state state;
                throwing_iterator first{input.data(), input.data() + failure, increment};
                throwing_iterator last{input.data() + count};
                EXPECT_THROW((wjr::experimental::make_btree_from_sorted_unique<tree_type>(
                                 first, last, std::less<>{}, alloc(&state))),
                             std::runtime_error);
                EXPECT_EQ(tracked::live, live);
                EXPECT_TRUE(state.blocks.empty());
                EXPECT_EQ(state.current, 0);
            }
        }
    }
    for (size_t failure : {1, 2, 3}) {
        allocation_state state;
        state.fail_pointer_request = failure;
        EXPECT_THROW((wjr::experimental::make_btree_from_sorted_unique<tree_type>(
                         input.begin(), input.end(), std::less<>{}, alloc(&state))),
                     std::bad_alloc);
        EXPECT_EQ(tracked::live, live);
        EXPECT_TRUE(state.blocks.empty());
        EXPECT_EQ(state.current, 0);
    }
}
} // namespace
