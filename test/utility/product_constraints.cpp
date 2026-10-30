#include "detail.hpp"

#include <wjr/compressed_pair.hpp>

namespace {

struct no_default {
    no_default() = delete;
    explicit no_default(int) noexcept {}
};

struct explicit_default {
    explicit explicit_default() noexcept = default;
};

struct throwing_destructor {
    throwing_destructor() = default;
    explicit throwing_destructor(int) noexcept {}
    ~throwing_destructor() noexcept(false) {}
};

struct empty {};
struct final_empty final {};
struct alignas(32) aligned {
    int value;
};

struct move_only {
    move_only() = default;
    move_only(const move_only &) = delete;
    move_only(move_only &&) noexcept = default;
    move_only &operator=(const move_only &) = delete;
    move_only &operator=(move_only &&) noexcept = default;
};

struct throwing_conversion {
    throwing_conversion(int) noexcept(false) {}
};

template <typename T, size_t I>
concept has_member_get = requires(T &&value) { std::forward<T>(value).template get<I>(); };

using wjr::capture_leaf;
using wjr::compressed_capture_leaf;
using wjr::tuple;

static_assert(std::is_default_constructible_v<capture_leaf<int>>);
static_assert(!std::is_default_constructible_v<capture_leaf<no_default>>);
static_assert(std::is_constructible_v<capture_leaf<no_default>, int>);
static_assert(!std::is_constructible_v<capture_leaf<no_default>, const char *>);
static_assert(!std::is_default_constructible_v<capture_leaf<int &>>);
static_assert(std::is_constructible_v<capture_leaf<int &>, int &>);
static_assert(!std::is_constructible_v<capture_leaf<int &>, int>);
static_assert(std::is_constructible_v<capture_leaf<final_empty>, final_empty>);
static_assert(std::is_default_constructible_v<compressed_capture_leaf<empty>>);
static_assert(!std::is_default_constructible_v<compressed_capture_leaf<no_default>>);
static_assert(std::is_constructible_v<compressed_capture_leaf<no_default>, int>);
static_assert(!std::is_constructible_v<compressed_capture_leaf<no_default>, const char *>);

// Construction traits intentionally admit throwing destruction, unlike standard concepts.
static_assert(std::is_constructible_v<throwing_destructor, int>);
static_assert(!std::constructible_from<throwing_destructor, int>);
static_assert(std::is_constructible_v<capture_leaf<throwing_destructor>, int>);
static_assert(std::is_default_constructible_v<capture_leaf<throwing_destructor>>);
static_assert(std::is_constructible_v<tuple<throwing_destructor>, int>);
static_assert(std::is_default_constructible_v<tuple<throwing_destructor>>);

static_assert(std::is_default_constructible_v<tuple<int, explicit_default>>);
static_assert(!wjr::is_default_convertible_v<tuple<int, explicit_default>>);
static_assert(!std::is_default_constructible_v<tuple<int, no_default>>);
static_assert(std::is_constructible_v<tuple<no_default, int>, int, short>);
static_assert(!std::is_constructible_v<tuple<int, int>, int>);
static_assert(!std::is_constructible_v<tuple<int>, int, int>);
static_assert(!std::is_constructible_v<tuple<int, no_default>, int, const char *>);
static_assert(std::is_constructible_v<tuple<int, long>, std::pair<short, int>>);
static_assert(std::is_constructible_v<tuple<int, long>, const std::tuple<short, int> &>);
static_assert(!std::is_constructible_v<tuple<int, long>, std::tuple<int>>);
static_assert(!std::is_constructible_v<tuple<int, long>, std::tuple<int, const char *>>);
static_assert(!std::is_constructible_v<tuple<int, long>, int>);
static_assert(std::is_constructible_v<tuple<move_only>, std::tuple<move_only> &&>);
static_assert(!std::is_constructible_v<tuple<move_only>, std::tuple<move_only> &>);
static_assert(std::is_assignable_v<tuple<int &, long> &, std::pair<short, int>>);
static_assert(!std::is_assignable_v<tuple<const int &, long> &, std::pair<short, int>>);
static_assert(!std::is_assignable_v<tuple<int, long> &, std::tuple<int>>);
static_assert(!std::is_assignable_v<tuple<int, long> &, int>);

static_assert(has_member_get<tuple<int> &, 0>);
static_assert(!has_member_get<tuple<int> &, 1>);
static_assert(std::same_as<decltype(std::get<0>(std::declval<tuple<int> &>())), int &>);
static_assert(std::same_as<decltype(std::get<0>(std::declval<const tuple<int> &>())), const int &>);
static_assert(std::same_as<decltype(std::get<int>(std::declval<tuple<int> &&>())), int &&>);
static_assert(
    std::same_as<decltype(std::get<int>(std::declval<const tuple<int> &&>())), const int &&>);
static_assert(std::same_as<decltype(std::get<0>(std::declval<const tuple<int &> &&>())), int &>);
static_assert(std::is_nothrow_constructible_v<tuple<int, long>, int, int>);
static_assert(!std::is_nothrow_constructible_v<tuple<throwing_conversion>, int>);
static_assert(noexcept(std::swap(std::declval<tuple<int> &>(), std::declval<tuple<int> &>())));

static_assert(std::is_trivially_copyable_v<capture_leaf<int>>);
static_assert(std::is_trivially_destructible_v<capture_leaf<int>>);
static_assert(std::is_trivially_copyable_v<tuple<int, long>>);
static_assert(std::is_trivially_destructible_v<tuple<int, long>>);
static_assert(sizeof(capture_leaf<int>) == sizeof(int));
static_assert(sizeof(capture_leaf<int &>) == sizeof(int *));
static_assert(sizeof(compressed_capture_leaf<empty>) == sizeof(empty));
static_assert(sizeof(tuple<int>) == sizeof(int));
static_assert(alignof(capture_leaf<aligned>) == alignof(aligned));
static_assert(alignof(tuple<aligned, int>) == alignof(aligned));
static_assert(wjr::get_relocate_mode_v<tuple<int, long>> ==
              wjr::get_common_relocate_mode_v<int, long>);

TEST(product_constraints, borrowed_assignment_and_swap) {
    int first = 1;
    tuple<int &, long> value(first, 2L);
    value = std::pair<short, int>{3, 4};
    EXPECT_EQ(first, 3);
    EXPECT_EQ(std::get<1>(value), 4);
    EXPECT_EQ(&std::get<0>(std::as_const(value)), &first);

    int second = 5;
    tuple<int &, long> other(second, 6L);
    std::swap(value, other);
    EXPECT_EQ(first, 5);
    EXPECT_EQ(second, 3);
    EXPECT_EQ(std::get<1>(value), 6);
}

TEST(product_constraints, capture_and_tuple_preserve_values) {
    capture_leaf<no_default> leaf(1);
    compressed_capture_leaf<no_default> compressed(2);
    (void)leaf;
    (void)compressed;
    tuple<empty, final_empty, int> value(empty{}, final_empty{}, 7);
    EXPECT_EQ(std::get<2>(value), 7);
    tuple<move_only> moved(std::tuple<move_only>{});
    (void)moved;
}

} // namespace
