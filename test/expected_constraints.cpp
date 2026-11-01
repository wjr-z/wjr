#include <concepts>
#include <type_traits>

#include <wjr/expected.hpp>

namespace {
struct explicit_integer {
    constexpr explicit explicit_integer(int v) noexcept : value(v) {}
    int value;
};

struct implicit_integer {
    constexpr implicit_integer(int v) noexcept : value(v) {}
    int value;
};

struct unrelated {};

struct throwing_destructor {
    explicit throwing_destructor(int) noexcept {}
    ~throwing_destructor() noexcept(false) {}
};

struct immobile {
    immobile() = default;
    immobile(const immobile &) = delete;
    immobile(immobile &&) = delete;
};

template <typename Result>
concept member_swappable = requires(Result &left, Result &right) { left.swap(right); };

using wjr::expected;
using wjr::unexpected;

static_assert(std::is_constructible_v<expected<explicit_integer, int>, int>);
static_assert(!std::is_convertible_v<int, expected<explicit_integer, int>>);
static_assert(std::is_convertible_v<int, expected<implicit_integer, int>>);
static_assert(!std::is_constructible_v<expected<explicit_integer, int>, unrelated>);
static_assert(std::is_convertible_v<const expected<short, short> &, expected<int, long>>);
static_assert(!std::is_convertible_v<const expected<int, int> &, expected<explicit_integer, long>>);
static_assert(
    std::is_constructible_v<expected<explicit_integer, long>, const expected<int, int> &>);

static_assert(std::is_convertible_v<unexpected<int>, expected<int, implicit_integer>>);
static_assert(!std::is_convertible_v<unexpected<int>, expected<int, explicit_integer>>);
static_assert(std::is_constructible_v<expected<int, explicit_integer>, unexpected<int>>);
static_assert(std::is_convertible_v<unexpected<int>, expected<void, implicit_integer>>);
static_assert(!std::is_convertible_v<unexpected<int>, expected<void, explicit_integer>>);
static_assert(std::is_constructible_v<expected<void, explicit_integer>, unexpected<int>>);

/// @note std::constructible_from also requires a nonthrowing destructor; the legacy domain does
/// not.
static_assert(!std::constructible_from<throwing_destructor, int>);
static_assert(std::is_constructible_v<unexpected<throwing_destructor>, int>);
static_assert(std::is_constructible_v<expected<throwing_destructor, int>, std::in_place_t, int>);

static_assert(member_swappable<expected<int, int>>);
static_assert(member_swappable<expected<void, int>>);
static_assert(!member_swappable<expected<int, immobile>>);
static_assert(!member_swappable<expected<void, immobile>>);
static_assert(std::is_trivially_copyable_v<expected<int, int>>);
static_assert(std::is_nothrow_move_constructible_v<expected<int, int>>);

constexpr bool converting_values() {
    const expected<int, int> original(17);
    const expected<explicit_integer, long> converted(original);
    const expected<void, explicit_integer> failure(unexpected<int>(23));
    return converted->value == 17 && !failure && failure.error().value == 23;
}
static_assert(converting_values());
} // namespace
