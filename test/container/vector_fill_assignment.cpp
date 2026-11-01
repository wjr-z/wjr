#include <wjr/vector.hpp>

#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {

struct fill_state {
    int live = 0;
    int allocations = 0, deallocations = 0;
    int hook_created = 0, hook_destroyed = 0;
    int copy_budget = -1, assign_budget = -1, hook_budget = -1;
    bool fail_allocate = false;
};

void consume(int &budget) {
    if (budget == 0) {
        throw std::runtime_error("fill operation failed");
    }
    if (budget > 0) {
        --budget;
    }
}

template <bool ThrowingCopy>
struct fill_value {
    fill_state *state;
    int value;
    fill_value(fill_state &s, int v) noexcept : state(&s), value(v) { ++state->live; }
    fill_value(const fill_value &other) noexcept(!ThrowingCopy)
        : state(other.state), value(other.value) {
        if constexpr (ThrowingCopy) {
            consume(state->copy_budget);
        }
        ++state->live;
    }
    fill_value(fill_value &&other) noexcept : state(other.state), value(other.value) {
        ++state->live;
    }
    fill_value &operator=(const fill_value &other) {
        consume(state->assign_budget);
        value = other.value;
        return *this;
    }
    ~fill_value() { --state->live; }
};

template <typename T>
struct fill_allocator {
    using value_type = T;
    fill_state *state;
    explicit fill_allocator(fill_state &s) noexcept : state(&s) {}
    template <typename U>
    fill_allocator(const fill_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(std::size_t count) {
        if (state->fail_allocate) {
            throw std::bad_alloc{};
        }
        auto *ptr = std::allocator<T>{}.allocate(count);
        ++state->allocations;
        return ptr;
    }
    void deallocate(T *ptr, std::size_t count) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(ptr, count);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        consume(state->hook_budget);
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++state->hook_created;
    }
    void destroy(T *ptr) noexcept {
        ++state->hook_destroyed;
        std::destroy_at(ptr);
    }
    friend bool operator==(const fill_allocator &, const fill_allocator &) = default;
};

enum class failure { allocation, copy, hook };

template <typename Vector>
void verify_growth(std::size_t initial, failure mode, int budget) {
    fill_state state;
    {
        typename Vector::value_type source(state, 99);
        {
            Vector target{typename Vector::allocator_type(state)};
            for (std::size_t i = 0; i != initial; ++i) {
                target.emplace_back(state, static_cast<int>(i));
            }
            auto *const original_data = target.data();
            const auto original_capacity = target.capacity();
            const auto original_blocks = state.allocations - state.deallocations;
            state.fail_allocate = mode == failure::allocation;
            state.copy_budget = mode == failure::copy ? budget : -1;
            state.hook_budget = mode == failure::hook ? budget : -1;
            if (mode == failure::allocation) {
                EXPECT_THROW(target.assign(12, source), std::bad_alloc);
            } else {
                EXPECT_THROW(target.assign(12, source), std::runtime_error);
            }
            state.fail_allocate = false;
            state.copy_budget = state.hook_budget = -1;
            EXPECT_EQ(target.size(), initial);
            EXPECT_EQ(target.data(), original_data);
            EXPECT_EQ(target.capacity(), original_capacity);
            EXPECT_EQ(state.allocations - state.deallocations, original_blocks);
            EXPECT_EQ(state.live, static_cast<int>(initial) + 1);
            for (std::size_t i = 0; i != initial; ++i) {
                EXPECT_EQ(target[i].value, static_cast<int>(i));
            }
            EXPECT_EQ(&target.assign(12, source), &target);
            ASSERT_EQ(target.size(), 12);
            for (const auto &value : target) {
                EXPECT_EQ(value.value, 99);
            }
        }
        EXPECT_EQ(state.live, 1);
    }
    EXPECT_EQ(state.live, 0);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.hook_created, state.hook_destroyed);
}

template <typename T>
void verify_growth_backends(failure mode, int budget) {
    for (const std::size_t initial : {0u, 2u, 5u}) {
        SCOPED_TRACE(testing::Message() << initial << "," << budget);
        verify_growth<wjr::vector<T, fill_allocator<T>>>(initial, mode, budget);
        verify_growth<wjr::small_vector<T, 0, fill_allocator<T>>>(initial, mode, budget);
        verify_growth<wjr::small_vector<T, 4, fill_allocator<T>>>(initial, mode, budget);
    }
}

TEST(vector_fill_assignment, allocation_failure_preserves_owned_storage) {
    verify_growth_backends<fill_value<false>>(failure::allocation, 0);
}

TEST(vector_fill_assignment, every_copy_failure_releases_temporary_storage) {
    for (int budget = 0; budget != 12; ++budget) {
        verify_growth_backends<fill_value<true>>(failure::copy, budget);
    }
}

TEST(vector_fill_assignment, every_allocator_construct_failure_releases_temporary_storage) {
    static_assert(std::is_nothrow_copy_constructible_v<fill_value<false>>);
    for (int budget = 0; budget != 12; ++budget) {
        verify_growth_backends<fill_value<false>>(failure::hook, budget);
    }
}

TEST(vector_fill_assignment, within_capacity_keeps_storage_and_basic_exception_guarantee) {
    for (int budget = 0; budget != 2; ++budget) {
        fill_state state;
        {
            fill_value<true> source(state, 99);
            wjr::vector<fill_value<true>, fill_allocator<fill_value<true>>> target{
                fill_allocator<fill_value<true>>(state)};
            target.reserve(4);
            target.emplace_back(state, 1);
            target.emplace_back(state, 2);
            auto *const data = target.data();
            const auto allocations = state.allocations;
            state.copy_budget = budget;
            EXPECT_THROW(target.assign(4, source), std::runtime_error);
            state.copy_budget = -1;
            EXPECT_EQ(target.size(), 2);
            EXPECT_EQ(target.data(), data);
            EXPECT_EQ(state.live, 3);
            EXPECT_EQ(state.allocations, allocations);
            state.assign_budget = budget;
            EXPECT_THROW(target.assign(4, source), std::runtime_error);
            state.assign_budget = -1;
            EXPECT_EQ(target.size(), 2);
            EXPECT_EQ(target.data(), data);
            EXPECT_EQ(state.live, 3);
            target.assign(4, source);
            EXPECT_EQ(target.size(), 4);
            EXPECT_EQ(target.data(), data);
            target.assign(1, source);
            EXPECT_EQ(target.size(), 1);
            target.assign(0, source);
            EXPECT_TRUE(target.empty());
        }
        EXPECT_EQ(state.live, 0);
        EXPECT_EQ(state.allocations, state.deallocations);
        EXPECT_EQ(state.hook_created, state.hook_destroyed);
    }
}

} // namespace
#endif
