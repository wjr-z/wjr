#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

#include <wjr/math/ctz.hpp>
#include <wjr/math/neg.hpp>
#include <wjr/math/replace.hpp>

#if defined(WJR_X86)
#include <wjr/x86/sub.hpp>
#endif

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) {
    T ret = a;
    U c = ret < b;
    ret -= b;
    c |= ret < c_in;
    ret -= c_in;
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

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR T subc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_L(1, c_in == 0 || c_in == 1);
    WJR_ASSUME((c_in == 0 || c_in == 1));

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](const auto &x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x) ? (x == 0 ? 2 : 1) : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

#if WJR_HAS_BUILTIN(ASM_SUBC)
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        return asm_subc_1(a, b, c_out);
    }
#endif

#if WJR_HAS_BUILTIN(SUBC)
    return builtin_subc(a, b, c_in, c_out);
#else
    return fallback_subc(a, b, c_in, c_out);
#endif // WJR_HAS_BUILTIN(SUBC)

#endif
}

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR U subc_1(T *dst, const T *src0, size_t n, type_identity_t<T> src1,
                                 U c_in) {
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

template <size_t div, typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_n_res(T *dst, const T *src0, const T *src1, size_t n,
                                     U c_in) {

    constexpr size_t mask = div - 1;

    n &= mask;

    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

#define WJR_REGISTER_SUBC_RES_CASE_CALLER(idx)                                           \
    case idx: {                                                                          \
        dst[-idx] = subc(src0[-idx], src1[-idx], c_in, c_in);                            \
        WJR_FALLTHROUGH;                                                                 \
    }

#define WJR_REGISTER_SUBC_RES_SWITCH_CALLER(size)                                        \
    if constexpr (div == size) {                                                         \
        switch (n) {                                                                     \
            WJR_PP_TRANSFORM_PUT(                                                        \
                WJR_PP_QUEUE_TRANSFORM(                                                  \
                    WJR_PP_QUEUE_REVERSE((WJR_PP_IOTA(WJR_PP_DEC(size)))), WJR_PP_INC),  \
                WJR_REGISTER_SUBC_RES_CASE_CALLER)                                       \
        }                                                                                \
        return c_in;                                                                     \
    } else

    WJR_REGISTER_SUBC_RES_SWITCH_CALLER(2)
    WJR_REGISTER_SUBC_RES_SWITCH_CALLER(4) WJR_REGISTER_SUBC_RES_SWITCH_CALLER(8) {
        static_assert(div <= 8, "not support yet");
    }

#undef WJR_REGISTER_SUBC_RES_CASE_CALLER
#undef WJR_REGISTER_SUBC_RES_SWITCH_CALLER
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

    return subc_n_res<4>(dst, src0, src1, n, c_in);
}

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR U subc_n(T *dst, const T *src0, const T *src1, size_t n, U c_in) {
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    if (is_constant_evaluated()) {
        return fallback_subc_n(dst, src0, src1, n, c_in);
    }

    return asm_subc_n(dst, src0, src1, n, c_in);
#else
    return fallback_subc_n(dst, src0, src1, n, c_in);
#endif
}

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR U subc_s(T *dst, const T *src0, size_t n, const T *src1, size_t m,
                                 U c_in) {
    WJR_ASSERT(n >= m);
    WJR_ASSUME(n >= m);

    c_in = subc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0u, c_in);
    }

    return c_in;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR int abs_subc_n(T *dst, const T *src0, const T *src1, size_t n) {
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

    return c;
}

// dst = abs(src0 - src1)
// return compare(src0, src1)
template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR int abs_subc_s(T *dst, const T *src0, size_t n, const T *src1,
                                       size_t m) {
    WJR_ASSERT(n >= m);
    WJR_ASSUME(n >= m);

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
    WJR_ASSUME(cf == 0);
    (void)(cf);

    return 1;
}

} // namespace wjr

#endif // WJR_MATH_SUB_HPP__