using mp_ptr = uint64_t *;
using mp_size_t = size_t;
using mp_bitcnt_t = size_t;
using mp_srcptr = const uint64_t *;
using mp_limb_t = uint64_t;

#define GMP_NUMB_BITS 64
#define GMP_LIMB_BITS 64
#define GMP_NUMB_MAX UINT64_MAX
#define GMP_NUMB_MASK UINT64_MAX

#define MPN_COPY(d, s, n) std::copy_n(s, n, d)
#define MPN_INCR_U(d, n, c) (void)addc_1(d, d, n, c)
#define MPN_DECR_U(d, n, c) (void)subc_1(d, d, n, c)
#define MPN_LIMB_HIGHBIT (UINT64_C(1) << 63)
#define GMP_NUMB_HIGHBIT MPN_LIMB_HIGHBIT
#define MPN_ZERO(d, n) std::fill_n(d, n, 0)
#define MPN_FILL(d, n, c) std::fill_n(d, n, c)

#define TMP_DECL unique_stack_allocator stkal
#define TMP_MARK
#define TMP_FREE

#define TMP_BALLOC_TYPE(n, type) stkal.template allocate<type>(n)
#define TMP_BALLOC_LIMBS(n) TMP_BALLOC_TYPE(n, uint64_t)
#define TMP_BALLOC_MP_PTRS(n) TMP_BALLOC_TYPE(n, uint64_t *)
#define TMP_SALLOC_LIMBS(n) TMP_BALLOC_LIMBS(n)
#define TMP_ALLOC_LIMBS(n) TMP_BALLOC_LIMBS(n)

#define BELOW_THRESHOLD(a, b) (a) < (b)

#define MOD_BKNP1_ONLY3 0
#define FFT_FIRST_K 4

#ifndef TUNE_PROGRAM_BUILD
    #define TUNE_PROGRAM_BUILD 0
#endif

#undef WANT_ADDSUB

#undef HAVE_NATIVE_mpn_add_n_sub_n
#define HAVE_NATIVE_mpn_add_n_sub_n 0

#undef HAVE_NATIVE_mpn_rsh1add_n
#define HAVE_NATIVE_mpn_rsh1add_n 0

#undef HAVE_NATIVE_mpn_rsh1add_nc
#define HAVE_NATIVE_mpn_rsh1add_nc 0

#undef HAVE_NATIVE_mpn_add_nc
#define HAVE_NATIVE_mpn_add_nc 1

#define TRACE(x)

#define MULMOD_BNM1_THRESHOLD WJR_MULMOD_BNM1_THRESHOLD
#define SQRMOD_BNM1_THRESHOLD WJR_SQRMOD_BNM1_THRESHOLD

#define MUL_FFT_MODF_THRESHOLD WJR_MUL_FFT_MODF_THRESHOLD
#define MUL_FFT_TABLE3 WJR_MUL_FFT_TABLE3
#define MUL_FFT_TABLE3_SIZE WJR_MUL_FFT_TABLE3_SIZE
#define MUL_FFT_THRESHOLD WJR_MUL_FFT_THRESHOLD

#define SQR_FFT_MODF_THRESHOLD WJR_SQR_FFT_MODF_THRESHOLD
#define SQR_FFT_TABLE3 WJR_SQR_FFT_TABLE3
#define SQR_FFT_TABLE3_SIZE WJR_SQR_FFT_TABLE3_SIZE
#define SQR_FFT_THRESHOLD WJR_SQR_FFT_THRESHOLD

#define FFT_TABLE_ATTRS static constexpr

#define mpn_lshift wjr::lshift_n
#define mpn_lshiftc wjr::lshiftc_n
#define mpn_rshift wjr::rshift_n
#define mpn_rshiftc wjr::rshiftc_n
#define mpn_com wjr::math::bi_not_n
#define mpn_neg !wjr::math::bi_negate_n
#define mpn_add_1 wjr::addc_1
#define CNST_LIMB(x) UINT64_C(x)
#define mpn_sub_1 wjr::subc_1
#define LIKELY WJR_LIKELY
#define UNLIKELY WJR_UNLIKELY
#define mpn_add_n wjr::addc_n
#define mpn_add_nc wjr::addc_n
#define mpn_sub_n wjr::subc_n
#define mpn_sub_nc wjr::subc_n
#define GMP_LIMB_HIGHBIT ((UINT64_C(1)) << 63)
#define ASSERT WJR_ASSERT
#define ASSERT_ALWAYS WJR_CHECK
#define ASSERT_NOCARRY(x) (void)x
#define MPN_MULMOD_BKNP1_USABLE wjr::mulmod_bknp1_usable
#define MPN_SQRMOD_BKNP1_USABLE wjr::sqrmod_bknp1_usable
#define mpn_mulmod_bknp1_itch mulmod_bknp1_itch
#define mpn_sqrmod_bknp1_itch sqrmod_bknp1_itch
#define mpn_mulmod_bknp1 mulmod_bknp1
#define mpn_sqrmod_bknp1 sqrmod_bknp1
#define mpn_mul wjr::mul_s
#define mpn_mul_n wjr::mul_n
#define mpn_mul_fft wjr::mul_fft
#define mpn_sqr wjr::sqr
#define mpn_add wjr::addc_s
#define mpn_sub wjr::subc_s
#define mpn_cmp wjr::reverse_compare_n
#define mpn_divexact_byc(dst, src, n, c) wjr::divexact_byc(dst, src, n, c##_u64, 0)
#define mpn_divexact_by3(dst, src, n) mpn_divexact_byc(dst, src, n, 3)
#define mpn_divexact_by5(dst, src, n) mpn_divexact_byc(dst, src, n, 5)
#define mpn_divexact_by7(dst, src, n) mpn_divexact_byc(dst, src, n, 7)
#define mpn_divexact_by11(dst, src, n) mpn_divexact_byc(dst, src, n, 11)
#define mpn_divexact_by13(dst, src, n) mpn_divexact_byc(dst, src, n, 13)
#define mpn_divexact_by15(dst, src, n) mpn_divexact_byc(dst, src, n, 15)
#define mpn_divexact_by17(dst, src, n) mpn_divexact_byc(dst, src, n, 17)
#define mpn_mod_34lsub1 wjr::mod_34lsub1
#define mpn_fft_next_size wjr::fft_next_size
#define mpn_fft_best_k wjr::fft_best_k
#define mpn_zero_p(src, n) (wjr::find_not_n(src, 0, n) == n)

WJR_CONST WJR_INTRINSIC_CONSTEXPR bool mulmod_bknp1_usable(size_t rn, unsigned &k,
                                                           size_t mn) noexcept {
    return ((mn) >= 18) && ((rn) > 16) &&
           (((rn) % ((k) = 3) == 0) ||
            (((((mn) >= 35) && ((rn) >= 32))) &&
             ((((rn) % ((k) = 5) == 0)) ||
              (((mn) >= 49) && (((rn) % ((k) = 7) == 0) ||
                                (((mn) >= 104) && ((rn) >= 64) &&
                                 (((rn) % ((k) = 13) == 0) || (((mn) >= 136) && ((rn) >= 128) &&
                                                               ((rn) % ((k) = 17) == 0)))))))));
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR bool sqrmod_bknp1_usable(size_t rn, unsigned &k,
                                                           size_t mn) noexcept {
    return (mn >= 27) && (rn > 24) &&
           ((rn % (k = 3) == 0) ||
            ((((mn >= 55) && (rn > 50))) &&
             (((rn % (k = 5) == 0)) ||
              ((mn >= 56) && ((rn % (k = 7) == 0) ||
                              ((mn >= 143) && (rn >= 128) &&
                               ((rn % (k = 13) == 0) ||
                                ((mn >= 272) && (rn >= 256) && (rn % (k = 17) == 0)))))))));
}