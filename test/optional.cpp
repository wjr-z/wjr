#include "detail.hpp"

#include <string>
#include <vector>
#include <wjr/optional.hpp>

using namespace wjr;

namespace wjr {
template class optional<int>;
template class optional<compressed_value<int, 0>>;
} // namespace wjr

TEST(optional, trivial) {
    {
        using type = optional<int>;

        static_assert(std::is_trivially_destructible_v<type>);
        static_assert(std::is_trivially_copy_constructible_v<type>);
        static_assert(std::is_trivially_move_constructible_v<type>);
        static_assert(std::is_trivially_copy_assignable_v<type>);
        static_assert(std::is_trivially_move_assignable_v<type>);
    }

    {
        using type = optional<compressed_value<int, 0>>;

        static_assert(sizeof(type) == sizeof(int));
        static_assert(std::is_trivially_destructible_v<type>);
        static_assert(std::is_trivially_copy_constructible_v<type>);
        static_assert(std::is_trivially_move_constructible_v<type>);
        static_assert(std::is_trivially_copy_assignable_v<type>);
        static_assert(std::is_trivially_move_assignable_v<type>);
    }

    {
        using type = optional<compressed_value<const char *, nullptr>>;

        static_assert(sizeof(type) == sizeof(const char *));
        static_assert(std::is_trivially_destructible_v<type>);
        static_assert(std::is_trivially_copy_constructible_v<type>);
        static_assert(std::is_trivially_move_constructible_v<type>);
        static_assert(std::is_trivially_copy_assignable_v<type>);
        static_assert(std::is_trivially_move_assignable_v<type>);
    }
}

TEST(optional, constructor) {
    auto test = [](auto x, auto val) {
        using type = decltype(x);
        {
            type a;
            EXPECT_TRUE(!a.has_value());
        }
        {
            type a;
            type b(a);
            EXPECT_TRUE(!b.has_value());
            EXPECT_TRUE(a == b);
        }
        {
            type a;
            type b(std::move(a));
            EXPECT_TRUE(!b.has_value());
        }
        {
            type a(val);
            EXPECT_TRUE(a.has_value());
            EXPECT_TRUE(*a == val);
            type b(a);
            EXPECT_TRUE(b.has_value());
            EXPECT_TRUE(*b == val);
            EXPECT_TRUE(a == b);
            type c(std::move(a));
            EXPECT_TRUE(c.has_value());
            EXPECT_TRUE(*c == val);
            EXPECT_TRUE(b == c);
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
            type a(nullopt);
            EXPECT_TRUE(!a.has_value());
            type b(a);
            EXPECT_TRUE(!b.has_value());
            EXPECT_TRUE(a == b);
        }
    };

    test(optional<int>{}, 3);
    test(optional<compressed_value<int, 0>>{}, 3);
    test(optional<std::string>{}, "wjr");

    auto test_void = [](auto x) {
        using type = decltype(x);
        {
            type a;
            EXPECT_TRUE(!a.has_value());
        }
        {
            type a;
            type b(a);
            EXPECT_TRUE(!b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            EXPECT_TRUE(!c.has_value());
        }
        {
            type a(std::in_place);
            EXPECT_TRUE(a.has_value());
            type b(a);
            EXPECT_TRUE(b.has_value());
            type c(std::move(a));
            EXPECT_TRUE(c.has_value());
        }
    };

    test_void(optional<void>{});
}

TEST(optional, assignment) {
    // Test nullopt assignment
    {
        optional<int> a(42);
        EXPECT_TRUE(a.has_value());
        a = nullopt;
        EXPECT_TRUE(!a.has_value());
    }

    // Test value assignment
    {
        optional<int> a;
        EXPECT_TRUE(!a.has_value());
        a = 42;
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == 42);

        a = 99;
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == 99);
    }

    // Test optional assignment
    {
        optional<int> a(42);
        optional<int> b;

        b = a;
        EXPECT_TRUE(b.has_value());
        EXPECT_TRUE(*b == 42);

        optional<int> c;
        b = std::move(c);
        EXPECT_TRUE(!b.has_value());
    }

    // Test string assignment
    {
        optional<std::string> a;
        a = std::string("hello");
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == "hello");

        a = "world";
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == "world");
    }
}

TEST(optional, access_operators) {
    // Test operator* and operator->
    {
        optional<std::string> a("hello");
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == "hello");
        EXPECT_TRUE(a->size() == 5);
        EXPECT_TRUE(a->empty() == false);
    }

    // Test const access
    {
        const optional<std::string> a("world");
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == "world");
        EXPECT_TRUE(a->size() == 5);
    }

    // Test move access
    {
        optional<std::string> a("move_test");
        std::string moved = std::move(*a);
        EXPECT_TRUE(moved == "move_test");
    }
}

TEST(optional, value_method) {
    // Test value() with valid optional
    {
        optional<int> a(42);
        EXPECT_TRUE(a.value() == 42);

        const optional<int> b(99);
        EXPECT_TRUE(b.value() == 99);
    }

    // Test value() with empty optional - should throw
    {
        optional<int> a;
        bool caught = false;
        try {
            a.value();
        } catch (const bad_optional_access &) {
            caught = true;
        }
        EXPECT_TRUE(caught);
    }

    // Test value() move semantics
    {
        optional<std::string> a("move_value");
        std::string moved = std::move(a).value();
        EXPECT_TRUE(moved == "move_value");
    }
}

TEST(optional, value_or_method) {
    // Test value_or with valid optional
    {
        optional<int> a(42);
        EXPECT_TRUE(a.value_or(99) == 42);
    }

    // Test value_or with empty optional
    {
        optional<int> a;
        EXPECT_TRUE(a.value_or(99) == 99);
    }

    // Test value_or with string
    {
        optional<std::string> a;
        EXPECT_TRUE(a.value_or("default") == "default");

        a = "hello";
        EXPECT_TRUE(a.value_or("default") == "hello");
    }

    // Test value_or with move
    {
        optional<std::string> a("move_test");
        std::string result = std::move(a).value_or("default");
        EXPECT_TRUE(result == "move_test");
    }
}

TEST(optional, comparison_operators) {
    // Test optional vs optional
    {
        optional<int> a(42);
        optional<int> b(42);
        optional<int> c(99);
        optional<int> d;
        optional<int> e;

        EXPECT_TRUE(a == b);
        EXPECT_TRUE(!(a == c));
        EXPECT_TRUE(!(a == d));
        EXPECT_TRUE(d == e);
    }

    // Test optional vs nullopt
    {
        optional<int> a(42);
        optional<int> b;

        EXPECT_TRUE(!(a == nullopt));
        EXPECT_TRUE(b == nullopt);
        EXPECT_TRUE(nullopt == b);
        EXPECT_TRUE(!(nullopt == a));
    }

    // Test optional vs value
    {
        optional<int> a(42);
        optional<int> b;

        EXPECT_TRUE(a == 42);
        EXPECT_TRUE(42 == a);
        EXPECT_TRUE(!(b == 42));
        EXPECT_TRUE(!(42 == b));
    }

    // Test with different types
    {
        optional<std::string> a("hello");
        optional<std::string> b;

        EXPECT_TRUE(a == "hello");
        EXPECT_TRUE("hello" == a);
        EXPECT_TRUE(!(b == "hello"));
        EXPECT_TRUE(!("hello" == b));
    }
}

TEST(optional, and_then) {
    // Test and_then with valid optional
    {
        optional<int> a(42);
        auto result = a.and_then([](int x) { return optional<int>(x * 2); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 84);
    }

    // Test and_then with empty optional
    {
        optional<int> a;
        auto result = a.and_then([](int x) { return optional<int>(x * 2); });
        EXPECT_TRUE(!result.has_value());
    }

    // Test and_then returning empty optional
    {
        optional<int> a(42);
        auto result = a.and_then([](int) { return optional<int>(); });
        EXPECT_TRUE(!result.has_value());
    }

    // Test and_then with const optional
    {
        const optional<int> a(42);
        auto result = a.and_then([](int x) { return optional<std::string>(std::to_string(x)); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == "42");
    }

    // Test and_then with move
    {
        optional<std::string> a("hello");
        auto result = std::move(a).and_then(
            [](std::string s) { return optional<std::string>(s + " world"); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == "hello world");
    }
}

TEST(optional, or_else) {
    // Test or_else with valid optional
    {
        optional<int> a(42);
        auto result = a.or_else([]() { return optional<int>(99); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 42);
    }

    // Test or_else with empty optional
    {
        optional<int> a;
        auto result = a.or_else([]() { return optional<int>(99); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 99);
    }

    // Test or_else returning empty optional
    {
        optional<int> a;
        auto result = a.or_else([]() { return optional<int>(); });
        EXPECT_TRUE(!result.has_value());
    }

    // Test or_else with const optional
    {
        const optional<int> a;
        auto result = a.or_else([]() { return optional<int>(99); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 99);
    }
}

TEST(optional, transform) {
    // Test transform with valid optional
    {
        optional<int> a(42);
        auto result = a.transform([](int x) { return x * 2; });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 84);
    }

    // Test transform with empty optional
    {
        optional<int> a;
        auto result = a.transform([](int x) { return x * 2; });
        EXPECT_TRUE(!result.has_value());
    }

    // Test transform changing type
    {
        optional<int> a(42);
        auto result = a.transform([](int x) { return std::to_string(x); });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == "42");
    }

    // Test transform with const optional
    {
        const optional<int> a(42);
        auto result = a.transform([](int x) { return x * 2; });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 84);
    }

    // Test transform with move
    {
        optional<std::string> a("hello");
        auto result = std::move(a).transform([](std::string s) { return s + " world"; });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == "hello world");
    }
}

TEST(optional, emplace) {
    // Test emplace with empty optional
    {
        optional<int> a;
        EXPECT_TRUE(!a.has_value());
        auto &ref = a.emplace(42);
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == 42);
        EXPECT_TRUE(&ref == &(*a));
    }

    // Test emplace with valid optional (should replace)
    {
        optional<int> a(99);
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == 99);
        auto &ref = a.emplace(42);
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == 42);
        EXPECT_TRUE(&ref == &(*a));
    }

    // Test emplace with multiple arguments
    {
        optional<std::string> a;
        auto &ref = a.emplace(5, 'x');
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(*a == "xxxxx");
        EXPECT_TRUE(&ref == &(*a));
    }

    // Test emplace with initializer list
    {
        optional<std::vector<int>> a;
        auto &ref = a.emplace({1, 2, 3, 4});
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(a->size() == 4);
        EXPECT_TRUE((*a)[0] == 1);
        EXPECT_TRUE((*a)[3] == 4);
        EXPECT_TRUE(&ref == &(*a));
    }
}

TEST(optional, swap) {
    // Test swap both empty
    {
        optional<int> a, b;
        a.swap(b);
        EXPECT_TRUE(!a.has_value());
        EXPECT_TRUE(!b.has_value());
    }

    // Test swap both valid
    {
        optional<int> a(42), b(99);
        a.swap(b);
        EXPECT_TRUE(a.has_value() && *a == 99);
        EXPECT_TRUE(b.has_value() && *b == 42);
    }

    // Test swap one empty, one valid
    {
        optional<int> a(42), b;
        a.swap(b);
        EXPECT_TRUE(!a.has_value());
        EXPECT_TRUE(b.has_value() && *b == 42);
    }

    // Test swap with std::swap
    {
        optional<std::string> a("hello"), b("world");
        std::swap(a, b);
        EXPECT_TRUE(a.has_value() && *a == "world");
        EXPECT_TRUE(b.has_value() && *b == "hello");
    }
}

TEST(optional, void_specialization) {
    // Test void optional construction
    {
        optional<void> a;
        EXPECT_TRUE(!a.has_value());

        optional<void> b(std::in_place);
        EXPECT_TRUE(b.has_value());

        optional<void> c(voidopt);
        EXPECT_TRUE(c.has_value());

        optional<void> d(nullopt);
        EXPECT_TRUE(!d.has_value());
    }

    // Test void optional assignment
    {
        optional<void> a;
        a = nullopt;
        EXPECT_TRUE(!a.has_value());

        a.emplace();
        EXPECT_TRUE(a.has_value());
    }

    // Test void optional comparison
    {
        optional<void> a, b;
        optional<void> c(std::in_place), d(std::in_place);

        EXPECT_TRUE(a == b);
        EXPECT_TRUE(c == d);
        EXPECT_TRUE(!(a == c));

        EXPECT_TRUE(a == nullopt);
        EXPECT_TRUE(!(c == nullopt));
    }

    // Test void optional monadic operations
    {
        optional<void> a(std::in_place);
        bool called = false;
        auto result = a.and_then([&]() {
            called = true;
            return optional<int>(42);
        });
        EXPECT_TRUE(called);
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 42);

        optional<void> b;
        called = false;
        auto result2 = b.and_then([&]() {
            called = true;
            return optional<int>(42);
        });
        EXPECT_TRUE(!called);
        EXPECT_TRUE(!result2.has_value());
    }

    // Test void optional or_else
    {
        optional<void> a;
        auto result = a.or_else([]() { return optional<void>(std::in_place); });
        EXPECT_TRUE(result.has_value());

        optional<void> b(std::in_place);
        auto result2 = b.or_else([]() { return optional<void>(); });
        EXPECT_TRUE(result2.has_value());
    }

    // Test void optional transform
    {
        optional<void> a(std::in_place);
        auto result = a.transform([]() { return 42; });
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(*result == 42);

        optional<void> b;
        auto result2 = b.transform([]() { return 42; });
        EXPECT_TRUE(!result2.has_value());
    }
}

TEST(optional, compressed_value) {
    // Test compressed_value<int, 0>
    {
        using type = optional<compressed_value<int, 0>>;

        type a;
        EXPECT_TRUE(!a.has_value());

        type b(42);
        EXPECT_TRUE(b.has_value());
        EXPECT_TRUE(*b == 42);
    }

    // Test compressed_value<pointer, nullptr>
    {
        int value = 42;
        using type = optional<compressed_value<int *, nullptr>>;

        type a;
        EXPECT_TRUE(!a.has_value());

        type b(&value);
        EXPECT_TRUE(b.has_value());
        EXPECT_TRUE(**b == 42);
    }

    // Test compressed_optional typedef
    {
        int value = 99;
        compressed_optional<int *> a;
        EXPECT_TRUE(!a.has_value());

        a = &value;
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(**a == 99);
    }
}

TEST(optional, macros) {
    // Test WJR_OPTIONAL_TRY macro
    {
        auto test_func = [](bool success) -> optional<int> {
            WJR_OPTIONAL_TRY(success ? optional<void>(std::in_place) : optional<void>());
            return 42;
        };

        auto result1 = test_func(true);
        EXPECT_TRUE(result1.has_value());
        EXPECT_TRUE(*result1 == 42);

        auto result2 = test_func(false);
        EXPECT_TRUE(!result2.has_value());
    }

    // Test WJR_OPTIONAL_INIT macro
    {
        auto test_func = [](bool success) -> optional<int> {
            WJR_OPTIONAL_INIT(value, success ? optional<int>(99) : optional<int>());
            return *value * 2;
        };

        auto result1 = test_func(true);
        EXPECT_TRUE(result1.has_value());
        EXPECT_TRUE(*result1 == 198);

        auto result2 = test_func(false);
        EXPECT_TRUE(!result2.has_value());
    }

    // Test WJR_OPTIONAL_SET macro
    {
        auto test_func = [](bool success) -> optional<int> {
            int result = 0;
            WJR_OPTIONAL_SET(result, success ? optional<int>(99) : optional<int>());
            return result * 2;
        };

        auto result1 = test_func(true);
        EXPECT_TRUE(result1.has_value());
        EXPECT_TRUE(*result1 == 198);

        auto result2 = test_func(false);
        EXPECT_TRUE(!result2.has_value());
    }
}
