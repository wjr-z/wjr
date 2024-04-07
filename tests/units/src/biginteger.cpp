#include "details.hpp"

#if defined(WJR_USE_GMP)
#include <gmp.h>
#endif

#include <wjr/biginteger.hpp>

using namespace wjr;

TEST(biginteger, construct) {
    {
        biginteger a;
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
    }

    {
        biginteger a;
        biginteger b(a);
        biginteger c(std::move(a));
        biginteger d = c;

        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(b.size() == 0);
        WJR_ASSERT(c.size() == 0);
        WJR_ASSERT(d.size() == 0);
    }

    {
        biginteger a(1);
        biginteger b(a);

        WJR_ASSERT(a.size() == 1);
        WJR_ASSERT(a[0] == 1);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(b.size() == 1);
        WJR_ASSERT(b[0] == 1);
        WJR_ASSERT(!b.is_negate());

        biginteger c(std::move(a));
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 1);
        WJR_ASSERT(!c.is_negate());
    }

    {
        biginteger a(-1);
        biginteger b(a);

        WJR_ASSERT(a.size() == 1);
        WJR_ASSERT(a[0] == 1);
        WJR_ASSERT(a.is_negate());
        WJR_ASSERT(b.size() == 1);
        WJR_ASSERT(b[0] == 1);
        WJR_ASSERT(b.is_negate());

        biginteger c(std::move(a));
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 1);
        WJR_ASSERT(c.is_negate());
    }

    {
        biginteger a(0);
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());

        biginteger b(1);
        WJR_ASSERT(b.size() == 1);
        WJR_ASSERT(b[0] == 1);
        WJR_ASSERT(!b.is_negate());

        biginteger c(-1);
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 1);
        WJR_ASSERT(c.is_negate());
    }
}

TEST(biginteger, assignment) {
    {
        biginteger a;
        a = 0;
        biginteger b;
        b = a;
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(b.size() == 0);
        WJR_ASSERT(!b.is_negate());

        biginteger c;
        c = std::move(a);
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(c.size() == 0);
        WJR_ASSERT(!c.is_negate());
    }

    {
        biginteger a;
        a = 1;
        biginteger b;
        b = a;
        WJR_ASSERT(a.size() == 1);
        WJR_ASSERT(a[0] == 1);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(b.size() == 1);
        WJR_ASSERT(b[0] == 1);
        WJR_ASSERT(!b.is_negate());

        biginteger c;
        c = std::move(a);
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 1);
        WJR_ASSERT(!c.is_negate());
    }

    {
        biginteger a;
        a = -1;
        biginteger b;
        b = a;
        WJR_ASSERT(a.size() == 1);
        WJR_ASSERT(a[0] == 1);
        WJR_ASSERT(a.is_negate());
        WJR_ASSERT(b.size() == 1);
        WJR_ASSERT(b[0] == 1);
        WJR_ASSERT(b.is_negate());

        biginteger c;
        c = std::move(a);
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 1);
        WJR_ASSERT(c.is_negate());
    }
}

TEST(biginteger, add) {
    {
        biginteger a(1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 2);
        WJR_ASSERT(!c.is_negate());
    }

    {
        biginteger a(1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c.size() == 0);
        WJR_ASSERT(!c.is_negate());
    }

    {
        biginteger a(-1);
        biginteger b(1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c.size() == 0);
        WJR_ASSERT(!c.is_negate());
    }

    {
        biginteger a(-1);
        biginteger b(-1);
        biginteger c;

        add(c, a, b);
        WJR_ASSERT(c.size() == 1);
        WJR_ASSERT(c[0] == 2);
        WJR_ASSERT(c.is_negate());
    }
}

TEST(biginteger, convert) {

    {
        biginteger a;

        auto check = [&a](const auto &str, std::string_view expect,
                          unsigned int base = 10) {
            auto get = [&a](unsigned int base) {
                std::string str;
                to_chars_unchecked(std::back_inserter(str), a, base);
                return str;
            };

            auto sp = wjr::span<const char>(str);
            auto sv = std::string_view(str);

            const bool thr = expect.empty();
            {
                auto ret = wjr::from_chars(sp.begin(), sp.end(), a, base);
                if (!thr) {
                    WJR_ASSERT((bool)ret);
                    WJR_ASSERT(get(base) == expect);
                } else {
                    WJR_ASSERT(get(base) == "0");
                }
            }

            if (sp.size() != sv.size()) {
                auto ret = wjr::from_chars(sv.data(), sv.data() + sv.size(), a, base);
                if (!thr) {
                    WJR_ASSERT((bool)ret);
                    WJR_ASSERT(get(base) == expect);
                } else {
                    WJR_ASSERT(get(base) == "0");
                }
            }
        };

        check("0", "0");
        check("1", "1");
        check("-1", "-1");
        check("123456789012345678901234567890", "123456789012345678901234567890");
        check("-123456789012345678901234567890", "-123456789012345678901234567890");
        check("    123456789012345678901234567890    ", "123456789012345678901234567890");
        check("    -123456789012345678901234567890    ",
              "-123456789012345678901234567890");
        check("   0000123456789012345678901234567890   ",
              "123456789012345678901234567890");
        check("   0000123456789012345678901234567890a   ",
              "123456789012345678901234567890");

        check("", "");
        check("a", "");
        check("-", "");
        check("0", "", 0);
        check("2", "", 2);
    }
}