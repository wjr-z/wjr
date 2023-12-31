#ifndef WJR_MATH_SHIFT_HPP__
#define WJR_MATH_SHIFT_HPP__

#include <wjr/simd/simd.hpp>
#include <wjr/unroll.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shld(T hi, T lo, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return hi << c | lo >> (digits - c);
}

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_SHLD WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SHLD)

template <typename T>
WJR_INTRINSIC_INLINE T asm_shld(T hi, T lo, unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_SHLD(args)                                              \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER)
#define WJR_REGISTER_BUILTIN_ASM_SHLD_I(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        asm("shld{" #suffix " %b2, %1, %0| %0, %1, %2 }"                                 \
            : "+r"(hi)                                                                   \
            : "r"(lo), "ci"(c)                                                           \
            : "cc");                                                                     \
        return hi;                                                                       \
    } else
#define WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER(args) WJR_REGISTER_BUILTIN_ASM_SHLD_I args

    WJR_REGISTER_BUILTIN_ASM_SHLD(
        ((b, uint8_t), (w, uint16_t), (l, uint32_t), (q, uint64_t))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER
#undef WJR_REGISTER_BUILTIN_ASM_SHLD_I
#undef WJR_REGISTER_BUILTIN_ASM_SHLD
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T shld(T hi, T lo, unsigned int c) {

#if WJR_HAS_BUILTIN(ASM_SHLD)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(c == 0) && c == 0)) {
        return fallback_shld(hi, lo, c);
    }

    return asm_shld(hi, lo, c);
#else
    return fallback_shld(hi, lo, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shrd(T lo, T hi, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return lo >> c | hi << (digits - c);
}

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_SHRD WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SHRD)

template <typename T>
WJR_INTRINSIC_INLINE T asm_shrd(T lo, T hi, unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_SHRD(args)                                              \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_ASM_SHRD_I_CALLER)
#define WJR_REGISTER_BUILTIN_ASM_SHRD_I(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        asm("shrd{" #suffix " %b2, %1, %0| %0, %1, %2 }"                                 \
            : "+r"(lo)                                                                   \
            : "r"(hi), "ci"(c)                                                           \
            : "cc");                                                                     \
        return lo;                                                                       \
    } else
#define WJR_REGISTER_BUILTIN_ASM_SHRD_I_CALLER(args) WJR_REGISTER_BUILTIN_ASM_SHRD_I args

    WJR_REGISTER_BUILTIN_ASM_SHRD(
        ((b, uint8_t), (w, uint16_t), (l, uint32_t), (q, uint64_t))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_SHRD_I_CALLER
#undef WJR_REGISTER_BUILTIN_ASM_SHRD_I
#undef WJR_REGISTER_BUILTIN_ASM_SHRD
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T shrd(T lo, T hi, unsigned int c) {

#if WJR_HAS_BUILTIN(ASM_SHRD)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(c == 0) && c == 0)) {
        return fallback_shrd(lo, hi, c);
    }

    return asm_shrd(lo, hi, c);
#else
    return fallback_shrd(lo, hi, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rshift(T *dst, const T *src, size_t n,
                                          unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[0] << (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[i] = fallback_shrd(src[i], src[i + 1], c);
    }
    dst[n - 1] = src[n - 1] >> c;
    return ret;
}

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_RSHIFT WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(RSHIFT)

template <size_t unroll, typename T>
WJR_INLINE void builtin_unroll_rshift(T *dst, const T *src, size_t n, unsigned int c) {
    (void)unroll_call<unroll>([dst, src, n, c](auto ic) -> std::optional<empty> {
        constexpr auto idx = decltype(ic)::value;

        if (idx == n) {
            return empty{};
        }

        dst[idx] = fallback_shrd(src[idx], src[idx + 1], c);
        return std::nullopt;
    });
}

template <typename T>
WJR_INLINE void builtin_simd_rshift_impl(T *dst, const T *src, size_t n, unsigned int c) {
    using simd = sse;
    using simd_int = typename simd::int_type;
    using simd_int_tag = typename simd::int_tag_type;
    using simd_float_tag = typename simd::float_tag_type;

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;

    WJR_ASSUME(n >= simd_width);

    {
        size_t k = n % simd_width;
        builtin_unroll_rshift<simd_width - 1>(dst, src, k, c);

        dst += k;
        src += k;
        n -= k;
    }

    WJR_ASSUME(n % simd_width == 0);
    WJR_ASSUME(n != 0);

    simd_int y = simd_cast<uint32_t, simd_int_tag>(c);
    simd_int z = simd_cast<uint32_t, simd_int_tag>(64 - c);

    simd_int x0 = simd::set1(src[0], T());

#define WJR_REGISTER_RSHIFT_IMPL(index)                                                  \
    do {                                                                                 \
        simd_int x1 = simd::loadu((simd_int *)(src + 1 + (index)));                      \
        x0 = simd_cast<simd_float_tag, simd_int_tag>(                                    \
            simd::template shuffle_ps<78>(simd_cast<simd_int_tag, simd_float_tag>(x0),   \
                                          simd_cast<simd_int_tag, simd_float_tag>(x1))); \
                                                                                         \
        simd_int r0 = simd::srl(x0, y, T());                                             \
        simd_int r1 = simd::sll(x1, z, T());                                             \
                                                                                         \
        simd_int r = simd::Or(r0, r1);                                                   \
                                                                                         \
        simd::storeu((simd_int *)(dst + (index)), r);                                    \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

    if ((n / simd_width) & 1) {
        WJR_REGISTER_RSHIFT_IMPL(0);

        dst += simd_width;
        src += simd_width;
        n -= simd_width;
    }

    if ((n / (simd_width * 2)) & 1) {
        WJR_REGISTER_RSHIFT_IMPL(0);
        WJR_REGISTER_RSHIFT_IMPL(simd_width);

        dst += simd_width * 2;
        src += simd_width * 2;
        n -= simd_width * 2;
    }

    if (!n) {
        return;
    }

    WJR_ASSUME(n % (simd_width * 4) == 0);

    WJR_REGISTER_RSHIFT_IMPL(0);
    WJR_REGISTER_RSHIFT_IMPL(simd_width);
    WJR_REGISTER_RSHIFT_IMPL(simd_width * 2);
    WJR_REGISTER_RSHIFT_IMPL(simd_width * 3);

    n -= simd_width * 4;

    if (WJR_UNLIKELY(!n)) {
        return;
    }

    dst += simd_width * 4;
    src += simd_width * 4;

    size_t idx = 0;

    do {
        WJR_REGISTER_RSHIFT_IMPL(idx);
        WJR_REGISTER_RSHIFT_IMPL(idx + simd_width);
        WJR_REGISTER_RSHIFT_IMPL(idx + simd_width * 2);
        WJR_REGISTER_RSHIFT_IMPL(idx + simd_width * 3);

        idx += simd_width * 4;
    } while (idx != n);

#undef WJR_REGISTER_RSHIFT_IMPL
}

template <typename T>
WJR_INLINE void builtin_rshift_impl(T *dst, const T *src, size_t n, unsigned int c) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    using simd = sse;

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    constexpr auto threshold = std::max<size_t>(simd_width, 4);

    if (WJR_UNLIKELY(n < threshold)) {
        return builtin_unroll_rshift<threshold - 1>(dst, src, n, c);
    }

    WJR_ASSUME(n >= threshold);
    return builtin_simd_rshift_impl(dst, src, n, c);
}

template <typename T>
WJR_INLINE T builtin_rshift(T *dst, const T *src, size_t n, unsigned int c) {
    T ret = src[0] << (64 - c);
    builtin_rshift_impl(dst, src, n - 1, c);
    dst[n - 1] = src[n - 1] >> c;
    return ret;
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T rshift(T *dst, const T *src, size_t n, unsigned int c) {
#if WJR_HAS_BUILTIN(RSHIFT)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_rshift(dst, src, n, c);
        }

        return builtin_rshift(dst, src, n, c);
    } else {
        return fallback_rshift(dst, src, n, c);
    }
#else
    return fallback_rshift(dst, src, n, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_lshift(T *dst, const T *src, size_t n,
                                          unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[n - 1] >> (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[n - i - 1] = fallback_shld(src[n - i - 1], src[n - i - 2], c);
    }
    dst[0] = src[0] << c;
    return ret;
}

#if WJR_HAS_BUILTIN(RSHIFT)
#define WJR_HAS_BUILTIN_LSHIFT WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(LSHIFT)

template <size_t unroll, typename T>
WJR_INLINE void builtin_unroll_lshift(T *dst, const T *src, size_t n, unsigned int c) {
    (void)unroll_call<unroll>([dst, src, n, c](auto ic) -> std::optional<empty> {
        constexpr auto idx = decltype(ic)::value;

        if (idx == n) {
            return empty{};
        }

        dst[-idx - 1] = fallback_shld(src[-idx - 1], src[-idx - 2], c);
        return std::nullopt;
    });
}

template <typename T>
WJR_INLINE void builtin_simd_lshift_impl(T *dst, const T *src, size_t n, unsigned int c) {
    using simd = sse;
    using simd_int = typename simd::int_type;
    using simd_int_tag = typename simd::int_tag_type;
    using simd_float_tag = typename simd::float_tag_type;

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;

    WJR_ASSUME(n >= simd_width);

    dst += n;
    src += n;

    {
        size_t k = n % simd_width;
        builtin_unroll_lshift<simd_width - 1>(dst, src, k, c);

        dst -= k;
        src -= k;
        n -= k;
    }

    WJR_ASSUME(n % simd_width == 0);
    WJR_ASSUME(n != 0);

    simd_int y = simd_cast<uint32_t, simd_int_tag>(c);
    simd_int z = simd_cast<uint32_t, simd_int_tag>(64 - c);

    simd_int x0 = simd::set1(src[-1], T());

#define WJR_REGISTER_LSHIFT_IMPL(index)                                                  \
    do {                                                                                 \
        simd_int x1 = simd::loadu((simd_int *)(src - 1 - simd_width - (index)));         \
        x0 = simd_cast<simd_float_tag, simd_int_tag>(                                    \
            simd::template shuffle_ps<78>(simd_cast<simd_int_tag, simd_float_tag>(x1),   \
                                          simd_cast<simd_int_tag, simd_float_tag>(x0))); \
                                                                                         \
        simd_int r0 = simd::sll(x0, y, T());                                             \
        simd_int r1 = simd::srl(x1, z, T());                                             \
                                                                                         \
        simd_int r = simd::Or(r0, r1);                                                   \
                                                                                         \
        simd::storeu((simd_int *)(dst - simd_width - (index)), r);                       \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

    if ((n / simd_width) & 1) {
        WJR_REGISTER_LSHIFT_IMPL(0);

        dst -= simd_width;
        src -= simd_width;
        n -= simd_width;
    }

    if ((n / (simd_width * 2)) & 1) {
        WJR_REGISTER_LSHIFT_IMPL(0);
        WJR_REGISTER_LSHIFT_IMPL(simd_width);

        dst -= simd_width * 2;
        src -= simd_width * 2;
        n -= simd_width * 2;
    }

    if (!n) {
        return;
    }

    WJR_ASSUME(n % (simd_width * 4) == 0);

    WJR_REGISTER_LSHIFT_IMPL(0);
    WJR_REGISTER_LSHIFT_IMPL(simd_width);
    WJR_REGISTER_LSHIFT_IMPL(simd_width * 2);
    WJR_REGISTER_LSHIFT_IMPL(simd_width * 3);

    n -= simd_width * 4;

    if (WJR_UNLIKELY(!n)) {
        return;
    }

    dst -= simd_width * 4;
    src -= simd_width * 4;

    size_t idx = 0;

    do {
        WJR_REGISTER_LSHIFT_IMPL(idx);
        WJR_REGISTER_LSHIFT_IMPL(idx + simd_width);
        WJR_REGISTER_LSHIFT_IMPL(idx + simd_width * 2);
        WJR_REGISTER_LSHIFT_IMPL(idx + simd_width * 3);

        idx += simd_width * 4;
    } while (idx != n);

#undef WJR_REGISTER_LSHIFT_IMPL
}

template <typename T>
WJR_INLINE void builtin_lshift_impl(T *dst, const T *src, size_t n, unsigned int c) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    using simd = sse;

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    constexpr auto threshold = std::max<size_t>(simd_width, 4);

    if (WJR_UNLIKELY(n < threshold)) {
        return builtin_unroll_lshift<threshold - 1>(dst, src, n, c);
    }

    WJR_ASSUME(n >= threshold);
    return builtin_simd_lshift_impl(dst, src, n, c);
}

template <typename T>
WJR_INLINE T builtin_lshift(T *dst, const T *src, size_t n, unsigned int c) {
    T ret = src[n - 1] >> (64 - c);
    builtin_lshift_impl(dst + 1, src + 1, n - 1, c);
    dst[0] = src[0] << c;
    return ret;
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T lshift(T *dst, const T *src, size_t n, unsigned int c) {
#if WJR_HAS_BUILTIN(LSHIFT)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_lshift(dst, src, n, c);
        }

        return builtin_lshift(dst, src, n, c);
    } else {
        return fallback_lshift(dst, src, n, c);
    }
#else
    return fallback_lshift(dst, src, n, c);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SHIFT_HPP__