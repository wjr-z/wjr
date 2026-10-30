#include <map>
#include <memory>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace {
struct allocation_state {
    static inline allocation_state *selected = nullptr;
    static inline std::map<void *, allocation_state *> owners;
    int outstanding = 0;
    int wrong_deallocations = 0;
};

template <typename T, bool CopyPropagate, bool MovePropagate>
struct tracking_allocator {
    using value_type = T;
    using propagate_on_container_copy_assignment = std::bool_constant<CopyPropagate>;
    using propagate_on_container_move_assignment = std::bool_constant<MovePropagate>;
    using is_always_equal = std::false_type;
    template <typename U>
    struct rebind {
        using other = tracking_allocator<U, CopyPropagate, MovePropagate>;
    };

    allocation_state *state = allocation_state::selected;

    tracking_allocator() = default;
    template <typename U>
    tracking_allocator(const tracking_allocator<U, CopyPropagate, MovePropagate> &other) noexcept
        : state(other.state) {}

    T *allocate(size_t n) {
        auto *ptr = std::allocator<T>{}.allocate(n);
        allocation_state::owners.emplace(ptr, state);
        ++state->outstanding;
        return ptr;
    }

    void deallocate(T *ptr, size_t n) noexcept {
        const auto iter = allocation_state::owners.find(ptr);
        if (iter == allocation_state::owners.end()) {
            ADD_FAILURE() << "Allocation was already released";
            return;
        }
        if (iter->second != state) {
            ++state->wrong_deallocations;
        }
        --iter->second->outstanding;
        allocation_state::owners.erase(iter);
        std::allocator<T>{}.deallocate(ptr, n);
    }

    bool operator==(const tracking_allocator &) const = default;
};

struct directional_compare {
    bool descending = false;
    bool operator()(int lhs, int rhs) const noexcept { return descending ? lhs > rhs : lhs < rhs; }
};

template <typename Tree>
void fill_tree(Tree &tree, int count) {
    for (int i = 0; i != count; ++i) {
        if constexpr (std::is_void_v<typename Tree::mapped_type>) {
            tree.emplace(i);
        } else {
            tree.emplace(i, std::to_string(i));
        }
    }
}

template <typename Tree>
void check_tree(const Tree &tree, int count, bool descending) {
    ASSERT_EQ(tree.size(), static_cast<size_t>(count));
    EXPECT_EQ(tree.key_comp().descending, descending);
    auto iter = tree.begin();
    for (int i = 0; i != count; ++i) {
        ASSERT_NE(iter, tree.end());
        const int key = descending ? count - i - 1 : i;
        if constexpr (std::is_void_v<typename Tree::mapped_type>) {
            EXPECT_EQ(*iter, key);
        } else {
            EXPECT_EQ(iter->first, key);
            EXPECT_EQ(iter->second, std::to_string(key));
        }
        EXPECT_EQ(tree.lower_bound(key), iter);
        ++iter;
    }
    EXPECT_EQ(iter, tree.end());
}

template <typename Tree>
void check_move_to_small() {
    for (int count : {0, 1, 2}) {
        Tree target(directional_compare{false});
        Tree source(directional_compare{true});
        fill_tree(target, 80);
        fill_tree(source, count);
        target = std::move(source);
        check_tree(target, count, true);
        check_tree(source, 0, true);
        target.clear();
        fill_tree(target, 40);
        check_tree(target, 40, true);
    }
}

TEST(BtreeAssignment, MoveFromSmallReplacesFreedRoot) {
    check_move_to_small<wjr::btree_map<int, std::string, directional_compare>>();
    check_move_to_small<wjr::btree_set<int, directional_compare>>();
}

template <typename Tree, bool Move>
void check_assignment(bool equal_allocators, bool propagates) {
    for (int target_count : {0, 2, 80}) {
        for (int source_count : {0, 1, 2, 3, 80}) {
            SCOPED_TRACE(::testing::Message()
                         << "target=" << target_count << ", source=" << source_count
                         << ", equal_allocators=" << equal_allocators);
            allocation_state lhs_state;
            allocation_state rhs_state;
            {
                allocation_state::selected = &lhs_state;
                Tree target(directional_compare{false});
                fill_tree(target, target_count);
                allocation_state::selected = equal_allocators ? &lhs_state : &rhs_state;
                Tree source(directional_compare{true});
                fill_tree(source, source_count);
                if constexpr (Move) {
                    target = std::move(source);
                } else {
                    target = source;
                    check_tree(source, source_count, true);
                }
                check_tree(target, source_count, true);
                if (!equal_allocators && propagates) {
                    EXPECT_EQ(lhs_state.outstanding, 0);
                }
                target.clear();
                auto &expected_state = propagates && !equal_allocators ? rhs_state : lhs_state;
                const int allocations_before = expected_state.outstanding;
                fill_tree(target, 20);
                EXPECT_GT(expected_state.outstanding, allocations_before);
                check_tree(target, 20, true);
            }
            EXPECT_EQ(lhs_state.outstanding, 0);
            EXPECT_EQ(rhs_state.outstanding, 0);
            EXPECT_EQ(lhs_state.wrong_deallocations, 0);
            EXPECT_EQ(rhs_state.wrong_deallocations, 0);
        }
    }
    allocation_state::selected = nullptr;
    EXPECT_TRUE(allocation_state::owners.empty());
}

TEST(BtreeAssignment, CopyPreservesAllocatorOwnershipAndComparator) {
    using propagating = tracking_allocator<char, true, false>;
    using retaining = tracking_allocator<char, false, false>;
    for (bool equal_allocators : {false, true}) {
        check_assignment<wjr::btree_map<int, std::string, directional_compare, propagating>, false>(
            equal_allocators, true);
        check_assignment<wjr::btree_set<int, directional_compare, propagating>, false>(
            equal_allocators, true);
        check_assignment<wjr::btree_map<int, std::string, directional_compare, retaining>, false>(
            equal_allocators, false);
        check_assignment<wjr::btree_set<int, directional_compare, retaining>, false>(
            equal_allocators, false);
    }
}

TEST(BtreeAssignment, MovePreservesAllocatorOwnershipAndComparator) {
    using propagating = tracking_allocator<char, false, true>;
    using retaining = tracking_allocator<char, false, false>;
    for (bool equal_allocators : {false, true}) {
        check_assignment<wjr::btree_map<int, std::string, directional_compare, propagating>, true>(
            equal_allocators, true);
        check_assignment<wjr::btree_set<int, directional_compare, propagating>, true>(
            equal_allocators, true);
        check_assignment<wjr::btree_map<int, std::string, directional_compare, retaining>, true>(
            equal_allocators, false);
        check_assignment<wjr::btree_set<int, directional_compare, retaining>, true>(
            equal_allocators, false);
    }
}
} // namespace
