#ifndef WJR_ARCH_X86_MATH_SHIFT_HPP__
#define WJR_ARCH_X86_MATH_SHIFT_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T shld(T hi, T lo, unsigned int cl) noexcept;
template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T shrd(T lo, T hi, unsigned int cl) noexcept;

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_BUILTIN_LSHIFT_N WJR_HAS_DEF
    #define WJR_HAS_BUILTIN_RSHIFT_N WJR_HAS_DEF
#endif

namespace shift_detail {
#ifndef WJR_SMALL_SHIFT_N_THRESHOLD
    #define WJR_SMALL_SHIFT_N_THRESHOLD 6
#endif

inline constexpr size_t small_shift_n_threshold = WJR_SMALL_SHIFT_N_THRESHOLD;
} // namespace shift_detail

#if WJR_HAS_BUILTIN(LSHIFT_N)

template <bool is_constant, typename T>
WJR_ALL_NONNULL void large_builtin_lshift_n_impl(T *dst, const T *src, size_t n,
                                                 unsigned int cl) noexcept;

extern template void large_builtin_lshift_n_impl<false, uint64_t>(uint64_t *dst,
                                                                  const uint64_t *src, size_t n,
                                                                  unsigned int cl) noexcept;
extern template void large_builtin_lshift_n_impl<true, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                                 size_t n,
                                                                 unsigned int cl) noexcept;

template <typename T>
WJR_ALL_NONNULL WJR_INTRINSIC_INLINE void builtin_lshift_n_impl(T *dst, const T *src, size_t n,
                                                                unsigned int cl) noexcept {
    if (n <= shift_detail::small_shift_n_threshold) {
        if (WJR_LIKELY(--n)) {
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM) && defined(__BMI2__) &&                              \
        !WJR_HAS_FEATURE(FORCE_NOVECTOR)
            unsigned int tcl = 64 - cl;
            uint64_t __cl = cl;
            uint64_t __tcl = tcl;
            uint64_t t0, t1;
            asm volatile(
                ".Lloop%=:\n\t"
                "shlx{q %[cl], (%[src], %[n], 8), %[t0]| %[t0], [%[src] + %[n] * 8], %[cl]}\n\t"
                "shrx{q %[tcl], -8(%[src], %[n], 8), %[t1]| %[t1], [%[src] + %[n] * 8 - 8], %[tcl]}\n\t"
                "or{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                "mov{q %[t0], (%[dst], %[n], 8)| [%[dst] + %[n] * 8], %[t0]}\n\t"
                // todo : Maybe need to use `sub'.
                "dec %[n]\n\t"
                "jne .Lloop%=\n\t"
                : [dst] "+r"(dst), [src] "+r"(src), [n] "+&r"(n), [t0] "=&r"(t0), [t1] "=&r"(t1)
                : [cl] "r"(__cl), [tcl] "r"(__tcl)
                : "cc", "memory");
    #else
            WJR_NOVECTOR
            do {
                dst[n] = shld(src[n], src[n - 1], cl);
            } while (WJR_LIKELY(--n));
    #endif
        }
        return;
    }

    src += n;
    dst += n;

    if (WJR_BUILTIN_CONSTANT_P(cl)) {
        return large_builtin_lshift_n_impl<true>(dst, src, n - 1, cl);
    }

    return large_builtin_lshift_n_impl<false>(dst, src, n - 1, cl);
}

template <typename T>
WJR_INTRINSIC_INLINE T builtin_lshift_n(T *dst, const T *src, size_t n, unsigned int cl,
                                        T lo) noexcept {
    const T ret = src[n - 1] >> (64 - cl);
    builtin_lshift_n_impl(dst, src, n, cl);
    dst[0] = shld(src[0], lo, cl);
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

template <bool is_constant, typename T>
WJR_ALL_NONNULL void large_builtin_rshift_n_impl(T *dst, const T *src, size_t n,
                                                 unsigned int cl) noexcept;

extern template void large_builtin_rshift_n_impl<false, uint64_t>(uint64_t *dst,
                                                                  const uint64_t *src, size_t n,
                                                                  unsigned int cl) noexcept;
extern template void large_builtin_rshift_n_impl<true, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                                 size_t n,
                                                                 unsigned int cl) noexcept;

template <typename T>
WJR_ALL_NONNULL WJR_INTRINSIC_INLINE void builtin_rshift_n_impl(T *dst, const T *src, size_t n,
                                                                unsigned int cl) noexcept {
    if (n <= shift_detail::small_shift_n_threshold) {
        if (WJR_LIKELY(--n)) {
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM) && defined(__BMI2__) &&                              \
        !WJR_HAS_FEATURE(FORCE_NOVECTOR)
            unsigned int tcl = 64 - cl;
            uint64_t __cl = cl;
            uint64_t __tcl = tcl;
            uint64_t t0, t1;
            size_t i = 0;
            asm volatile(
                ".Lloop%=:\n\t"
                "shrx{q %[cl], (%[src], %[i], 8), %[t0]| %[t0], [%[src] + %[i] * 8], %[cl]}\n\t"
                "shlx{q %[tcl], 8(%[src], %[i], 8), %[t1]| %[t1], [%[src] + %[i] * 8 + 8], %[tcl]}\n\t"
                "or{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                "mov{q %[t0], (%[dst], %[i], 8)| [%[dst] + %[i] * 8], %[t0]}\n\t"
                "add{q $1, %[i]| %[i], 1}\n\t"
                "cmp{q %[i], %[n]| %[n], %[i]}\n\t"
                "jne .Lloop%=\n\t"
                : [dst] "+r"(dst), [src] "+r"(src), [i] "+&r"(i), [t0] "=&r"(t0), [t1] "=&r"(t1)
                : [cl] "r"(__cl), [tcl] "r"(__tcl), [n] "r"(n)
                : "cc", "memory");
    #else
            WJR_NOVECTOR
            do {
                dst[0] = shrd(src[0], src[1], cl);
                ++dst;
                ++src;
            } while (WJR_LIKELY(--n));
    #endif
        }

        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(cl)) {
        return large_builtin_rshift_n_impl<true>(dst, src, n - 1, cl);
    }

    return large_builtin_rshift_n_impl<false>(dst, src, n - 1, cl);
}

template <typename T>
WJR_INTRINSIC_INLINE T builtin_rshift_n(T *dst, const T *src, size_t n, unsigned int cl,
                                        T hi) noexcept {
    const T ret = src[0] << (64 - cl);
    builtin_rshift_n_impl(dst, src, n, cl);
    dst[n - 1] = shrd(src[n - 1], hi, cl);
    return ret;
}

#endif

#undef WJR_SHIFT_FUNCTION_OPTIMIZE

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_SHIFT_HPP__