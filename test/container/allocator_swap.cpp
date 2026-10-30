#include <memory>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/ring_buffer.hpp>
#include <wjr/container/vector.hpp>

namespace {
struct swap_state {
    static inline int selected = 0;
    static inline int swaps = 0;
    static inline int constructed_by = 0;
};

template <typename T, bool Propagate, bool AlwaysEqual>
struct swap_allocator {
    using value_type = T;
    using propagate_on_container_swap = std::bool_constant<Propagate>;
    using is_always_equal = std::bool_constant<AlwaysEqual>;
    template <typename U>
    struct rebind {
        using other = swap_allocator<U, Propagate, AlwaysEqual>;
    };

    int id = swap_state::selected;
    swap_allocator() noexcept = default;
    explicit swap_allocator(int value) noexcept : id(value) {}
    template <typename U>
    swap_allocator(const swap_allocator<U, Propagate, AlwaysEqual> &other) noexcept
        : id(other.id) {}

    T *allocate(size_t count) { return std::allocator<T>{}.allocate(count); }
    void deallocate(T *ptr, size_t count) noexcept { std::allocator<T>{}.deallocate(ptr, count); }

    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        swap_state::constructed_by = id;
        std::construct_at(ptr, std::forward<Args>(args)...);
    }

    bool operator==(const swap_allocator &other) const noexcept {
        return AlwaysEqual || id % 10 == other.id % 10;
    }

    friend void swap(swap_allocator &lhs, swap_allocator &rhs) noexcept {
        ++swap_state::swaps;
        std::swap(lhs.id, rhs.id);
    }
};

template <template <typename, typename> class Container, bool Propagate, bool AlwaysEqual>
void check_sequence_swap(int rhs_id) {
    using allocator = swap_allocator<int, Propagate, AlwaysEqual>;
    for (int lhs_size : {0, 1, 32}) {
        for (int rhs_size : {0, 2, 48}) {
            Container<int, allocator> lhs(allocator{1});
            Container<int, allocator> rhs(allocator{rhs_id});
            for (int i = 0; i != lhs_size; ++i) {
                lhs.push_back(i);
            }
            for (int i = 0; i != rhs_size; ++i) {
                rhs.push_back(100 + i);
            }
            swap_state::swaps = 0;
            lhs.swap(rhs);
            EXPECT_EQ(swap_state::swaps, Propagate ? 1 : 0);
            EXPECT_EQ(lhs.get_allocator().id, Propagate ? rhs_id : 1);
            EXPECT_EQ(rhs.get_allocator().id, Propagate ? 1 : rhs_id);
            EXPECT_EQ(lhs.size(), static_cast<size_t>(rhs_size));
            EXPECT_EQ(rhs.size(), static_cast<size_t>(lhs_size));
            for (int i = 0; i != rhs_size; ++i) {
                EXPECT_EQ(lhs[i], 100 + i);
            }
            for (int i = 0; i != lhs_size; ++i) {
                EXPECT_EQ(rhs[i], i);
            }
        }
    }
}

template <bool Propagate, bool AlwaysEqual>
void check_tree_swap(int rhs_id) {
    using allocator = swap_allocator<char, Propagate, AlwaysEqual>;
    for (int lhs_size : {0, 2, 80}) {
        for (int rhs_size : {0, 1, 80}) {
            swap_state::selected = 1;
            wjr::btree_map<int, int, std::less<int>, allocator> lhs;
            swap_state::selected = rhs_id;
            wjr::btree_map<int, int, std::less<int>, allocator> rhs;
            for (int i = 0; i != lhs_size; ++i) {
                lhs.emplace(i, i);
            }
            for (int i = 0; i != rhs_size; ++i) {
                rhs.emplace(100 + i, i);
            }
            swap_state::swaps = 0;
            lhs.swap(rhs);
            EXPECT_EQ(swap_state::swaps, Propagate ? 1 : 0);
            EXPECT_EQ(lhs.size(), static_cast<size_t>(rhs_size));
            EXPECT_EQ(rhs.size(), static_cast<size_t>(lhs_size));
            lhs.emplace(-1, 1);
            EXPECT_EQ(swap_state::constructed_by, Propagate ? rhs_id : 1);
            rhs.emplace(-1, 2);
            EXPECT_EQ(swap_state::constructed_by, Propagate ? 1 : rhs_id);
            EXPECT_EQ(lhs.at(-1), 1);
            EXPECT_EQ(rhs.at(-1), 2);
        }
    }
    swap_state::selected = 0;
}

template <bool Propagate, bool AlwaysEqual>
void check_swaps(int rhs_id) {
    check_sequence_swap<wjr::vector, Propagate, AlwaysEqual>(rhs_id);
    check_sequence_swap<wjr::ring_buffer, Propagate, AlwaysEqual>(rhs_id);
    check_tree_swap<Propagate, AlwaysEqual>(rhs_id);
}

TEST(ContainerAllocatorSwap, PropagatesWithAdlEvenWhenAllocatorsCompareEqual) {
    check_swaps<true, true>(2);
    check_swaps<true, false>(11);
    check_swaps<true, false>(2);
}

TEST(ContainerAllocatorSwap, NonpropagatingAllocatorsRemainInPlace) {
    check_swaps<false, true>(2);
    check_swaps<false, false>(11);
}

struct swap_error {};
struct throwing_move {
    int value;
    explicit throwing_move(int v) noexcept : value(v) {}
    throwing_move(const throwing_move &) = default;
    throwing_move(throwing_move &&) { WJR_THROW(swap_error{}); }
};

TEST(ContainerAllocatorSwap, ExceptionSpecificationIncludesStorageSwap) {
    using container = wjr::inplace_vector<throwing_move, 4>;
    container lhs, rhs;
    EXPECT_FALSE(noexcept(lhs.swap(rhs)));
#if !defined(WJR_DISABLE_EXCEPTIONS)
    if constexpr (!noexcept(lhs.swap(rhs))) {
        lhs.emplace_back(7);
        EXPECT_THROW(lhs.swap(rhs), swap_error);
        EXPECT_EQ(lhs.size(), 1u);
        EXPECT_EQ(lhs.front().value, 7);
        EXPECT_TRUE(rhs.empty());
    }
#endif
}
} // namespace
