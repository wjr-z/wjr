// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

#ifndef WJR_ADDSUB_I
    #error "abort"
#endif

#if WJR_ADDSUB_I == 1
    #define WJR_addsub add
    #define WJR_adcsbb adc
    #define __WJR_TEST_ASSEMBLY ASM_ADDLSH_N
#else
    #define WJR_addsub rsb
    #define WJR_adcsbb sbb
    #define __WJR_TEST_ASSEMBLY ASM_RSBLSH_N
#endif

#if WJR_HAS_BUILTIN(__WJR_TEST_ASSEMBLY) == 1

extern WJR_ALL_NONNULL uint64_t WJR_PP_CONCAT(__wjr_asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t cl) noexcept;

#else

extern WJR_ALL_NONNULL
    "C" WJR_MS_ABI uint64_t WJR_PP_CONCAT(__wjr_asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(
        uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t cl) noexcept;

#endif

WJR_INTRINSIC_INLINE uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t cl) noexcept {
    return WJR_PP_CONCAT(__wjr_asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(dst, src0, src1, n, cl);
}

#undef __WJR_TEST_ASSEMBLY
#undef WJR_adcsbb
#undef WJR_addsub
#undef WJR_ADDSUB_I