#include <array>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include <wjr/container/vector.hpp>

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {
struct assignment_state {
    int live = 0;
    int allocations = 0;
    int deallocations = 0;
    int budget = -1;
};

struct assignment_element {
    assignment_state *state;
    int value;
    assignment_element(assignment_state &s, int v) noexcept : state(&s), value(v) { ++state->live; }
    assignment_element(const assignment_element &other) : state(other.state), value(other.value) {
        if (state->budget == 0)
            throw std::runtime_error("copy failed");
        if (state->budget > 0)
            --state->budget;
        ++state->live;
    }
    assignment_element(assignment_element &&other) noexcept
        : state(other.state), value(other.value) {
        ++state->live;
    }
    assignment_element &operator=(const assignment_element &other) noexcept {
        value = other.value;
        return *this;
    }
    ~assignment_element() { --state->live; }
};

template <class T>
struct assignment_allocator {
    using value_type = T;
    assignment_state *state;
    explicit assignment_allocator(assignment_state &s) noexcept : state(&s) {}
    template <class U>
    explicit assignment_allocator(const assignment_allocator<U> &other) noexcept
        : state(other.state) {}
    T *allocate(size_t n) {
        auto *result = std::allocator<T>{}.allocate(n);
        ++state->allocations;
        return result;
    }
    void deallocate(T *p, size_t n) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(p, n);
    }
    bool operator==(const assignment_allocator &) const = default;
};

// Keep the throwing call and catch in one out-of-line fixture on Windows ASan.
template <class Vector>
WJR_NOINLINE void check_assignment_rollback(size_t initial, size_t count = 12) {
    for (const int budget : {0, 1, 2}) {
        assignment_state state;
        {
            std::array<assignment_element, 12> source{{{state, 0},
                                                       {state, 1},
                                                       {state, 2},
                                                       {state, 3},
                                                       {state, 4},
                                                       {state, 5},
                                                       {state, 6},
                                                       {state, 7},
                                                       {state, 8},
                                                       {state, 9},
                                                       {state, 10},
                                                       {state, 11}}};
            const auto allocator = [&] {
                using A = typename Vector::allocator_type;
                if constexpr (std::is_constructible_v<A, assignment_state &>)
                    return A(state);
                else
                    return A{};
            }();
            Vector target(allocator);
            for (size_t i = 0; i != initial; ++i)
                target.emplace_back(state, -static_cast<int>(i) - 1);
            auto *const original_data = target.data();
            const auto original_capacity = target.capacity();
            const int original_blocks = state.allocations - state.deallocations;
            state.budget = budget;
            EXPECT_THROW(target.assign(source.begin(), source.begin() + count), std::runtime_error);
            state.budget = -1;
            EXPECT_EQ(target.size(), initial);
            EXPECT_EQ(target.data(), original_data);
            EXPECT_EQ(target.capacity(), original_capacity);
            EXPECT_EQ(state.live, source.size() + initial);
            EXPECT_EQ(state.allocations - state.deallocations, original_blocks);
            if (count > original_capacity) {
                for (size_t i = 0; i != initial; ++i)
                    EXPECT_EQ(target[i].value, -static_cast<int>(i) - 1);
            }
            target.assign(source.begin(), source.begin() + count);
            EXPECT_EQ(target.size(), count);
            for (size_t i = 0; i != count; ++i)
                EXPECT_EQ(target[i].value, static_cast<int>(i));
        }
        EXPECT_EQ(state.live, 0);
        EXPECT_EQ(state.allocations, state.deallocations);
    }
}

TEST(vector, assignment_growth_releases_temporary_allocations) {
    using allocator = assignment_allocator<assignment_element>;
    for (size_t initial : {0u, 2u, 5u}) {
        check_assignment_rollback<wjr::vector<assignment_element, allocator>>(initial);
        check_assignment_rollback<wjr::small_vector<assignment_element, 0, allocator>>(initial);
        check_assignment_rollback<wjr::small_vector<assignment_element, 4, allocator>>(initial);
    }
}

TEST(vector, assignment_within_inline_capacity_keeps_its_storage) {
    using allocator = assignment_allocator<assignment_element>;
    check_assignment_rollback<wjr::small_vector<assignment_element, 4, allocator>>(0, 3);
    check_assignment_rollback<wjr::inplace_vector<assignment_element, 4>>(0, 3);
}
} // namespace
#endif
