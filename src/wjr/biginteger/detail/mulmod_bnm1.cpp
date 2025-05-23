/**
 * @file mulmod_bnm1.cpp
 * @author wjr
 * @brief GMP...
 * @version 0.1
 * @date 2025-01-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <wjr/biginteger/detail/mul.hpp>
#include <wjr/biginteger/detail/negate.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {
namespace {
#include "gmp-helper.hpp"

/* Inputs are {ap,rn} and {bp,rn}; output is {rp,rn}, computation is
   mod B^rn - 1, and values are semi-normalised; zero is represented
   as either 0 or B^n - 1.  Needs a scratch of 2rn limbs at tp.
   tp==rp is allowed. */
void mpn_bc_mulmod_bnm1(mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t rn, mp_ptr tp) {
    mp_limb_t cy;

    ASSERT(0 < rn);

    mpn_mul_n(tp, ap, bp, rn);
    cy = mpn_add_n(rp, tp, tp + rn, rn);
    /* If cy == 1, then the value of rp is at most B^rn - 2, so there can
     * be no overflow when adding in the carry. */
    MPN_INCR_U(rp, rn, cy);
}

/* Inputs are {ap,rn+1} and {bp,rn+1}; output is {rp,rn+1}, in
   normalised representation, computation is mod B^rn + 1. Needs
   a scratch area of 2rn limbs at tp; tp == rp is allowed.
   Output is normalised. */
static void mpn_bc_mulmod_bnp1(mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t rn, mp_ptr tp) {
    mp_limb_t cy;
    unsigned k;

    ASSERT(0 < rn);

    if (UNLIKELY(ap[rn] | bp[rn])) {
        if (ap[rn])
            cy = bp[rn] + mpn_neg(rp, bp, rn);
        else /* ap[rn] == 0 */
            cy = mpn_neg(rp, ap, rn);
    } else if (MPN_MULMOD_BKNP1_USABLE(rn, k, MUL_FFT_MODF_THRESHOLD)) {
        mp_size_t n_k = rn / k;
        TMP_DECL;

        TMP_MARK;
        mpn_mulmod_bknp1(rp, ap, bp, n_k, k, TMP_ALLOC_LIMBS(mpn_mulmod_bknp1_itch(rn)));
        TMP_FREE;
        return;
    } else {
        mpn_mul_n(tp, ap, bp, rn);
        cy = mpn_sub_n(rp, tp, tp + rn, rn);
    }
    rp[rn] = 0;
    MPN_INCR_U(rp, rn + 1, cy);
}

} // namespace

/* Computes {rp,MIN(rn,an+bn)} <- {ap,an}*{bp,bn} Mod(B^rn-1)
 *
 * The result is expected to be ZERO if and only if one of the operand
 * already is. Otherwise the class [0] Mod(B^rn-1) is represented by
 * B^rn-1. This should not be a problem if mulmod_bnm1 is used to
 * combine results and obtain a natural number when one knows in
 * advance that the final value is less than (B^rn-1).
 * Moreover it should not be a problem if mulmod_bnm1 is used to
 * compute the full product with an+bn <= rn, because this condition
 * implies (B^an-1)(B^bn-1) < (B^rn-1) .
 *
 * Requires 0 < bn <= an <= rn and an + bn > rn/2
 * Scratch need: rn + (need for recursive call OR rn + 4). This gives
 *
 * S(n) <= rn + MAX (rn + 4, S(n/2)) <= 2rn + 4
 */
void mulmod_bnm1(uint64_t *rp, size_t rn, const uint64_t *ap, size_t an, const uint64_t *bp,
                 size_t bn, uint64_t *tp) noexcept {
    ASSERT(0 < bn);
    ASSERT(bn <= an);
    ASSERT(an <= rn);

    if ((rn & 1) != 0 || BELOW_THRESHOLD(rn, MULMOD_BNM1_THRESHOLD)) {
        if (UNLIKELY(bn < rn)) {
            if (UNLIKELY(an + bn <= rn)) {
                mpn_mul(rp, ap, an, bp, bn);
            } else {
                mp_limb_t cy;
                mpn_mul(tp, ap, an, bp, bn);
                cy = mpn_add(rp, tp, rn, tp + rn, an + bn - rn);
                MPN_INCR_U(rp, rn, cy);
            }
        } else
            mpn_bc_mulmod_bnm1(rp, ap, bp, rn, tp);
    } else {
        mp_size_t n;
        mp_limb_t cy;
        mp_limb_t hi;

        n = rn >> 1;

        /* We need at least an + bn >= n, to be able to fit one of the
       recursive products at rp. Requiring strict inequality makes
       the code slightly simpler. If desired, we could avoid this
       restriction by initially halving rn as long as rn is even and
       an + bn <= rn/2. */

        ASSERT(an + bn > n);

        /* Compute xm = a*b mod (B^n - 1), xp = a*b mod (B^n + 1)
       and crt together as

       x = -xp * B^n + (B^n + 1) * [ (xp + xm)/2 mod (B^n-1)]
        */

#define a0 ap
#define a1 (ap + n)
#define b0 bp
#define b1 (bp + n)

#define xp tp /* 2n + 2 */
              /* am1  maybe in {xp, n} */
              /* bm1  maybe in {xp + n, n} */
#define sp1 (tp + 2 * n + 2)
        /* ap1  maybe in {sp1, n + 1} */
        /* bp1  maybe in {sp1 + n + 1, n + 1} */

        {
            mp_srcptr am1, bm1;
            mp_size_t anm, bnm;
            mp_ptr so;

            bm1 = b0;
            bnm = bn;
            if (LIKELY(an > n)) {
                am1 = xp;
                cy = mpn_add(xp, a0, n, a1, an - n);
                MPN_INCR_U(xp, n, cy);
                anm = n;
                so = xp + n;
                if (LIKELY(bn > n)) {
                    bm1 = so;
                    cy = mpn_add(so, b0, n, b1, bn - n);
                    MPN_INCR_U(so, n, cy);
                    bnm = n;
                    so += n;
                }
            } else {
                so = xp;
                am1 = a0;
                anm = an;
            }

            mulmod_bnm1(rp, n, am1, anm, bm1, bnm, so);
        }

        {
            int k;
            mp_srcptr ap1, bp1;
            mp_size_t anp, bnp;

            bp1 = b0;
            bnp = bn;
            if (LIKELY(an > n)) {
                ap1 = sp1;
                cy = mpn_sub(sp1, a0, n, a1, an - n);
                sp1[n] = 0;
                MPN_INCR_U(sp1, n + 1, cy);
                anp = n + ap1[n];
                if (LIKELY(bn > n)) {
                    bp1 = sp1 + n + 1;
                    cy = mpn_sub(sp1 + n + 1, b0, n, b1, bn - n);
                    sp1[2 * n + 1] = 0;
                    MPN_INCR_U(sp1 + n + 1, n + 1, cy);
                    bnp = n + bp1[n];
                }
            } else {
                ap1 = a0;
                anp = an;
            }

            if (BELOW_THRESHOLD(n, MUL_FFT_MODF_THRESHOLD))
                k = 0;
            else {
                int mask;
                k = fft_best_k(n, 0);
                mask = (1 << k) - 1;
                while (n & mask) {
                    k--;
                    mask >>= 1;
                };
            }
            if (k >= FFT_FIRST_K)
                xp[n] = mpn_mul_fft(xp, n, ap1, anp, bp1, bnp, k);
            else if (UNLIKELY(bp1 == b0)) {
                ASSERT(anp + bnp <= 2 * n + 1);
                ASSERT(anp + bnp > n);
                ASSERT(anp >= bnp);
                mpn_mul(xp, ap1, anp, bp1, bnp);
                anp = anp + bnp - n;
                ASSERT(anp <= n || xp[2 * n] == 0);
                anp -= anp > n;
                cy = mpn_sub(xp, xp, n, xp + n, anp);
                xp[n] = 0;
                MPN_INCR_U(xp, n + 1, cy);
            } else
                mpn_bc_mulmod_bnp1(xp, ap1, bp1, n, xp);
        }

        /* Here the CRT recomposition begins.

       xm <- (xp + xm)/2 = (xp + xm)B^n/2 mod (B^n-1)
       Division by 2 is a bitwise rotation.

       Assumes xp normalised mod (B^n+1).

       The residue class [0] is represented by [B^n-1]; except when
       both input are ZERO.
        */

#if HAVE_NATIVE_mpn_rsh1add_n || HAVE_NATIVE_mpn_rsh1add_nc
    #if HAVE_NATIVE_mpn_rsh1add_nc
        cy = mpn_rsh1add_nc(rp, rp, xp, n, xp[n]); /* B^n = 1 */
        hi = cy << (GMP_NUMB_BITS - 1);
        cy = 0;
            /* next update of rp[n-1] will set cy = 1 only if rp[n-1]+=hi
           overflows, i.e. a further increment will not overflow again. */
    #else /* ! _nc */
        cy = xp[n] + mpn_rsh1add_n(rp, rp, xp, n);        /* B^n = 1 */
        hi = (cy << (GMP_NUMB_BITS - 1)) & GMP_NUMB_MASK; /* (cy&1) << ... */
        cy >>= 1;
            /* cy = 1 only if xp[n] = 1 i.e. {xp,n} = ZERO, this implies that
           the rsh1add was a simple rshift: the top bit is 0. cy=1 => hi=0. */
    #endif
    #if GMP_NAIL_BITS == 0
        add_ssaaaa(cy, rp[n - 1], cy, rp[n - 1], 0, hi);
    #else
        cy += (hi & rp[n - 1]) >> (GMP_NUMB_BITS - 1);
        rp[n - 1] ^= hi;
    #endif
#else /* ! HAVE_NATIVE_mpn_rsh1add_n */
    #if HAVE_NATIVE_mpn_add_nc
        cy = mpn_add_nc(rp, rp, xp, n, xp[n]);
    #else /* ! _nc */
        cy = xp[n] + mpn_add_n(rp, rp, xp, n); /* xp[n] == 1 implies {xp,n} == ZERO */
    #endif
        cy += (rp[0] & 1);
        mpn_rshift(rp, rp, n, 1);
        ASSERT(cy <= 2);
        hi = (cy << (GMP_NUMB_BITS - 1)) & GMP_NUMB_MASK; /* (cy&1) << ... */
        cy >>= 1;
        /* We can have cy != 0 only if hi = 0... */
        ASSERT((rp[n - 1] & GMP_NUMB_HIGHBIT) == 0);
        rp[n - 1] |= hi;
        /* ... rp[n-1] + cy can not overflow, the following INCR is correct. */
#endif
        ASSERT(cy <= 1);
        /* Next increment can not overflow, read the previous comments about cy. */
        ASSERT((cy == 0) || ((rp[n - 1] & GMP_NUMB_HIGHBIT) == 0));
        MPN_INCR_U(rp, n, cy);

        /* Compute the highest half:
       ([(xp + xm)/2 mod (B^n-1)] - xp ) * B^n
         */
        if (UNLIKELY(an + bn < rn)) {
            /* Note that in this case, the only way the result can equal
               zero mod B^{rn} - 1 is if one of the inputs is zero, and
               then the output of both the recursive calls and this CRT
               reconstruction is zero, not B^{rn} - 1. Which is good,
               since the latter representation doesn't fit in the output
               area.*/
            cy = mpn_sub_n(rp + n, rp, xp, an + bn - n);

            /* FIXME: This subtraction of the high parts is not really
               necessary, we do it to get the carry out, and for sanity
               checking. */
            cy = xp[n] + mpn_sub_nc(xp + an + bn - n, rp + an + bn - n, xp + an + bn - n,
                                    rn - (an + bn), cy);
            ASSERT(an + bn == rn - 1 || mpn_zero_p(xp + an + bn - n + 1, rn - 1 - (an + bn)));
            cy = mpn_sub_1(rp, rp, an + bn, cy);
            ASSERT(cy == (xp + an + bn - n)[0]);
        } else {
            cy = xp[n] + mpn_sub_n(rp + n, rp, xp, n);
            /* cy = 1 only if {xp,n+1} is not ZERO, i.e. {rp,n} is not ZERO.
               DECR will affect _at most_ the lowest n limbs. */
            MPN_DECR_U(rp, 2 * n, cy);
        }
#undef a0
#undef a1
#undef b0
#undef b1
#undef xp
#undef sp1
    }
}

} // namespace wjr