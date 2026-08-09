#include "detail.hpp"

#include <wjr/functional.hpp>

using namespace wjr;

namespace {
void __set4(int &x) { x = 4; }
int __square(int x) { return x * x; }
int __noexcept_square(int x) noexcept { return x * x; }

struct const_callable {
    int operator()(int x) const { return x + 1; }
};
} // namespace

TEST(functional, function_ref) {
    int a = 0;

    {
        auto func = [&a]() { a = 1; };
        function_ref<void()> f = func;
        f();
        EXPECT_EQ(a, 1);
    }

    {
        auto func = [&a]() mutable { a = 2; };
        function_ref<void()> f = func;
        f();
        EXPECT_EQ(a, 2);
    }

    {
        auto func = [](int &x) { x = 3; };
        function_ref<void(int &)> f = func;
        f(a);
        EXPECT_EQ(a, 3);
    }

    {
        function_ref<void(int &)> f = nontype<&__set4>;
        f(a);
        EXPECT_EQ(a, 4);
    }

    static_assert(std::is_constructible_v<function_ref<void()>, void (*)() noexcept>);
}

TEST(functional, function_ref_const) {
    int a = 0;

    {
        auto func = [&a]() { a = 1; };
        function_ref<void() const> f = func;
        f();
        EXPECT_EQ(a, 1);
    }

    {
        auto func = [](int &x) { x = 3; };
        function_ref<void(int &) const> f = func;
        f(a);
        EXPECT_EQ(a, 3);
    }

    static_assert(std::is_constructible_v<function_ref<void() const>, void (*)() noexcept>);
    auto mutable_func = [](int &) mutable {};
    static_assert(!std::is_constructible_v<function_ref<void() const>, decltype(mutable_func)>);
}

TEST(functional, function_ref_function_pointer) {
    function_ref<int(int)> f = &__square;

    EXPECT_EQ(f(4), 16);
}

TEST(functional, function_ref_noexcept) {
    function_ref<int(int) noexcept> f = &__noexcept_square;

    EXPECT_EQ(f(4), 16);
}

TEST(functional, function_ref_const_callable) {
    const const_callable callable;
    function_ref<int(int) const> f = callable;

    EXPECT_EQ(f(4), 5);
}