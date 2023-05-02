#include <wjr/asm/asm.h>

_WJR_ASM_BEGIN

uint64_t __large__u128d64t64(uint64_t hi, uint64_t lo, uint64_t den, uint64_t& r) {
    constexpr uint64_t b = ((uint64_t)1 << 32);

    // Check for overflow and divide by 0.
    if (hi >= den) {
        r = ~0ull;
        return ~0ull;
    }

    int shift = masm::clz(den);

    den <<= shift;
    hi <<= shift;
    hi |= (lo >> (-shift & 63)) & (-(int64_t)shift >> 63);
    lo <<= shift;

    // Extract the low digits of the numerator and both digits of the denominator.
    uint32_t num1 = (uint32_t)(lo >> 32);
    uint32_t num0 = (uint32_t)(lo & 0xFFFFFFFFu);
    uint32_t den1 = (uint32_t)(den >> 32);
    uint32_t den0 = (uint32_t)(den & 0xFFFFFFFFu);

    // We wish to compute q1 = [n3 n2 n1] / [d1 d0].
    // Estimate q1 as [n3 n2] / [d1], and then correct it.
    // Note while qhat may be 2 digits, q1 is always 1 digit.
    uint64_t qhat = hi / den1;
    uint64_t rhat = hi % den1;
    uint64_t c1 = qhat * den0;
    uint64_t c2 = rhat * b + num1;
    if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
    uint32_t q1 = (uint32_t)qhat;

    // Compute the true (partial) remainder.
    uint64_t rem = hi * b + num1 - q1 * den;

    // We wish to compute q0 = [rem1 rem0 n0] / [d1 d0].
    // Estimate q0 as [rem1 rem0] / [d1] and correct it.
    qhat = rem / den1;
    rhat = rem % den1;
    c1 = qhat * den0;
    c2 = rhat * b + num0;
    if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
    uint32_t q0 = (uint32_t)qhat;

    // Return remainder if requested.
    r = (rem * b + num0 - q0 * den) >> shift;
    return ((uint64_t)q1 << 32) | q0;
}

_WJR_ASM_END