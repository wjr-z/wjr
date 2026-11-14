#include <gtest/gtest.h>

#include <wjr/expected.hpp>

namespace {
struct adl_only {
    static inline int swaps = 0;
    static inline int moves = 0;
    static inline int live = 0;
    int value;

    explicit adl_only(int initial) noexcept : value(initial) { ++live; }
    adl_only(adl_only &&other) noexcept : value(other.value) {
        ++moves;
        ++live;
    }
    adl_only &operator=(adl_only &&) = delete;
    ~adl_only() { --live; }

    friend void swap(adl_only &left, adl_only &right) noexcept {
        ++swaps;
        const int previous = left.value;
        left.value = right.value;
        right.value = previous;
    }
};

struct swap_error {};
struct throwing_adl {
    int value;
    explicit throwing_adl(int initial) noexcept : value(initial) {}
    throwing_adl(throwing_adl &&) noexcept = default;
    throwing_adl &operator=(throwing_adl &&) = delete;

    friend void swap(throwing_adl &, throwing_adl &) noexcept(false) {
#ifndef WJR_DISABLE_EXCEPTIONS
        throw swap_error{};
#endif
    }
};

static_assert(std::is_swappable_v<adl_only>);
static_assert(!std::is_move_assignable_v<adl_only>);
static_assert(noexcept(std::declval<wjr::expected<adl_only, int> &>().swap(
    std::declval<wjr::expected<adl_only, int> &>())));
static_assert(!noexcept(std::declval<wjr::expected<throwing_adl, int> &>().swap(
    std::declval<wjr::expected<throwing_adl, int> &>())));

TEST(expected_swap, value_state_uses_element_adl_without_moves) {
    adl_only::swaps = 0;
    adl_only::moves = 0;
    {
        wjr::expected<adl_only, int> first(std::in_place, 17);
        wjr::expected<adl_only, int> second(std::in_place, 23);
        first.swap(second);
        EXPECT_EQ(first->value, 23);
        EXPECT_EQ(second->value, 17);
        std::swap(first, second);
        EXPECT_EQ(first->value, 17);
        EXPECT_EQ(second->value, 23);
        EXPECT_EQ(adl_only::swaps, 2);
        EXPECT_EQ(adl_only::moves, 0);
        EXPECT_EQ(adl_only::live, 2);
    }
    EXPECT_EQ(adl_only::live, 0);
}

template <typename Result>
void check_error_state_swap() {
    Result first(wjr::unexpect, 17);
    Result second(wjr::unexpect, 23);
    first.swap(second);
    ASSERT_FALSE(first);
    ASSERT_FALSE(second);
    EXPECT_EQ(first.error().value, 23);
    EXPECT_EQ(second.error().value, 17);
    std::swap(first, second);
    EXPECT_EQ(first.error().value, 17);
    EXPECT_EQ(second.error().value, 23);
}

TEST(expected_swap, error_states_use_element_adl_without_moves) {
    adl_only::swaps = 0;
    adl_only::moves = 0;
    check_error_state_swap<wjr::expected<int, adl_only>>();
    check_error_state_swap<wjr::expected<void, adl_only>>();
    EXPECT_EQ(adl_only::swaps, 4);
    EXPECT_EQ(adl_only::moves, 0);
    EXPECT_EQ(adl_only::live, 0);
}

TEST(expected_swap, mixed_states_move_construct_without_element_swap) {
    adl_only::swaps = 0;
    adl_only::moves = 0;
    {
        wjr::expected<adl_only, int> first(std::in_place, 17);
        wjr::expected<adl_only, int> second(wjr::unexpect, 23);
        first.swap(second);
        ASSERT_FALSE(first);
        EXPECT_EQ(first.error(), 23);
        ASSERT_TRUE(second);
        EXPECT_EQ(second->value, 17);
        EXPECT_EQ(adl_only::swaps, 0);
        EXPECT_EQ(adl_only::moves, 1);
        EXPECT_EQ(adl_only::live, 1);
    }
    EXPECT_EQ(adl_only::live, 0);
}

#ifndef WJR_DISABLE_EXCEPTIONS
TEST(expected_swap, throwing_adl_swap_propagates_from_each_state) {
    wjr::expected<throwing_adl, int> first(std::in_place, 17);
    wjr::expected<throwing_adl, int> second(std::in_place, 23);
    EXPECT_THROW(first.swap(second), swap_error);
    EXPECT_EQ(first->value, 17);
    EXPECT_EQ(second->value, 23);
    wjr::expected<int, throwing_adl> error_first(wjr::unexpect, 17);
    wjr::expected<int, throwing_adl> error_second(wjr::unexpect, 23);
    EXPECT_THROW(error_first.swap(error_second), swap_error);
    EXPECT_EQ(error_first.error().value, 17);
    EXPECT_EQ(error_second.error().value, 23);
    wjr::expected<void, throwing_adl> void_first(wjr::unexpect, 17);
    wjr::expected<void, throwing_adl> void_second(wjr::unexpect, 23);
    EXPECT_THROW(void_first.swap(void_second), swap_error);
    EXPECT_EQ(void_first.error().value, 17);
    EXPECT_EQ(void_second.error().value, 23);
}
#endif
} // namespace
