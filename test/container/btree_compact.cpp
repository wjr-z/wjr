#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <memory_resource>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

namespace {
struct counted {
    static inline int live = 0;
    const counted *self = this;
    int value;
    explicit counted(int v = 0) noexcept : value(v) { ++live; }
    counted(const counted &other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++live;
    }
    counted(counted &&other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++live;
        other.value = -1;
    }
    counted &operator=(const counted &) = delete;
    counted &operator=(counted &&) = delete;
    ~counted() noexcept {
        EXPECT_EQ(self, this);
        self = nullptr;
        --live;
    }
};
static_assert(wjr::get_relocate_mode_v<counted> == wjr::relocate_t::normal);
static_assert(wjr::get_relocate_mode_v<std::pair<const int, counted>> == wjr::relocate_t::normal);
static_assert(wjr::get_common_relocate_mode_v<int, int, counted> == wjr::relocate_t::normal);
struct throwing_move {
    throwing_move(throwing_move &&) noexcept(false);
};
template <class Key, class Value>
concept supports_compact = requires { typename wjr::experimental::compact_btree_map<Key, Value>; };
static_assert(!supports_compact<std::string, int>);
static_assert(!supports_compact<int, throwing_move>);
static_assert(supports_compact<int, std::unique_ptr<int>>);
using integer_tree = wjr::experimental::compact_btree_map<int, int>;
static_assert(std::same_as<integer_tree::reference, std::pair<const int, int> &>);
static_assert(
    !std::is_assignable_v<decltype((std::declval<integer_tree::reference>().first)), int>);
static_assert(sizeof(wjr::btree_map<int, int>) == 48);

template <class Tree, class Reference>
void check(const Tree &tree, const Reference &reference) {
    ASSERT_EQ(tree.size(), reference.size());
    auto it = tree.begin();
    for (const auto &[key, val] : reference) {
        ASSERT_NE(it, tree.end());
        EXPECT_EQ(it->first, key);
        if constexpr (std::same_as<typename Tree::mapped_type, counted>) {
            EXPECT_EQ(it->second.value, val);
            EXPECT_EQ(it->second.self, &it->second);
        } else {
            EXPECT_EQ(it->second, val);
        }
        ++it;
    }
    EXPECT_EQ(it, tree.end());
    auto rev = tree.rbegin();
    for (auto it = reference.rbegin(); it != reference.rend(); ++it, ++rev) {
        ASSERT_NE(rev, tree.rend());
        EXPECT_EQ(rev->first, it->first);
    }
    EXPECT_EQ(rev, tree.rend());
}

template <size_t Capacity>
void exercise_relocation() {
    using tree = wjr::experimental::compact_btree_map<int, counted, std::less<int>,
                                                      std::allocator<char>, Capacity>;
    std::mt19937 random(73 + Capacity);
    std::map<int, int> reference;
    tree values;
    for (int i = 0; i != 8000; ++i) {
        const int key = random() % 700;
        if ((random() % 4) != 0) {
            auto [it, inserted] = values.try_emplace(key, key * 3);
            EXPECT_EQ(inserted, reference.emplace(key, key * 3).second);
            EXPECT_EQ(it->second.value, key * 3);
        } else {
            auto it = values.lower_bound(key);
            auto ref = reference.lower_bound(key);
            if (ref != reference.end()) {
                ASSERT_NE(it, values.end());
                auto next = values.erase(it);
                auto expected = reference.erase(ref);
                ASSERT_EQ(next == values.end(), expected == reference.end());
                if (next != values.end())
                    EXPECT_EQ(next->first, expected->first);
            }
        }
        if (i % 67 == 0)
            check(values, reference);
    }
    check(values, reference);
    tree copy(values);
    check(copy, reference);
    tree assigned;
    assigned = copy;
    check(assigned, reference);
    tree moved(std::move(copy));
    EXPECT_TRUE(copy.empty());
    check(moved, reference);
    assigned = std::move(moved);
    EXPECT_TRUE(moved.empty());
    check(assigned, reference);
    copy.clear();
    values.clear();
    while (!assigned.empty()) {
        auto it = assigned.begin();
        std::advance(it, random() % assigned.size());
        reference.erase(it->first);
        auto next = assigned.erase(it);
        if (next != assigned.end())
            EXPECT_TRUE(reference.contains(next->first));
        if (reference.size() % 31 == 0)
            check(assigned, reference);
    }
}
TEST(CompactBtree, NontrivialLifetimeAcrossSplitBorrowMerge) {
    exercise_relocation<4>();
    exercise_relocation<5>();
    exercise_relocation<16>();
    exercise_relocation<32>();
    EXPECT_EQ(counted::live, 0);
}

struct hook_state {
    std::unordered_map<void *, int> values;
    std::unordered_map<void *, std::pair<size_t, int>> blocks;
    int allocations = 0;
    int constructions = 0;
    int destructions = 0;
};
template <class T, bool Propagate = true>
struct hook_allocator {
    using value_type = T;
    using propagate_on_container_swap = std::bool_constant<Propagate>;
    using propagate_on_container_move_assignment = std::bool_constant<Propagate>;
    using propagate_on_container_copy_assignment = std::bool_constant<Propagate>;
    using is_always_equal = std::false_type;
    template <class U>
    struct rebind {
        using other = hook_allocator<U, Propagate>;
    };
    hook_state *state = nullptr;
    int id = 0;
    hook_allocator() = default;
    hook_allocator(hook_state &s, int i) : state(&s), id(i) {}
    template <class U>
    hook_allocator(const hook_allocator<U, Propagate> &other) noexcept
        : state(other.state), id(other.id) {}
    T *allocate(size_t n) {
        T *p = std::allocator<T>{}.allocate(n);
        EXPECT_TRUE(state->blocks.emplace(p, std::pair{n * sizeof(T), id}).second);
        ++state->allocations;
        return p;
    }
    void deallocate(T *p, size_t n) {
        auto it = state->blocks.find(p);
        ASSERT_NE(it, state->blocks.end());
        EXPECT_EQ(it->second, (std::pair{n * sizeof(T), id}));
        state->blocks.erase(it);
        std::allocator<T>{}.deallocate(p, n);
    }
    template <class U, class... Args>
    void construct(U *p, Args &&...args) noexcept {
        std::construct_at(p, std::forward<Args>(args)...);
        if constexpr ((std::same_as<U, std::pair<const int, counted>> ||
                       std::same_as<U, std::pair<const int, int>>)) {
            EXPECT_TRUE(state->values.emplace(p, id).second);
            ++state->constructions;
        }
    }
    template <class U>
    void destroy(U *p) noexcept {
        if constexpr ((std::same_as<U, std::pair<const int, counted>> ||
                       std::same_as<U, std::pair<const int, int>>)) {
            auto it = state->values.find(p);
            ASSERT_NE(it, state->values.end());
            EXPECT_EQ(it->second, id);
            state->values.erase(it);
            ++state->destructions;
        }
        std::destroy_at(p);
    }
    template <class U>
    bool operator==(const hook_allocator<U, Propagate> &other) const noexcept {
        return state == other.state && id == other.id;
    }
};

TEST(CompactBtree, AllocatorHooksSmallMoveSwapCopyAndUnequalMove) {
    hook_state state;
    using allocator = hook_allocator<char>;
    using tree = wjr::experimental::compact_btree_map<int, counted, std::less<int>, allocator>;
    for (int left_size : {0, 1, 2, 3, 80}) {
        for (int right_size : {0, 1, 2, 3, 80}) {
            tree lhs(std::less<int>{}, allocator(state, 1));
            tree rhs(std::less<int>{}, allocator(state, 2));
            for (int i = 0; i < left_size; ++i)
                lhs.try_emplace(i, i);
            for (int i = 0; i < right_size; ++i)
                rhs.try_emplace(i + 100, i + 100);
            const int before = state.allocations;
            lhs.swap(rhs);
            EXPECT_EQ(lhs.size(), right_size);
            EXPECT_EQ(rhs.size(), left_size);
            EXPECT_EQ(state.allocations, before);
            lhs.swap(lhs);
            tree moved(std::move(lhs));
            EXPECT_TRUE(lhs.empty());
            tree copied(rhs);
            copied = rhs;
            moved = std::move(rhs);
            EXPECT_TRUE(rhs.empty());
        }
    }
    EXPECT_TRUE(state.values.empty());
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(state.constructions, state.destructions);
    EXPECT_EQ(counted::live, 0);
    using fixed_allocator = hook_allocator<char, false>;
    using fixed_tree =
        wjr::experimental::compact_btree_map<int, counted, std::less<int>, fixed_allocator>;
    {
        fixed_tree lhs(std::less<int>{}, fixed_allocator(state, 3));
        fixed_tree rhs(std::less<int>{}, fixed_allocator(state, 4));
        for (int i = 0; i != 50; ++i)
            lhs.try_emplace(i, i);
        rhs = std::move(lhs);
        EXPECT_EQ(rhs.size(), 50u);
        for (const auto &[k, v] : rhs)
            EXPECT_EQ(k, v.value);
    }
    EXPECT_TRUE(state.values.empty());
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(counted::live, 0);
}

struct alignas(64) aligned_payload {
    std::unique_ptr<int> value;
    explicit aligned_payload(int n) : value(std::make_unique<int>(n)) {}
};
TEST(CompactBtree, MoveOnlyOveralignedAndNoValueAllocations) {
    hook_state state;
    using tree =
        wjr::experimental::compact_btree_map<int, int, std::less<int>, hook_allocator<char>>;
    {
        tree values(std::less<int>{}, hook_allocator<char>(state, 8));
        values.try_emplace(1, 1);
        values.try_emplace(2, 2);
        EXPECT_EQ(state.allocations, 0);
        for (int i = 3; i <= 500; ++i)
            values.try_emplace(i, i);
        EXPECT_LT(state.allocations, 100);
    }
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_TRUE(state.values.empty());
    EXPECT_EQ(state.constructions, state.destructions);
    wjr::experimental::compact_btree_map<int, aligned_payload> aligned;
    for (int i = 0; i != 300; ++i)
        aligned.try_emplace(i, i);
    for (auto &[key, value] : aligned) {
        EXPECT_EQ(reinterpret_cast<uintptr_t>(&value) % 64, 0u);
        ASSERT_TRUE(value.value);
        EXPECT_EQ(*value.value, key);
    }
    while (!aligned.empty())
        aligned.erase(aligned.begin());
}
TEST(CompactBtree, AllocatorAwareMappedValuesKeepResource) {
    std::pmr::unsynchronized_pool_resource pool;
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree =
        wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<>, allocator>;
    tree values(std::less<>{}, allocator(&pool));
    for (int i = 0; i != 150; ++i)
        values.try_emplace(i, 128, char('a' + i % 26));
    for (const auto &[key, text] : values) {
        EXPECT_EQ(text.get_allocator().resource(), &pool);
        EXPECT_EQ(text, std::pmr::string(128, char('a' + key % 26), &pool));
    }
    while (!values.empty())
        values.erase(values.begin());
}

TEST(CompactBtree, RelocatableMoveOnlyValuesAndDuplicateConstruction) {
    wjr::experimental::compact_btree_map<int, std::unique_ptr<int>> values;
    for (int i = 0; i != 300; ++i)
        values.try_emplace(i, std::make_unique<int>(i));
    for (int i = 0; i != 300; i += 2)
        values.erase(values.lower_bound(i));
    auto moved = std::move(values);
    EXPECT_TRUE(values.empty());
    for (const auto &[key, value] : moved) {
        ASSERT_TRUE(value);
        EXPECT_EQ(*value, key);
    }
    wjr::experimental::compact_btree_map<int, counted> counted_values;
    counted_values.emplace(std::piecewise_construct, std::forward_as_tuple(2),
                           std::forward_as_tuple(20));
    const int live = counted::live;
    auto [position, inserted] = counted_values.emplace(
        std::piecewise_construct, std::forward_as_tuple(2), std::forward_as_tuple(99));
    EXPECT_FALSE(inserted);
    EXPECT_EQ(position->second.value, 20);
    EXPECT_EQ(counted::live, live);
}
} // namespace
