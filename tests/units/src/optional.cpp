#include "detail.hpp"

#include <wjr/optional.hpp>

using namespace wjr;

namespace wjr {
template class optional<int>;
template class optional<compressed_value<int, 0>>;
} // namespace wjr

TEST(optional, trivial) {
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

TEST(optional, constructor) {
    auto test = [](auto x, auto val) {
        using type = decltype(x);
        {
            type a;
            WJR_ASSERT_ALWAYS(!a.has_value());
        }
        {
            type a;
            type b(a);
            WJR_ASSERT_ALWAYS(!b.has_value());
            WJR_ASSERT_ALWAYS(a == b);
        }
        {
            type a;
            type b(std::move(a));
            WJR_ASSERT_ALWAYS(!b.has_value());
        }
        {
            type a(val);
            WJR_ASSERT_ALWAYS(a.has_value());
            WJR_ASSERT_ALWAYS(*a == val);
            type b(a);
            WJR_ASSERT_ALWAYS(b.has_value());
            WJR_ASSERT_ALWAYS(*b == val);
            WJR_ASSERT_ALWAYS(a == b);
            type c(std::move(a));
            WJR_ASSERT_ALWAYS(c.has_value());
            WJR_ASSERT_ALWAYS(*c == val);
            WJR_ASSERT_ALWAYS(b == c);
        }
        {
            auto val2(val);
            type a(std::move(val2));
            WJR_ASSERT_ALWAYS(a.has_value());
            WJR_ASSERT_ALWAYS(*a == val);
        }
        {
            type a(std::in_place, val);
            WJR_ASSERT_ALWAYS(a.has_value());
            WJR_ASSERT_ALWAYS(*a == val);
        }
        {
            auto val2(val);
            type a(std::in_place, std::move(val2));
            WJR_ASSERT_ALWAYS(a.has_value());
            WJR_ASSERT_ALWAYS(*a == val);
        }
        {
            type a(nullopt);
            WJR_ASSERT_ALWAYS(!a.has_value());
            type b(a);
            WJR_ASSERT_ALWAYS(!b.has_value());
            WJR_ASSERT_ALWAYS(a == b);
        }
    };

    test(optional<int>{}, 3);
    test(optional<compressed_value<int, 0>>{}, 3);
    test(optional<std::string>{}, "wjr");

    auto test_void = [](auto x) {
        using type = decltype(x);
        {
            type a;
            WJR_ASSERT_ALWAYS(!a.has_value());
        }
        {
            type a;
            type b(a);
            WJR_ASSERT_ALWAYS(!b.has_value());
        }
        {
            type a;
            type c(std::move(a));
            WJR_ASSERT_ALWAYS(!c.has_value());
        }
        {
            type a(std::in_place);
            WJR_ASSERT_ALWAYS(a.has_value());
            type b(a);
            WJR_ASSERT_ALWAYS(b.has_value());
            type c(std::move(a));
            WJR_ASSERT_ALWAYS(c.has_value());
        }
    };

    test_void(optional<void>{});
}
