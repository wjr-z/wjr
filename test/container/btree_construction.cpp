#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace {
struct construction_state {
    static inline construction_state *selected = nullptr;
    static inline std::map<void *, construction_state *> owners;
    construction_state *copy_state = nullptr;
    int outstanding = 0;
    int wrong_releases = 0;
    int selections = 0;
    int allocator_moves = 0;
    int comparator_copies = 0;
    int comparator_moves = 0;
};

template <typename T>
struct construction_allocator {
    using value_type = T;
    construction_state *state = construction_state::selected;
    construction_allocator() noexcept = default;
    explicit construction_allocator(construction_state *s) noexcept : state(s) {}
    construction_allocator(const construction_allocator &) noexcept = default;
    construction_allocator(construction_allocator &&other) noexcept : state(other.state) {
        ++state->allocator_moves;
    }
    template <typename U>
    construction_allocator(const construction_allocator<U> &other) noexcept : state(other.state) {}

    construction_allocator select_on_container_copy_construction() const noexcept {
        ++state->selections;
        return construction_allocator(state->copy_state);
    }

    T *allocate(size_t count) {
        auto *ptr = std::allocator<T>{}.allocate(count);
        construction_state::owners.emplace(ptr, state);
        ++state->outstanding;
        return ptr;
    }

    void deallocate(T *ptr, size_t count) noexcept {
        const auto iter = construction_state::owners.find(ptr);
        if (iter == construction_state::owners.end()) {
            ADD_FAILURE() << "Allocation was already released";
            return;
        }
        if (iter->second != state) {
            ++state->wrong_releases;
        }
        --iter->second->outstanding;
        construction_state::owners.erase(iter);
        std::allocator<T>{}.deallocate(ptr, count);
    }

    bool operator==(const construction_allocator &) const = default;
};

struct construction_compare {
    construction_state *state;
    bool descending = true;
    explicit construction_compare(construction_state &s) noexcept : state(&s) {}
    construction_compare(const construction_compare &other) noexcept
        : state(other.state), descending(other.descending) {
        ++state->comparator_copies;
    }
    construction_compare(construction_compare &&other) noexcept
        : state(other.state), descending(other.descending) {
        ++state->comparator_moves;
        other.descending = false;
    }
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
void check_tree(const Tree &tree, int count) {
    EXPECT_EQ(tree.size(), static_cast<size_t>(count));
    EXPECT_TRUE(tree.key_comp().descending);
    auto iter = tree.begin();
    for (int key = count - 1; key >= 0; --key) {
        ASSERT_NE(iter, tree.end());
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

template <typename Tree, bool Move>
void check_construction() {
    for (int count : {0, 1, 2, 3, 80}) {
        SCOPED_TRACE(count);
        construction_state source_state;
        construction_state selected_state;
        construction_state default_state;
        source_state.copy_state = &selected_state;
        {
            construction_state::selected = &source_state;
            Tree source{construction_compare(source_state)};
            fill_tree(source, count);
            construction_state::selected = &default_state;
            const int copies_before = source_state.comparator_copies;
            const int moves_before = source_state.comparator_moves;
            const int allocator_moves_before = source_state.allocator_moves;
            Tree target = [&]() {
                if constexpr (Move) {
                    return Tree(std::move(source));
                } else {
                    return Tree(source);
                }
            }();
            check_tree(target, count);
            EXPECT_EQ(default_state.outstanding, 0);
            if constexpr (Move) {
                EXPECT_EQ(source_state.comparator_copies, copies_before);
                EXPECT_EQ(source_state.comparator_moves, moves_before + 1);
                EXPECT_EQ(source_state.allocator_moves, allocator_moves_before + 1);
                EXPECT_EQ(source_state.selections, 0);
                EXPECT_TRUE(source.empty());
                fill_tree(source, 20);
                EXPECT_EQ(source.size(), 20u);
            } else {
                EXPECT_EQ(source_state.comparator_copies, copies_before + 1);
                EXPECT_EQ(source_state.comparator_moves, moves_before);
                EXPECT_EQ(source_state.selections, 1);
                check_tree(source, count);
            }
            target.clear();
            fill_tree(target, 20);
            check_tree(target, 20);
            EXPECT_EQ(default_state.outstanding, 0);
            if constexpr (Move) {
                EXPECT_GT(source_state.outstanding, 0);
            } else {
                EXPECT_GT(selected_state.outstanding, 0);
            }
        }
        for (auto *state : {&source_state, &selected_state, &default_state}) {
            EXPECT_EQ(state->outstanding, 0);
            EXPECT_EQ(state->wrong_releases, 0);
        }
    }
    construction_state::selected = nullptr;
    EXPECT_TRUE(construction_state::owners.empty());
}

TEST(BtreeConstruction, CopySelectsAllocatorAndCopiesComparator) {
    using allocator = construction_allocator<char>;
    check_construction<wjr::btree_map<int, std::string, construction_compare, allocator>, false>();
    check_construction<wjr::btree_set<int, construction_compare, allocator>, false>();
}

TEST(BtreeConstruction, MoveTransfersAllocatorAndComparatorWithStorage) {
    using allocator = construction_allocator<char>;
    check_construction<wjr::btree_map<int, std::string, construction_compare, allocator>, true>();
    check_construction<wjr::btree_set<int, construction_compare, allocator>, true>();
}

struct throwing_compare {
    static inline bool fail_copy = true;
    throwing_compare() { WJR_THROW(std::runtime_error("default comparator")); }
    explicit throwing_compare(int) noexcept {}
    throwing_compare(const throwing_compare &) {
        if (fail_copy) {
            WJR_THROW(std::runtime_error("copy comparator"));
        }
    }
    throwing_compare(throwing_compare &&) { WJR_THROW(std::runtime_error("move comparator")); }
    bool operator()(int lhs, int rhs) const noexcept { return lhs < rhs; }
};

template <typename T>
struct throwing_allocator : std::allocator<T> {
    static inline bool fail_default = false;
    static inline bool fail_move = false;
    template <typename U>
    struct rebind {
        using other = throwing_allocator<U>;
    };
    throwing_allocator() noexcept(false) {
        if (fail_default) {
            WJR_THROW(std::runtime_error("default allocator"));
        }
    }
    throwing_allocator(const throwing_allocator &) noexcept = default;
    template <typename U>
    throwing_allocator(const throwing_allocator<U> &) noexcept {}
    throwing_allocator(throwing_allocator &&) noexcept(false) {
        if (fail_move) {
            WJR_THROW(std::runtime_error("move allocator"));
        }
    }
};

TEST(BtreeConstruction, ExceptionSpecificationsFollowConstructedState) {
    using normal = wjr::btree_map<int, int>;
    using compare_tree = wjr::btree_map<int, int, throwing_compare>;
    using allocator_tree = wjr::btree_map<int, int, std::less<int>, throwing_allocator<char>>;
    EXPECT_TRUE(std::is_nothrow_default_constructible_v<normal>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<normal>);
    EXPECT_FALSE(std::is_nothrow_default_constructible_v<compare_tree>);
    EXPECT_FALSE(std::is_nothrow_copy_constructible_v<compare_tree>);
    EXPECT_FALSE(std::is_nothrow_move_constructible_v<compare_tree>);
    EXPECT_FALSE(std::is_nothrow_default_constructible_v<allocator_tree>);
    EXPECT_FALSE(std::is_nothrow_move_constructible_v<allocator_tree>);
    EXPECT_FALSE((std::is_nothrow_constructible_v<allocator_tree, const std::less<int> &>));
#if !defined(WJR_DISABLE_EXCEPTIONS)
    EXPECT_THROW(compare_tree{}, std::runtime_error);
    const throwing_compare comp(0);
    EXPECT_THROW(compare_tree{comp}, std::runtime_error);
    throwing_compare::fail_copy = false;
    compare_tree source(comp);
    throwing_compare::fail_copy = true;
    EXPECT_THROW((void)compare_tree(source), std::runtime_error);
    EXPECT_THROW((void)compare_tree(std::move(source)), std::runtime_error);
    EXPECT_TRUE(source.empty());
    allocator_tree allocator_source;
    throwing_allocator<char>::fail_default = true;
    EXPECT_THROW(allocator_tree{}, std::runtime_error);
    const std::less<int> less;
    EXPECT_THROW(allocator_tree{less}, std::runtime_error);
    throwing_allocator<char>::fail_default = false;
    throwing_allocator<char>::fail_move = true;
    EXPECT_THROW((void)allocator_tree(std::move(allocator_source)), std::runtime_error);
    throwing_allocator<char>::fail_move = false;
#endif
}
} // namespace
