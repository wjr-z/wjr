#include "detail.hpp"

#include <wjr/expected.hpp>

using namespace wjr;

namespace wjr {
template class expected<int, bool>;
template class expected<int, compressed_value<bool, false>>;
} // namespace wjr

TEST_CASE("expected - trivial") {
    {
        using type = expected<int, bool>;

        static_assert(std::is_trivially_destructible_v<type>, "");
        static_assert(std::is_trivially_copy_constructible_v<type>, "");
        static_assert(std::is_trivially_move_constructible_v<type>, "");
        static_assert(std::is_trivially_copy_assignable_v<type>, "");
        static_assert(std::is_trivially_move_assignable_v<type>, "");
    }

    {
        using type = expected<int, compressed_value<bool, false>>;

        static_assert(sizeof(type) == sizeof(int) * 2, "");
        static_assert(std::is_trivially_destructible_v<type>, "");
        static_assert(std::is_trivially_copy_constructible_v<type>, "");
        static_assert(std::is_trivially_move_constructible_v<type>, "");
        static_assert(std::is_trivially_copy_assignable_v<type>, "");
        static_assert(std::is_trivially_move_assignable_v<type>, "");
    }
}

TEST_CASE("expected - constructor") {
    auto test = [](auto x, auto val, auto err) {
        using type = decltype(x);
        {
            type a;
            CHECK(a.has_value());
            CHECK(*a == x);
        }
        {
            type a;
            type b(a);
            CHECK(b.has_value());
            CHECK(*b == x);
            CHECK(a == b);
        }
        {
            type a;
            type b(std::move(a));
            CHECK(b.has_value());
            CHECK(*b == x);
        }
        {
            type a(val);
            CHECK(a.has_value());
            CHECK(*a == val);
        }
        {
            auto val2(val);
            type a(std::move(val2));
            CHECK(a.has_value());
            CHECK(*a == val);
        }
        {
            type a(std::in_place, val);
            CHECK(a.has_value());
            CHECK(*a == val);
        }
        {
            auto val2(val);
            type a(std::in_place, std::move(val2));
            CHECK(a.has_value());
            CHECK(*a == val);
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            CHECK(!a.has_value());
            CHECK(a.error() == err);
            type b(a);
            CHECK(!b.has_value());
            CHECK(b.error() == err);
            CHECK(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            CHECK(!a.has_value());
            CHECK(a.error() == err);
            type b(std::move(a));
            CHECK(!b.has_value());
            CHECK(b.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, ec);
            CHECK(!a.has_value());
            CHECK(a.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            CHECK(!a.has_value());
            CHECK(a.error() == err);
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
            CHECK(a.has_value());
        }
        {
            type a;
            type b(a);
            CHECK(b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            CHECK(c.has_value());
        }
        {
            type a(std::in_place);
            CHECK(a.has_value());
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            CHECK(!a.has_value());
            CHECK(a.error() == err);
            type b(a);
            CHECK(!b.has_value());
            CHECK(b.error() == err);
            CHECK(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            CHECK(!a.has_value());
            CHECK(a.error() == err);
            type b(std::move(a));
            CHECK(!b.has_value());
            CHECK(b.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, ec);
            CHECK(!a.has_value());
            CHECK(a.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            CHECK(!a.has_value());
            CHECK(a.error() == err);
        }
    };

    test_void(expected<void, int>{}, 3);
    test_void(expected<void, std::string>{}, "wjr");
    test_void(expected<void, compressed_value<int, 3>>{}, 7);
}

TEST_CASE("expected - value_and_error_access") {
    expected<int, std::string> a(42);
    CHECK(a.has_value());
    CHECK_EQ(a.value(), 42);
    CHECK_EQ(*a, 42);
    a = wjr::unexpected(std::string("err"));
    CHECK_FALSE(a.has_value());
    CHECK_EQ(a.error(), "err");
    // value() on error should throw
    CHECK_THROWS_AS((void)a.value(), std::exception);
}

TEST_CASE("expected - swap") {
    expected<int, std::string> a(1), b(wjr::unexpected(std::string("err")));
    a.swap(b);
    CHECK_FALSE(a.has_value());
    CHECK_EQ(a.error(), "err");
    CHECK(b.has_value());
    CHECK_EQ(b.value(), 1);
}

TEST_CASE("expected - emplace") {
    expected<std::string, int> a;
    a.emplace("hello");
    CHECK(a.has_value());
    CHECK_EQ(a.value(), "hello");
    a = wjr::unexpected(7);
    a.emplace("world");
    CHECK(a.has_value());
    CHECK_EQ(a.value(), "world");
}

TEST_CASE("expected - operator_eq_ne") {
    expected<int, int> a(1), b(1), c(2), d(wjr::unexpected(3));
    CHECK(a == b);
    CHECK_FALSE(a == c);
    CHECK_FALSE(a == d);
    // Test inequality by using !(a == b) pattern if != is not available
    CHECK_FALSE(!(a == b)); // equivalent to a == b
    CHECK(!(a == c));       // equivalent to a != c
    CHECK(!(a == d));       // equivalent to a != d
}

TEST_CASE("expected - compressed_value_size") {
    using exp = expected<int, compressed_value<bool, false>>;
    using exp2 = expected<int, bool>;
    CHECK_EQ(sizeof(exp), sizeof(int) * 2);
    CHECK_EQ(sizeof(exp2), sizeof(int) * 2);
}

TEST_CASE("expected - nested_expected") {
    expected<expected<int, int>, int> a(expected<int, int>(5));
    CHECK(a.has_value());
    CHECK(a.value().has_value());
    CHECK_EQ(a.value().value(), 5);
    a = wjr::unexpected(7);
    CHECK_FALSE(a.has_value());
    CHECK_EQ(a.error(), 7);
}

TEST_CASE("expected - void_error_access") {
    expected<void, int> a;
    CHECK(a.has_value());
    a = wjr::unexpected(3);
    CHECK_FALSE(a.has_value());
    CHECK_EQ(a.error(), 3);
}

TEST_CASE("expected - value_or") {
    expected<int, int> a(1);
    expected<int, int> b(wjr::unexpected(2));
    CHECK_EQ(a.value_or(99), 1);
    CHECK_EQ(b.value_or(99), 99);
}

TEST_CASE("expected - error_or") {
    expected<int, int> a(1);
    expected<int, int> b(wjr::unexpected(2));
    CHECK_EQ(a.error_or(99), 99);
    CHECK_EQ(b.error_or(99), 2);
}

// Type parametrized tests for both void and non-void expected
template <typename T>
class ExpectedTest {};

TEMPLATE_TEST_CASE("ExpectedTest - default_constructor", "", (expected<int, std::string>),
                   (expected<std::string, int>), (expected<void, int>),
                   (expected<void, std::string>), (expected<void, compressed_value<int, -1>>)) {
    using ExpectedType = TestType;
    ExpectedType a;
    CHECK(a.has_value());
}

TEMPLATE_TEST_CASE("ExpectedTest - error_constructor", "", (expected<int, std::string>),
                   (expected<std::string, int>), (expected<void, int>),
                   (expected<void, std::string>), (expected<void, compressed_value<int, -1>>)) {
    using ExpectedType = TestType;
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
    CHECK_FALSE(a.has_value());
    CHECK_EQ(a.error(), err);
}

// Comprehensive swap tests covering all 4 cases
TEST_CASE("expected - swap_all_cases") {
    // Case 1: value ↔ value
    {
        expected<int, std::string> a(1), b(2);
        a.swap(b);
        CHECK(a.has_value());
        CHECK(b.has_value());
        CHECK_EQ(*a, 2);
        CHECK_EQ(*b, 1);
    }

    // Case 2: value ↔ error
    {
        expected<int, std::string> a(1), b(wjr::unexpected(std::string("err")));
        a.swap(b);
        CHECK_FALSE(a.has_value());
        CHECK(b.has_value());
        CHECK_EQ(a.error(), "err");
        CHECK_EQ(*b, 1);
    }

    // Case 3: error ↔ value
    {
        expected<int, std::string> a(wjr::unexpected(std::string("err"))), b(2);
        a.swap(b);
        CHECK(a.has_value());
        CHECK_FALSE(b.has_value());
        CHECK_EQ(*a, 2);
        CHECK_EQ(b.error(), "err");
    }

    // Case 4: error ↔ error
    {
        expected<int, std::string> a(wjr::unexpected(std::string("err1"))),
            b(wjr::unexpected(std::string("err2")));
        a.swap(b);
        CHECK_FALSE(a.has_value());
        CHECK_FALSE(b.has_value());
        CHECK_EQ(a.error(), "err2");
        CHECK_EQ(b.error(), "err1");
    }
}

// void expected swap tests covering all 4 cases
TEST_CASE("expected - void_swap_all_cases") {
    // Case 1: value ↔ value
    {
        expected<void, std::string> a, b;
        a.swap(b);
        CHECK(a.has_value());
        CHECK(b.has_value());
    }

    // Case 2: value ↔ error
    {
        expected<void, std::string> a, b(wjr::unexpected(std::string("err")));
        a.swap(b);
        CHECK_FALSE(a.has_value());
        CHECK(b.has_value());
        CHECK_EQ(a.error(), "err");
    }

    // Case 3: error ↔ value
    {
        expected<void, std::string> a(wjr::unexpected(std::string("err"))), b;
        a.swap(b);
        CHECK(a.has_value());
        CHECK_FALSE(b.has_value());
        CHECK_EQ(b.error(), "err");
    }

    // Case 4: error ↔ error
    {
        expected<void, std::string> a(wjr::unexpected(std::string("err1"))),
            b(wjr::unexpected(std::string("err2")));
        a.swap(b);
        CHECK_FALSE(a.has_value());
        CHECK_FALSE(b.has_value());
        CHECK_EQ(a.error(), "err2");
        CHECK_EQ(b.error(), "err1");
    }
}

TEST_CASE("expected - void_emplace_and_value") {
    expected<void, int> a(wjr::unexpected(5));
    CHECK_FALSE(a.has_value());
    a.emplace();
    CHECK(a.has_value());
    a.value(); // Should not throw for void expected with value

    a = wjr::unexpected(10);
    CHECK_THROWS_AS(a.value(), std::exception);
}

TEST_CASE("expected - assignment_operators") {
    // Test value assignment
    {
        expected<int, std::string> a;
        a = 42;
        CHECK(a.has_value());
        CHECK_EQ(*a, 42);
    }

    // Test unexpected assignment
    {
        expected<int, std::string> a(5);
        a = wjr::unexpected(std::string("error"));
        CHECK_FALSE(a.has_value());
        CHECK_EQ(a.error(), "error");
    }

    // Test void expected assignment
    {
        expected<void, std::string> a(wjr::unexpected(std::string("err")));
        expected<void, std::string> b;
        a = b; // assign from value expected
        CHECK(a.has_value());
    }
}

TEST_CASE("expected - void_equality_operators") {
    expected<void, int> a, b, c(wjr::unexpected(1)), d(wjr::unexpected(2)), e(wjr::unexpected(1));

    // value == value
    CHECK(a == b);

    // value != error
    CHECK_FALSE(a == c);

    // error != error (different values)
    CHECK_FALSE(c == d);

    // error == error (same values)
    CHECK(c == e);
}

TEST_CASE("expected - compressed_value_detailed") {
    // Test different compressed_value configurations
    {
        using exp = expected<int, compressed_value<bool, false>>;
        using exp_normal = expected<int, bool>;

        exp a(42);
        exp b(wjr::unexpected(true));

        CHECK(a.has_value());
        CHECK_EQ(*a, 42);
        CHECK_FALSE(b.has_value());
        CHECK_EQ(b.error(), true);

        // Size should be optimized
        CHECK_EQ(sizeof(exp), sizeof(exp_normal));
    }

    // Test void expected with compressed_value
    {
        using void_exp = expected<void, compressed_value<int, -1>>;

        void_exp a;
        void_exp b(wjr::unexpected(99));

        CHECK(a.has_value());
        CHECK_FALSE(b.has_value());
        CHECK_EQ(b.error(), 99);
    }
}
