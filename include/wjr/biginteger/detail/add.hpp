#ifndef WJR_BIGINTEGER_DETAIL_ADD_HPP__
#define WJR_BIGINTEGER_DETAIL_ADD_HPP__

#include <wjr/biginteger/detail/add-impl.hpp>
#include <wjr/math/add.hpp>
#include <wjr/math/replace.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/biginteger/detail/add.hpp>
#endif

namespace wjr::math {

WJR_INTRINSIC_CONSTEXPR20 uint64_t __addc_1_impl(uint64_t *dst, const uint64_t *src0, size_t n,
                                                 uint64_t src1, uint64_t c_in) noexcept {
    uint8_t overflow;
    dst[0] = addc_cc(src0[0], src1, static_cast<uint8_t>(c_in), overflow);
    if (overflow) {
        const size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, UINT64_MAX, 0);
        if (WJR_UNLIKELY(idx == n)) {
            return 1;
        }

        dst[idx] = src0[idx] + 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return 0;
}

/**
 * @brief Add biginteger(src0) and number with carry-in, and return the
 * result(dst) and carry-out.
 *
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral
 * type.
 * @param[out] dst The result of the addition.
 * @param[in] src0 The biginteger to be added.
 * @param[in] n The number of elements in the biginteger.
 * @param[in] src1 The number to be added.
 * @param[in] c_in The carry-in flag.
 * @return The carry-out flag.
 *
 * @pre 1. n >= 1
 * @pre 2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
 * @pre 3. c_in <= 1
 */
WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_1(uint64_t *dst, const uint64_t *src0, size_t n,
                                          uint64_t src1, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_ASSUME(c_in <= 1);

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n == 1)) {
        uint64_t overflow;
        dst[0] = addc(src0[0], src1, c_in, overflow);
        return overflow;
    }

    return __addc_1_impl(dst, src0, n, src1, c_in);
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_addc_n(uint64_t *dst, const uint64_t *src0,
                                                 const uint64_t *src1, size_t n,
                                                 uint64_t c_in) noexcept {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = addc(src0[0], src1[0], c_in, c_in);
        dst[1] = addc(src0[1], src1[1], c_in, c_in);
        dst[2] = addc(src0[2], src1[2], c_in, c_in);
        dst[3] = addc(src0[3], src1[3], c_in, c_in);

        dst += 4;
        src0 += 4;
        src1 += 4;
    }

    n &= 3;
    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

    switch (n) {
    case 3: {
        dst[-3] = addc(src0[-3], src1[-3], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 2: {
        dst[-2] = addc(src0[-2], src1[-2], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 1: {
        dst[-1] = addc(src0[-1], src1[-1], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    return c_in;
}

/**
 * @brief Add biginteger(src0) and biginteger(src1) with carry-in, and return
 * the result (dst) and carry-out.
 *
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral
 * type.
 * @param[out] dst The result of the addition.
 * @param[in] src0 The biginteger to be added.
 * @param[in] src1 The biginteger to be added.
 * @param[in] n The number of elements in the biginteger.
 * @param[in] c_in The carry-in flag.
 * @return The carry-out flag.
 *
 * @pre 1. n >= 1
 * @pre 2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
 * @pre 3. WJR_IS_SAME_OR_INCR_P(dst, n, src1, n)
 */
WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_n(uint64_t *dst, const uint64_t *src0, const uint64_t *src1,
                                          size_t n, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_ADDC_N)
    if (is_constant_evaluated()) {
        return fallback_addc_n(dst, src0, src1, n, c_in);
    }

    return asm_addc_n(dst, src0, src1, n, c_in);
#else
    return fallback_addc_n(dst, src0, src1, n, c_in);
#endif
}

/**
 * @brief Add biginteger(src0) and biginteger(src1) with carry-in, and return
 * the result (dst) and carry-out.
 *
 * @pre 1. m >= 1
 * @pre 2. n >= m
 * @pre 3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
 * @pre 4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
 */
WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_s(uint64_t *dst, const uint64_t *src0, size_t n,
                                          const uint64_t *src1, size_t m, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    c_in = addc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

/**
 * @brief Add biginteger(src0) and biginteger(src1) with carry-in, and return
 * the result (dst) and carry-out.
 *
 * @pre 1. m >= 0
 * @pre 2. n >= m and n >= 1
 * @pre 3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
 * @pre 4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
 */
WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_sz(uint64_t *dst, const uint64_t *src0, size_t n,
                                           const uint64_t *src1, size_t m, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_LIKELY(m != 0)) {
        c_in = addc_n(dst, src0, src1, m, c_in);
    }

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

} // namespace wjr::math

#endif // WJR_BIGINTEGER_DETAIL_ADD_HPP__