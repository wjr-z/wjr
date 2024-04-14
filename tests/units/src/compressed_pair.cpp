#include "details.hpp"

#include <wjr/compressed_pair.hpp>

using namespace wjr;

struct disable_construct : enable_default_constructor_base<false, disable_construct> {
    using Mybase = enable_default_constructor_base<false, disable_construct>;

    disable_construct(enable_default_constructor_t)
        : Mybase(enable_default_constructor) {}
};

TEST(compressed_pair, constructor) {
    {
        using type = compressed_pair<int, int>;
        static_assert(std::is_trivially_copyable_v<type>, "trivially copyable error");
        type a;
        WJR_ASSERT(a.first() == 0);
        WJR_ASSERT(a.second() == 0);
        type b(dctor, dctor);
        type c(dctor, 3);
        (void)(b);
        WJR_ASSERT(c.second() == 3);
    }

    {
        compressed_pair<int, int> a(1, 2);
        WJR_ASSERT(a.first() == 1);
        WJR_ASSERT(a.second() == 2);
    }

    {
        compressed_pair<int, int> a(1, 2);
        compressed_pair<int, int> b(a);
        compressed_pair<int, int> c(std::move(a));
        compressed_pair<int, int> d = c;

        WJR_ASSERT(b.first() == 1);
        WJR_ASSERT(b.second() == 2);
        WJR_ASSERT(c.first() == 1);
        WJR_ASSERT(c.second() == 2);
        WJR_ASSERT(d.first() == 1);
        WJR_ASSERT(d.second() == 2);
    }

    {
        using type = compressed_pair<int, std::string>;
        static_assert(!std::is_trivially_copyable_v<type>, "trivially copyable error");
        type a(1, "hello");
        WJR_ASSERT(a.first() == 1);
        WJR_ASSERT(a.second() == "hello");
    }

    {
        using type = compressed_pair<in_place_empty_t, int>;
        static_assert(sizeof(type) == sizeof(int), "size error");
        type a;
        (void)(a);
        type b({}, 3);
        WJR_ASSERT(b.second() == 3);
    }

    {
        using type = compressed_pair<disable_construct, int>;
        static_assert(sizeof(type) == sizeof(int), "size error");
        static_assert(!std::is_default_constructible_v<type>,
                      "default constructible error");
        type a(enable_default_constructor, 3);
        WJR_ASSERT(a.second() == 3);
        type b(a);
        type c(std::move(a));
        type d = c;
        type e = std::move(c);
        WJR_ASSERT(b.second() == 3);
        WJR_ASSERT(c.second() == 3);
        WJR_ASSERT(d.second() == 3);
        WJR_ASSERT(e.second() == 3);
    }
}