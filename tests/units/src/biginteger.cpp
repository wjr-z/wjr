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

    {
        biginteger a;

        auto check = [&a](const auto &str, bool negate, bool thr,
                          std::initializer_list<uint64_t> list, unsigned int base = 10) {
            WJR_TRY {
                a.assign(str, base);
                WJR_ASSERT(!thr);
                WJR_ASSERT(a.is_negate() == negate);
                WJR_ASSERT(std::equal(a.begin(), a.end(), list.begin(), list.end()));
            }
            WJR_CATCH(const std::invalid_argument &) { WJR_ASSERT(thr); }

            WJR_TRY {
                a.assign(std::string_view(str), base);
                WJR_ASSERT(!thr);
                WJR_ASSERT(a.is_negate() == negate);
                WJR_ASSERT(std::equal(a.begin(), a.end(), list.begin(), list.end()));
            }
            WJR_CATCH(const std::invalid_argument &) { WJR_ASSERT(thr); }
        };

        check("0", false, false, {});
        check("1", false, false, {1});
        check("-1", true, false, {1});
        check("123456789012345678901234567890", false, false,
              {14083847773837265618ul, 6692605942ul});
        check("-123456789012345678901234567890", true, false,
              {14083847773837265618ul, 6692605942ul});
        check("    123456789012345678901234567890    ", false, false,
              {14083847773837265618ul, 6692605942ul});
        check("    -123456789012345678901234567890    ", true, false,
              {14083847773837265618ul, 6692605942ul});
        check("   0000123456789012345678901234567890   ", false, false,
              {14083847773837265618ul, 6692605942ul});
        check("   0000123456789012345678901234567890a   ", false, false,
              {14083847773837265618ul, 6692605942ul});

        check("", false, true, {});
        check("a", false, true, {});
        check("-", false, true, {});
        check("0", false, true, {}, 0);
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