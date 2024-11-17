#ifndef WJR_BIGINTEGER_DETAIL_SUB_HPP__
#define WJR_BIGINTEGER_DETAIL_SUB_HPP__

#include <wjr/assert.hpp>
#include <wjr/biginteger/detail/sub-impl.hpp>
#include <wjr/math/detail.hpp>
#include <wjr/math/replace.hpp>
#include <wjr/math/sub-impl.hpp>

#if defined(WJR_X86)
    #include <wjr/arch/x86/biginteger/detail/sub.hpp>
#endif

namespace wjr {

WJR_INTRINSIC_CONSTEXPR20 uint64_t __subc_1_impl(uint64_t *dst, const uint64_t *src0, size_t n,
                                                 uint64_t src1, uint64_t c_in) noexcept {
    uint8_t overflow;
    dst[0] = subc_cc(src0[0], src1, static_cast<uint8_t>(c_in), overflow);

    if (overflow) {
        size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, 0, UINT64_MAX);

        if (WJR_UNLIKELY(idx == n)) {
            return 1;
        }

        dst[idx] = src0[idx] - 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return 0;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_1(uint64_t *dst, const uint64_t *src0, size_t n,
                                          uint64_t src1, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == 1)) {
        uint64_t overflow;
        dst[0] = subc(src0[0], src1, c_in, overflow);
        return overflow;
    }

    return __subc_1_impl(dst, src0, n, src1, c_in);
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_subc_n(uint64_t *dst, const uint64_t *src0,
                                                 const uint64_t *src1, size_t n,
                                                 uint64_t c_in) noexcept {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = subc(src0[0], src1[0], c_in, c_in);
        dst[1] = subc(src0[1], src1[1], c_in, c_in);
        dst[2] = subc(src0[2], src1[2], c_in, c_in);
        dst[3] = subc(src0[3], src1[3], c_in, c_in);

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
        dst[-3] = subc(src0[-3], src1[-3], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 2: {
        dst[-2] = subc(src0[-2], src1[-2], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 1: {
        dst[-1] = subc(src0[-1], src1[-1], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
3. WJR_IS_SAME_OR_INCR_P(dst, n, src1, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_n(uint64_t *dst, const uint64_t *src0, const uint64_t *src1,
                                          size_t n, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    if (is_constant_evaluated()) {
        return fallback_subc_n(dst, src0, src1, n, c_in);
    }

    return asm_subc_n(dst, src0, src1, n, c_in);
#else
    return fallback_subc_n(dst, src0, src1, n, c_in);
#endif
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_s(uint64_t *dst, const uint64_t *src0, size_t n,
                                          const uint64_t *src1, size_t m, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    c_in = subc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

/*
require :
1. n >= 0
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_sz(uint64_t *dst, const uint64_t *src0, size_t n,
                                           const uint64_t *src1, size_t m, uint64_t c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_LIKELY(m != 0)) {
        c_in = subc_n(dst, src0, src1, m, c_in);
    }

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n(uint64_t *dst, const uint64_t *src0,
                                             const uint64_t *src1, size_t n) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, n));

    size_t idx = reverse_find_not_n(src0, src1, n);

    if (WJR_UNLIKELY(idx != n)) {
        set_n(dst + idx, 0, n - idx);

        if (WJR_UNLIKELY(idx == 0)) {
            return 0;
        }
    }

    WJR_ASSUME(idx >= 1);

    uint64_t hi = 0;
    WJR_ASSUME(src0[idx - 1] != src1[idx - 1]);
    const bool overflow = sub_overflow(src0[idx - 1], src1[idx - 1], hi);

    if (overflow) {
        std::swap(src0, src1);
        hi = -hi;
    }

    do {
        if (WJR_UNLIKELY(idx == 1)) {
            dst[0] = hi;
            break;
        }

        hi -= subc_n(dst, src0, src1, idx - 1);
        dst[idx - 1] = hi;
    } while (false);

    return overflow ? -1 : 1;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n_pos(uint64_t *dst, const uint64_t *src0,
                                                 const uint64_t *src1, size_t n) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, n));

    size_t idx = reverse_find_not_n(src0, src1, n);

    if (WJR_UNLIKELY(idx != n)) {
        set_n(dst + idx, 0, n - idx);

        if (WJR_UNLIKELY(idx == 0)) {
            return 0;
        }
    }

    WJR_ASSUME(idx >= 1);

    uint64_t hi = 0;
    WJR_ASSUME(src0[idx - 1] != src1[idx - 1]);
    const bool overflow = sub_overflow(src0[idx - 1], src1[idx - 1], hi);

    if (overflow) {
        std::swap(src0, src1);
        hi = -hi;
    }

    ssize_t ret = __fast_from_unsigned(idx);
    WJR_ASSUME(ret >= 1);

    do {
        if (WJR_UNLIKELY(idx == 1)) {
            dst[0] = hi;
            break;
        }

        WJR_ASSUME(ret >= 2);

        hi -= subc_n(dst, src0, src1, idx - 1);

        if (WJR_LIKELY(hi != 0)) {
            dst[idx - 1] = hi;
        } else {
            --ret;
        }
    } while (false);

    WJR_ASSUME(ret > 0);
    return overflow ? -ret : ret;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s1(uint64_t *dst, const uint64_t *src0, size_t n,
                                              const uint64_t *src1, size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    WJR_ASSERT(n - m <= 1);

    do {
        if (n == m) {
            break;
        }

        if (WJR_UNLIKELY(src0[m] == 0)) {
            dst[m] = 0;
            break;
        }

        uint64_t hi = src0[m];
        hi -= subc_n(dst, src0, src1, m);
        dst[m] = hi;
        return 1;
    } while (false);

    return abs_subc_n(dst, src0, src1, m);
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s(uint64_t *dst, const uint64_t *src0, size_t n,
                                             const uint64_t *src1, size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n - m <= 1)) {
        return abs_subc_s1(dst, src0, n, src1, m);
    }

    size_t idx = reverse_replace_find_not(dst + m, src0 + m, n - m, 0, 0);

    if (WJR_UNLIKELY(idx == 0)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    (void)subc_s(dst, src0, m + idx, src1, m);
    return 1;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s_pos(uint64_t *dst, const uint64_t *src0, size_t n,
                                                 const uint64_t *src1, size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return abs_subc_n_pos(dst, src0, src1, m);
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n - m <= 1)) {
        do {
            if (n == m) {
                break;
            }

            if (WJR_UNLIKELY(src0[m] == 0)) {
                dst[m] = 0;
                break;
            }

            uint64_t hi = src0[m];
            hi -= subc_n(dst, src0, src1, m);
            ssize_t ret = __fast_from_unsigned(m + 1);

            if (WJR_LIKELY(hi != 0)) {
                dst[m] = hi;
            } else {
                --ret;
            }

            WJR_ASSUME(ret > 0);
            return ret;
        } while (false);

        return abs_subc_n_pos(dst, src0, src1, m);
    }

    size_t idx = reverse_replace_find_not(dst + m, src0 + m, n - m, 0, 0);

    if (WJR_UNLIKELY(idx == 0)) {
        return abs_subc_n_pos(dst, src0, src1, m);
    }

    (void)subc_s(dst, src0, m + idx, src1, m);
    ssize_t ret = __fast_from_unsigned(m + idx);
    WJR_ASSUME(ret >= 2);
    ret -= dst[m + idx - 1] == 0;
    WJR_ASSUME(ret >= 1);
    return ret;
}

// just like abs_subc_n.
WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n(uint64_t *dst, const uint64_t *src0,
                                             const uint64_t *src1, size_t n, uint64_t &c_out,
                                             uint64_t cf0, uint64_t cf1) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    if (WJR_LIKELY(cf0 != cf1)) {
        ssize_t ret = __fast_from_unsigned(n);
        uint64_t cf = 0;
        if (cf0 < cf1) {
            std::swap(src0, src1);
            ret = __fast_negate(ret);
            cf = cf1 - cf0;
        } else {
            cf = cf0 - cf1;
        }

        c_out = cf - subc_n(dst, src0, src1, n);
        return ret;
    }
    
    c_out = 0;
    return abs_subc_n(dst, src0, src1, n);
}

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_SUB_HPP__