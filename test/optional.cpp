#include "detail.hpp"

#include <string>
#include <vector>
#include <wjr/optional.hpp>

using namespace wjr;

namespace wjr {
template class optional<int>;
template class optional<compressed_value<int, 0>>;
} // namespace wjr

TEST_CASE("optional - trivial") {
    {
        using type = optional<int>;

        static_assert(std::is_trivially_destructible_v<type>, "");
        static_assert(std::is_trivially_copy_constructible_v<type>, "");
        static_assert(std::is_trivially_move_constructible_v<type>, "");
        static_assert(std::is_trivially_copy_assignable_v<type>, "");
        static_assert(std::is_trivially_move_assignable_v<type>, "");
    }

    {
        using type = optional<compressed_value<int, 0>>;

        static_assert(sizeof(type) == sizeof(int), "");
        static_assert(std::is_trivially_destructible_v<type>, "");
        static_assert(std::is_trivially_copy_constructible_v<type>, "");
        static_assert(std::is_trivially_move_constructible_v<type>, "");
        static_assert(std::is_trivially_copy_assignable_v<type>, "");
        static_assert(std::is_trivially_move_assignable_v<type>, "");
    }

    {
        using type = optional<compressed_value<const char *, nullptr>>;

        static_assert(sizeof(type) == sizeof(const char *), "");
        static_assert(std::is_trivially_destructible_v<type>, "");
        static_assert(std::is_trivially_copy_constructible_v<type>, "");
        static_assert(std::is_trivially_move_constructible_v<type>, "");
        static_assert(std::is_trivially_copy_assignable_v<type>, "");
        static_assert(std::is_trivially_move_assignable_v<type>, "");
    }
}

TEST_CASE("optional - constructor") {
    auto test = [](auto x, auto val) {
        using type = decltype(x);
        {
            type a;
            CHECK(!a.has_value());
        }
        {
            type a;
            type b(a);
            CHECK(!b.has_value());
            CHECK(a == b);
        }
        {
            type a;
            type b(std::move(a));
            CHECK(!b.has_value());
        }
        {
            type a(val);
            CHECK(a.has_value());
            CHECK(*a == val);
            type b(a);
            CHECK(b.has_value());
            CHECK(*b == val);
            CHECK(a == b);
            type c(std::move(a));
            CHECK(c.has_value());
            CHECK(*c == val);
            CHECK(b == c);
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
            type a(nullopt);
            CHECK(!a.has_value());
            type b(a);
            CHECK(!b.has_value());
            CHECK(a == b);
        }
    };

    test(optional<int>{}, 3);
    test(optional<compressed_value<int, 0>>{}, 3);
    test(optional<std::string>{}, "wjr");

    auto test_void = [](auto x) {
        using type = decltype(x);
        {
            type a;
            CHECK(!a.has_value());
        }
        {
            type a;
            type b(a);
            CHECK(!b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            CHECK(!c.has_value());
        }
        {
            type a(std::in_place);
            CHECK(a.has_value());
            type b(a);
            CHECK(b.has_value());
            type c(std::move(a));
            CHECK(c.has_value());
        }
    };

    test_void(optional<void>{});
}

TEST_CASE("optional - assignment") {
    // Test nullopt assignment
    {
        optional<int> a(42);
        CHECK(a.has_value());
        a = nullopt;
        CHECK(!a.has_value());
    }

    // Test value assignment
    {
        optional<int> a;
        CHECK(!a.has_value());
        a = 42;
        CHECK(a.has_value());
        CHECK(*a == 42);

        a = 99;
        CHECK(a.has_value());
        CHECK(*a == 99);
    }

    // Test optional assignment
    {
        optional<int> a(42);
        optional<int> b;

        b = a;
        CHECK(b.has_value());
        CHECK(*b == 42);

        optional<int> c;
        b = std::move(c);
        CHECK(!b.has_value());
    }

    // Test string assignment
    {
        optional<std::string> a;
        a = std::string("hello");
        CHECK(a.has_value());
        CHECK(*a == "hello");

        a = "world";
        CHECK(a.has_value());
        CHECK(*a == "world");
    }
}

TEST_CASE("optional - access_operators") {
    // Test operator* and operator->
    {
        optional<std::string> a("hello");
        CHECK(a.has_value());
        CHECK(*a == "hello");
        CHECK(a->size() == 5);
        CHECK(a->empty() == false);
    }

    // Test const access
    {
        const optional<std::string> a("world");
        CHECK(a.has_value());
        CHECK(*a == "world");
        CHECK(a->size() == 5);
    }

    // Test move access
    {
        optional<std::string> a("move_test");
        std::string moved = std::move(*a);
        CHECK(moved == "move_test");
    }
}

TEST_CASE("optional - value_method") {
    // Test value() with valid optional
    {
        optional<int> a(42);
        CHECK(a.value() == 42);

        const optional<int> b(99);
        CHECK(b.value() == 99);
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
        CHECK(caught);
    }

    // Test value() move semantics
    {
        optional<std::string> a("move_value");
        std::string moved = std::move(a).value();
        CHECK(moved == "move_value");
    }
}

TEST_CASE("optional - value_or_method") {
    // Test value_or with valid optional
    {
        optional<int> a(42);
        CHECK(a.value_or(99) == 42);
    }

    // Test value_or with empty optional
    {
        optional<int> a;
        CHECK(a.value_or(99) == 99);
    }

    // Test value_or with string
    {
        optional<std::string> a;
        CHECK(a.value_or("default") == "default");

        a = "hello";
        CHECK(a.value_or("default") == "hello");
    }

    // Test value_or with move
    {
        optional<std::string> a("move_test");
        std::string result = std::move(a).value_or("default");
        CHECK(result == "move_test");
    }
}

TEST_CASE("optional - comparison_operators") {
    // Test optional vs optional
    {
        optional<int> a(42);
        optional<int> b(42);
        optional<int> c(99);
        optional<int> d;
        optional<int> e;

        CHECK(a == b);
        CHECK(!(a == c));
        CHECK(!(a == d));
        CHECK(d == e);
    }

    // Test optional vs nullopt
    {
        optional<int> a(42);
        optional<int> b;

        CHECK(!(a == nullopt));
        CHECK(b == nullopt);
        CHECK(nullopt == b);
        CHECK(!(nullopt == a));
    }

    // Test optional vs value
    {
        optional<int> a(42);
        optional<int> b;

        CHECK(a == 42);
        CHECK(42 == a);
        CHECK(!(b == 42));
        CHECK(!(42 == b));
    }

    // Test with different types
    {
        optional<std::string> a("hello");
        optional<std::string> b;

        CHECK(a == "hello");
        CHECK("hello" == a);
        CHECK(!(b == "hello"));
        CHECK(!("hello" == b));
    }
}

TEST_CASE("optional - and_then") {
    // Test and_then with valid optional
    {
        optional<int> a(42);
        auto result = a.and_then([](int x) { return optional<int>(x * 2); });
        CHECK(result.has_value());
        CHECK(*result == 84);
    }

    // Test and_then with empty optional
    {
        optional<int> a;
        auto result = a.and_then([](int x) { return optional<int>(x * 2); });
        CHECK(!result.has_value());
    }

    // Test and_then returning empty optional
    {
        optional<int> a(42);
        auto result = a.and_then([](int) { return optional<int>(); });
        CHECK(!result.has_value());
    }

    // Test and_then with const optional
    {
        const optional<int> a(42);
        auto result = a.and_then([](int x) { return optional<std::string>(std::to_string(x)); });
        CHECK(result.has_value());
        CHECK(*result == "42");
    }

    // Test and_then with move
    {
        optional<std::string> a("hello");
        auto result = std::move(a).and_then(
            [](std::string s) { return optional<std::string>(s + " world"); });
        CHECK(result.has_value());
        CHECK(*result == "hello world");
    }
}

TEST_CASE("optional - or_else") {
    // Test or_else with valid optional
    {
        optional<int> a(42);
        auto result = a.or_else([]() { return optional<int>(99); });
        CHECK(result.has_value());
        CHECK(*result == 42);
    }

    // Test or_else with empty optional
    {
        optional<int> a;
        auto result = a.or_else([]() { return optional<int>(99); });
        CHECK(result.has_value());
        CHECK(*result == 99);
    }

    // Test or_else returning empty optional
    {
        optional<int> a;
        auto result = a.or_else([]() { return optional<int>(); });
        CHECK(!result.has_value());
    }

    // Test or_else with const optional
    {
        const optional<int> a;
        auto result = a.or_else([]() { return optional<int>(99); });
        CHECK(result.has_value());
        CHECK(*result == 99);
    }
}

TEST_CASE("optional - transform") {
    // Test transform with valid optional
    {
        optional<int> a(42);
        auto result = a.transform([](int x) { return x * 2; });
        CHECK(result.has_value());
        CHECK(*result == 84);
    }

    // Test transform with empty optional
    {
        optional<int> a;
        auto result = a.transform([](int x) { return x * 2; });
        CHECK(!result.has_value());
    }

    // Test transform changing type
    {
        optional<int> a(42);
        auto result = a.transform([](int x) { return std::to_string(x); });
        CHECK(result.has_value());
        CHECK(*result == "42");
    }

    // Test transform with const optional
    {
        const optional<int> a(42);
        auto result = a.transform([](int x) { return x * 2; });
        CHECK(result.has_value());
        CHECK(*result == 84);
    }

    // Test transform with move
    {
        optional<std::string> a("hello");
        auto result = std::move(a).transform([](std::string s) { return s + " world"; });
        CHECK(result.has_value());
        CHECK(*result == "hello world");
    }
}

TEST_CASE("optional - emplace") {
    // Test emplace with empty optional
    {
        optional<int> a;
        CHECK(!a.has_value());
        auto &ref = a.emplace(42);
        CHECK(a.has_value());
        CHECK(*a == 42);
        CHECK(&ref == &(*a));
    }

    // Test emplace with valid optional (should replace)
    {
        optional<int> a(99);
        CHECK(a.has_value());
        CHECK(*a == 99);
        auto &ref = a.emplace(42);
        CHECK(a.has_value());
        CHECK(*a == 42);
        CHECK(&ref == &(*a));
    }

    // Test emplace with multiple arguments
    {
        optional<std::string> a;
        auto &ref = a.emplace(5, 'x');
        CHECK(a.has_value());
        CHECK(*a == "xxxxx");
        CHECK(&ref == &(*a));
    }

    // Test emplace with initializer list
    {
        optional<std::vector<int>> a;
        auto &ref = a.emplace({1, 2, 3, 4});
        CHECK(a.has_value());
        CHECK(a->size() == 4);
        CHECK((*a)[0] == 1);
        CHECK((*a)[3] == 4);
        CHECK(&ref == &(*a));
    }
}

TEST_CASE("optional - swap") {
    // Test swap both empty
    {
        optional<int> a, b;
        a.swap(b);
        CHECK(!a.has_value());
        CHECK(!b.has_value());
    }

    // Test swap both valid
    {
        optional<int> a(42), b(99);
        a.swap(b);
        CHECK(a.has_value());
        CHECK(*a == 99);
        CHECK(b.has_value());
        CHECK(*b == 42);
    }

    // Test swap one empty, one valid
    {
        optional<int> a(42), b;
        a.swap(b);
        CHECK(!a.has_value());
        CHECK(b.has_value());
        CHECK(*b == 42);
    }

    // Test swap with std::swap
    {
        optional<std::string> a("hello"), b("world");
        std::swap(a, b);
        CHECK(a.has_value());
        CHECK(*a == "world");
        CHECK(b.has_value());
        CHECK(*b == "hello");
    }
}

TEST_CASE("optional - void_specialization") {
    // Test void optional construction
    {
        optional<void> a;
        CHECK(!a.has_value());

        optional<void> b(std::in_place);
        CHECK(b.has_value());

        optional<void> c(voidopt);
        CHECK(c.has_value());

        optional<void> d(nullopt);
        CHECK(!d.has_value());
    }

    // Test void optional assignment
    {
        optional<void> a;
        a = nullopt;
        CHECK(!a.has_value());

        a.emplace();
        CHECK(a.has_value());
    }

    // Test void optional comparison
    {
        optional<void> a, b;
        optional<void> c(std::in_place), d(std::in_place);

        CHECK(a == b);
        CHECK(c == d);
        CHECK(!(a == c));

        CHECK(a == nullopt);
        CHECK(!(c == nullopt));
    }

    // Test void optional monadic operations
    {
        optional<void> a(std::in_place);
        bool called = false;
        auto result = a.and_then([&]() {
            called = true;
            return optional<int>(42);
        });
        CHECK(called);
        CHECK(result.has_value());
        CHECK(*result == 42);

        optional<void> b;
        called = false;
        auto result2 = b.and_then([&]() {
            called = true;
            return optional<int>(42);
        });
        CHECK(!called);
        CHECK(!result2.has_value());
    }

    // Test void optional or_else
    {
        optional<void> a;
        auto result = a.or_else([]() { return optional<void>(std::in_place); });
        CHECK(result.has_value());

        optional<void> b(std::in_place);
        auto result2 = b.or_else([]() { return optional<void>(); });
        CHECK(result2.has_value());
    }

    // Test void optional transform
    {
        optional<void> a(std::in_place);
        auto result = a.transform([]() { return 42; });
        CHECK(result.has_value());
        CHECK(*result == 42);

        optional<void> b;
        auto result2 = b.transform([]() { return 42; });
        CHECK(!result2.has_value());
    }
}

TEST_CASE("optional - compressed_value") {
    // Test compressed_value<int, 0>
    {
        using type = optional<compressed_value<int, 0>>;

        type a;
        CHECK(!a.has_value());

        type b(42);
        CHECK(b.has_value());
        CHECK(*b == 42);
    }

    // Test compressed_value<pointer, nullptr>
    {
        int value = 42;
        using type = optional<compressed_value<int *, nullptr>>;

        type a;
        CHECK(!a.has_value());

        type b(&value);
        CHECK(b.has_value());
        CHECK(**b == 42);
    }

    // Test compressed_optional typedef
    {
        int value = 99;
        compressed_optional<int *> a;
        CHECK(!a.has_value());

        a = &value;
        CHECK(a.has_value());
        CHECK(**a == 99);
    }
}

TEST_CASE("optional - macros") {
    // Test WJR_OPTIONAL_TRY macro
    {
        auto test_func = [](bool success) -> optional<int> {
            WJR_OPTIONAL_TRY(success ? optional<void>(std::in_place) : optional<void>());
            return 42;
        };

        auto result1 = test_func(true);
        CHECK(result1.has_value());
        CHECK(*result1 == 42);

        auto result2 = test_func(false);
        CHECK(!result2.has_value());
    }

    // Test WJR_OPTIONAL_INIT macro
    {
        auto test_func = [](bool success) -> optional<int> {
            WJR_OPTIONAL_INIT(value, success ? optional<int>(99) : optional<int>());
            return *value * 2;
        };

        auto result1 = test_func(true);
        CHECK(result1.has_value());
        CHECK(*result1 == 198);

        auto result2 = test_func(false);
        CHECK(!result2.has_value());
    }

    // Test WJR_OPTIONAL_SET macro
    {
        auto test_func = [](bool success) -> optional<int> {
            int result = 0;
            WJR_OPTIONAL_SET(result, success ? optional<int>(99) : optional<int>());
            return result * 2;
        };

        auto result1 = test_func(true);
        CHECK(result1.has_value());
        CHECK(*result1 == 198);

        auto result2 = test_func(false);
        CHECK(!result2.has_value());
    }
}
