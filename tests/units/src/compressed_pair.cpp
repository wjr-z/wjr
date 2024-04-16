#include "details.hpp"

#include <wjr/compressed_pair.hpp>
#include <wjr/tuple.hpp>

using namespace wjr;

struct disable_construct : enable_default_constructor_base<false, disable_construct> {
    using Mybase = enable_default_constructor_base<false, disable_construct>;

    disable_construct(enable_default_constructor_t)
        : Mybase(enable_default_constructor) {}
};

TEST(compressed_pair, constructor) {
    using cpi2 = compressed_pair<int, int>;
    using cpis = compressed_pair<int, std::string>;
    static_assert(std::is_trivially_copyable_v<cpi2>, "trivially copyable error");
    static_assert(!std::is_trivially_copyable_v<cpis>, "trivially copyable error");

    {
        cpi2 a;
        WJR_ASSERT(a.first() == 0);
        WJR_ASSERT(a.second() == 0);
        cpi2 b(dctor, dctor);
        cpi2 c(dctor, 3);
        (void)(b);
        WJR_ASSERT(c.second() == 3);

        a = std::make_pair(1, 2);
        WJR_ASSERT(a.first() == 1);
        WJR_ASSERT(a.second() == 2);

        a = std::make_tuple(3, 4);
        WJR_ASSERT(a.first() == 3);
        WJR_ASSERT(a.second() == 4);

        a = make_compressed_pair(5, 6);
        WJR_ASSERT(a.first() == 5);
        WJR_ASSERT(a.second() == 6);

        a = std::array<char, 2>{7, 8};
        WJR_ASSERT(a.first() == 7);
        WJR_ASSERT(a.second() == 8);
    }

    {
        tuple<int, int> a(1, 2);

        cpi2 b(std::make_pair(1, 3));
        WJR_ASSERT(b.first() == 1);
        WJR_ASSERT(b.second() == 3);

        auto [x, y] = a;
        WJR_ASSERT(x == 1);
        WJR_ASSERT(y == 2);

        auto &[z, w] = a;
        z = 5;
        w = 6;
    }

    {
        cpi2 a(1, 2);
        cpi2 b(a);
        cpi2 c(std::move(a));
        cpi2 d = c;

        WJR_ASSERT(b.first() == 1);
        WJR_ASSERT(b.second() == 2);
        WJR_ASSERT(c.first() == 1);
        WJR_ASSERT(c.second() == 2);
        WJR_ASSERT(d.first() == 1);
        WJR_ASSERT(d.second() == 2);
    }

    {
        cpis a(1, "hello");
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