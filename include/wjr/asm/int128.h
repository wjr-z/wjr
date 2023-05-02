#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) uint64_t u64x64(uint64_t a, uint64_t b, uint64_t& hi) {
    if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
        return _umul128(a, b, &hi);
#elif defined(WJR_HAS_INT128)
        unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
        hi = (uint64_t)(r >> 64);
        return (uint64_t)r;
#elif defined(WJR_INLINE_ASM)
        asm volatile("mulq %[u1]" : [u1] "+d"(a) : "a"(b) : "cc");
        hi = a;
        return b;
#endif
    }
    // fallback
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    uint64_t t = rl + (rm0 << 32);
    uint64_t lo = t + (rm1 << 32);
    uint64_t c = t < rl;
    hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;

    return lo;
}

WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint32_t u32x32(uint32_t a, uint32_t b, uint32_t& hi) {
    uint64_t r = (uint64_t)a * (uint64_t)b;
    hi = (uint32_t)(r >> 32);
    return (uint32_t)r;
}

WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint16_t u16x16(uint16_t a, uint16_t b, uint16_t& hi) {
    uint32_t r = (uint32_t)a * (uint32_t)b;
    hi = (uint16_t)(r >> 16);
    return (uint16_t)r;
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint64_t u64x64lo(uint64_t a, uint64_t b) {
    return a * b;
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t u32x32hi(uint32_t a, uint32_t b) {
    return (uint32_t)(((uint64_t)a * (uint64_t)b) >> 32);
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) uint64_t u64x64hi(uint64_t a, uint64_t b) {
    if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
        return __umulh(a, b);
#elif defined(WJR_HAS_INT128)
        return (uint64_t)(((unsigned __int128)a * (unsigned __int128)b) >> 64);
#elif defined(WJR_INLINE_ASM)
        asm volatile("mulq %[u1]" : [u1] "+d"(a) : "a"(b) : "cc");
        return a;
#endif 
    }
    // fallback
    uint32_t ah = a >> 32;
    uint32_t al = a & 0xFFFFFFFF;
    uint32_t bh = b >> 32;
    uint32_t bl = b & 0xFFFFFFFF;

    uint32_t albl_hi = u32x32hi(al, bl);
    uint64_t albh = al * (uint64_t)bh;
    uint64_t ahbl = ah * (uint64_t)bl;
    uint64_t ahbh = ah * (uint64_t)bh;

    uint64_t temp = ahbl + albl_hi;
    uint64_t temp_lo = temp & 0xFFFFFFFF;
    uint64_t temp_hi = temp >> 32;

    return ahbh + temp_hi + ((temp_lo + albh) >> 32);
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t u32x32lo(uint32_t a, uint32_t b) {
    return a * b;
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t u16x16hi(uint16_t a, uint16_t b) {
    return (uint16_t)(((uint32_t)a * (uint32_t)b) >> 16);
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t u16x16lo(uint16_t a, uint16_t b) {
    return a * b;
}

// uint128_t / uint64_t -> uint64_t
// Must ensure that the quotient is within the uint64_t range
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) uint64_t u128d64t64(uint64_t hi, uint64_t lo, uint64_t den, uint64_t& r) {
    if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
        return _udiv128(hi, lo, den, &r);
#elif defined(WJR_INLINE_ASM)
        asm volatile("divq %[u2]" : [u2] "+d"(hi), "+a"(lo) : "r"(den) : "cc");
        r = hi;
        return lo;
#endif
    }
    // fallback
    extern uint64_t __large__u128d64t64(uint64_t hi, uint64_t lo, uint64_t den, uint64_t & r);
    return __large__u128d64t64(hi, lo, den, r);
}

WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) uint32_t u64d32t32(uint64_t x, uint32_t den, uint32_t& r) {
    if (wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86)
        return _udiv64(x, den, &r);
#elif defined(WJR_INLINE_ASM)
        uint32_t hi = x >> 32;
        uint32_t lo = x;
        asm volatile("divl %[u2]" : [u2] "+d"(hi), "+a"(lo) : "r"(den) : "cc");
        r = hi;
        return lo;
#endif 
    }
    // fallback
    uint32_t q = x / den;
    r = x % den;
    return q;
}

WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint16_t u32d16t16(uint32_t x, uint16_t den, uint16_t& r) {
    uint16_t q = x / den;
    r = x % den;
    return q;
}

WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint8_t u16d8t8(uint16_t x, uint8_t den, uint8_t& r) {
    uint8_t q = x / den;
    r = x % den;
    return q;
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) int64_t s64x64hi(int64_t a, int64_t b) {
    if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
        return __mulh(a, b);
#elif defined(WJR_HAS_INT128)
        return (int64_t)(((__int128)a * (__int128)b) >> 64);
#elif defined(WJR_INLINE_ASM)
        asm volatile("imulq %[u1]" : [u1] "+d"(a) : "a"(b) : "cc");
        return a;
#endif 
    }
    // fallback
    int32_t ah = a >> 32;
    uint32_t al = a & 0xFFFFFFFF;
    int32_t bh = b >> 32;
    uint32_t bl = b & 0xFFFFFFFF;

    uint32_t albl_hi = u32x32hi(al, bl);
    int64_t t = ah * (int64_t)bl + albl_hi;
    int64_t w1 = al * (int64_t)bh + (t & 0xFFFFFFFF);

    return ah * (int64_t)bh + (t >> 32) + (w1 >> 32);
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint64_t s64x64lo(uint64_t a, uint64_t b) {
    return a * b;
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t s32x32hi(uint32_t a, uint32_t b) {
    return (uint32_t)(((uint64_t)a * (uint64_t)b) >> 32);
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t s32x32lo(uint32_t a, uint32_t b) {
    return a * b;
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t s16x16hi(uint16_t a, uint16_t b) {
    return (uint16_t)(((uint32_t)a * (uint32_t)b) >> 16);
}

WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t s16x16lo(uint16_t a, uint16_t b) {
    return a * b;
}

_WJR_ASM_END