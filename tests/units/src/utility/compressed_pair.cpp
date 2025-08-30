#include "detail.hpp"

#include <array>

#include <wjr/compressed_pair.hpp>
#include <wjr/tuple.hpp>

using namespace wjr;

struct disable_construct : enable_default_constructor_base<false, disable_construct> {
    using Mybase = enable_default_constructor_base<false, disable_construct>;

    explicit disable_construct(enable_default_constructor_t) : Mybase(enable_default_constructor) {}
};

struct default_construtor_struct {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(default_construtor_struct);
    default_construtor_struct(enable_default_constructor_t) {}
};

TEST(compressed_pair, constructor) {
    using cpi2 = compressed_pair<int, int>;
    using cpis = compressed_pair<int, std::string>;

    static_assert(std::is_trivially_copyable_v<cpi2>,
                  "compressed_pair<int, int> must be trivially copyable");

    static_assert(!std::is_trivially_copyable_v<cpis>,
                  "Why compressed_pair<int, std::string> is trivially copyable?");

    static_assert(sizeof(compressed_pair<std::less<>, compressed_pair<std::greater<>, int>>) ==
                      sizeof(int),
                  "compressed_pair EBO miss");

    static_assert(sizeof(compressed_pair<std::allocator<int>, int>) == sizeof(int),
                  "compressed_pair EBO miss");

    static_assert(std::is_standard_layout_v<compressed_pair<int, int>>,
                  "compressed_pair must be standard layout");

    static_assert(std::is_standard_layout_v<compressed_pair<std::allocator<int>, int>>,
                  "compressed_pair must be standard layout");

    static_assert(std::is_standard_layout_v<compressed_pair<int, std::allocator<int>>>,
                  "compressed_pair must be standard layout");

    static_assert(std::is_constructible_v<compressed_pair<default_construtor_struct, int>,
                                          enable_default_constructor_t, int>,
                  "There is ambiguity in the constructor of comrpessed_pair \
         when using enable_default_constructor_t");

    {
        cpi2 a;
        WJR_CHECK(a.first() == 0);
        WJR_CHECK(a.second() == 0);
        cpi2 b(default_construct, default_construct);
        cpi2 c(default_construct, 3);
        (void)(b);
        WJR_CHECK(c.second() == 3);

        a = std::make_pair(1, 2);
        WJR_CHECK(a.first() == 1);
        WJR_CHECK(a.second() == 2);

        a = std::make_tuple(3, 4);
        WJR_CHECK(a.first() == 3);
        WJR_CHECK(a.second() == 4);

        a = make_compressed_pair(5, 6);
        WJR_CHECK(a.first() == 5);
        WJR_CHECK(a.second() == 6);

        a = std::array<char, 2>{7, 8};
        WJR_CHECK(a.first() == 7);
        WJR_CHECK(a.second() == 8);
    }

    {
        tuple<int, int> a(1, 2);

        cpi2 b(std::make_pair(1, 3));
        WJR_CHECK(b.first() == 1);
        WJR_CHECK(b.second() == 3);

        auto [x, y] = a;
        WJR_CHECK(x == 1);
        WJR_CHECK(y == 2);

        auto &[z, w] = a;
        z = 5;
        w = 6;
    }

    {
        cpi2 a(1, 2);
        cpi2 b(a);
        cpi2 c(std::move(a));
        cpi2 d = c;

        WJR_CHECK(b.first() == 1);
        WJR_CHECK(b.second() == 2);
        WJR_CHECK(c.first() == 1);
        WJR_CHECK(c.second() == 2);
        WJR_CHECK(d.first() == 1);
        WJR_CHECK(d.second() == 2);
    }

    {
        cpis a(1, "hello");
        WJR_CHECK(a.first() == 1);
        WJR_CHECK(a.second() == "hello");
    }

    {
        using type = compressed_pair<in_place_empty_t, int>;
        static_assert(sizeof(type) == sizeof(int), "size error");
        type a;
        (void)(a);
        type b({}, 3);
        WJR_CHECK(b.second() == 3);
    }

    {
        using type = compressed_pair<disable_construct, int>;
        static_assert(sizeof(type) == sizeof(int), "size error");
        static_assert(!std::is_default_constructible_v<type>, "default constructible error");
        type a(enable_default_constructor, 3);
        WJR_CHECK(a.second() == 3);
        type b(a);
        type c(std::move(a));
        type d = c;
        type e = std::move(c);
        WJR_CHECK(b.second() == 3);
        WJR_CHECK(c.second() == 3);
        WJR_CHECK(d.second() == 3);
        WJR_CHECK(e.second() == 3);
    }
}