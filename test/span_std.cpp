#include <algorithm>
#include <array>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/vector.hpp>
#include <wjr/span_std.hpp>

namespace {

template <typename T>
concept accepts_std_bridge = requires(T &&value) { wjr::as_std_span(std::forward<T>(value)); };

template <typename T>
concept accepts_wjr_bridge = requires(T &&value) { wjr::as_wjr_span(std::forward<T>(value)); };

static_assert(accepts_std_bridge<wjr::span<int>>);
static_assert(accepts_std_bridge<const wjr::span<int> &>);
static_assert(accepts_wjr_bridge<std::span<int>>);
static_assert(accepts_wjr_bridge<const std::span<int> &>);
static_assert(!accepts_std_bridge<std::vector<int> &>);
static_assert(!accepts_std_bridge<std::vector<int>>);
static_assert(!accepts_std_bridge<wjr::vector<int> &>);
static_assert(!accepts_std_bridge<wjr::vector<int>>);
static_assert(!accepts_wjr_bridge<std::vector<int> &>);
static_assert(!accepts_wjr_bridge<std::vector<int>>);
static_assert(!accepts_std_bridge<std::array<int, 3> &>);
static_assert(!accepts_wjr_bridge<int (&)[3]>);
static_assert(
    std::same_as<decltype(wjr::as_std_span(std::declval<wjr::span<int, 3>>())), std::span<int, 3>>);
static_assert(std::same_as<decltype(wjr::as_wjr_span(std::declval<std::span<const int, 3>>())),
                           wjr::span<const int, 3>>);
static_assert(
    std::same_as<decltype(wjr::as_std_span(std::declval<wjr::span<const volatile int>>())),
                 std::span<const volatile int>>);
static_assert(
    std::same_as<decltype(wjr::as_wjr_span(std::declval<std::span<const volatile int>>())),
                 wjr::span<const volatile int>>);
static_assert(noexcept(wjr::as_std_span(std::declval<wjr::span<int>>())));
static_assert(noexcept(wjr::as_wjr_span(std::declval<std::span<int>>())));
static_assert(!std::ranges::borrowed_range<wjr::span<int>>);
static_assert(std::ranges::borrowed_range<std::span<int>>);
static_assert(ranges::contiguous_iterator<wjr::span<int>::iterator>);
static_assert(ranges::contiguous_iterator<wjr::span<int>::const_iterator>);
static_assert(
    std::ranges::contiguous_range<decltype(wjr::as_std_span(std::declval<wjr::span<int>>()))>);

constexpr bool check_round_trip() {
    int values[] = {1, 2, 3};
    wjr::span original(values);
    auto standard = wjr::as_std_span(original);
    auto restored = wjr::as_wjr_span(standard);
    standard[1] = 7;
    return standard.data() == original.data() && restored.data() == original.data() &&
           standard.size() == 3 && restored.size() == 3 && restored[1] == 7;
}

static_assert(check_round_trip());

constexpr bool check_empty_spans() {
    auto standard = wjr::as_std_span(wjr::span<int>{});
    auto restored = wjr::as_wjr_span(standard);
    auto fixed = wjr::as_std_span(wjr::span<int, 0>{});
    auto fixed_restored = wjr::as_wjr_span(fixed);
    static_assert(std::same_as<decltype(fixed), std::span<int, 0>>);
    static_assert(std::same_as<decltype(fixed_restored), wjr::span<int, 0>>);
    return standard.empty() && restored.empty() && fixed.empty() && fixed_restored.empty() &&
           standard.data() == nullptr && fixed_restored.data() == nullptr;
}

static_assert(check_empty_spans());

} // namespace

TEST(span_std, standard_algorithms_use_live_storage) {
    int values[] = {4, 2, 1, 3};
    auto standard = wjr::as_std_span(wjr::span(values));
    std::ranges::sort(standard);
    EXPECT_TRUE(std::ranges::equal(values, std::array{1, 2, 3, 4}));

    // The temporary WJR descriptor is gone; the standard iterator still refers to values.
    auto found = std::ranges::find(wjr::as_std_span(wjr::span(values)), 3);
    EXPECT_EQ(*found, 3);
    *found = 8;
    EXPECT_EQ(values[2], 8);
}

TEST(span_std, explicit_owner_view_and_round_trip) {
    std::vector<int> values = {1, 2, 3};
    auto standard = wjr::as_std_span(wjr::span(values));
    auto restored = wjr::as_wjr_span(standard);
    EXPECT_EQ(standard.data(), values.data());
    EXPECT_EQ(restored.data(), values.data());
    EXPECT_EQ(restored.size(), values.size());
    restored[0] = 9;
    EXPECT_EQ(values[0], 9);

    wjr::vector<int> custom = {4, 5, 6};
    auto custom_standard = wjr::as_std_span(wjr::span(custom));
    EXPECT_EQ(custom_standard.data(), custom.data());
    EXPECT_EQ(std::ranges::find(custom_standard, 5) - custom_standard.begin(), 1);
}

TEST(span_std, standard_descriptor_can_be_temporary) {
    const int values[] = {3, 5, 7};
    auto restored = wjr::as_wjr_span(std::span(values));
    static_assert(std::same_as<decltype(restored), wjr::span<const int, 3>>);
    EXPECT_EQ(restored.data(), values);
    EXPECT_EQ(restored.size(), 3);
    EXPECT_EQ(*restored.cbegin(), 3);
    EXPECT_EQ(*restored.crbegin(), 7);
    EXPECT_EQ(std::to_address(restored.end()), values + 3);
}
