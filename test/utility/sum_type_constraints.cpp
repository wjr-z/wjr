#include <wjr/optional.hpp>
#include <wjr/utility/variant.hpp>

#include <gtest/gtest.h>

namespace {
struct explicit_value {
    explicit constexpr explicit_value(int v) : value(v) {}
    int value;
};
struct throwing_destructor {
    explicit throwing_destructor(int) {}
    ~throwing_destructor() noexcept(false) {}
};
struct derived_variant : std::variant<int> {
    using variant::variant;
};
struct private_variant : private std::variant<int> {};
struct ambiguous_variant : std::variant<int>, std::variant<double> {};
struct identity {
    template <class T>
    constexpr T &&operator()(T &&value) const noexcept {
        return std::forward<T>(value);
    }
};

template <class T>
concept visitable = requires(T &&value) { wjr::visit(identity{}, std::forward<T>(value)); };
template <class T>
concept matchable = requires(T &&value) { wjr::match(std::forward<T>(value), identity{}); };

static_assert(std::is_constructible_v<wjr::optional<explicit_value>, int>);
static_assert(!std::is_convertible_v<int, wjr::optional<explicit_value>>);
static_assert(std::is_constructible_v<wjr::optional<throwing_destructor>, std::in_place_t, int>);
static_assert(std::is_constructible_v<wjr::optional<long>, const wjr::optional<int> &>);
static_assert(std::is_convertible_v<const wjr::optional<int> &, wjr::optional<long>>);
static_assert(!std::is_constructible_v<wjr::optional<int>, std::in_place_t, const char *>);
static_assert(!std::is_constructible_v<wjr::optional<int>, const char *>);
static_assert(visitable<std::variant<int> &> && visitable<const std::variant<int> &>);
static_assert(visitable<derived_variant &> && visitable<const derived_variant &&>);
static_assert(matchable<derived_variant &>);
static_assert(!visitable<int> && !matchable<int>);
static_assert(!visitable<private_variant> && !visitable<ambiguous_variant>);
static_assert(!wjr::is_as_variant_v<private_variant>);
static_assert(!wjr::is_as_variant_v<ambiguous_variant>);
static_assert(
    std::same_as<decltype(wjr::visit(identity{}, std::declval<std::variant<int> &>())), int &>);
static_assert(
    std::same_as<decltype(wjr::match(std::declval<const derived_variant &&>(), identity{})),
                 const int &&>);
} // namespace

TEST(sum_type_constraints, optional_conversions_keep_value_and_engagement) {
    wjr::optional<int> source(std::in_place, 7);
    wjr::optional<long> converted(source);
    EXPECT_EQ(*converted, 7);
    EXPECT_TRUE(source.has_value());
    wjr::optional<explicit_value> explicit_result(9);
    EXPECT_EQ(explicit_result->value, 9);
    source = wjr::nullopt;
    wjr::optional<long> empty(source);
    EXPECT_FALSE(empty.has_value());
    wjr::optional<bool> false_value(wjr::optional<int>(0));
    EXPECT_TRUE(false_value.has_value());
    EXPECT_FALSE(*false_value);
}

TEST(sum_type_constraints, variant_admission_keeps_derived_borrowing) {
    derived_variant value(11);
    auto &borrowed = wjr::visit(identity{}, value);
    borrowed = 13;
    EXPECT_EQ(std::get<0>(value), 13);
    auto &matched = wjr::match(value, identity{});
    EXPECT_EQ(&matched, &borrowed);
}
