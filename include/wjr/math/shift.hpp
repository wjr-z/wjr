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
WJR_INLINE void builtin_unroll_rshift_impl_n(T *dst, const T *src, size_t n,
                                             unsigned int c) {
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
WJR_INLINE void builtin_rshift_impl(T *dst, const T *src, size_t n, unsigned int c) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (WJR_UNLIKELY(n < 5)) {
        return builtin_unroll_rshift_impl_n<4>(dst, src, n, c);
    }

    size_t m = n & 1;
    builtin_unroll_rshift_impl_n<1>(dst, src, m, c);

    dst += m;
    src += m;
    n -= m;

    auto y = simd_cast<uint32_t, __m128i_tag>(c);
    auto z = simd_cast<uint32_t, __m128i_tag>(64 - c);

    auto x0 = sse::set1(src[0], T());
    __m128i x1, r0, r1, r;

#define WJR_REGISTER_RSHIFT_IMPL(index)                                                  \
    do {                                                                                 \
        auto x1 = sse::loadu((__m128i *)(src + 1 + (index)));                            \
        x0 = simd_cast<__m128_tag, __m128i_tag>(                                         \
            sse::shuffle_ps<78>(simd_cast<__m128i_tag, __m128_tag>(x0),                  \
                                simd_cast<__m128i_tag, __m128_tag>(x1)));                \
                                                                                         \
        auto r0 = sse::srl(x0, y, T());                                                  \
        auto r1 = sse::sll(x1, z, T());                                                  \
                                                                                         \
        auto r = sse::Or(r0, r1);                                                        \
                                                                                         \
        sse::storeu((__m128i *)(dst + (index)), r);                                      \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

    if (n & 2) {
        WJR_REGISTER_RSHIFT_IMPL(0);

        dst += 2;
        src += 2;
        n -= 2;
    }

    if (n == 0) {
        return;
    }

    size_t idx = 0;

    do {
        WJR_REGISTER_RSHIFT_IMPL(idx);
        WJR_REGISTER_RSHIFT_IMPL(idx + 2);

        idx += 4;
    } while (idx != n);

#undef WJR_REGISTER_RSHIFT_IMPL
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
WJR_INLINE void builtin_unroll_lshift_impl_n(T *dst, const T *src, size_t n,
                                             unsigned int c) {
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
WJR_INLINE void builtin_lshift_impl(T *dst, const T *src, size_t n, unsigned int c) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    dst += n;
    src += n;

    if (WJR_UNLIKELY(n < 5)) {
        return builtin_unroll_lshift_impl_n<4>(dst, src, n, c);
    }

    size_t m = n & 1;
    builtin_unroll_lshift_impl_n<1>(dst, src, m, c);

    dst -= m;
    src -= m;
    n -= m;

    auto y = simd_cast<uint32_t, __m128i_tag>(c);
    auto z = simd_cast<uint32_t, __m128i_tag>(64 - c);

    auto x0 = sse::set1(src[-1], T());
    __m128i x1, r0, r1, r;

#define WJR_REGISTER_LSHIFT_IMPL(index)                                                  \
    do {                                                                                 \
        auto x1 = sse::loadu((__m128i *)(src - 3 - (index)));                            \
        x0 = simd_cast<__m128_tag, __m128i_tag>(                                         \
            sse::shuffle_ps<78>(simd_cast<__m128i_tag, __m128_tag>(x1),                  \
                                simd_cast<__m128i_tag, __m128_tag>(x0)));                \
                                                                                         \
        auto r0 = sse::sll(x0, y, T());                                                  \
        auto r1 = sse::srl(x1, z, T());                                                  \
                                                                                         \
        auto r = sse::Or(r0, r1);                                                        \
                                                                                         \
        sse::storeu((__m128i *)(dst - 2 - (index)), r);                                  \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

    if (n & 2) {
        WJR_REGISTER_LSHIFT_IMPL(0);

        dst -= 2;
        src -= 2;
        n -= 2;
    }

    if (n == 0) {
        return;
    }

    size_t idx = 0;

    do {
        WJR_REGISTER_LSHIFT_IMPL(idx);
        WJR_REGISTER_LSHIFT_IMPL(idx + 2);

        idx += 4;
    } while (idx != n);

#undef WJR_REGISTER_LSHIFT_IMPL
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