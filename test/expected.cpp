#include "detail.hpp"

#include <initializer_list>
#include <span>
#include <string>
#include <vector>

#include <wjr/expected.hpp>

using namespace wjr;

namespace wjr {
template class expected<int, bool>;
template class expected<int, compressed_value<bool, false>>;
} // namespace wjr

namespace {

struct non_default_value {
    int value;
    non_default_value() = delete;
    constexpr explicit non_default_value(int v) : value(v) {}
    constexpr non_default_value(std::initializer_list<int> values, int initial) : value(initial) {
        for (int v : values) {
            value += v;
        }
    }
};

struct non_default_move_only {
    int value;
    non_default_move_only() = delete;
    constexpr explicit non_default_move_only(int v) : value(v) {}
    non_default_move_only(const non_default_move_only &) = delete;
    non_default_move_only(non_default_move_only &&) = default;
    non_default_move_only &operator=(const non_default_move_only &) = delete;
    non_default_move_only &operator=(non_default_move_only &&) = default;
};

using non_default_result = expected<non_default_value, int>;
static_assert(!std::is_default_constructible_v<non_default_result>);
static_assert(std::is_trivially_copy_constructible_v<non_default_result>);
static_assert(std::is_trivially_move_constructible_v<non_default_result>);
static_assert(std::is_trivially_copy_assignable_v<non_default_result>);
static_assert(std::is_trivially_move_assignable_v<non_default_result>);
static_assert(std::is_trivially_destructible_v<non_default_result>);
static_assert(sizeof(non_default_result) == sizeof(expected<int, int>));
static_assert(alignof(non_default_result) == alignof(expected<int, int>));
using move_only_result = expected<non_default_move_only, int>;
static_assert(!std::is_default_constructible_v<move_only_result>);
static_assert(!std::is_copy_constructible_v<move_only_result>);
static_assert(!std::is_copy_assignable_v<move_only_result>);
static_assert(std::is_move_constructible_v<move_only_result>);
static_assert(std::is_move_assignable_v<move_only_result>);

constexpr bool non_default_construction() {
    non_default_result value(non_default_value(7));
    non_default_result placed(std::in_place, 8);
    non_default_result listed(std::in_place, {1, 2}, 3);
    non_default_result error(unexpect, 9);
    const wjr::unexpected<int> failure(10);
    non_default_result copied_error(failure);
    non_default_result moved_error(wjr::unexpected<int>(11));
    return value->value == 7 && placed->value == 8 && listed->value == 6 && !error &&
           error.error() == 9 && !copied_error && copied_error.error() == 10 && !moved_error &&
           moved_error.error() == 11;
}
static_assert(non_default_construction());

constexpr bool static_view_construction() {
    int values[4] = {1, 2, 3, 4};
    using view = std::span<int, 4>;
    static_assert(!std::is_default_constructible_v<expected<view, int>>);
    expected<view, int> result(std::in_place, values);
    expected<view, int> error(unexpect, 5);
    return (*result)[3] == 4 && !error && error.error() == 5;
}
static_assert(static_view_construction());

} // namespace

TEST(expected, non_default_value_construction) {
    EXPECT_TRUE(non_default_construction());
    EXPECT_TRUE(static_view_construction());
    move_only_result moved(non_default_move_only(12));
    move_only_result destination(std::move(moved));
    EXPECT_EQ(destination->value, 12);
    expected<non_default_value, non_default_value> value(std::in_place, 3);
    expected<non_default_value, non_default_value> error(unexpect, {1, 2}, 4);
    EXPECT_EQ(value->value, 3);
    ASSERT_FALSE(error);
    EXPECT_EQ(error.error().value, 7);
}

TEST(expected, non_default_converting_construction) {
    const expected<int, short> value(7);
    const expected<int, short> error(unexpect, 9);
    non_default_result copied_value(value);
    non_default_result copied_error(error);
    non_default_result moved_value(expected<int, short>(8));
    non_default_result moved_error(expected<int, short>(unexpect, 10));
    EXPECT_EQ(copied_value->value, 7);
    EXPECT_EQ(moved_value->value, 8);
    ASSERT_FALSE(copied_error);
    ASSERT_FALSE(moved_error);
    EXPECT_EQ(copied_error.error(), 9);
    EXPECT_EQ(moved_error.error(), 10);
}

TEST(expected, non_default_compressed_and_void_errors) {
    using compressed_result = expected<non_default_value, compressed_value<int, 0>>;
    static_assert(!std::is_default_constructible_v<compressed_result>);
    compressed_result value(std::in_place, 7);
    compressed_result error(unexpect, 8);
    compressed_result unexpected_error(wjr::unexpected<int>(9));
    EXPECT_EQ(value->value, 7);
    ASSERT_FALSE(error);
    ASSERT_FALSE(unexpected_error);
    EXPECT_EQ(error.error(), 8);
    EXPECT_EQ(unexpected_error.error(), 9);

    expected<void, non_default_value> empty;
    expected<void, non_default_value> placed(std::in_place);
    expected<void, non_default_value> void_error(unexpect, {1, 2}, 4);
    expected<void, non_default_value> from_error(
        wjr::unexpected<non_default_value>(std::in_place, 11));
    EXPECT_TRUE(empty);
    EXPECT_TRUE(placed);
    ASSERT_FALSE(void_error);
    ASSERT_FALSE(from_error);
    EXPECT_EQ(void_error.error().value, 7);
    EXPECT_EQ(from_error.error().value, 11);
}

TEST(expected, trivial) {
    {
        using type = expected<int, bool>;

        static_assert(std::is_trivially_destructible_v<type>);
        static_assert(std::is_trivially_copy_constructible_v<type>);
        static_assert(std::is_trivially_move_constructible_v<type>);
        static_assert(std::is_trivially_copy_assignable_v<type>);
        static_assert(std::is_trivially_move_assignable_v<type>);
    }

    {
        using type = expected<int, compressed_value<bool, false>>;

        static_assert(sizeof(type) == sizeof(int) * 2);
        static_assert(std::is_trivially_destructible_v<type>);
        static_assert(std::is_trivially_copy_constructible_v<type>);
        static_assert(std::is_trivially_move_constructible_v<type>);
        static_assert(std::is_trivially_copy_assignable_v<type>);
        static_assert(std::is_trivially_move_assignable_v<type>);
    }
}

TEST(expected, constructor) {
    auto test = [](auto x, auto val, auto err) {
        using type = decltype(x);
        {
            type a;
            EXPECT_TRUE(a.has_value());
            EXPECT_TRUE(*a == x);
        }
        {
            type a;
            type b(a);
            EXPECT_TRUE(b.has_value());
            EXPECT_TRUE(*b == x);
            EXPECT_TRUE(a == b);
        }
        {
            type a;
            type b(std::move(a));
            EXPECT_TRUE(b.has_value());
            EXPECT_TRUE(*b == x);
        }
        {
            type a(val);
            EXPECT_TRUE(a.has_value());
            EXPECT_TRUE(*a == val);
        }
        {
            auto val2(val);
            type a(std::move(val2));
            EXPECT_TRUE(a.has_value());
            EXPECT_TRUE(*a == val);
        }
        {
            type a(std::in_place, val);
            EXPECT_TRUE(a.has_value());
            EXPECT_TRUE(*a == val);
        }
        {
            auto val2(val);
            type a(std::in_place, std::move(val2));
            EXPECT_TRUE(a.has_value());
            EXPECT_TRUE(*a == val);
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
            type b(a);
            EXPECT_TRUE(!b.has_value());
            EXPECT_TRUE(b.error() == err);
            EXPECT_TRUE(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
            type b(std::move(a));
            EXPECT_TRUE(!b.has_value());
            EXPECT_TRUE(b.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, ec);
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
        }
    };

    test(expected<int, bool>{}, 3, false);
    test(expected<int, int>{}, 3, 7);
    test(expected<int, compressed_value<int, 0>>{}, 3, 7);
    test(expected<std::string, int>{}, "wjr", 3);
    test(expected<std::string, compressed_value<int, 0>>{}, "wjr", 3);
    test(expected<std::string, std::string>{}, "wjr", "hello");
    test(expected<int, std::string>{}, 3, "wjr");

    auto test_void = [](auto x, auto err) {
        using type = decltype(x);
        {
            type a;
            EXPECT_TRUE(a.has_value());
        }
        {
            type a;
            type b(a);
            EXPECT_TRUE(b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            EXPECT_TRUE(c.has_value());
        }
        {
            type a(std::in_place);
            EXPECT_TRUE(a.has_value());
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
            type b(a);
            EXPECT_TRUE(!b.has_value());
            EXPECT_TRUE(b.error() == err);
            EXPECT_TRUE(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
            type b(std::move(a));
            EXPECT_TRUE(!b.has_value());
            EXPECT_TRUE(b.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, ec);
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            EXPECT_TRUE(!a.has_value());
            EXPECT_TRUE(a.error() == err);
        }
    };

    test_void(expected<void, int>{}, 3);
    test_void(expected<void, std::string>{}, "wjr");
    test_void(expected<void, compressed_value<int, 3>>{}, 7);
}

TEST(expected, value_and_error_access) {
    expected<int, std::string> a(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), 42);
    EXPECT_EQ(*a, 42);
    a = wjr::unexpected(std::string("err"));
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.error(), "err");
    // value() on error should throw
    EXPECT_THROW((void)a.value(), std::exception);
}

TEST(expected, swap) {
    expected<int, std::string> a(1), b(wjr::unexpected(std::string("err")));
    a.swap(b);
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.error(), "err");
    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(b.value(), 1);
}

TEST(expected, emplace) {
    expected<std::string, int> a;
    a.emplace("hello");
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), "hello");
    a = wjr::unexpected(7);
    a.emplace("world");
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), "world");
}

TEST(expected, operator_eq_ne) {
    expected<int, int> a(1), b(1), c(2), d(wjr::unexpected(3));
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
    // Test inequality by using !(a == b) pattern if != is not available
    EXPECT_FALSE(!(a == b)); // equivalent to a == b
    EXPECT_TRUE(!(a == c));  // equivalent to a != c
    EXPECT_TRUE(!(a == d));  // equivalent to a != d
}

TEST(expected, compressed_value_size) {
    using exp = expected<int, compressed_value<bool, false>>;
    using exp2 = expected<int, bool>;
    EXPECT_EQ(sizeof(exp), sizeof(int) * 2);
    EXPECT_EQ(sizeof(exp2), sizeof(int) * 2);
}

TEST(expected, nested_expected) {
    expected<expected<int, int>, int> a(expected<int, int>(5));
    EXPECT_TRUE(a.has_value());
    EXPECT_TRUE(a.value().has_value());
    EXPECT_EQ(a.value().value(), 5);
    a = wjr::unexpected(7);
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.error(), 7);
}

TEST(expected, void_error_access) {
    expected<void, int> a;
    EXPECT_TRUE(a.has_value());
    a = wjr::unexpected(3);
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.error(), 3);
}

TEST(expected, value_or) {
    expected<int, int> a(1);
    expected<int, int> b(wjr::unexpected(2));
    EXPECT_EQ(a.value_or(99), 1);
    EXPECT_EQ(b.value_or(99), 99);
}

TEST(expected, error_or) {
    expected<int, int> a(1);
    expected<int, int> b(wjr::unexpected(2));
    EXPECT_EQ(a.error_or(99), 99);
    EXPECT_EQ(b.error_or(99), 2);
}

TEST(expected, observers_and_transformations) {
    expected<std::string, int> value("wjr");
    EXPECT_TRUE(static_cast<bool>(value));
    EXPECT_EQ(value->size(), 3U);

    auto length =
        value.and_then([](const std::string &text) { return expected<size_t, int>(text.size()); });
    EXPECT_TRUE(length.has_value());
    EXPECT_EQ(length.value(), 3U);

    auto transformed = value.transform([](const std::string &text) { return text.size() + 1; });
    EXPECT_TRUE(transformed.has_value());
    EXPECT_EQ(transformed.value(), 4U);

    expected<std::string, int> error(unexpect, 7);
    EXPECT_FALSE(static_cast<bool>(error));

    auto propagated = error.and_then([](const std::string &) { return expected<size_t, int>(0); });
    EXPECT_FALSE(propagated.has_value());
    EXPECT_EQ(propagated.error(), 7);

    auto transformed_error = error.transform_error(
        [](int value) { return std::string("error ") + std::to_string(value); });
    EXPECT_FALSE(transformed_error.has_value());
    EXPECT_EQ(transformed_error.error(), "error 7");

    auto recovered = error.or_else(
        [](int value) { return expected<std::string, int>(std::in_place, std::to_string(value)); });
    EXPECT_TRUE(recovered.has_value());
    EXPECT_EQ(recovered.value(), "7");
}

// Type parametrized tests for both void and non-void expected
template <typename T>
class ExpectedTest : public ::testing::Test {};

using ExpectedTypes =
    ::testing::Types<expected<int, std::string>, expected<std::string, int>, expected<void, int>,
                     expected<void, std::string>, expected<void, compressed_value<int, -1>>>;
TYPED_TEST_SUITE(ExpectedTest, ExpectedTypes);

TYPED_TEST(ExpectedTest, default_constructor) {
    using ExpectedType = TypeParam;
    ExpectedType a;
    EXPECT_TRUE(a.has_value());
}

TYPED_TEST(ExpectedTest, error_constructor) {
    using ExpectedType = TypeParam;
    using ErrorType = typename ExpectedType::error_type;

    ErrorType err{};
    if constexpr (std::is_same_v<ErrorType, int>) {
        err = 42;
    } else if constexpr (std::is_same_v<ErrorType, std::string>) {
        err = "error";
    } else if constexpr (std::is_same_v<ErrorType, compressed_value<int, -1>>) {
        err = 99;
    }

    ExpectedType a = wjr::unexpected(err);
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.error(), err);
}

// Comprehensive swap tests covering all 4 cases
TEST(expected, swap_all_cases) {
    // Case 1: value ↔ value
    {
        expected<int, std::string> a(1), b(2);
        a.swap(b);
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(b.has_value());
        EXPECT_EQ(*a, 2);
        EXPECT_EQ(*b, 1);
    }

    // Case 2: value ↔ error
    {
        expected<int, std::string> a(1), b(wjr::unexpected(std::string("err")));
        a.swap(b);
        EXPECT_FALSE(a.has_value());
        EXPECT_TRUE(b.has_value());
        EXPECT_EQ(a.error(), "err");
        EXPECT_EQ(*b, 1);
    }

    // Case 3: error ↔ value
    {
        expected<int, std::string> a(wjr::unexpected(std::string("err"))), b(2);
        a.swap(b);
        EXPECT_TRUE(a.has_value());
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(*a, 2);
        EXPECT_EQ(b.error(), "err");
    }

    // Case 4: error ↔ error
    {
        expected<int, std::string> a(wjr::unexpected(std::string("err1"))),
            b(wjr::unexpected(std::string("err2")));
        a.swap(b);
        EXPECT_FALSE(a.has_value());
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(a.error(), "err2");
        EXPECT_EQ(b.error(), "err1");
    }
}

// void expected swap tests covering all 4 cases
TEST(expected, void_swap_all_cases) {
    // Case 1: value ↔ value
    {
        expected<void, std::string> a, b;
        a.swap(b);
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(b.has_value());
    }

    // Case 2: value ↔ error
    {
        expected<void, std::string> a, b(wjr::unexpected(std::string("err")));
        a.swap(b);
        EXPECT_FALSE(a.has_value());
        EXPECT_TRUE(b.has_value());
        EXPECT_EQ(a.error(), "err");
    }

    // Case 3: error ↔ value
    {
        expected<void, std::string> a(wjr::unexpected(std::string("err"))), b;
        a.swap(b);
        EXPECT_TRUE(a.has_value());
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(b.error(), "err");
    }

    // Case 4: error ↔ error
    {
        expected<void, std::string> a(wjr::unexpected(std::string("err1"))),
            b(wjr::unexpected(std::string("err2")));
        a.swap(b);
        EXPECT_FALSE(a.has_value());
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(a.error(), "err2");
        EXPECT_EQ(b.error(), "err1");
    }
}

TEST(expected, void_emplace_and_value) {
    expected<void, int> a(wjr::unexpected(5));
    EXPECT_FALSE(a.has_value());
    a.emplace();
    EXPECT_TRUE(a.has_value());
    a.value(); // Should not throw for void expected with value

    a = wjr::unexpected(10);
    EXPECT_THROW(a.value(), std::exception);
}

TEST(expected, assignment_operators) {
    // Test value assignment
    {
        expected<int, std::string> a;
        a = 42;
        EXPECT_TRUE(a.has_value());
        EXPECT_EQ(*a, 42);
    }

    // Test unexpected assignment
    {
        expected<int, std::string> a(5);
        a = wjr::unexpected(std::string("error"));
        EXPECT_FALSE(a.has_value());
        EXPECT_EQ(a.error(), "error");
    }

    // Test void expected assignment
    {
        expected<void, std::string> a(wjr::unexpected(std::string("err")));
        expected<void, std::string> b;
        a = b; // assign from value expected
        EXPECT_TRUE(a.has_value());
    }
}

TEST(expected, void_equality_operators) {
    expected<void, int> a, b, c(wjr::unexpected(1)), d(wjr::unexpected(2)), e(wjr::unexpected(1));

    // value == value
    EXPECT_TRUE(a == b);

    // value != error
    EXPECT_FALSE(a == c);

    // error != error (different values)
    EXPECT_FALSE(c == d);

    // error == error (same values)
    EXPECT_TRUE(c == e);
}

TEST(expected, compressed_value_detailed) {
    // Test different compressed_value configurations
    {
        using exp = expected<int, compressed_value<bool, false>>;
        using exp_normal = expected<int, bool>;

        exp a(42);
        exp b(wjr::unexpected(true));

        EXPECT_TRUE(a.has_value());
        EXPECT_EQ(*a, 42);
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(b.error(), true);

        // Size should be optimized
        EXPECT_EQ(sizeof(exp), sizeof(exp_normal));
    }

    // Test void expected with compressed_value
    {
        using void_exp = expected<void, compressed_value<int, -1>>;

        void_exp a;
        void_exp b(wjr::unexpected(99));

        EXPECT_TRUE(a.has_value());
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(b.error(), 99);
    }
}

TEST(expected, copy_and_move_assignment) {
    {
        expected<int, std::string> source(7);
        expected<int, std::string> target(1);
        target = source;
        EXPECT_TRUE(target.has_value());
        EXPECT_EQ(target.value(), 7);

        target = std::move(source);
        EXPECT_TRUE(target.has_value());
        EXPECT_EQ(target.value(), 7);
    }

    {
        expected<int, std::string> source(unexpect, "source");
        expected<int, std::string> target(unexpect, "target");
        target = source;
        EXPECT_FALSE(target.has_value());
        EXPECT_EQ(target.error(), "source");

        target = std::move(source);
        EXPECT_FALSE(target.has_value());
        EXPECT_EQ(target.error(), "source");
    }

    {
        expected<int, std::string> source(unexpect, "error");
        expected<int, std::string> target(1);
        target = source;
        EXPECT_FALSE(target.has_value());
        EXPECT_EQ(target.error(), "error");
    }

    {
        expected<int, std::string> source(2);
        expected<int, std::string> target(unexpect, "error");
        target = source;
        EXPECT_TRUE(target.has_value());
        EXPECT_EQ(target.value(), 2);

        target = std::move(source);
        EXPECT_TRUE(target.has_value());
        EXPECT_EQ(target.value(), 2);
    }

    {
        expected<void, std::string> value_source;
        expected<void, std::string> value_target;
        expected<void, std::string> error_source(unexpect, "error");
        expected<void, std::string> error_target(unexpect, "old");

        error_target = value_source;
        EXPECT_TRUE(error_target.has_value());
        value_target = error_source;
        EXPECT_FALSE(value_target.has_value());
        EXPECT_EQ(value_target.error(), "error");

        error_target = error_source;
        EXPECT_FALSE(error_target.has_value());
        EXPECT_EQ(error_target.error(), "error");
        value_target = std::move(value_source);
        EXPECT_TRUE(value_target.has_value());
    }
}

TEST(expected, cross_type_construction) {
    expected<short, const char *> value_source(12);
    expected<int, std::string> value_copy(value_source);
    EXPECT_TRUE(value_copy.has_value());
    EXPECT_EQ(value_copy.value(), 12);

    expected<int, std::string> value_move(expected<short, const char *>(13));
    EXPECT_TRUE(value_move.has_value());
    EXPECT_EQ(value_move.value(), 13);

    expected<short, const char *> error_source(unexpect, "failure");
    expected<int, std::string> error_copy(error_source);
    EXPECT_FALSE(error_copy.has_value());
    EXPECT_EQ(error_copy.error(), "failure");

    expected<int, std::string> error_move(expected<short, const char *>(unexpect, "moved failure"));
    EXPECT_FALSE(error_move.has_value());
    EXPECT_EQ(error_move.error(), "moved failure");

    expected<void, const char *> void_error_source(unexpect, "void failure");
    expected<void, std::string> void_error_copy(void_error_source);
    EXPECT_FALSE(void_error_copy.has_value());
    EXPECT_EQ(void_error_copy.error(), "void failure");

    expected<void, std::string> void_value_copy((expected<void, const char *>(std::in_place)));
    EXPECT_TRUE(void_value_copy.has_value());
}

TEST(expected, access_qualifiers_and_bad_access) {
    const expected<std::string, int> value("value");
    EXPECT_EQ(value.value(), "value");
    EXPECT_EQ((static_cast<const expected<std::string, int> &&>(value).value()), "value");

    const expected<std::string, int> error(unexpect, 17);
    expected<std::string, int> movable_error(unexpect, 19);
    EXPECT_THROW(error.value(), bad_expected_access<int>);
    EXPECT_THROW(std::move(movable_error).value(), bad_expected_access<int>);

    bad_expected_access<int> exception(17);
    EXPECT_EQ(exception.error(), 17);
    EXPECT_EQ(std::move(exception).error(), 17);
    EXPECT_STREQ(exception.what(), "Bad expected access");
}

TEST(expected, initializer_list_and_unexpected) {
    expected<std::vector<int>, int> value(std::in_place, std::initializer_list<int>{1, 2, 3});
    EXPECT_EQ(value.value(), (std::vector<int>{1, 2, 3}));

    value.emplace(std::initializer_list<int>{4, 5});
    EXPECT_EQ(value.value(), (std::vector<int>{4, 5}));

    wjr::unexpected<std::string> first(std::in_place, {'e', 'r', 'r'});
    wjr::unexpected<std::string> second("other");
    EXPECT_EQ(first.error(), "err");
    EXPECT_TRUE(first != second);
    first.swap(second);
    EXPECT_EQ(first.error(), "other");
    EXPECT_EQ(second.error(), "err");
    EXPECT_EQ(std::move(second).error(), "err");
}

TEST(expected, monadic_value_categories) {
    expected<int, std::string> value(3);
    const expected<int, std::string> const_value(4);
    expected<int, std::string> error(unexpect, "error");
    const expected<int, std::string> const_error(unexpect, "const error");

    auto check_value = [](auto &&item) { return expected<int, std::string>(item + 1); };
    EXPECT_EQ(value.and_then(check_value).value(), 4);
    EXPECT_EQ(const_value.and_then(check_value).value(), 5);
    EXPECT_FALSE(std::move(error).and_then(check_value).has_value());
    EXPECT_EQ((static_cast<const expected<int, std::string> &&>(const_error)
                   .and_then(check_value)
                   .error()),
              "const error");

    EXPECT_EQ(value.or_else([](auto &&) { return expected<int, std::string>(9); }).value(), 3);
    EXPECT_EQ(const_value.or_else([](auto &&) { return expected<int, std::string>(9); }).value(),
              4);
    EXPECT_EQ(
        std::move(error).or_else([](auto &&) { return expected<int, std::string>(9); }).value(), 9);
    EXPECT_EQ((static_cast<const expected<int, std::string> &&>(const_error)
                   .or_else([](auto &&) { return expected<int, std::string>(10); })
                   .value()),
              10);

    EXPECT_EQ(value.transform([](int item) { return item * 2; }).value(), 6);
    EXPECT_EQ(const_value.transform([](int item) { return item * 2; }).value(), 8);
    EXPECT_EQ(std::move(value).transform([](int item) { return item * 2; }).value(), 6);
    EXPECT_EQ((static_cast<const expected<int, std::string> &&>(const_value)
                   .transform([](int item) { return item * 2; })
                   .value()),
              8);

    auto transformed_value = expected<int, std::string>(3).transform([](int) {});
    EXPECT_TRUE(transformed_value.has_value());
    EXPECT_FALSE(error.transform([](int) { return 0; }).has_value());

    EXPECT_EQ((expected<int, int>(3)
                   .transform_error([](int item) { return std::to_string(item); })
                   .value()),
              3);
    EXPECT_EQ((expected<int, int>(unexpect, 3)
                   .transform_error([](int item) { return std::to_string(item); })
                   .error()),
              "3");
    EXPECT_EQ((expected<int, int>(unexpect, 4)
                   .transform_error([](int item) { return std::to_string(item); })
                   .error()),
              "4");
}

TEST(expected, void_monadic_operations) {
    expected<void, int> value;
    const expected<void, int> const_value;
    expected<void, int> error(unexpect, 3);
    const expected<void, int> const_error(unexpect, 4);

    EXPECT_EQ(value.and_then([] { return expected<int, int>(7); }).value(), 7);
    EXPECT_EQ(const_value.and_then([] { return expected<int, int>(8); }).value(), 8);
    EXPECT_EQ((static_cast<expected<void, int> &&>(value)
                   .and_then([] { return expected<int, int>(9); })
                   .value()),
              9);
    EXPECT_EQ((static_cast<const expected<void, int> &&>(const_value)
                   .and_then([] { return expected<int, int>(10); })
                   .value()),
              10);
    EXPECT_EQ(error.and_then([] { return expected<int, int>(0); }).error(), 3);
    EXPECT_EQ((static_cast<const expected<void, int> &&>(const_error)
                   .and_then([] { return expected<int, int>(0); })
                   .error()),
              4);

    EXPECT_TRUE(
        (expected<void, int>().or_else([](int) { return expected<void, int>(); }).has_value()));
    EXPECT_TRUE((expected<void, int>(unexpect, 5)
                     .or_else([](int value) { return expected<void, int>(unexpect, value + 1); })
                     .error() == 6));

    EXPECT_TRUE((expected<void, int>().transform([] { return 1; }).has_value()));
    EXPECT_EQ((expected<void, int>(unexpect, 7).transform([] { return 1; }).error()), 7);
    EXPECT_TRUE((expected<void, int>().transform([] {}).has_value()));

    EXPECT_TRUE((expected<void, int>()
                     .transform_error([](int value) { return std::to_string(value); })
                     .has_value()));
    EXPECT_EQ((expected<void, int>(unexpect, 8)
                   .transform_error([](int value) { return std::to_string(value); })
                   .error()),
              "8");
}

TEST(expected, macros) {
    auto try_value = [](bool success) -> expected<int, int> {
        WJR_EXPECTED_TRY(success ? expected<void, int>() : expected<void, int>(unexpect, 3));
        return 42;
    };
    EXPECT_EQ(try_value(true).value(), 42);
    EXPECT_EQ(try_value(false).error(), 3);

    auto init_value = [](bool success) -> expected<int, int> {
        WJR_EXPECTED_INIT(value,
                          success ? expected<int, int>(21) : expected<int, int>(unexpect, 4));
        return *value * 2;
    };
    EXPECT_EQ(init_value(true).value(), 42);
    EXPECT_EQ(init_value(false).error(), 4);

    auto set_value = [](bool success) -> expected<int, int> {
        int result = 0;
        WJR_EXPECTED_SET(result,
                         success ? expected<int, int>(21) : expected<int, int>(unexpect, 5));
        return result * 2;
    };
    EXPECT_EQ(set_value(true).value(), 42);
    EXPECT_EQ(set_value(false).error(), 5);
}
