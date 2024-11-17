#include <wjr/assert.hpp>
#include <wjr/biginteger/detail/mul.hpp>
#include <wjr/biginteger/detail/pow.hpp>

namespace wjr {
size_t pow_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t exp, uint64_t *tp) noexcept {
    uint64_t x;
    int cnt, i;
    size_t dn;
    int par;

    WJR_ASSERT(n >= 1);
    /* FIXME: Add operand overlap criteria */

    if (exp <= 1) {
        if (exp == 0) {
            dst[0] = 1;
            return 1;
        } else {
            std::copy_n(src, n, dst);
            return n;
        }
    }

    /* Count number of bits in exp, and compute where to put initial square in
       order to magically get results in the entry dst.  Use simple code,
       optimized for small exp.  For large exp, the bignum operations will take
       so much time that the slowness of this code will be negligible.  */
    par = 0;
    cnt = 64;
    x = exp;
    do {
        par ^= x;
        cnt--;
        x >>= 1;
    } while (x != 0);
    exp <<= cnt;

    if (n == 1) {
        uint64_t rl, rh, bl = src[0];

        if ((cnt & 1) != 0)
            std::swap(dst, tp);

        rl = mul<uint64_t>(bl, bl, rh);
        dst[0] = rl;
        dst[1] = rh;
        dn = 1 + (rh != 0);

        for (i = 64 - cnt - 1;;) {
            exp <<= 1;
            if (__has_high_bit(exp)) {
                dst[dn] = rh = mul_1(dst, dst, dn, bl);
                dn += rh != 0;
            }

            if (--i == 0)
                break;

            sqr(tp, dst, dn);
            dn = 2 * dn;
            dn -= tp[dn - 1] == 0;
            std::swap(dst, tp);
        }
    } else {
        if (((par ^ cnt) & 1) == 0)
            std::swap(dst, tp);

        sqr(dst, src, n);
        dn = 2 * n;
        dn -= dst[dn - 1] == 0;

        for (i = 64 - cnt - 1;;) {
            exp <<= 1;
            if (__has_high_bit(exp)) {
                mul_s(tp, dst, dn, src, n);
                dn = dn + n - (tp[dn + n - 1] == 0);
                std::swap(dst, tp);
            }

            if (--i == 0)
                break;

            sqr(tp, dst, dn);
            dn = 2 * dn;
            dn -= tp[dn - 1] == 0;
            std::swap(dst, tp);
        }
    }

    return dn;
}
} // namespace wjr