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
            WJR_ASSERT(a.has_value());
            WJR_ASSERT(*a == x);
        }
        {
            type a;
            type b(a);
            WJR_ASSERT(b.has_value());
            WJR_ASSERT(*b == x);
            WJR_ASSERT(a == b);
        }
        {
            type a;
            type b(std::move(a));
            WJR_ASSERT(b.has_value());
            WJR_ASSERT(*b == x);
        }
        {
            type a(val);
            WJR_ASSERT(a.has_value());
            WJR_ASSERT(*a == val);
        }
        {
            auto val2(val);
            type a(std::move(val2));
            WJR_ASSERT(a.has_value());
            WJR_ASSERT(*a == val);
        }
        {
            type a(std::in_place, val);
            WJR_ASSERT(a.has_value());
            WJR_ASSERT(*a == val);
        }
        {
            auto val2(val);
            type a(std::in_place, std::move(val2));
            WJR_ASSERT(a.has_value());
            WJR_ASSERT(*a == val);
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
            type b(a);
            WJR_ASSERT(!b.has_value());
            WJR_ASSERT(b.error() == err);
            WJR_ASSERT(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
            type b(std::move(a));
            WJR_ASSERT(!b.has_value());
            WJR_ASSERT(b.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, ec);
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
        }

        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
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
            WJR_ASSERT(a.has_value());
        }
        {
            type a;
            type b(a);
            WJR_ASSERT(b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            WJR_ASSERT(c.has_value());
        }
        {
            type a(std::in_place);
            WJR_ASSERT(a.has_value());
        }
        {
            wjr::unexpected ec(err);
            type a(ec);
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
            type b(a);
            WJR_ASSERT(!b.has_value());
            WJR_ASSERT(b.error() == err);
            WJR_ASSERT(a == b);
        }
        {
            wjr::unexpected ec(err);
            type a(std::move(ec));
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
            type b(std::move(a));
            WJR_ASSERT(!b.has_value());
            WJR_ASSERT(b.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, ec);
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
        }
        {
            auto ec(err);
            type a(unexpect, std::move(ec));
            WJR_ASSERT(!a.has_value());
            WJR_ASSERT(a.error() == err);
        }
    };

    test_void(expected<void, int>{}, 3);
    test_void(expected<void, std::string>{}, "wjr");
    test_void(expected<void, compressed_value<int, 3>>{}, 7);
}
