#include <wjr/functional.hpp>

#include <memory>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

constexpr int increment(int value) noexcept { return value + 1; }
int decrement(int value) { return value - 1; }
int wrong_arguments() noexcept { return 0; }
void wrong_result(int) noexcept {}

struct receiver {
    int value = 0;
    int add(int delta) noexcept { return value += delta; }
    int read() const noexcept { return value; }
};

struct borrowed_callable {
    int value = 0;
    borrowed_callable() = default;
    borrowed_callable(const borrowed_callable &) = delete;
    borrowed_callable(borrowed_callable &&) = delete;
    int operator()(int delta) noexcept { return value += delta; }
};

using ordinary_ref = wjr::function_ref<int(int)>;
using nothrow_ref = wjr::function_ref<int(int) noexcept>;
static_assert(std::is_constructible_v<ordinary_ref, decltype(&increment)>);
static_assert(std::is_constructible_v<nothrow_ref, decltype(&increment)>);
static_assert(std::is_constructible_v<ordinary_ref, decltype(&decrement)>);
static_assert(!std::is_constructible_v<nothrow_ref, decltype(&decrement)>);
static_assert(!std::is_constructible_v<ordinary_ref, decltype(&wrong_arguments)>);
static_assert(!std::is_constructible_v<ordinary_ref, decltype(&wrong_result)>);
static_assert(std::is_constructible_v<wjr::function_ref<void(int)>, decltype(&increment)>);
static_assert(std::is_constructible_v<nothrow_ref, borrowed_callable &>);
static_assert(!std::is_constructible_v<nothrow_ref, const borrowed_callable &>);
static_assert(!std::is_constructible_v<ordinary_ref, int>);
static_assert(!std::is_constructible_v<ordinary_ref, decltype(&receiver::add)>);
static_assert(std::is_copy_assignable_v<ordinary_ref>);
static_assert(!std::is_assignable_v<ordinary_ref &, borrowed_callable &>);
static_assert(std::is_assignable_v<ordinary_ref &, decltype(&increment)>);

using fixed_increment = wjr::nontype_t<&increment>;
using fixed_decrement = wjr::nontype_t<&decrement>;
static_assert(std::is_constructible_v<nothrow_ref, fixed_increment>);
static_assert(std::is_constructible_v<ordinary_ref, fixed_decrement>);
static_assert(!std::is_constructible_v<nothrow_ref, fixed_decrement>);
static_assert(!std::is_constructible_v<ordinary_ref, wjr::nontype_t<&wrong_arguments>>);
static_assert(std::is_constructible_v<wjr::function_ref<int(receiver &, int) noexcept>,
                                      wjr::nontype_t<&receiver::add>>);
static_assert(std::is_constructible_v<wjr::function_ref<int(const receiver &) const noexcept>,
                                      wjr::nontype_t<&receiver::read>>);
static_assert(std::is_constructible_v<wjr::function_ref<int &(receiver &) noexcept>,
                                      wjr::nontype_t<&receiver::value>>);
static_assert(!std::is_constructible_v<wjr::function_ref<int &(const receiver &) noexcept>,
                                       wjr::nontype_t<&receiver::value>>);

static_assert(wjr::invoke_r<long>(increment, 3) == 4);
static_assert(noexcept(wjr::invoke_r<int>(increment, 3)));
static_assert(!noexcept(wjr::invoke_r<int>(decrement, 3)));
static_assert(std::is_void_v<decltype(wjr::invoke_r<void>(increment, 3))>);

template <typename Function>
concept can_invoke_as_int =
    requires(Function &&function) { wjr::invoke_r<int>(std::forward<Function>(function), 3); };
static_assert(can_invoke_as_int<decltype(&increment)>);
static_assert(!can_invoke_as_int<decltype(&wrong_arguments)>);
static_assert(!can_invoke_as_int<decltype(&wrong_result)>);

} // namespace

TEST(function_ref_constraints, borrows_noncopyable_callable_identity) {
    borrowed_callable callable;
    nothrow_ref reference(callable);
    auto copy = reference;
    EXPECT_EQ(reference(3), 3);
    EXPECT_EQ(copy(5), 8);
    EXPECT_EQ(callable.value, 8);
    EXPECT_TRUE(noexcept(reference(1)));
}

TEST(function_ref_constraints, rebinds_to_function_and_copies_an_existing_reference) {
    ordinary_ref reference(&increment);
    EXPECT_EQ(reference(3), 4);
    reference = &decrement;
    EXPECT_EQ(reference(3), 2);
    ordinary_ref other(wjr::nontype<&increment>);
    reference = other;
    EXPECT_EQ(reference(3), 4);
    wjr::function_ref<void(int)> discarding(&increment);
    discarding(3);
}

TEST(function_ref_constraints, nontype_targets_invoke_members_and_preserve_references) {
    receiver object;
    wjr::function_ref<int(receiver &, int) noexcept> add(wjr::nontype<&receiver::add>);
    wjr::function_ref<int(const receiver &) const noexcept> read(wjr::nontype<&receiver::read>);
    wjr::function_ref<int &(receiver &) noexcept> value(wjr::nontype<&receiver::value>);
    EXPECT_EQ(add(object, 7), 7);
    EXPECT_EQ(read(object), 7);
    EXPECT_EQ(std::addressof(value(object)), std::addressof(object.value));
    value(object) = 11;
    EXPECT_EQ(read(object), 11);
}

TEST(function_ref_constraints, invoke_r_handles_void_conversions_and_member_access) {
    receiver object;
    EXPECT_EQ(wjr::invoke_r<long>(&receiver::add, object, 13), 13);
    wjr::invoke_r<void>(&receiver::add, object, 4);
    EXPECT_EQ(object.value, 17);
    auto &reference = wjr::invoke_r<int &>(&receiver::value, object);
    EXPECT_EQ(std::addressof(reference), std::addressof(object.value));
}
