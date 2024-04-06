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
        biginteger a("0");
        WJR_ASSERT(a.size() == 0);
        WJR_ASSERT(!a.is_negate());

        a.assign("1");
        WJR_ASSERT(a.size() == 1);
        WJR_ASSERT(a[0] == 1);
        WJR_ASSERT(!a.is_negate());

        a.assign("-1");
        WJR_ASSERT(a.size() == 1);
        WJR_ASSERT(a[0] == 1);
        WJR_ASSERT(a.is_negate());

        a.assign("123456789012345678901234567890");
        WJR_ASSERT(a.size() == 2);
        // 123456789012345678901234567890 % 2^64, 123456789012345678901234567890 / 2^64
        WJR_ASSERT(a[0] == 14083847773837265618ul);
        WJR_ASSERT(a[1] == 6692605942ul);
        WJR_ASSERT(!a.is_negate());

        a.assign("-123456789012345678901234567890");
        WJR_ASSERT(a.size() == 2);
        WJR_ASSERT(a[0] == 14083847773837265618ul);
        WJR_ASSERT(a[1] == 6692605942ul);
        WJR_ASSERT(a.is_negate());

        a.assign("    123456789012345678901234567890    ");
        WJR_ASSERT(a.size() == 2);
        WJR_ASSERT(a[0] == 14083847773837265618ul);
        WJR_ASSERT(a[1] == 6692605942ul);
        WJR_ASSERT(!a.is_negate());

        a.assign("    -123456789012345678901234567890    ");
        WJR_ASSERT(a.size() == 2);
        WJR_ASSERT(a[0] == 14083847773837265618ul);
        WJR_ASSERT(a[1] == 6692605942ul);
        WJR_ASSERT(a.is_negate());

        a.assign("    -123456789012345678901234567890    ", 0);
        WJR_ASSERT(a.size() == 2);
        WJR_ASSERT(a[0] == 14083847773837265618ul);
        WJR_ASSERT(a[1] == 6692605942ul);
        WJR_ASSERT(a.is_negate());

        WJR_TRY {
            a.assign("");
            WJR_ASSERT(false, "should throw");
        }
        WJR_CATCH(const std::invalid_argument &) {}

        WJR_TRY {
            a.assign("a");
            WJR_ASSERT(false, "should throw");
        }
        WJR_CATCH(const std::invalid_argument &) {}

        WJR_TRY {
            a.assign("-");
            WJR_ASSERT(false, "should throw");
        }
        WJR_CATCH(const std::invalid_argument &) {}

        WJR_TRY {
            a.assign("0", 0);
            WJR_ASSERT(false, "should throw");
        }
        WJR_CATCH(const std::invalid_argument &) {}
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