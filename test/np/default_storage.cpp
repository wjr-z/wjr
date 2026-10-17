#include <array>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include <wjr/np/copy.hpp>
#include <wjr/np/evaluate.hpp>
#include <wjr/np/reduce.hpp>

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 2>;

struct construction_state {
    size_t limit = 100;
    int allocations = 0;
    int deallocations = 0;
    int constructions = 0;
    int destructions = 0;
    int budget = -1;
};

template <class T>
struct hook_allocator {
    using value_type = T;
    construction_state *state;
    explicit hook_allocator(construction_state &s) noexcept : state(&s) {}
    template <class U>
    explicit hook_allocator(const hook_allocator<U> &other) noexcept : state(other.state) {}
    size_t max_size() const noexcept { return state->limit; }
    T *allocate(size_t n) {
        ++state->allocations;
        return std::allocator<T>{}.allocate(n);
    }
    void deallocate(T *p, size_t n) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(p, n);
    }
    void construct(T *p) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (state->budget == 0)
            throw std::runtime_error("construction failed");
#endif
        if (state->budget > 0)
            --state->budget;
        std::construct_at(p, -7);
        ++state->constructions;
    }
    void destroy(T *p) noexcept {
        ++state->destructions;
        std::destroy_at(p);
    }
    bool operator==(const hook_allocator &) const = default;
};

using hooked_owner =
    np::tensor<int, shape, np::layout_right, wjr::vector<int, hook_allocator<int>>>;

TEST(NpDefaultStorage, ValueInitializationAndExplicitDefaultInitialization) {
    using owner = np::tensor<int, shape>;
    auto initialized = np::make_tensor<owner>(std::array{2, 3});
    ASSERT_TRUE(initialized);
    EXPECT_EQ(np::sum(initialized->view()), 0);
    auto overwrite = np::make_tensor<owner>(std::array{2, 3}, wjr::default_construct);
    ASSERT_TRUE(overwrite);
    np::fill_to(overwrite->view(), 4);
    EXPECT_EQ(np::sum(overwrite->view()), 24);
    auto empty = np::make_tensor<owner>(std::array{3, 0}, wjr::default_construct);
    ASSERT_TRUE(empty);
    EXPECT_TRUE(empty->empty());
    EXPECT_TRUE(empty->has_storage());
    using scalar = np::tensor<int, np::extents<size_t>>;
    auto single = np::make_tensor<scalar>(std::array<int, 0>{}, wjr::default_construct);
    ASSERT_TRUE(single);
    single->view()() = 9;
    EXPECT_EQ(single->view()(), 9);
}

struct defaulted_element {
    static inline int live = 0;
    int value = 23;
    defaulted_element() { ++live; }
    ~defaulted_element() { --live; }
};

TEST(NpDefaultStorage, ConstructsNontrivialElements) {
    using owner = np::tensor<defaulted_element, shape>;
    {
        auto result = np::make_tensor<owner>(std::array{2, 3}, wjr::default_construct);
        ASSERT_TRUE(result);
        EXPECT_EQ(defaulted_element::live, 6);
        EXPECT_EQ(result->view()(1, 2).value, 23);
    }
    EXPECT_EQ(defaulted_element::live, 0);
}

TEST(NpDefaultStorage, PreservesAllocatorConstructionAndDestructionHooks) {
    construction_state state;
    const hook_allocator<int> allocator(state);
    {
        auto result = np::make_tensor<hooked_owner>(std::array{2, 3}, wjr::default_construct,
                                                    std::allocator_arg, allocator);
        ASSERT_TRUE(result);
        EXPECT_EQ(state.allocations, 1);
        EXPECT_EQ(state.constructions, 6);
        EXPECT_EQ(result->view()(1, 2), -7);
        auto evaluated = np::eval(np::as_expr(result->view()) + 10, std::allocator_arg, allocator);
        ASSERT_TRUE(evaluated);
        EXPECT_EQ(state.constructions, 12);
        EXPECT_EQ(evaluated->view()(1, 2), 3);
    }
    EXPECT_EQ(state.constructions, state.destructions);
    EXPECT_EQ(state.allocations, state.deallocations);
}

TEST(NpDefaultStorage, ChecksAllocatorLimitBeforeAllocatingEitherInitializationMode) {
    construction_state state;
    state.limit = 5;
    const hook_allocator<int> allocator(state);
    auto initialized =
        np::make_tensor<hooked_owner>(std::array{2, 3}, std::allocator_arg, allocator);
    auto overwrite = np::make_tensor<hooked_owner>(std::array{2, 3}, wjr::default_construct,
                                                   std::allocator_arg, allocator);
    ASSERT_FALSE(initialized);
    ASSERT_FALSE(overwrite);
    EXPECT_EQ(initialized.error(), np::shape_error::container_size_limit);
    EXPECT_EQ(overwrite.error(), np::shape_error::container_size_limit);
    EXPECT_EQ(state.allocations, 0);
    auto empty = np::make_tensor<hooked_owner>(std::array{3, 0}, wjr::default_construct,
                                               std::allocator_arg, allocator);
    ASSERT_TRUE(empty);
    EXPECT_EQ(state.allocations, 0);
    EXPECT_EQ(state.constructions, 0);
}

TEST(NpDefaultStorage, MovedFromOwnersRetainQueryableStorageAndCanBeReassigned) {
    using owner = np::tensor<int, shape>;
    auto original = np::make_tensor<owner>(std::array{2, 3});
    ASSERT_TRUE(original);
    original->view()(1, 2) = 7;
    owner moved = std::move(*original);
    EXPECT_EQ(moved.view()(1, 2), 7);
    EXPECT_EQ(original->size(), 6);
    EXPECT_EQ(original->storage_size(), 0);
    EXPECT_FALSE(original->has_storage());
    EXPECT_FALSE(original->try_view());
    owner copied = *original;
    EXPECT_EQ(copied.size(), 6);
    EXPECT_EQ(copied.storage_size(), 0);
    *original = moved;
    EXPECT_EQ(original->view()(1, 2), 7);
    copied = std::move(moved);
    EXPECT_EQ(copied.view()(1, 2), 7);
    EXPECT_EQ(moved.storage_size(), 0);
    EXPECT_FALSE(moved.try_view());
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
template <class T>
struct failing_allocator {
    using value_type = T;
    using is_always_equal = std::true_type;
    static inline bool fail = false;
    static inline size_t limit = 100;
    static inline int allocations = 0;
    size_t max_size() const noexcept { return limit; }
    T *allocate(size_t n) {
        ++allocations;
        if (fail || n > limit)
            throw std::bad_alloc();
        return std::allocator<T>{}.allocate(n);
    }
    void deallocate(T *p, size_t n) noexcept { std::allocator<T>{}.deallocate(p, n); }
    bool operator==(const failing_allocator &) const = default;
};
using failing_owner =
    np::tensor<int, shape, np::layout_right, wjr::vector<int, failing_allocator<int>>>;
static_assert(!std::is_nothrow_copy_assignable_v<failing_owner>);
static_assert(!std::is_nothrow_copy_assignable_v<np::tensor<int, shape>>);

TEST(NpDefaultStorage, DefaultConstructionChecksAllocatorLimitBeforeAllocation) {
    using owner = np::tensor<int, np::extents<size_t, 6>, np::layout_right,
                             wjr::vector<int, failing_allocator<int>>>;
    failing_allocator<int>::allocations = 0;
    failing_allocator<int>::limit = 2;
    EXPECT_THROW((void)owner{}, std::length_error);
    failing_allocator<int>::limit = 100;
    EXPECT_EQ(failing_allocator<int>::allocations, 0);
}

WJR_NOINLINE void assign_owner(failing_owner &target, const failing_owner &source) {
    target = source;
}

TEST(NpDefaultStorage, CopyAllocationFailurePropagatesAndRetainsMetadata) {
    failing_owner source;
    auto made = np::make_tensor<failing_owner>(std::array{2, 3});
    ASSERT_TRUE(made);
    source = std::move(*made);
    source.view()(1, 2) = 17;
    failing_owner target;
    failing_allocator<int>::fail = true;
    EXPECT_THROW(assign_owner(target, source), std::bad_alloc);
    failing_allocator<int>::fail = false;
    EXPECT_EQ(target.extent(0), 0);
    EXPECT_EQ(target.extent(1), 0);
    EXPECT_TRUE(target.has_storage());
    target = source;
    EXPECT_EQ(target.view()(1, 2), 17);
}

WJR_NOINLINE void construct_hooked_owner(const hook_allocator<int> &allocator) {
    (void)np::make_tensor<hooked_owner>(std::array{2, 3}, wjr::default_construct,
                                        std::allocator_arg, allocator);
}

TEST(NpDefaultStorage, RollsBackPartiallyConstructedDefaultInitializedStorage) {
    for (int budget : {0, 1, 5}) {
        construction_state state;
        state.budget = budget;
        EXPECT_THROW(construct_hooked_owner(hook_allocator<int>(state)), std::runtime_error);
        EXPECT_EQ(state.constructions, budget);
        EXPECT_EQ(state.destructions, budget);
        EXPECT_EQ(state.allocations, state.deallocations);
    }
}
#endif
} // namespace
