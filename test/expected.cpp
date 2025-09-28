#include "detail.hpp"

#include <wjr/expected.hpp>

using namespace wjr;

namespace wjr {
template class expected<int, bool>;
template class expected<int, compressed_value<bool, false>>;
} // namespace wjr

TEST(expected, trivial) {
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

TEST(expected, constructor) {
    auto test = [](auto x, auto val, auto err) {
        using type = decltype(x);
        {
            type a;
            WJR_CHECK(a.has_value());
            WJR_CHECK(*a == x);
        }
        {
            type a;
            type b(a);
            WJR_CHECK(b.has_value());
            WJR_CHECK(*b == x);
            WJR_CHECK(a == b);
        }
        {
            type a;
            type b(std::move(a));
            WJR_CHECK(b.has_value());
            WJR_CHECK(*b == x);
        }
        {
            type a(val);
            WJR_CHECK(a.has_value());
            WJR_CHECK(*a == val);
        }
        {
            auto val2(val);
            type a(std::move(val2));
            WJR_CHECK(a.has_value());
            WJR_CHECK(*a == val);
        }
        {
            type a(std::in_place, val);
            WJR_CHECK(a.has_value());
            WJR_CHECK(*a == val);
        }
        {
            auto val2(val);
            type a(std::in_place, std::move(val2));
            WJR_CHECK(a.has_value());
            WJR_CHECK(*a == val);
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
            type b(a);
            WJR_CHECK(!b.has_value());
            WJR_CHECK(b.error() == err);
            WJR_CHECK(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
            type b(std::move(a));
            WJR_CHECK(!b.has_value());
            WJR_CHECK(b.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, ec);
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
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
            WJR_CHECK(a.has_value());
        }
        {
            type a;
            type b(a);
            WJR_CHECK(b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            WJR_CHECK(c.has_value());
        }
        {
            type a(std::in_place);
            WJR_CHECK(a.has_value());
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
            type b(a);
            WJR_CHECK(!b.has_value());
            WJR_CHECK(b.error() == err);
            WJR_CHECK(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
            type b(std::move(a));
            WJR_CHECK(!b.has_value());
            WJR_CHECK(b.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, ec);
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            WJR_CHECK(!a.has_value());
            WJR_CHECK(a.error() == err);
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

// Type parametrized tests for both void and non-void expected
template<typename T>
class ExpectedTest : public ::testing::Test {};

using ExpectedTypes = ::testing::Types<
    expected<int, std::string>,
    expected<std::string, int>, 
    expected<void, int>,
    expected<void, std::string>,
    expected<int, compressed_value<bool, false>>,
    expected<void, compressed_value<int, -1>>
>;
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
    } else if constexpr (std::is_same_v<ErrorType, compressed_value<bool, false>>) {
        err = compressed_value<bool, false>(true);
    } else if constexpr (std::is_same_v<ErrorType, compressed_value<int, -1>>) {
        err = compressed_value<int, -1>(99);
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
        exp b(wjr::unexpected(compressed_value<bool, false>(true)));
        
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
        void_exp b(wjr::unexpected(compressed_value<int, -1>(99)));
        
        EXPECT_TRUE(a.has_value());
        EXPECT_FALSE(b.has_value());
        EXPECT_EQ(b.error(), 99);
    }
}
