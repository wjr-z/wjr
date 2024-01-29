#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

#include <wjr/math/replace.hpp>
#include <wjr/math/sub-impl.hpp>

#if defined(WJR_X86)
#include <wjr/x86/sub.hpp>
#endif

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) {
    T ret = a - b;
    U c = ret > a;
    a = ret;
    ret -= c_in;
    c |= ret > a;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_subc)
#define WJR_HAS_BUILTIN_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_subc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_SUBC(suffix, type)                                          \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_subc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else

    WJR_REGISTER_BUILTIN_SUBC(b, unsigned char)
    WJR_REGISTER_BUILTIN_SUBC(s, unsigned short)
    WJR_REGISTER_BUILTIN_SUBC(, unsigned int)
    WJR_REGISTER_BUILTIN_SUBC(l, unsigned long)
    WJR_REGISTER_BUILTIN_SUBC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_SUBC
}

#endif // WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U,
          std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E T subc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_L(1, c_in == 0 || c_in == 1);
    WJR_ASSUME((c_in == 0 || c_in == 1));

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ASM_SUBC), asm_subc,
                              WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc,
                                             fallback_subc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc,
                              fallback_subc)(a, b, c_in, c_out);
    }
#endif
}

template <typename T, typename U,
          std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in) {
    WJR_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));

    dst[0] = subc(src0[0], src1, c_in, c_in);

    if (c_in) {
        size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, 0, -1);

        if (idx == n) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] - 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return static_cast<U>(0);
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                          U c_in) {
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

template <typename T, typename U,
          std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in) {
    WJR_ASSERT(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_subc_n(dst, src0, src1, n, c_in);
        }

        return asm_subc_n(dst, src0, src1, n, c_in);
    } else {
        return fallback_subc_n(dst, src0, src1, n, c_in);
    }
#else
    return fallback_subc_n(dst, src0, src1, n, c_in);
#endif
}

template <typename T, typename U,
          std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in) {
    WJR_ASSERT(n >= m);

    c_in = subc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0u, c_in);
    }

    return c_in;
}

// ret :
// > 0 : src0 > src1
// == 0 : src0 == src1
// < 0 : src0 < src1
// abs(ret) :
// non-zero pos
template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n) {
    size_t idx = reverse_find_not_n(src0, src1, n);
    set_n(dst + idx, 0u, n - idx);

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    int c = src0[idx - 1] < src1[idx - 1] ? -1 : 1;

    if (c < 0) {
        std::swap(src0, src1);
    }

    auto cf = subc_n(dst, src0, src1, idx, 0u);
    WJR_ASSERT(cf == 0);
    (void)(cf);

    ssize_t ret = n;
    WJR_ASSUME(ret > 0);
    return c > 0 ? idx : -idx;
}

// dst = abs(src0 - src1)
// return compare(src0, src1)
template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_s(T *dst, const T *src0, size_t n,
                                             const T *src1, size_t m) {
    WJR_ASSERT(n >= m);

    size_t delta = n - m;
    size_t idx = reverse_find_not_n(src0 + m, 0u, delta);

    if (dst != src0) {
        set_n(dst + m + idx, 0u, delta - idx);
    }

    if (idx == 0) {
        return abs_subc_n(dst, src0, src1, m);
    }

    auto cf = subc_s(dst, src0, m + idx, src1, m, 0u);
    WJR_ASSERT(cf == 0);
    (void)(cf);

    ssize_t ret = m + idx;
    WJR_ASSUME(ret > 0);
    return ret;
}

WJR_INTRINSIC_CONSTEXPR void __fallback_subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                 uint64_t hi0, uint64_t lo1,
                                                 uint64_t hi1) {
    uint64_t __al = lo0 - lo1;
    ah = hi0 - hi1 - (__al > lo0);
    al = __al;
}

WJR_INTRINSIC_CONSTEXPR_E void __subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                          uint64_t hi0, uint64_t lo1, uint64_t hi1) {
#if WJR_HAS_BUILTIN(__ASM_SUBC_128)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(lo0 == 0) && lo0 == 0) ||
        (WJR_BUILTIN_CONSTANT_P(lo1 == 0) && lo1 == 0)) {
        return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1);
    }
    return __asm_subc_128(al, ah, lo0, hi0, lo1, hi1);
#else
    return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SUB_HPP__