#include <array>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/compressed_pair.hpp>

namespace {

using wjr::compressed_pair;

struct empty {};
struct final_empty final {};
struct alignas(64) aligned_empty {};
struct derived_empty : empty {};
struct derived_value : empty {
    int value;
};

struct value_from_int {
    int value;
    constexpr explicit value_from_int(int x) noexcept : value(x) {}
};

struct default_value {
    int value = 17;
};

struct empty_from_int {
    inline static int last_value = 0;
    explicit empty_from_int(int value) noexcept { last_value = value; }
};

static_assert(std::is_constructible_v<compressed_pair<value_from_int, default_value>, int,
                                      wjr::default_construct_t>);
static_assert(std::is_nothrow_constructible_v<compressed_pair<empty_from_int, default_value>, int,
                                              wjr::default_construct_t>);
static_assert(std::is_nothrow_constructible_v<compressed_pair<value_from_int, empty>, int,
                                              wjr::default_construct_t>);
static_assert(
    std::is_constructible_v<compressed_pair<int &, empty>, int &, wjr::default_construct_t>);
static_assert(!std::is_constructible_v<compressed_pair<std::unique_ptr<int>, empty>,
                                       std::unique_ptr<int> &, wjr::default_construct_t>);

struct assignment_error {};

struct ref_qualified_assignment {
    int value = 0;

    ref_qualified_assignment &operator=(int x) & noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (x == 7) {
            throw assignment_error{};
        }
#endif
        value = x;
        return *this;
    }

    ref_qualified_assignment &&operator=(int x) && noexcept {
        value = x;
        return std::move(*this);
    }
};

struct category_assignment {
    int value = 0;

    category_assignment &operator=(int &x) noexcept {
        value = x;
        return *this;
    }

    category_assignment &operator=(int &&x) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (x == 7) {
            throw assignment_error{};
        }
#endif
        value = x;
        return *this;
    }
};

using int_pair = compressed_pair<int, int>;
using ref_assignment_pair = compressed_pair<ref_qualified_assignment, ref_qualified_assignment>;
using category_pair = compressed_pair<category_assignment, category_assignment>;
static_assert(noexcept(std::declval<int_pair &>() = std::declval<std::pair<int, int>>()));
static_assert(
    !noexcept(std::declval<ref_assignment_pair &>() = std::declval<std::pair<int, int> &>()));
static_assert(noexcept(std::declval<category_pair &>() = std::declval<std::pair<int, int> &>()));
static_assert(!noexcept(std::declval<category_pair &>() = std::declval<std::pair<int, int>>()));

struct adl_only_swap {
    int value = 0;

    adl_only_swap() = default;
    adl_only_swap(adl_only_swap &&) = delete;
    adl_only_swap &operator=(adl_only_swap &&) = delete;

    friend void swap(adl_only_swap &lhs, adl_only_swap &rhs) noexcept {
        std::swap(lhs.value, rhs.value);
    }
};

struct tracked_swap {
    inline static int swaps = 0;
    int value = 0;

    friend void swap(tracked_swap &lhs, tracked_swap &rhs) noexcept {
        ++swaps;
        std::swap(lhs.value, rhs.value);
    }
};

struct throwing_swap {
    friend void swap(throwing_swap &, throwing_swap &) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw assignment_error{};
#endif
    }
};

struct unswappable {
    unswappable() = default;
    unswappable(unswappable &&) = delete;
    unswappable &operator=(unswappable &&) = delete;
};

using adl_pair = compressed_pair<adl_only_swap, int>;
static_assert(!std::is_move_constructible_v<adl_pair>);
static_assert(std::is_nothrow_swappable_v<adl_pair>);
static_assert(noexcept(std::declval<adl_pair &>().swap(std::declval<adl_pair &>())));
static_assert(std::is_swappable_v<compressed_pair<throwing_swap, int>>);
static_assert(!std::is_nothrow_swappable_v<compressed_pair<throwing_swap, int>>);
static_assert(!std::is_swappable_v<compressed_pair<unswappable, int>>);
static_assert(std::is_nothrow_swappable_v<compressed_pair<adl_pair, int>>);
static_assert(!std::is_constructible_v<adl_pair, compressed_pair<adl_pair, int> &>);

template <class T, class U>
constexpr bool check_get_types() {
    using pair = compressed_pair<T, U>;
    static_assert(std::is_same_v<decltype(std::declval<pair &>().template get<0>()), T &>);
    static_assert(
        std::is_same_v<decltype(std::declval<const pair &>().template get<0>()), const T &>);
    static_assert(std::is_same_v<decltype(std::declval<pair &&>().template get<0>()), T &&>);
    static_assert(
        std::is_same_v<decltype(std::declval<const pair &&>().template get<0>()), const T &&>);
    static_assert(std::is_same_v<decltype(std::declval<pair &&>().template get<1>()), U &&>);
    static_assert(
        std::is_same_v<decltype(std::declval<const pair &&>().template get<1>()), const U &&>);
    static_assert(std::is_same_v<decltype(std::get<0>(std::declval<pair &&>())), T &&>);
    static_assert(std::is_same_v<decltype(std::get<0>(std::declval<const pair &&>())), const T &&>);
    static_assert(std::is_same_v<decltype(std::get<1>(std::declval<pair &&>())), U &&>);
    static_assert(std::is_same_v<decltype(std::get<1>(std::declval<const pair &&>())), const U &&>);
    return true;
}

static_assert(check_get_types<int, long>());
static_assert(check_get_types<const int, volatile long>());
static_assert(check_get_types<int &, const long &>());

constexpr bool rvalue_access() {
    compressed_pair<int, long> pair(1, 2);
    const compressed_pair<int, long> constant(3, 4);
    return std::move(pair).get<0>() == 1 && std::move(pair).get<1>() == 2 &&
           std::move(constant).get<0>() == 3 && std::move(constant).get<1>() == 4 &&
           std::get<0>(std::move(constant)) == 3 && std::get<long>(std::move(constant)) == 4 &&
           std::move(pair)[wjr::integral_constant<size_t, 1>{}] == 2 &&
           std::move(constant)[wjr::integral_constant<size_t, 0>{}] == 3;
}
static_assert(rvalue_access());

constexpr bool tuple_like_construction() {
    compressed_pair<int, long> from_pair(std::pair<short, int>(1, 2));
    compressed_pair<int, long> from_tuple(std::tuple<short, int>(3, 4));
    compressed_pair<int, long> from_array(std::array<short, 2>{5, 6});
    auto copy = from_pair;
    auto moved = std::move(copy);
    wjr::tuple<int, long> tuple(std::tuple<short, int>(7, 8));
    auto tuple_copy = tuple;
    auto tuple_moved = std::move(tuple_copy);
    return moved.first() == 1 && moved.second() == 2 && from_tuple.first() == 3 &&
           from_tuple.second() == 4 && from_array.first() == 5 && from_array.second() == 6 &&
           std::get<0>(tuple_moved) == 7 && std::get<1>(tuple_moved) == 8;
}
static_assert(tuple_like_construction());

static_assert(sizeof(compressed_pair<empty, int>) == sizeof(int));
static_assert(sizeof(compressed_pair<int, empty>) == sizeof(int));
static_assert(sizeof(compressed_pair<final_empty, int>) == 2 * sizeof(int));
static_assert(alignof(compressed_pair<aligned_empty, int>) == alignof(aligned_empty));
static_assert(sizeof(compressed_pair<aligned_empty, int>) == sizeof(aligned_empty));
static_assert(std::is_trivially_copyable_v<int_pair>);
static_assert(!std::is_copy_constructible_v<compressed_pair<std::unique_ptr<int>, empty>>);
static_assert(std::is_nothrow_move_constructible_v<compressed_pair<std::unique_ptr<int>, empty>>);
static_assert(!std::is_copy_assignable_v<compressed_pair<const empty, int>>);
static_assert(sizeof(compressed_pair<empty, empty>) >= 2 * sizeof(empty));
static_assert(sizeof(compressed_pair<aligned_empty, aligned_empty>) >= 2 * sizeof(aligned_empty));

} // namespace

TEST(compressed_pair, value_and_default_initialization) {
    compressed_pair<value_from_int, default_value> ordinary(3, wjr::default_construct);
    EXPECT_EQ(ordinary.first().value, 3);
    EXPECT_EQ(ordinary.second().value, 17);

    compressed_pair<empty_from_int, default_value> first_compressed(5, wjr::default_construct);
    EXPECT_EQ(empty_from_int::last_value, 5);
    EXPECT_EQ(first_compressed.second().value, 17);

    compressed_pair<value_from_int, empty> second_compressed(7, wjr::default_construct);
    EXPECT_EQ(second_compressed.first().value, 7);

    auto pointer = std::make_unique<int>(11);
    compressed_pair<std::unique_ptr<int>, empty> move_only(std::move(pointer),
                                                           wjr::default_construct);
    EXPECT_EQ(pointer, nullptr);
    EXPECT_EQ(*move_only.first(), 11);

    int value = 13;
    compressed_pair<int &, empty> reference(value, wjr::default_construct);
    reference.first() = 19;
    EXPECT_EQ(value, 19);
}

TEST(compressed_pair, assignment_forwards_value_category) {
    category_pair pair;
    std::pair<int, int> source(3, 5);
    pair = source;
    EXPECT_EQ(pair.first().value, 3);
    EXPECT_EQ(pair.second().value, 5);
    pair = std::pair<int, int>(11, 13);
    EXPECT_EQ(pair.first().value, 11);
    EXPECT_EQ(pair.second().value, 13);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(compressed_pair, assignment_exceptions_propagate) {
    std::pair<int, int> source(7, 7);
    ref_assignment_pair ref_qualified;
    EXPECT_THROW((ref_qualified = source), assignment_error);
    category_pair category;
    EXPECT_THROW((category = std::move(source)), assignment_error);
}

TEST(compressed_pair, swap_exceptions_propagate) {
    compressed_pair<throwing_swap, int> lhs;
    compressed_pair<throwing_swap, int> rhs;
    EXPECT_THROW(std::swap(lhs, rhs), assignment_error);
    EXPECT_THROW(std::ranges::swap(lhs, rhs), assignment_error);
}
#endif

TEST(compressed_pair, swap_uses_element_customization) {
    adl_pair lhs;
    adl_pair rhs;
    lhs.first().value = 1;
    lhs.second() = 2;
    rhs.first().value = 3;
    rhs.second() = 4;
    std::swap(lhs, rhs);
    EXPECT_EQ(lhs.first().value, 3);
    EXPECT_EQ(lhs.second(), 4);
    EXPECT_EQ(rhs.first().value, 1);
    EXPECT_EQ(rhs.second(), 2);

    using std::swap;
    swap(lhs, rhs);
    EXPECT_EQ(lhs.first().value, 1);
    EXPECT_EQ(lhs.second(), 2);
    std::ranges::swap(lhs, rhs);
    EXPECT_EQ(lhs.first().value, 3);
    EXPECT_EQ(lhs.second(), 4);

    compressed_pair<adl_pair, int> outer_lhs;
    compressed_pair<adl_pair, int> outer_rhs;
    outer_lhs.first().first().value = 7;
    outer_rhs.first().first().value = 11;
    swap(outer_lhs, outer_rhs);
    EXPECT_EQ(outer_lhs.first().first().value, 11);
    EXPECT_EQ(outer_rhs.first().first().value, 7);

    compressed_pair<tracked_swap, tracked_swap> first;
    compressed_pair<tracked_swap, tracked_swap> second;
    tracked_swap::swaps = 0;
    first.swap(second);
    EXPECT_EQ(tracked_swap::swaps, 2);
}

TEST(compressed_pair, rvalue_reference_elements_remain_references) {
    int first = 1;
    long second = 2;
    compressed_pair<int &, long &> pair(first, second);
    std::move(pair).get<0>() = 3;
    std::get<1>(std::move(std::as_const(pair))) = 4;
    EXPECT_EQ(first, 3);
    EXPECT_EQ(second, 4);
}

TEST(compressed_pair, identifiable_empty_subobjects_have_distinct_addresses) {
    compressed_pair<empty, empty> repeated;
    EXPECT_NE(std::addressof(repeated.first()), std::addressof(repeated.second()));
    EXPECT_EQ((&wjr::container_of<decltype(repeated), std::in_place_index_t<0>>(repeated.first())),
              &repeated);
    EXPECT_EQ((&wjr::container_of<decltype(repeated), std::in_place_index_t<1>>(repeated.second())),
              &repeated);

    compressed_pair<const empty, volatile empty> qualified;
    EXPECT_NE(static_cast<const volatile void *>(std::addressof(qualified.first())),
              static_cast<const volatile void *>(std::addressof(qualified.second())));

    compressed_pair<derived_empty, empty> derived_first;
    EXPECT_NE(static_cast<empty *>(std::addressof(derived_first.first())),
              std::addressof(derived_first.second()));
    compressed_pair<empty, derived_empty> derived_second;
    EXPECT_NE(std::addressof(derived_second.first()),
              static_cast<empty *>(std::addressof(derived_second.second())));

    compressed_pair<derived_value, empty> value_first;
    EXPECT_NE(static_cast<empty *>(std::addressof(value_first.first())),
              std::addressof(value_first.second()));
    compressed_pair<empty, derived_value> value_second;
    EXPECT_NE(std::addressof(value_second.first()),
              static_cast<empty *>(std::addressof(value_second.second())));

    compressed_pair<aligned_empty, aligned_empty> aligned;
    EXPECT_NE(std::addressof(aligned.first()), std::addressof(aligned.second()));
    EXPECT_EQ(alignof(decltype(aligned)), alignof(aligned_empty));
}
