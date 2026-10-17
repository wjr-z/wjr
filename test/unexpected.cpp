#include <wjr/expected.hpp>

#include <functional>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace unexpected_test {

struct std_swap_error {
    int value;
    int *calls;
};

} // namespace unexpected_test

namespace std {

template <>
inline void swap(unexpected_test::std_swap_error &lhs,
                 unexpected_test::std_swap_error &rhs) noexcept {
    ++*lhs.calls;
    swap(lhs.value, rhs.value);
}

} // namespace std

namespace {

struct operation_error {};

struct comparison_error {
    friend bool operator==(const comparison_error &, const comparison_error &) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw operation_error{};
#else
        return true;
#endif
    }

    friend bool operator!=(const comparison_error &, const comparison_error &) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw operation_error{};
#else
        return false;
#endif
    }

    friend constexpr bool operator==(comparison_error &&, comparison_error &&) noexcept {
        return true;
    }
    friend constexpr bool operator!=(comparison_error &&, comparison_error &&) noexcept {
        return false;
    }
};

struct comparison_result {
    operator bool() const noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw operation_error{};
#else
        return true;
#endif
    }
};

struct proxy_error {
    friend constexpr comparison_result operator==(const proxy_error &,
                                                  const proxy_error &) noexcept {
        return {};
    }
    friend constexpr comparison_result operator!=(const proxy_error &,
                                                  const proxy_error &) noexcept {
        return {};
    }
};

struct non_boolean_result {};

struct non_boolean_error {
    friend non_boolean_result operator==(const non_boolean_error &, const non_boolean_error &);
    friend non_boolean_result operator!=(const non_boolean_error &, const non_boolean_error &);
};

struct independent_comparison_error {
    friend constexpr bool operator==(const independent_comparison_error &,
                                     const independent_comparison_error &) noexcept {
        return true;
    }
    friend constexpr bool operator!=(const independent_comparison_error &,
                                     const independent_comparison_error &) noexcept {
        return true;
    }
};

struct adl_error {
    int value;

    constexpr explicit adl_error(int x) noexcept : value(x) {}
    adl_error(const adl_error &) = delete;
    adl_error(adl_error &&) = delete;

    friend constexpr void swap(adl_error &lhs, adl_error &rhs) noexcept {
        std::swap(lhs.value, rhs.value);
    }
};

struct throwing_swap_error {
    int value;

    friend void swap(throwing_swap_error &, throwing_swap_error &) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw operation_error{};
#endif
    }
};

struct unswappable_error {
    unswappable_error() = default;
    unswappable_error(const unswappable_error &) = delete;
    unswappable_error(unswappable_error &&) = delete;
};

template <typename T>
concept has_member_swap = requires(T &lhs, T &rhs) { lhs.swap(rhs); };

template <typename T>
constexpr bool check_comparison_traits() {
    static_assert(std::is_invocable_r_v<bool, std::equal_to<>, T &, T &>);
    static_assert(std::is_invocable_r_v<bool, std::equal_to<>, const T &, const T &>);
    static_assert(std::is_invocable_r_v<bool, std::equal_to<>, T, T>);
    static_assert(std::is_invocable_r_v<bool, std::not_equal_to<>, const T &, T>);
    static_assert(!std::is_nothrow_invocable_r_v<bool, std::equal_to<>, T &, T &>);
    static_assert(!std::is_nothrow_invocable_r_v<bool, std::not_equal_to<>, const T &, T>);
    return true;
}

static_assert(check_comparison_traits<wjr::unexpected<comparison_error>>());
static_assert(check_comparison_traits<wjr::unexpected<proxy_error>>());
static_assert(std::is_nothrow_invocable_r_v<bool, std::equal_to<>, const wjr::unexpected<int> &,
                                            wjr::unexpected<long>>);
static_assert(std::is_nothrow_invocable_r_v<bool, std::not_equal_to<>, const wjr::unexpected<int> &,
                                            wjr::unexpected<long>>);
static_assert(
    !std::is_invocable_r_v<bool, std::equal_to<>, const wjr::unexpected<non_boolean_error> &,
                           const wjr::unexpected<non_boolean_error> &>);
static_assert(
    !std::is_invocable_r_v<bool, std::not_equal_to<>, const wjr::unexpected<non_boolean_error> &,
                           const wjr::unexpected<non_boolean_error> &>);
static_assert(has_member_swap<wjr::unexpected<adl_error>>);
static_assert(std::is_nothrow_swappable_v<wjr::unexpected<adl_error>>);
static_assert(std::is_swappable_v<wjr::unexpected<throwing_swap_error>>);
static_assert(!std::is_nothrow_swappable_v<wjr::unexpected<throwing_swap_error>>);
static_assert(!has_member_swap<wjr::unexpected<unswappable_error>>);
static_assert(!std::is_swappable_v<wjr::unexpected<unswappable_error>>);

constexpr wjr::unexpected<independent_comparison_error> independent_left(std::in_place);
constexpr wjr::unexpected<independent_comparison_error> independent_right(std::in_place);
static_assert(independent_left == independent_right);
static_assert(independent_left != independent_right);

constexpr wjr::unexpected<int> comparison_left(7);
constexpr wjr::unexpected<long> comparison_right(7L);
static_assert(comparison_left == comparison_right);
static_assert(!(comparison_left != comparison_right));
static_assert(wjr::unexpected<int>(7) == comparison_right);
static_assert(comparison_left != wjr::unexpected<long>(8L));

constexpr bool check_constexpr_swap() {
    wjr::unexpected<adl_error> a(std::in_place, 1), b(std::in_place, 2);
    a.swap(b);
    if (a.error().value != 2 || b.error().value != 1) {
        return false;
    }
    using std::swap;
    swap(a, b);
    if (a.error().value != 1 || b.error().value != 2) {
        return false;
    }
    std::swap(a, b);
    return a.error().value == 2 && b.error().value == 1;
}

static_assert(check_constexpr_swap());

} // namespace

TEST(unexpected, const_and_rvalue_comparisons) {
    const wjr::unexpected<int> x(7);
    wjr::unexpected<long> y(7L);
    EXPECT_TRUE(x == y);
    EXPECT_FALSE(x != y);
    EXPECT_TRUE(wjr::unexpected<int>(7) == y);
    EXPECT_FALSE(y != wjr::unexpected<int>(7));
    EXPECT_TRUE(x != wjr::unexpected<long>(8L));
}

TEST(unexpected, adl_swap_supports_nonmovable_error) {
    wjr::unexpected<adl_error> a(std::in_place, 1), b(std::in_place, 2);
    a.swap(b);
    EXPECT_EQ(a.error().value, 2);
    EXPECT_EQ(b.error().value, 1);
    using std::swap;
    swap(a, b);
    EXPECT_EQ(a.error().value, 1);
    EXPECT_EQ(b.error().value, 2);
    std::swap(a, b);
    EXPECT_EQ(a.error().value, 2);
    EXPECT_EQ(b.error().value, 1);
}

TEST(unexpected, error_std_swap_specialization_is_preserved) {
    int calls = 0;
    wjr::unexpected<unexpected_test::std_swap_error> x(std::in_place, 1, &calls);
    wjr::unexpected<unexpected_test::std_swap_error> y(std::in_place, 2, &calls);
    x.swap(y);
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(x.error().value, 2);
    EXPECT_EQ(y.error().value, 1);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(unexpected, const_comparison_exceptions_propagate) {
    wjr::unexpected<comparison_error> x(std::in_place), y(std::in_place);
    EXPECT_THROW((void)(x == y), operation_error);
    EXPECT_THROW((void)(x != y), operation_error);
    EXPECT_THROW((void)(std::as_const(x) == std::move(y)), operation_error);
}

TEST(unexpected, comparison_result_conversion_exceptions_propagate) {
    const wjr::unexpected<proxy_error> x(std::in_place), y(std::in_place);
    EXPECT_THROW((void)(x == y), operation_error);
    EXPECT_THROW((void)(x != y), operation_error);
}

TEST(unexpected, adl_swap_exceptions_propagate) {
    wjr::unexpected<throwing_swap_error> x(std::in_place, 1), y(std::in_place, 2);
    EXPECT_THROW(x.swap(y), operation_error);
    EXPECT_EQ(x.error().value, 1);
    EXPECT_EQ(y.error().value, 2);
    using std::swap;
    EXPECT_THROW(swap(x, y), operation_error);
    EXPECT_THROW(std::swap(x, y), operation_error);
}
#endif
