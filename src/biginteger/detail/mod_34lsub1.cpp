/**
 * @file mod_34lsub1.cpp
 * @author wjr
 * @brief GMP...
 * @version 0.1
 * @date 2025-01-19
 *
 * @todo Assembly optimization.
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <wjr/biginteger/detail/mul.hpp>

namespace wjr {
#define B1 16
#define B2 32
#define B3 48

#define M1 ((UINT64_C(1) << B1) - 1)
#define M2 ((UINT64_C(1) << B2) - 1)
#define M3 ((UINT64_C(1) << B3) - 1)

#define LOW0(n) ((n) & M3)
#define HIGH0(n) ((n) >> B3)

#define LOW1(n) (((n) & M2) << B1)
#define HIGH1(n) ((n) >> B2)

#define LOW2(n) (((n) & M1) << B2)
#define HIGH2(n) ((n) >> B1)

#define PARTS0(n) (LOW0(n) + HIGH0(n))
#define PARTS1(n) (LOW1(n) + HIGH1(n))
#define PARTS2(n) (LOW2(n) + HIGH2(n))

#define ADD(c, a, val)                                                                             \
    do {                                                                                           \
        (c) += add_overflow(a, val, a);                                                            \
    } while (0)

uint64_t mod_34lsub1(const uint64_t *p, size_t n) noexcept {
    uint64_t c0, c1, c2;
    uint64_t a0, a1, a2;

    WJR_ASSERT(n >= 1);
    WJR_ASSERT(n / 3 < UINT64_MAX);

    a0 = a1 = a2 = 0;
    c0 = c1 = c2 = 0;

    while ((ssize_t)(n -= 3) >= 0) {
        ADD(c0, a0, p[0]);
        ADD(c1, a1, p[1]);
        ADD(c2, a2, p[2]);
        p += 3;
    }

    if (n != -3ull) {
        ADD(c0, a0, p[0]);
        if (n != -2ull)
            ADD(c1, a1, p[1]);
    }

    return PARTS0(a0) + PARTS1(a1) + PARTS2(a2) + PARTS1(c0) + PARTS2(c1) + PARTS0(c2);
}

} // namespace wjr