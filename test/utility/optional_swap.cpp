#include <wjr/optional.hpp>

#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

struct adl_only {
    static inline int swaps = 0;
    static inline int moves = 0;
    static inline int alive = 0;
    static inline int destroyed = 0;
    int value;

    explicit adl_only(int v) noexcept : value(v) { ++alive; }
    adl_only(adl_only &&other) noexcept : value(other.value) {
        other.value = -1;
        ++moves;
        ++alive;
    }
    adl_only &operator=(adl_only &&) = delete;
    ~adl_only() {
        --alive;
        ++destroyed;
    }

    friend void swap(adl_only &lhs, adl_only &rhs) noexcept {
        ++swaps;
        const int previous = lhs.value;
        lhs.value = rhs.value;
        rhs.value = previous;
    }
};

struct swap_error {};

struct throwing_adl {
    static inline int calls = 0;
    int value;
    explicit throwing_adl(int v) noexcept : value(v) {}
    throwing_adl(throwing_adl &&) noexcept = default;
    throwing_adl &operator=(throwing_adl &&) = delete;

    friend void swap(throwing_adl &, throwing_adl &) noexcept(false) {
        ++calls;
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw swap_error{};
#endif
    }
};

struct throwing_move {
    throwing_move(throwing_move &&) noexcept(false);
    friend void swap(throwing_move &, throwing_move &) noexcept;
};

struct unswappable {
    unswappable(unswappable &&) noexcept = default;
    unswappable &operator=(unswappable &&) = delete;
};

template <typename T>
concept has_member_swap = requires(wjr::optional<T> &lhs, wjr::optional<T> &rhs) { lhs.swap(rhs); };

using adl_optional = wjr::optional<adl_only>;
using throwing_optional = wjr::optional<throwing_adl>;
static_assert(std::is_nothrow_swappable_v<adl_only>);
static_assert(std::is_nothrow_move_constructible_v<adl_only>);
static_assert(!std::is_move_assignable_v<adl_only>);
static_assert(has_member_swap<adl_only>);
static_assert(noexcept(std::declval<adl_optional &>().swap(std::declval<adl_optional &>())));
static_assert(noexcept(std::swap(std::declval<adl_optional &>(), std::declval<adl_optional &>())));
static_assert(has_member_swap<throwing_adl>);
static_assert(
    !noexcept(std::declval<throwing_optional &>().swap(std::declval<throwing_optional &>())));
static_assert(!noexcept(std::swap(std::declval<throwing_optional &>(),
                                  std::declval<throwing_optional &>())));
static_assert(!has_member_swap<throwing_move>);
static_assert(!has_member_swap<unswappable>);

void reset_counts() {
    adl_only::swaps = 0;
    adl_only::moves = 0;
    adl_only::alive = 0;
    adl_only::destroyed = 0;
}

} // namespace

TEST(optional_swap, engaged_values_use_element_adl_without_moves) {
    reset_counts();
    {
        adl_optional first(std::in_place, 17);
        adl_optional second(std::in_place, 23);
        first.swap(second);
        EXPECT_TRUE(first.has_value());
        EXPECT_TRUE(second.has_value());
        EXPECT_EQ(first->value, 23);
        EXPECT_EQ(second->value, 17);
        EXPECT_EQ(adl_only::swaps, 1);
        EXPECT_EQ(adl_only::moves, 0);
        EXPECT_EQ(adl_only::alive, 2);
        EXPECT_EQ(adl_only::destroyed, 0);

        std::swap(first, second);
        EXPECT_EQ(first->value, 17);
        EXPECT_EQ(second->value, 23);
        EXPECT_EQ(adl_only::swaps, 2);
        EXPECT_EQ(adl_only::moves, 0);
        EXPECT_EQ(adl_only::destroyed, 0);
    }
    EXPECT_EQ(adl_only::alive, 0);
    EXPECT_EQ(adl_only::destroyed, 2);
}

TEST(optional_swap, transfers_engagement_without_calling_element_swap) {
    reset_counts();
    {
        adl_optional first(std::in_place, 31);
        adl_optional second;
        first.swap(second);
        EXPECT_FALSE(first.has_value());
        EXPECT_TRUE(second.has_value());
        EXPECT_EQ(second->value, 31);
        EXPECT_EQ(adl_only::moves, 1);
        EXPECT_EQ(adl_only::destroyed, 1);
        EXPECT_EQ(adl_only::alive, 1);

        std::swap(first, second);
        EXPECT_TRUE(first.has_value());
        EXPECT_FALSE(second.has_value());
        EXPECT_EQ(first->value, 31);
        EXPECT_EQ(adl_only::moves, 2);
        EXPECT_EQ(adl_only::destroyed, 2);
        EXPECT_EQ(adl_only::swaps, 0);
    }
    EXPECT_EQ(adl_only::alive, 0);
    EXPECT_EQ(adl_only::destroyed, 3);
}

TEST(optional_swap, empty_and_self_swaps_preserve_lifetimes) {
    reset_counts();
    {
        adl_optional empty;
        adl_optional other_empty;
        empty.swap(other_empty);
        empty.swap(empty);
        EXPECT_FALSE(empty.has_value());
        EXPECT_FALSE(other_empty.has_value());
        EXPECT_EQ(adl_only::swaps, 0);
        EXPECT_EQ(adl_only::moves, 0);

        adl_optional value(std::in_place, 41);
        value.swap(value);
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(value->value, 41);
        EXPECT_EQ(adl_only::swaps, 1);
        EXPECT_EQ(adl_only::moves, 0);
        EXPECT_EQ(adl_only::alive, 1);
        EXPECT_EQ(adl_only::destroyed, 0);
    }
    EXPECT_EQ(adl_only::alive, 0);
    EXPECT_EQ(adl_only::destroyed, 1);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(optional_swap, propagates_element_swap_failure_without_changing_engagement) {
    throwing_adl::calls = 0;
    throwing_optional first(std::in_place, 43);
    throwing_optional second(std::in_place, 47);
    EXPECT_THROW(first.swap(second), swap_error);
    EXPECT_TRUE(first.has_value());
    EXPECT_TRUE(second.has_value());
    EXPECT_EQ(first->value, 43);
    EXPECT_EQ(second->value, 47);
    EXPECT_EQ(throwing_adl::calls, 1);
    EXPECT_THROW(std::swap(first, second), swap_error);
    EXPECT_EQ(throwing_adl::calls, 2);
}
#endif
