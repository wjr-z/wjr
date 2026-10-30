#include <wjr/tuple.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

template <typename T>
constexpr bool check_get_types() {
    using tuple = wjr::tuple<T>;
    static_assert(std::is_same_v<decltype(std::get<0>(std::declval<tuple &>())), T &>);
    static_assert(std::is_same_v<decltype(std::get<0>(std::declval<const tuple &>())), const T &>);
    static_assert(std::is_same_v<decltype(std::get<0>(std::declval<tuple &&>())), T &&>);
    static_assert(
        std::is_same_v<decltype(std::get<0>(std::declval<const tuple &&>())), const T &&>);
    static_assert(std::is_same_v<decltype(std::get<T>(std::declval<tuple &>())), T &>);
    static_assert(std::is_same_v<decltype(std::get<T>(std::declval<const tuple &>())), const T &>);
    static_assert(std::is_same_v<decltype(std::get<T>(std::declval<tuple &&>())), T &&>);
    static_assert(
        std::is_same_v<decltype(std::get<T>(std::declval<const tuple &&>())), const T &&>);
    static_assert(noexcept(std::get<0>(std::declval<const tuple &>())));
    static_assert(noexcept(std::get<T>(std::declval<const tuple &&>())));
    return true;
}

static_assert(check_get_types<int>());
static_assert(check_get_types<const int>());
static_assert(check_get_types<volatile int>());
static_assert(check_get_types<std::string>());
static_assert(check_get_types<int &>());
static_assert(check_get_types<const int &>());
static_assert(check_get_types<int &&>());
static_assert(check_get_types<const int &&>());

constexpr bool constexpr_access() {
    const wjr::tuple<int, long> value(3, 5);
    return std::get<0>(value) == 3 && std::get<long>(value) == 5 &&
           std::get<0>(std::move(value)) == 3 && std::get<long>(std::move(value)) == 5;
}
static_assert(constexpr_access());

struct counted_value {
    static inline int copies = 0;
    static inline int moves = 0;
    int value;

    explicit counted_value(int v) noexcept : value(v) {}
    counted_value(const counted_value &other) noexcept : value(other.value) { ++copies; }
    counted_value(counted_value &&other) noexcept : value(other.value) { ++moves; }
};

struct materialized_value {
    int number;
    std::string text;
    materialized_value(int n, const std::string &value) : number(n), text(value) {}
};

} // namespace

TEST(tuple_access, const_owned_elements_keep_identity_for_both_get_forms) {
    const wjr::tuple<int, std::string> value(17, "tuple");
    EXPECT_EQ(std::get<0>(value), 17);
    EXPECT_EQ(std::get<std::string>(value), "tuple");
    EXPECT_EQ(std::addressof(std::get<0>(value)), std::addressof(value.get<0>()));
    EXPECT_EQ(std::addressof(std::get<std::string>(value)), std::addressof(value.get<1>()));

    const int &&first = std::get<0>(std::move(value));
    const std::string &&second = std::get<std::string>(std::move(value));
    EXPECT_EQ(std::addressof(first), std::addressof(value.get<0>()));
    EXPECT_EQ(std::addressof(second), std::addressof(value.get<1>()));
}

TEST(tuple_access, const_reference_tuple_keeps_referent_mutability) {
    int first = 23;
    std::string second = "reference";
    const wjr::tuple<int &, std::string &&> references(first, std::move(second));
    std::get<0>(references) = 31;
    std::get<int &>(std::move(references)) = 37;
    auto &&text = std::get<std::string &&>(std::move(references));
    text += " tuple";
    EXPECT_EQ(first, 37);
    EXPECT_EQ(second, "reference tuple");
    EXPECT_EQ(std::addressof(std::get<0>(references)), std::addressof(first));
    EXPECT_EQ(std::addressof(text), std::addressof(second));
}

TEST(tuple_access, apply_and_comparisons_accept_const_owned_tuples) {
    const wjr::tuple<int, std::string> first(3, "value");
    const wjr::tuple<int, std::string> same(3, "value");
    const wjr::tuple<int, std::string> greater(3, "zebra");
    EXPECT_EQ(
        wjr::apply([](const int &number,
                      const std::string &text) { return number + static_cast<int>(text.size()); },
                   first),
        8);
    EXPECT_TRUE(first == same);
    EXPECT_TRUE(first != greater);
    EXPECT_TRUE(first < greater);
    EXPECT_TRUE(first <= same);
    EXPECT_TRUE(greater > first);
    EXPECT_TRUE(same >= first);
}

TEST(tuple_access, make_from_tuple_uses_internal_apply_for_standard_element_types) {
    const wjr::tuple<int, std::string> extended(7, "extended");
    const std::tuple<int, std::string> standard(11, "standard");
    auto first = wjr::make_from_tuple<materialized_value>(extended);
    auto second = wjr::make_from_tuple<materialized_value>(standard);
    EXPECT_EQ(first.number, 7);
    EXPECT_EQ(first.text, "extended");
    EXPECT_EQ(second.number, 11);
    EXPECT_EQ(second.text, "standard");
}

TEST(tuple_access, concatenation_copies_const_sources_into_owning_results) {
    const wjr::tuple<counted_value> source(counted_value(41));
    counted_value::copies = 0;
    counted_value::moves = 0;
    auto copied = wjr::tuple_cat(source);
    static_assert(std::is_same_v<decltype(copied), wjr::tuple<counted_value>>);
    EXPECT_EQ(std::get<0>(copied).value, 41);
    EXPECT_EQ(counted_value::copies, 1);
    EXPECT_EQ(counted_value::moves, 0);

    counted_value::copies = 0;
    auto from_const_rvalue = wjr::tuple_cat(std::move(source));
    static_assert(std::is_same_v<decltype(from_const_rvalue), wjr::tuple<counted_value>>);
    EXPECT_EQ(std::get<0>(from_const_rvalue).value, 41);
    EXPECT_EQ(counted_value::copies, 1);
    EXPECT_EQ(counted_value::moves, 0);
}

TEST(tuple_access, concatenation_combines_const_standard_and_extended_sources) {
    const std::tuple<int> standard(43);
    const wjr::tuple<std::string> extended("value");
    auto combined = wjr::tuple_cat(standard, extended);
    static_assert(std::is_same_v<decltype(combined), wjr::tuple<int, std::string>>);
    EXPECT_EQ(std::get<0>(combined), 43);
    EXPECT_EQ(std::get<1>(combined), "value");
}
