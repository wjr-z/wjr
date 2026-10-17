#include <array>
#include <deque>
#include <initializer_list>
#include <list>
#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/vector.hpp>
#include <wjr/span.hpp>

namespace {

using wjr::span;

struct base {
    int value;
};

struct derived : base {
    int extra;
};

struct incompatible_data_range {
    base *begin() const noexcept { return nullptr; }
    base *end() const noexcept { return nullptr; }
    derived *data() const noexcept { return nullptr; }
};

struct nonthrowing_range {
    int values[3] = {1, 2, 3};
    int *begin() noexcept { return values; }
    int *end() noexcept { return values + 3; }
    int *data() noexcept { return values; }
    std::size_t size() const noexcept { return 3; }
};

struct throwing_data_range : nonthrowing_range {
    int *data() noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw std::runtime_error("range data failed");
#else
        return values;
#endif
    }
};

struct throwing_size_range : nonthrowing_range {
    std::size_t size() const noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw std::runtime_error("range size failed");
#else
        return 3;
#endif
    }
};

static_assert(std::default_initializable<span<int>>);
static_assert(std::default_initializable<span<int, 0>>);
static_assert(!std::default_initializable<span<int, 1>>);
static_assert(std::is_constructible_v<span<int>, int *, std::size_t>);
static_assert(std::is_constructible_v<span<const int>, int *, std::size_t>);
static_assert(!std::is_constructible_v<span<int>, const int *, std::size_t>);
static_assert(!std::is_constructible_v<span<int>, int, std::size_t>);
static_assert(!std::is_constructible_v<span<int>, std::list<int>::iterator, std::size_t>);
static_assert(!std::is_constructible_v<span<base>, derived *, std::size_t>);
static_assert(!std::is_constructible_v<span<base>, derived (&)[3]>);
static_assert(!std::is_constructible_v<span<base>, std::array<derived, 3> &>);
static_assert(!std::is_constructible_v<span<const base>, const std::array<derived, 3> &>);
static_assert(!std::is_constructible_v<span<base>, span<derived>>);
static_assert(!std::is_constructible_v<span<const base>, std::vector<derived> &>);
static_assert(!std::is_constructible_v<span<base>, incompatible_data_range &>);
static_assert(std::is_convertible_v<span<int, 3>, span<const int, 3>>);
static_assert(std::is_convertible_v<span<int, 3>, span<const int>>);
static_assert(!std::is_convertible_v<span<int>, span<const int, 3>>);
static_assert(std::is_constructible_v<span<const int, 3>, span<int>>);
static_assert(!std::is_constructible_v<span<const int, 3>, span<int, 4>>);
static_assert(std::is_constructible_v<span<int, 3>, int (&)[3]>);
static_assert(!std::is_constructible_v<span<int, 3>, int (&)[4]>);
static_assert(!std::is_constructible_v<span<int, 3>, std::array<int, 4> &>);
static_assert(std::is_constructible_v<span<int>, std::vector<int> &>);
static_assert(std::is_nothrow_constructible_v<span<int>, std::vector<int> &>);
static_assert(std::is_nothrow_constructible_v<span<int>, nonthrowing_range &>);
static_assert(!std::is_nothrow_constructible_v<span<int>, throwing_data_range &>);
static_assert(!std::is_nothrow_constructible_v<span<int>, throwing_size_range &>);
static_assert(std::is_constructible_v<span<const int>, const std::vector<int> &>);
static_assert(!std::is_constructible_v<span<int>, std::vector<int>>);
static_assert(std::is_constructible_v<span<const int>, std::vector<int>>);
static_assert(!std::is_constructible_v<span<int>, std::deque<int> &>);
static_assert(std::is_constructible_v<span<int>, std::span<int>>);
static_assert(std::is_constructible_v<span<int>, std::ranges::subrange<int *>>);
static_assert(!std::is_constructible_v<span<int>, std::initializer_list<int>>);
static_assert(std::is_constructible_v<span<const int>, std::initializer_list<int>>);
static_assert(std::same_as<std::iter_value_t<span<const int>::iterator>, int>);
static_assert(
    std::same_as<decltype(std::declval<const span<int> &>().begin()), span<int>::const_iterator>);
static_assert(std::same_as<decltype(std::declval<const span<int> &>().rbegin()),
                           span<int>::const_reverse_iterator>);
static_assert(std::is_trivially_copyable_v<span<int>>);
static_assert(std::is_trivially_copyable_v<span<int, 3>>);
static_assert(sizeof(span<int>) == sizeof(std::span<int>));
static_assert(sizeof(span<int, 3>) == sizeof(std::span<int, 3>));
static_assert(alignof(span<int>) == alignof(std::span<int>));

constexpr bool check_constant_views() {
    int values[] = {1, 2, 3, 4, 5, 6};
    span fixed(values);
    static_assert(std::same_as<decltype(fixed), span<int, 6>>);
    const auto prefix = fixed.first<2>();
    const auto suffix = fixed.last<2>();
    const auto middle = fixed.subspan<1, 3>();
    const auto rest = fixed.subspan<2>();
    static_assert(std::same_as<std::remove_cv_t<decltype(rest)>, span<int, 4>>);
    span<int> dynamic = fixed;
    const auto dynamic_middle = dynamic.subspan<2, 2>();
    const span<const int, 6> readonly = fixed;
    span<const int, 6> immutable = fixed;
    return prefix.size() == 2 && prefix[1] == 2 && suffix[0] == 5 && middle[2] == 4 &&
           rest.size() == 4 && rest[0] == 3 && dynamic_middle[1] == 4 && *readonly.begin() == 1 &&
           *readonly.rbegin() == 6 && std::to_address(readonly.end()) == values + 6 &&
           std::to_address(immutable.begin()) == values;
}

static_assert(check_constant_views());

constexpr bool check_empty_views() {
    span<int> dynamic;
    span<int, 0> fixed;
    return dynamic.empty() && fixed.empty() && dynamic.first<0>().empty() &&
           dynamic.last<0>().empty() && dynamic.subspan<0>().empty() &&
           dynamic.begin() == dynamic.end() && fixed.begin() == fixed.end();
}

static_assert(check_empty_views());

} // namespace

TEST(span, native_range_construction) {
    std::vector<int> values = {1, 2, 3, 4};
    wjr::span view(values);
    EXPECT_EQ(view.data(), values.data());
    EXPECT_EQ(view.size(), values.size());
    view[1] = 8;
    EXPECT_EQ(values[1], 8);

    const auto &readonly_values = values;
    wjr::span readonly(readonly_values);
    static_assert(std::same_as<decltype(readonly), wjr::span<const int>>);
    EXPECT_EQ(readonly[1], 8);

    std::span standard(values);
    wjr::span standard_view(standard);
    EXPECT_EQ(standard_view.data(), values.data());
    auto borrowed = std::ranges::subrange(values.data() + 1, values.data() + 3);
    wjr::span borrowed_view(std::move(borrowed));
    EXPECT_EQ(borrowed_view.size(), 2);
    EXPECT_EQ(borrowed_view[0], 8);

    wjr::vector<int> custom = {4, 5, 6};
    wjr::span custom_view(custom);
    EXPECT_EQ(custom_view.data(), custom.data());
    EXPECT_EQ(custom_view.size(), custom.size());
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(span, range_access_exceptions_propagate) {
    throwing_data_range data;
    throwing_size_range size;
    EXPECT_THROW((void)wjr::span<int>(data), std::runtime_error);
    EXPECT_THROW((void)wjr::span<int>(size), std::runtime_error);
}
#endif

TEST(span, arrays_and_fixed_extent_conversions) {
    std::array<int, 3> values = {1, 2, 3};
    wjr::span fixed(values);
    static_assert(std::same_as<decltype(fixed), wjr::span<int, 3>>);
    wjr::span<const int, 3> readonly = fixed;
    EXPECT_EQ(readonly.data(), values.data());
    wjr::span<int> dynamic = fixed;
    wjr::span<const int, 3> checked(dynamic);
    EXPECT_EQ(checked.data(), values.data());
    EXPECT_EQ(checked.size_bytes(), sizeof(values));
    EXPECT_EQ(std::to_address(readonly.cbegin()), values.data());
    EXPECT_EQ(*readonly.crbegin(), 3);
    EXPECT_EQ(*readonly.rbegin(), 3);
    EXPECT_EQ(readonly.rend() - readonly.rbegin(), 3);
}

TEST(span, slices_preserve_extent_and_mutability) {
    int values[] = {1, 2, 3, 4, 5, 6};
    wjr::span fixed(values);
    fixed.first<2>()[1] = 7;
    fixed.last<2>()[1] = 9;
    EXPECT_EQ(values[1], 7);
    EXPECT_EQ(values[5], 9);
    auto middle = fixed.subspan<1, 3>();
    EXPECT_EQ(middle.data(), values + 1);
    EXPECT_EQ(middle.size(), 3);
    EXPECT_EQ(fixed.first(2).size(), 2);
    EXPECT_EQ(fixed.last(2).data(), values + 4);
    EXPECT_EQ(fixed.subspan(2, 3).data(), values + 2);
    EXPECT_EQ(fixed.subspan(2).size(), 4);
    wjr::span<int> dynamic = fixed;
    EXPECT_EQ(dynamic.first<2>().size(), 2);
    EXPECT_EQ(dynamic.last<2>().data(), values + 4);
    EXPECT_TRUE(dynamic.subspan(6, 0).empty());
#if !defined(WJR_DISABLE_EXCEPTIONS)
    EXPECT_THROW((void)dynamic.at(6), std::out_of_range);
#endif
}

#if WJR_DEBUG_LEVEL >= 2 && GTEST_HAS_DEATH_TEST
TEST(span, invalid_subviews_are_checked) {
    int values[] = {1, 2, 3};
    wjr::span<int> dynamic(values);
    EXPECT_DEATH((void)dynamic.first<4>(), "");
    EXPECT_DEATH((void)dynamic.last<4>(), "");
    EXPECT_DEATH((void)dynamic.subspan(2, 2), "");
}
#endif
