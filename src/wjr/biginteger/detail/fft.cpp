/**
 * @file fft.cpp
 * @author wjr
 * @brief GMP...
 * @version 0.1
 * @date 2025-01-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <wjr/biginteger/detail/mul.hpp>
#include <wjr/biginteger/detail/not.hpp>
#include <wjr/math/compare.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {
namespace {

#include "gmp-helper.hpp"

#ifdef WANT_ADDSUB
    #include "generic/add_n_sub_n.c"
    #define HAVE_NATIVE_mpn_add_n_sub_n 1
#endif

static mp_limb_t mpn_mul_fft_internal(mp_ptr, mp_size_t, int, mp_ptr *, mp_ptr *, mp_ptr, mp_size_t,
                                      mp_size_t, mp_size_t, int **, mp_ptr, int);
static void mpn_mul_fft_decompose(mp_ptr, mp_ptr *, mp_size_t, mp_size_t, mp_srcptr, mp_size_t,
                                  mp_size_t, mp_size_t, mp_ptr);

/* Find the best k to use for a mod 2^(m*GMP_NUMB_BITS)+1 FFT for m >= n.
   We have sqr=0 if for a multiply, sqr=1 for a square.
   There are three generations of this code; we keep the old ones as long as
   some gmp-mparam.h is not updated.  */

/*****************************************************************************/

struct fft_table_nk {
    uint_least32_t n : 27;
    uint_least32_t k : 5;
};

#if TUNE_PROGRAM_BUILD || (defined(MUL_FFT_TABLE3) && defined(SQR_FFT_TABLE3))

    #ifndef FFT_TABLE3_SIZE /* When tuning this is defined in gmp-impl.h */
        #if defined(MUL_FFT_TABLE3_SIZE) && defined(SQR_FFT_TABLE3_SIZE)
            #if MUL_FFT_TABLE3_SIZE > SQR_FFT_TABLE3_SIZE
                #define FFT_TABLE3_SIZE MUL_FFT_TABLE3_SIZE
            #else
                #define FFT_TABLE3_SIZE SQR_FFT_TABLE3_SIZE
            #endif
        #endif
    #endif

    #ifndef FFT_TABLE3_SIZE
        #define FFT_TABLE3_SIZE 200
    #endif

FFT_TABLE_ATTRS struct fft_table_nk mpn_fft_table3[2][FFT_TABLE3_SIZE] = {MUL_FFT_TABLE3,
                                                                          SQR_FFT_TABLE3};

} // namespace

int fft_best_k(size_t n, int sqr) noexcept {
    const struct fft_table_nk *fft_tab, *tab;
    mp_size_t tab_n, thres;
    int last_k;

    fft_tab = mpn_fft_table3[sqr];
    last_k = fft_tab->k;
    for (tab = fft_tab + 1;; tab++) {
        tab_n = tab->n;
        thres = tab_n << last_k;
        if (n <= thres)
            break;
        last_k = tab->k;
    }
    return last_k;
}

namespace {

    #define MPN_FFT_BEST_READY 1
#endif

/*****************************************************************************/

#if !defined(MPN_FFT_BEST_READY)
FFT_TABLE_ATTRS mp_size_t mpn_fft_table[2][MPN_FFT_TABLE_SIZE] = {MUL_FFT_TABLE, SQR_FFT_TABLE};

} // namespace

int fft_best_k(mp_size_t n, int sqr) noexcept {
    int i;

    for (i = 0; mpn_fft_table[sqr][i] != 0; i++)
        if (n < mpn_fft_table[sqr][i])
            return i + FFT_FIRST_K;

    /* treat 4*last as one further entry */
    if (i == 0 || n < 4 * mpn_fft_table[sqr][i - 1])
        return i + FFT_FIRST_K;
    else
        return i + FFT_FIRST_K + 1;
}

namespace {

#endif

/*****************************************************************************/

/* Initialize l[i][j] with bitrev(j) */
static void mpn_fft_initl(int **l, int k) {
    int i, j, K;
    int *li;

    l[0][0] = 0;
    for (i = 1, K = 1; i <= k; i++, K *= 2) {
        li = l[i];
        for (j = 0; j < K; j++) {
            li[j] = 2 * l[i - 1][j];
            li[K + j] = 1 + li[j];
        }
    }
}

/* r <- a*2^d mod 2^(n*GMP_NUMB_BITS)+1 with a = {a, n+1}
   Assumes a is semi-normalized, i.e. a[n] <= 1.
   r and a must have n+1 limbs, and not overlap.
*/
static void mpn_fft_mul_2exp_modF(mp_ptr r, mp_srcptr a, mp_bitcnt_t d, mp_size_t n) {
    unsigned int sh;
    mp_size_t m;
    mp_limb_t cc, rd;

    sh = d % GMP_NUMB_BITS;
    m = d / GMP_NUMB_BITS;

    if (m >= n) /* negate */
    {
        /* r[0..m-1]  <-- lshift(a[n-m]..a[n-1], sh)
       r[m..n-1]  <-- -lshift(a[0]..a[n-m-1],  sh) */

        m -= n;
        if (sh != 0) {
            /* no out shift below since a[n] <= 1 */
            (void)mpn_lshift(r, a + n - m, m + 1, sh);
            rd = r[m];
            cc = mpn_lshiftc(r + m, a, n - m, sh);
        } else {
            MPN_COPY(r, a + n - m, m);
            rd = a[n];
            mpn_com(r + m, a, n - m);
            cc = 0;
        }

        /* add cc to r[0], and add rd to r[m] */

        /* now add 1 in r[m], subtract 1 in r[n], i.e. add 1 in r[0] */

        r[n] = 0;
        /* cc < 2^sh <= 2^(GMP_NUMB_BITS-1) thus no overflow here */
        ++cc;
        MPN_INCR_U(r, n + 1, cc);

        ++rd;
        /* rd might overflow when sh=GMP_NUMB_BITS-1 */
        cc = rd + (rd == 0);
        r = r + m + (rd == 0);
        MPN_INCR_U(r, n + 1 - m - (rd == 0), cc);
    } else {
        /* r[0..m-1]  <-- -lshift(a[n-m]..a[n-1], sh)
       r[m..n-1]  <-- lshift(a[0]..a[n-m-1],  sh)  */
        if (sh != 0) {
            /* no out bits below since a[n] <= 1 */
            (void)mpn_lshiftc(r, a + n - m, m + 1, sh);
            rd = ~r[m];
            /* {r, m+1} = {a+n-m, m+1} << sh */
            cc = mpn_lshift(r + m, a, n - m, sh); /* {r+m, n-m} = {a, n-m}<<sh */
        } else {
            /* r[m] is not used below, but we save a test for m=0 */
            mpn_com(r, a + n - m, m + 1);
            rd = a[n];
            MPN_COPY(r + m, a, n - m);
            cc = 0;
        }

        /* now complement {r, m}, subtract cc from r[0], subtract rd from r[m] */

        /* if m=0 we just have r[0]=a[n] << sh */
        if (m != 0) {
            /* now add 1 in r[0], subtract 1 in r[m] */
            if (cc-- == 0) /* then add 1 to r[0] */
                cc = mpn_add_1(r, r, n, CNST_LIMB(1));
            cc = mpn_sub_1(r, r, m, cc) + 1;
            /* add 1 to cc instead of rd since rd might overflow */
        }

        /* now subtract cc and rd from r[m..n] */

        r[n] = 2; /* Add a value, to avoid borrow propagation */
        MPN_DECR_U(r + m, n - m + 1, cc);
        MPN_DECR_U(r + m, n - m + 1, rd);
        /* Remove the added value, and check for a possible borrow. */
        if (UNLIKELY((r[n] -= 2) != 0)) {
            mp_limb_t cy = -r[n];
            /* cy should always be 1, except in the very unlikely case
               m=n-1, r[m]=0, cc+rd>GMP_NUMB_MAX+1. Never triggered.
               Is it actually possible? */
            r[n] = 0;
            MPN_INCR_U(r, n + 1, cy);
        }
    }
}

#if HAVE_NATIVE_mpn_add_n_sub_n
static inline void mpn_fft_add_sub_modF(mp_ptr A0, mp_ptr Ai, mp_srcptr tp, mp_size_t n) {
    mp_limb_t cyas, c, x;

    cyas = mpn_add_n_sub_n(A0, Ai, A0, tp, n);

    c = A0[n] - tp[n] - (cyas & 1);
    x = (-c) & -((c & GMP_LIMB_HIGHBIT) != 0);
    Ai[n] = x + c;
    MPN_INCR_U(Ai, n + 1, x);

    c = A0[n] + tp[n] + (cyas >> 1);
    x = (c - 1) & -(c != 0);
    A0[n] = c - x;
    MPN_DECR_U(A0, n + 1, x);
}

#else /* ! HAVE_NATIVE_mpn_add_n_sub_n  */

/* r <- a+b mod 2^(n*GMP_NUMB_BITS)+1.
   Assumes a and b are semi-normalized.
*/
static inline void mpn_fft_add_modF(mp_ptr r, mp_srcptr a, mp_srcptr b, mp_size_t n) {
    mp_limb_t c, x;

    c = a[n] + b[n] + mpn_add_n(r, a, b, n);
    /* 0 <= c <= 3 */

    #if 1
    /* GCC 4.1 outsmarts most expressions here, and generates a 50% branch.  The
       result is slower code, of course.  But the following outsmarts GCC.  */
    x = (c - 1) & -(c != 0);
    r[n] = c - x;
    MPN_DECR_U(r, n + 1, x);
    #endif
    #if 0
  if (c > 1)
    {
      r[n] = 1;                       /* r[n] - c = 1 */
      MPN_DECR_U (r, n + 1, c - 1);
    }
  else
    {
      r[n] = c;
    }
    #endif
}

/* r <- a-b mod 2^(n*GMP_NUMB_BITS)+1.
   Assumes a and b are semi-normalized.
*/
static inline void mpn_fft_sub_modF(mp_ptr r, mp_srcptr a, mp_srcptr b, mp_size_t n) {
    mp_limb_t c, x;

    c = a[n] - b[n] - mpn_sub_n(r, a, b, n);
    /* -2 <= c <= 1 */

    #if 1
    /* GCC 4.1 outsmarts most expressions here, and generates a 50% branch.  The
       result is slower code, of course.  But the following outsmarts GCC.  */
    x = (-c) & -((c & GMP_LIMB_HIGHBIT) != 0);
    r[n] = x + c;
    MPN_INCR_U(r, n + 1, x);
    #endif
    #if 0
  if ((c & GMP_LIMB_HIGHBIT) != 0)
    {
      r[n] = 0;
      MPN_INCR_U (r, n + 1, -c);
    }
  else
    {
      r[n] = c;
    }
    #endif
}
#endif /* HAVE_NATIVE_mpn_add_n_sub_n */

/* input: A[0] ... A[inc*(K-1)] are residues mod 2^N+1 where
      N=n*GMP_NUMB_BITS, and 2^omega is a primitive root mod 2^N+1
   output: A[inc*l[k][i]] <- \sum (2^omega)^(ij) A[inc*j] mod 2^N+1 */

static void mpn_fft_fft(mp_ptr *Ap, mp_size_t K, int **ll, mp_size_t omega, mp_size_t n,
                        mp_size_t inc, mp_ptr tp) {
    if (K == 2) {
        mp_limb_t cy;
#if HAVE_NATIVE_mpn_add_n_sub_n
        cy = mpn_add_n_sub_n(Ap[0], Ap[inc], Ap[0], Ap[inc], n + 1) & 1;
#else
        MPN_COPY(tp, Ap[0], n + 1);
        (void)mpn_add_n(Ap[0], Ap[0], Ap[inc], n + 1);
        cy = mpn_sub_n(Ap[inc], tp, Ap[inc], n + 1);
#endif
        if (Ap[0][n] > 1) /* can be 2 or 3 */
        {                 /* Ap[0][n] = 1 - mpn_sub_1 (Ap[0], Ap[0], n, Ap[0][n] - 1); */
            mp_limb_t cc = Ap[0][n] - 1;
            Ap[0][n] = 1;
            MPN_DECR_U(Ap[0], n + 1, cc);
        }
        if (cy) /* Ap[inc][n] can be -1 or -2 */
        {       /* Ap[inc][n] = mpn_add_1 (Ap[inc], Ap[inc], n, ~Ap[inc][n] + 1); */
            mp_limb_t cc = ~Ap[inc][n] + 1;
            Ap[inc][n] = 0;
            MPN_INCR_U(Ap[inc], n + 1, cc);
        }
    } else {
        mp_size_t j, K2 = K >> 1;
        int *lk = *ll;

        mpn_fft_fft(Ap, K2, ll - 1, 2 * omega, n, inc * 2, tp);
        mpn_fft_fft(Ap + inc, K2, ll - 1, 2 * omega, n, inc * 2, tp);
        /* A[2*j*inc]   <- A[2*j*inc] + omega^l[k][2*j*inc] A[(2j+1)inc]
       A[(2j+1)inc] <- A[2*j*inc] + omega^l[k][(2j+1)inc] A[(2j+1)inc] */
        for (j = 0; j < K2; j++, lk += 2, Ap += 2 * inc) {
            /* Ap[inc] <- Ap[0] + Ap[inc] * 2^(lk[1] * omega)
               Ap[0]   <- Ap[0] + Ap[inc] * 2^(lk[0] * omega) */
            mpn_fft_mul_2exp_modF(tp, Ap[inc], lk[0] * omega, n);
#if HAVE_NATIVE_mpn_add_n_sub_n
            mpn_fft_add_sub_modF(Ap[0], Ap[inc], tp, n);
#else
            mpn_fft_sub_modF(Ap[inc], Ap[0], tp, n);
            mpn_fft_add_modF(Ap[0], Ap[0], tp, n);
#endif
        }
    }
}

/* input: A[0] ... A[inc*(K-1)] are residues mod 2^N+1 where
      N=n*GMP_NUMB_BITS, and 2^omega is a primitive root mod 2^N+1
   output: A[inc*l[k][i]] <- \sum (2^omega)^(ij) A[inc*j] mod 2^N+1
   tp must have space for 2*(n+1) limbs.
*/

/* Given ap[0..n] with ap[n]<=1, reduce it modulo 2^(n*GMP_NUMB_BITS)+1,
   by subtracting that modulus if necessary.

   If ap[0..n] is exactly 2^(n*GMP_NUMB_BITS) then mpn_sub_1 produces a
   borrow and the limbs must be zeroed out again.  This will occur very
   infrequently.  */

static inline void mpn_fft_normalize(mp_ptr ap, mp_size_t n) {
    if (ap[n] != 0) {
        MPN_DECR_U(ap, n + 1, CNST_LIMB(1));
        if (ap[n] == 0) {
            /* This happens with very low probability; we have yet to trigger it,
               and thereby make sure this code is correct.  */
            MPN_ZERO(ap, n);
            ap[n] = 1;
        } else
            ap[n] = 0;
    }
}

/* a[i] <- a[i]*b[i] mod 2^(n*GMP_NUMB_BITS)+1 for 0 <= i < K */
static void mpn_fft_mul_modF_K(mp_ptr *ap, mp_ptr *bp, mp_size_t n, mp_size_t K) {
    unsigned i;
    unsigned k;
    int sqr = (ap == bp);
    TMP_DECL;

    TMP_MARK;

    if (n >= (sqr ? SQR_FFT_MODF_THRESHOLD : MUL_FFT_MODF_THRESHOLD)) {
        mp_size_t K2, nprime2, Nprime2, M2, maxLK, l, Mp2;
        int k;
        int **fft_l, *tmp;
        mp_ptr *Ap, *Bp, A, B, T;

        k = fft_best_k(n, sqr);
        K2 = (mp_size_t)1 << k;
        ASSERT_ALWAYS((n & (K2 - 1)) == 0);
        maxLK = (K2 > GMP_NUMB_BITS) ? K2 : GMP_NUMB_BITS;
        M2 = n * GMP_NUMB_BITS >> k;
        l = n >> k;
        Nprime2 = ((2 * M2 + k + 2 + maxLK) / maxLK) * maxLK;
        /* Nprime2 = ceil((2*M2+k+3)/maxLK)*maxLK*/
        nprime2 = Nprime2 / GMP_NUMB_BITS;

        /* we should ensure that nprime2 is a multiple of the next K */
        if (nprime2 >= (sqr ? SQR_FFT_MODF_THRESHOLD : MUL_FFT_MODF_THRESHOLD)) {
            mp_size_t K3;
            for (;;) {
                K3 = (mp_size_t)1 << fft_best_k(nprime2, sqr);
                if ((nprime2 & (K3 - 1)) == 0)
                    break;
                nprime2 = (nprime2 + K3 - 1) & -K3;
                Nprime2 = nprime2 * GMP_LIMB_BITS;
                /* warning: since nprime2 changed, K3 may change too! */
            }
        }
        ASSERT_ALWAYS(nprime2 < n); /* otherwise we'll loop */

        Mp2 = Nprime2 >> k;

        Ap = TMP_BALLOC_MP_PTRS(K2);
        Bp = TMP_BALLOC_MP_PTRS(K2);
        A = TMP_BALLOC_LIMBS(2 * (nprime2 + 1) << k);
        T = TMP_BALLOC_LIMBS(2 * (nprime2 + 1));
        B = A + ((nprime2 + 1) << k);
        fft_l = TMP_BALLOC_TYPE(k + 1, int *);
        tmp = TMP_BALLOC_TYPE((size_t)2 << k, int);
        for (i = 0; i <= (unsigned)k; i++) {
            fft_l[i] = tmp;
            tmp += (mp_size_t)1 << i;
        }

        mpn_fft_initl(fft_l, k);

        TRACE(printf("recurse: %ldx%ld limbs -> %ld times %ldx%ld (%1.2f)\n", n, n, K2, nprime2,
                     nprime2, 2.0 * (double)n / nprime2 / K2));
        for (i = 0; i < K; i++, ap++, bp++) {
            mp_limb_t cy;
            mpn_fft_normalize(*ap, n);
            if (!sqr)
                mpn_fft_normalize(*bp, n);

            mpn_mul_fft_decompose(A, Ap, K2, nprime2, *ap, (l << k) + 1, l, Mp2, T);
            if (!sqr)
                mpn_mul_fft_decompose(B, Bp, K2, nprime2, *bp, (l << k) + 1, l, Mp2, T);

            cy = mpn_mul_fft_internal(*ap, n, k, Ap, Bp, B, nprime2, l, Mp2, fft_l, T, sqr);
            (*ap)[n] = cy;
        }
    }
#if !TUNE_PROGRAM_BUILD
    else if (MPN_MULMOD_BKNP1_USABLE(n, k, MUL_FFT_MODF_THRESHOLD)) {
        mp_ptr a;
        mp_size_t n_k = n / k;

        if (sqr) {
            mp_ptr tp = TMP_SALLOC_LIMBS(mpn_sqrmod_bknp1_itch(n));
            for (i = 0; i < K; i++) {
                a = *ap++;
                mpn_sqrmod_bknp1(a, a, n_k, k, tp);
            }
        } else {
            mp_ptr b, tp = TMP_SALLOC_LIMBS(mpn_mulmod_bknp1_itch(n));
            for (i = 0; i < K; i++) {
                a = *ap++;
                b = *bp++;
                mpn_mulmod_bknp1(a, a, b, n_k, k, tp);
            }
        }
    }
#endif
    else {
        mp_ptr a, b, tp, tpn;
        mp_limb_t cc;
        mp_size_t n2 = 2 * n;
        tp = TMP_BALLOC_LIMBS(n2);
        tpn = tp + n;
        TRACE(printf("  mpn_mul_n %ld of %ld limbs\n", K, n));
        for (i = 0; i < K; i++) {
            a = *ap++;
            b = *bp++;
            if (sqr)
                mpn_sqr(tp, a, n);
            else
                mpn_mul_n(tp, b, a, n);
            if (a[n] != 0)
                cc = mpn_add_n(tpn, tpn, b, n);
            else
                cc = 0;
            if (b[n] != 0)
                cc += mpn_add_n(tpn, tpn, a, n) + a[n];
            if (cc != 0) {
                cc = mpn_add_1(tp, tp, n2, cc);
                /* If mpn_add_1 give a carry (cc != 0),
               the result (tp) is at most GMP_NUMB_MAX - 1,
               so the following addition can't overflow.
                */
                tp[0] += cc;
            }
            cc = mpn_sub_n(a, tp, tpn, n);
            a[n] = 0;
            MPN_INCR_U(a, n + 1, cc);
        }
    }
    TMP_FREE;
}

/* input: A^[l[k][0]] A^[l[k][1]] ... A^[l[k][K-1]]
   output: K*A[0] K*A[K-1] ... K*A[1].
   Assumes the Ap[] are pseudo-normalized, i.e. 0 <= Ap[][n] <= 1.
   This condition is also fulfilled at exit.
*/
static void mpn_fft_fftinv(mp_ptr *Ap, mp_size_t K, mp_size_t omega, mp_size_t n, mp_ptr tp) {
    if (K == 2) {
        mp_limb_t cy;
#if HAVE_NATIVE_mpn_add_n_sub_n
        cy = mpn_add_n_sub_n(Ap[0], Ap[1], Ap[0], Ap[1], n + 1) & 1;
#else
        MPN_COPY(tp, Ap[0], n + 1);
        (void)mpn_add_n(Ap[0], Ap[0], Ap[1], n + 1);
        cy = mpn_sub_n(Ap[1], tp, Ap[1], n + 1);
#endif
        if (Ap[0][n] > 1) /* can be 2 or 3 */
        {                 /* Ap[0][n] = 1 - mpn_sub_1 (Ap[0], Ap[0], n, Ap[0][n] - 1); */
            mp_limb_t cc = Ap[0][n] - 1;
            Ap[0][n] = 1;
            MPN_DECR_U(Ap[0], n + 1, cc);
        }
        if (cy) /* Ap[1][n] can be -1 or -2 */
        {       /* Ap[1][n] = mpn_add_1 (Ap[1], Ap[1], n, ~Ap[1][n] + 1); */
            mp_limb_t cc = ~Ap[1][n] + 1;
            Ap[1][n] = 0;
            MPN_INCR_U(Ap[1], n + 1, cc);
        }
    } else {
        mp_size_t j, K2 = K >> 1;

        mpn_fft_fftinv(Ap, K2, 2 * omega, n, tp);
        mpn_fft_fftinv(Ap + K2, K2, 2 * omega, n, tp);
        /* A[j]     <- A[j] + omega^j A[j+K/2]
       A[j+K/2] <- A[j] + omega^(j+K/2) A[j+K/2] */
        for (j = 0; j < K2; j++, Ap++) {
            /* Ap[K2] <- Ap[0] + Ap[K2] * 2^((j + K2) * omega)
               Ap[0]  <- Ap[0] + Ap[K2] * 2^(j * omega) */
            mpn_fft_mul_2exp_modF(tp, Ap[K2], j * omega, n);
#if HAVE_NATIVE_mpn_add_n_sub_n
            mpn_fft_add_sub_modF(Ap[0], Ap[K2], tp, n);
#else
            mpn_fft_sub_modF(Ap[K2], Ap[0], tp, n);
            mpn_fft_add_modF(Ap[0], Ap[0], tp, n);
#endif
        }
    }
}

/* R <- A/2^k mod 2^(n*GMP_NUMB_BITS)+1 */
static void mpn_fft_div_2exp_modF(mp_ptr r, mp_srcptr a, mp_bitcnt_t k, mp_size_t n) {
    mp_bitcnt_t i;

    ASSERT(r != a);
    i = (mp_bitcnt_t)2 * n * GMP_NUMB_BITS - k;
    mpn_fft_mul_2exp_modF(r, a, i, n);
    /* 1/2^k = 2^(2nL-k) mod 2^(n*GMP_NUMB_BITS)+1 */
    /* normalize so that R < 2^(n*GMP_NUMB_BITS)+1 */
    mpn_fft_normalize(r, n);
}

/* {rp,n} <- {ap,an} mod 2^(n*GMP_NUMB_BITS)+1, n <= an <= 3*n.
   Returns carry out, i.e. 1 iff {ap,an} = -1 mod 2^(n*GMP_NUMB_BITS)+1,
   then {rp,n}=0.
*/
static mp_size_t mpn_fft_norm_modF(mp_ptr rp, mp_size_t n, mp_ptr ap, mp_size_t an) {
    mp_size_t l, m, rpn;
    mp_limb_t cc;

    ASSERT((n <= an) && (an <= 3 * n));
    m = an - 2 * n;
    if ((ssize_t)m > 0) {
        l = n;
        /* add {ap, m} and {ap+2n, m} in {rp, m} */
        cc = mpn_add_n(rp, ap, ap + 2 * n, m);
        /* copy {ap+m, n-m} to {rp+m, n-m} */
        rpn = mpn_add_1(rp + m, ap + m, n - m, cc);
    } else {
        l = an - n; /* l <= n */
        MPN_COPY(rp, ap, n);
        rpn = 0;
    }

    /* remains to subtract {ap+n, l} from {rp, n+1} */
    rpn -= mpn_sub(rp, rp, n, ap + n, l);
    if ((ssize_t)rpn < 0) /* necessarily rpn = -1 */
        rpn = mpn_add_1(rp, rp, n, CNST_LIMB(1));
    return rpn;
}

/* store in A[0..nprime] the first M bits from {n, nl},
   in A[nprime+1..] the following M bits, ...
   Assumes M is a multiple of GMP_NUMB_BITS (M = l * GMP_NUMB_BITS).
   T must have space for at least (nprime + 1) limbs.
   We must have nl <= 2*K*l.
*/
static void mpn_mul_fft_decompose(mp_ptr A, mp_ptr *Ap, mp_size_t K, mp_size_t nprime, mp_srcptr n,
                                  mp_size_t nl, mp_size_t l, mp_size_t Mp, mp_ptr T) {
    mp_size_t i, j;
    mp_ptr tmp;
    mp_size_t Kl = K * l;
    TMP_DECL;
    TMP_MARK;

    if (nl > Kl) /* normalize {n, nl} mod 2^(Kl*GMP_NUMB_BITS)+1 */
    {
        mp_size_t dif = nl - Kl;

        tmp = TMP_BALLOC_LIMBS(Kl + 1);
        tmp[Kl] = 0;

        ASSERT_ALWAYS(dif <= Kl);
        {
            mp_limb_t cy;
            cy = mpn_sub(tmp, n, Kl, n + Kl, dif);
            MPN_INCR_U(tmp, Kl + 1, cy);
        }
        nl = Kl + 1;
        n = tmp;
    }
    for (i = 0; i < K; i++) {
        Ap[i] = A;
        /* store the next M bits of n into A[0..nprime] */
        if ((ssize_t)nl > 0) /* nl is the number of remaining limbs */
        {
            j = (l <= nl && i < K - 1) ? l : nl; /* store j next limbs */
            nl -= j;
            MPN_COPY(T, n, j);
            MPN_ZERO(T + j, nprime + 1 - j);
            n += l;
            mpn_fft_mul_2exp_modF(A, T, i * Mp, nprime);
        } else
            MPN_ZERO(A, nprime + 1);
        A += nprime + 1;
    }
    ASSERT_ALWAYS(nl == 0);
    TMP_FREE;
}

/* op <- n*m mod 2^N+1 with fft of size 2^k where N=pl*GMP_NUMB_BITS
   op is pl limbs, its high bit is returned.
   One must have pl = fft_next_size (pl, k).
   T must have space for 2 * (nprime + 1) limbs.
*/

static mp_limb_t mpn_mul_fft_internal(mp_ptr op, mp_size_t pl, int k, mp_ptr *Ap, mp_ptr *Bp,
                                      mp_ptr B, mp_size_t nprime, mp_size_t l, mp_size_t Mp,
                                      int **fft_l, mp_ptr T, int sqr) {
    mp_size_t K, i, pla, lo, sh, j;
    mp_ptr p;
    mp_limb_t cc;

    K = (mp_size_t)1 << k;

    /* direct fft's */
    mpn_fft_fft(Ap, K, fft_l + k, 2 * Mp, nprime, 1, T);
    if (!sqr)
        mpn_fft_fft(Bp, K, fft_l + k, 2 * Mp, nprime, 1, T);

    /* term to term multiplications */
    mpn_fft_mul_modF_K(Ap, sqr ? Ap : Bp, nprime, K);

    /* inverse fft's */
    mpn_fft_fftinv(Ap, K, 2 * Mp, nprime, T);

    /* division of terms after inverse fft */
    Bp[0] = T + nprime + 1;
    mpn_fft_div_2exp_modF(Bp[0], Ap[0], k, nprime);
    for (i = 1; i < K; i++) {
        Bp[i] = Ap[i - 1];
        mpn_fft_div_2exp_modF(Bp[i], Ap[i], k + (K - i) * Mp, nprime);
    }

    /* addition of terms in result p */
    MPN_ZERO(T, nprime + 1);
    pla = l * (K - 1) + nprime + 1; /* number of required limbs for p */
    p = B;                          /* B has K*(n' + 1) limbs, which is >= pla, i.e. enough */
    MPN_ZERO(p, pla);
    cc = 0; /* will accumulate the (signed) carry at p[pla] */
    for (i = K - 1, lo = l * i + nprime, sh = l * i; (ssize_t)i >= 0; i--, lo -= l, sh -= l) {
        mp_ptr n = p + sh;

        j = (K - i) & (K - 1);

        cc += mpn_add(n, n, pla - sh, Bp[j], nprime + 1);
        T[2 * l] = i + 1;                        /* T = (i + 1)*2^(2*M) */
        if (mpn_cmp(Bp[j], T, nprime + 1) > 0) { /* subtract 2^N'+1 */
            cc -= mpn_sub_1(n, n, pla - sh, CNST_LIMB(1));
            cc -= mpn_sub_1(p + lo, p + lo, pla - lo, CNST_LIMB(1));
        }
    }
    if (cc == -CNST_LIMB(1)) {
        if ((cc = mpn_add_1(p + pla - pl, p + pla - pl, pl, CNST_LIMB(1)))) {
            /* p[pla-pl]...p[pla-1] are all zero */
            (void)mpn_sub_1(p + pla - pl - 1, p + pla - pl - 1, pl + 1, CNST_LIMB(1));
            (void)mpn_sub_1(p + pla - 1, p + pla - 1, 1, CNST_LIMB(1));
        }
    } else if (cc == 1) {
        if (pla >= 2 * pl) {
            while ((cc = mpn_add_1(p + pla - 2 * pl, p + pla - 2 * pl, 2 * pl, cc)))
                ;
        } else {
            MPN_DECR_U(p + pla - pl, pl, cc);
        }
    } else
        ASSERT(cc == 0);

    /* here p < 2^(2M) [K 2^(M(K-1)) + (K-1) 2^(M(K-2)) + ... ]
       < K 2^(2M) [2^(M(K-1)) + 2^(M(K-2)) + ... ]
       < K 2^(2M) 2^(M(K-1))*2 = 2^(M*K+M+k+1) */
    return mpn_fft_norm_modF(op, pl, p, pla);
}

/* return the lcm of a and 2^k */
static mp_bitcnt_t mpn_mul_fft_lcm(mp_bitcnt_t a, int k) {
    mp_bitcnt_t l = k;

    while (a % 2 == 0 && k > 0) {
        a >>= 1;
        k--;
    }
    return a << l;
}

} // namespace

uint64_t mul_fft(uint64_t *op, size_t pl, const uint64_t *n, size_t nl, const uint64_t *m,
                 size_t ml, int k) noexcept {
    int i;
    mp_size_t K, maxLK;
    mp_size_t N, Nprime, nprime, M, Mp, l;
    mp_ptr *Ap, *Bp, A, T, B;
    int **fft_l, *tmp;
    int sqr = (n == m && nl == ml);
    mp_limb_t h;
    TMP_DECL;

    TRACE(printf("\nmpn_mul_fft pl=%ld nl=%ld ml=%ld k=%d\n", pl, nl, ml, k));
    ASSERT_ALWAYS(fft_next_size(pl, k) == pl);

    TMP_MARK;
    N = pl * GMP_NUMB_BITS;
    fft_l = TMP_BALLOC_TYPE(k + 1, int *);
    tmp = TMP_BALLOC_TYPE((size_t)2 << k, int);
    for (i = 0; i <= k; i++) {
        fft_l[i] = tmp;
        tmp += (mp_size_t)1 << i;
    }

    mpn_fft_initl(fft_l, k);
    K = (mp_size_t)1 << k;
    M = N >> k; /* N = 2^k M */
    l = 1 + (M - 1) / GMP_NUMB_BITS;
    maxLK = mpn_mul_fft_lcm(GMP_NUMB_BITS, k); /* lcm (GMP_NUMB_BITS, 2^k) */

    Nprime = (1 + (2 * M + k + 2) / maxLK) * maxLK;
    /* Nprime = ceil((2*M+k+3)/maxLK)*maxLK; */
    nprime = Nprime / GMP_NUMB_BITS;
    TRACE(printf("N=%ld K=%ld, M=%ld, l=%ld, maxLK=%ld, Np=%ld, np=%ld\n", N, K, M, l, maxLK,
                 Nprime, nprime));
    /* we should ensure that recursively, nprime is a multiple of the next K */
    if (nprime >= (sqr ? SQR_FFT_MODF_THRESHOLD : MUL_FFT_MODF_THRESHOLD)) {
        mp_size_t K2;
        for (;;) {
            K2 = (mp_size_t)1 << fft_best_k(nprime, sqr);
            if ((nprime & (K2 - 1)) == 0)
                break;
            nprime = (nprime + K2 - 1) & -K2;
            Nprime = nprime * GMP_LIMB_BITS;
            /* warning: since nprime changed, K2 may change too! */
        }
        TRACE(printf("new maxLK=%ld, Np=%ld, np=%ld\n", maxLK, Nprime, nprime));
    }
    ASSERT_ALWAYS(nprime < pl); /* otherwise we'll loop */

    T = TMP_BALLOC_LIMBS(2 * (nprime + 1));
    Mp = Nprime >> k;

    TRACE(printf("%ldx%ld limbs -> %ld times %ldx%ld limbs (%1.2f)\n", pl, pl, K, nprime, nprime,
                 2.0 * (double)N / Nprime / K);
          printf("   temp space %ld\n", 2 * K * (nprime + 1)));

    A = TMP_BALLOC_LIMBS(K * (nprime + 1));
    Ap = TMP_BALLOC_MP_PTRS(K);
    Bp = TMP_BALLOC_MP_PTRS(K);
    mpn_mul_fft_decompose(A, Ap, K, nprime, n, nl, l, Mp, T);
    if (sqr) {
        mp_size_t pla;
        pla = l * (K - 1) + nprime + 1; /* number of required limbs for p */
        B = TMP_BALLOC_LIMBS(pla);
    } else {
        B = TMP_BALLOC_LIMBS(K * (nprime + 1));
        mpn_mul_fft_decompose(B, Bp, K, nprime, m, ml, l, Mp, T);
    }
    h = mpn_mul_fft_internal(op, pl, k, Ap, Bp, B, nprime, l, Mp, fft_l, T, sqr);

    TMP_FREE;
    return h;
}

} // namespace wjr