#include <wjr/compressed_pair.hpp>

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace {

using wjr::compressed_pair;

struct implicit_default {};
struct explicit_default {
    explicit constexpr explicit_default() noexcept = default;
};
struct empty_from_int {
    explicit constexpr empty_from_int(int) noexcept {}
};
struct value_from_int final {
    int value;
    explicit constexpr value_from_int(int v) noexcept : value(v) {}
};

template <typename Pair>
void accept_pair(Pair);

template <typename Pair, typename... Args>
concept implicitly_list_constructible = requires { accept_pair<Pair>({std::declval<Args>()...}); };

static_assert(implicitly_list_constructible<compressed_pair<int, long>>);
static_assert(implicitly_list_constructible<compressed_pair<implicit_default, int>>);
static_assert(std::is_default_constructible_v<compressed_pair<explicit_default, int>>);
static_assert(!implicitly_list_constructible<compressed_pair<explicit_default, int>>);
static_assert(!implicitly_list_constructible<compressed_pair<int, explicit_default>>);
static_assert(!std::is_default_constructible_v<compressed_pair<empty_from_int, int>>);
static_assert(!std::is_default_constructible_v<compressed_pair<int, empty_from_int>>);
static_assert(!std::is_default_constructible_v<compressed_pair<value_from_int, int>>);

static_assert(implicitly_list_constructible<compressed_pair<int, long>, short, int>);
static_assert(std::is_constructible_v<compressed_pair<empty_from_int, int>, int, int>);
static_assert(!implicitly_list_constructible<compressed_pair<empty_from_int, int>, int, int>);
static_assert(!implicitly_list_constructible<compressed_pair<int, empty_from_int>, int, int>);
static_assert(!implicitly_list_constructible<compressed_pair<value_from_int, int>, int, int>);
static_assert(!std::is_constructible_v<compressed_pair<value_from_int, int>, void *, int>);
static_assert(!std::is_constructible_v<compressed_pair<int, int>, int>);
static_assert(!std::is_constructible_v<compressed_pair<int, int>, int, int, int>);

static_assert(
    std::is_constructible_v<compressed_pair<empty_from_int, int>, std::tuple<int, short>>);
static_assert(!std::is_convertible_v<std::tuple<int, short>, compressed_pair<empty_from_int, int>>);
static_assert(std::is_convertible_v<std::pair<short, int>, compressed_pair<int, long>>);
static_assert(!std::is_constructible_v<compressed_pair<int, long>, std::tuple<int>>);
static_assert(!std::is_constructible_v<compressed_pair<int, long>, std::tuple<int, int, int>>);
static_assert(!std::is_constructible_v<compressed_pair<int, long>, int>);
static_assert(!std::is_assignable_v<compressed_pair<const int, int> &, std::pair<int, int>>);
static_assert(!std::is_assignable_v<compressed_pair<int, int> &, std::tuple<int>>);

using move_only_pair = compressed_pair<std::unique_ptr<int>, implicit_default>;
static_assert(std::is_move_constructible_v<move_only_pair>);
static_assert(!std::is_copy_constructible_v<move_only_pair>);
static_assert(std::is_constructible_v<move_only_pair, std::unique_ptr<int>, implicit_default>);
static_assert(!std::is_constructible_v<move_only_pair, std::unique_ptr<int> &, implicit_default>);

template <typename Pair, size_t I>
concept member_gettable = requires(Pair &&pair) { std::forward<Pair>(pair).template get<I>(); };

template <typename Pair, size_t I>
concept standard_gettable = requires(Pair &&pair) { std::get<I>(std::forward<Pair>(pair)); };

static_assert(member_gettable<compressed_pair<int, long> &, 0>);
static_assert(member_gettable<const compressed_pair<int, long> &, 1>);
static_assert(member_gettable<compressed_pair<int, long>, 0>);
static_assert(standard_gettable<compressed_pair<int, long> &, 0>);
static_assert(standard_gettable<const compressed_pair<int, long> &, 1>);
static_assert(standard_gettable<compressed_pair<int, long>, 0>);

struct throwing_destructor_empty {
    ~throwing_destructor_empty() noexcept(false) {}
};
struct throwing_destructor_value final {
    int value;
    ~throwing_destructor_value() noexcept(false) {}
};

/// @note Storage constraints retain trait admission, including throwing destructors.
static_assert(!std::default_initializable<throwing_destructor_empty>);
static_assert(std::is_default_constructible_v<compressed_pair<throwing_destructor_empty, int>>);
static_assert(std::is_default_constructible_v<compressed_pair<int, throwing_destructor_empty>>);
static_assert(std::is_default_constructible_v<
              compressed_pair<throwing_destructor_value, throwing_destructor_empty>>);
static_assert(std::is_constructible_v<compressed_pair<throwing_destructor_empty, int>,
                                      wjr::default_construct_t, wjr::default_construct_t>);
static_assert(std::is_constructible_v<compressed_pair<int, throwing_destructor_empty>, int,
                                      wjr::default_construct_t>);

using ordinary_pair = compressed_pair<int, long>;
static_assert(std::same_as<decltype(&ordinary_pair::swap),
                           void (ordinary_pair::*)(ordinary_pair &) noexcept>);
static_assert([] {
    ordinary_pair first{1, 2}, second{3, 4};
    auto swap_members = &ordinary_pair::swap;
    (first.*swap_members)(second);
    return first.first() == 3 && first.second() == 4 && second.first() == 1 && second.second() == 2;
}());

} // namespace
