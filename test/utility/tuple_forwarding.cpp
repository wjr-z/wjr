#include <wjr/tp/args.hpp>
#include <wjr/tuple.hpp>

#include <concepts>
#include <memory>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

namespace {

struct immovable {
    int value = 17;
    immovable() = default;
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
};

struct move_error {};

struct throwing_move {
    static inline int moves = 0;
    throwing_move() = default;
    throwing_move(const throwing_move &) = delete;
    throwing_move(throwing_move &&) {
        ++moves;
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw move_error{};
#endif
    }
};

struct counted_value {
    static inline int copies = 0;
    static inline int moves = 0;
    int value = 0;

    counted_value() = default;
    explicit counted_value(int v) : value(v) {}
    counted_value(const counted_value &other) : value(other.value) { ++copies; }
    counted_value(counted_value &&other) noexcept : value(other.value) { ++moves; }
};

struct owning_callable {
    std::unique_ptr<int> value;
    int operator()(int delta) { return *value += delta; }
};

struct borrowed_callable {
    int value = 0;
    borrowed_callable() = default;
    borrowed_callable(const borrowed_callable &) = delete;
    borrowed_callable(borrowed_callable &&) = delete;
    int operator()(int delta) { return value += delta; }
};

struct copy_only {
    static inline int copies = 0;
    copy_only() = default;
    copy_only(const copy_only &) { ++copies; }
    copy_only(copy_only &&) = delete;
};

static_assert(std::same_as<decltype(wjr::forward_as_tuple()), wjr::tuple<>>);
static_assert(
    std::same_as<decltype(wjr::forward_as_tuple(std::declval<int &>())), wjr::tuple<int &>>);
static_assert(std::same_as<decltype(wjr::forward_as_tuple(std::declval<const int &>())),
                           wjr::tuple<const int &>>);
static_assert(
    std::same_as<decltype(wjr::forward_as_tuple(std::declval<int &&>())), wjr::tuple<int &&>>);
static_assert(noexcept(wjr::forward_as_tuple(std::declval<throwing_move &&>())));
static_assert(noexcept(wjr::forward_as_tuple(std::declval<immovable &&>())));

TEST(tuple_forwarding, preserves_reference_identity_without_copying_or_moving) {
    counted_value value(23);
    const int constant = 31;
    counted_value::copies = 0;
    counted_value::moves = 0;
    auto references = wjr::forward_as_tuple(value, std::move(value), constant);
    EXPECT_EQ(std::addressof(std::get<0>(references)), std::addressof(value));
    EXPECT_EQ(std::addressof(std::get<1>(references)), std::addressof(value));
    EXPECT_EQ(std::addressof(std::get<2>(references)), std::addressof(constant));
    static_assert(std::same_as<decltype(std::get<1>(std::move(references))), counted_value &&>);
    EXPECT_EQ(counted_value::copies, 0);
    EXPECT_EQ(counted_value::moves, 0);
}

TEST(tuple_forwarding, binds_immovable_and_throwing_move_values) {
    immovable value;
    auto reference = wjr::forward_as_tuple(std::move(value));
    EXPECT_EQ(std::addressof(std::get<0>(reference)), std::addressof(value));
    EXPECT_EQ(
        wjr::apply([](immovable &&item) { return item.value; }, wjr::forward_as_tuple(immovable{})),
        17);

    throwing_move throwing;
    throwing_move::moves = 0;
    auto throwing_reference = wjr::forward_as_tuple(std::move(throwing));
    EXPECT_EQ(std::addressof(std::get<0>(throwing_reference)), std::addressof(throwing));
    EXPECT_EQ(throwing_move::moves, 0);
}

TEST(tuple_forwarding, single_tuple_argument_is_borrowed_without_unpacking) {
    std::tuple<int> standard{23};
    wjr::tuple<int> extended{31};
    const std::tuple<int> constant_standard{41};
    const wjr::tuple<int> constant_extended{43};
    auto standard_lvalue = wjr::forward_as_tuple(standard);
    auto extended_lvalue = wjr::forward_as_tuple(extended);
    auto standard_constant = wjr::forward_as_tuple(constant_standard);
    auto extended_constant = wjr::forward_as_tuple(constant_extended);
    auto standard_const_rvalue = wjr::forward_as_tuple(std::move(constant_standard));
    auto extended_const_rvalue = wjr::forward_as_tuple(std::move(constant_extended));
    auto standard_reference = wjr::forward_as_tuple(std::move(standard));
    auto extended_reference = wjr::forward_as_tuple(std::move(extended));
    EXPECT_EQ(std::addressof(std::get<0>(standard_lvalue)), std::addressof(standard));
    EXPECT_EQ(std::addressof(std::get<0>(extended_lvalue)), std::addressof(extended));
    EXPECT_EQ(std::addressof(std::get<0>(standard_constant)), std::addressof(constant_standard));
    EXPECT_EQ(std::addressof(std::get<0>(extended_constant)), std::addressof(constant_extended));
    EXPECT_EQ(std::addressof(std::get<0>(standard_const_rvalue)),
              std::addressof(constant_standard));
    EXPECT_EQ(std::addressof(std::get<0>(extended_const_rvalue)),
              std::addressof(constant_extended));
    EXPECT_EQ(std::addressof(std::get<0>(standard_reference)), std::addressof(standard));
    EXPECT_EQ(std::addressof(std::get<0>(extended_reference)), std::addressof(extended));
}

TEST(tuple_forwarding, single_element_copy_and_move_construction) {
    wjr::tuple<counted_value> source{counted_value{23}};
    counted_value::copies = 0;
    counted_value::moves = 0;
    auto copied = source;
    EXPECT_EQ(std::get<0>(copied).value, 23);
    EXPECT_EQ(counted_value::copies, 1);
    EXPECT_EQ(counted_value::moves, 0);
    auto moved = std::move(source);
    EXPECT_EQ(std::get<0>(moved).value, 23);
    EXPECT_EQ(counted_value::copies, 1);
    EXPECT_EQ(counted_value::moves, 1);

    copy_only value;
    copy_only::copies = 0;
    const wjr::tuple<copy_only> copy_only_tuple(value);
    EXPECT_EQ(copy_only::copies, 1);
    const auto copy = copy_only_tuple;
    EXPECT_EQ(copy_only::copies, 2);
    static_assert(!std::is_copy_constructible_v<wjr::tuple<std::unique_ptr<int>>>);
    static_assert(std::is_move_constructible_v<wjr::tuple<std::unique_ptr<int>>>);
}

TEST(tuple_forwarding, nested_values_keep_direct_construction_and_self_copy) {
    const std::tuple<int> standard{23};
    const wjr::tuple<int> extended{31};
    wjr::tuple<std::tuple<int>> standard_owner(standard);
    wjr::tuple<wjr::tuple<int>> extended_owner(extended);
    EXPECT_EQ(std::get<0>(std::get<0>(standard_owner)), 23);
    EXPECT_EQ(std::get<0>(std::get<0>(extended_owner)), 31);
    auto copy = extended_owner;
    auto moved = std::move(copy);
    EXPECT_EQ(std::get<0>(std::get<0>(moved)), 31);

    std::tuple<std::unique_ptr<int>> source{std::make_unique<int>(41)};
    wjr::tuple<std::tuple<std::unique_ptr<int>>> owner(std::move(source));
    EXPECT_EQ(*std::get<0>(std::get<0>(owner)), 41);
    EXPECT_EQ(std::get<0>(source), nullptr);
    auto moved_owner = std::move(owner);
    EXPECT_EQ(*std::get<0>(std::get<0>(moved_owner)), 41);
}

TEST(tuple_forwarding, concatenation_owns_move_only_results) {
    std::tuple<std::unique_ptr<int>> standard{std::make_unique<int>(23)};
    wjr::tuple<std::unique_ptr<int>> extended{std::make_unique<int>(31)};
    auto result = wjr::tuple_cat(std::move(standard), std::move(extended));
    static_assert(
        std::same_as<decltype(result), wjr::tuple<std::unique_ptr<int>, std::unique_ptr<int>>>);
    EXPECT_EQ(*std::get<0>(result), 23);
    EXPECT_EQ(*std::get<1>(result), 31);
    EXPECT_EQ(std::get<0>(standard), nullptr);
    EXPECT_EQ(std::get<0>(extended), nullptr);
}

TEST(tuple_forwarding, concatenation_moves_elements_once) {
    std::tuple<counted_value> source{counted_value{23}};
    counted_value::copies = 0;
    counted_value::moves = 0;
    auto result = wjr::tuple_cat(std::move(source));
    EXPECT_EQ(std::get<0>(result).value, 23);
    EXPECT_EQ(counted_value::copies, 0);
    EXPECT_EQ(counted_value::moves, 1);
}

TEST(tuple_forwarding, saved_dispatch_owns_temporary_and_borrows_lvalue_callables) {
    auto single = wjr::tp_make_functions(owning_callable{std::make_unique<int>(7)});
    EXPECT_EQ(single(std::integral_constant<std::size_t, 0>{}, 2), 9);
    EXPECT_EQ(single(std::integral_constant<std::size_t, 0>{}, 3), 12);

    borrowed_callable borrowed;
    auto mixed = wjr::tp_make_functions(owning_callable{std::make_unique<int>(20)}, borrowed);
    EXPECT_EQ(mixed(std::integral_constant<std::size_t, 0>{}, 2), 22);
    EXPECT_EQ(mixed(std::integral_constant<std::size_t, 1>{}, 5), 5);
    EXPECT_EQ(borrowed.value, 5);
}

} // namespace
