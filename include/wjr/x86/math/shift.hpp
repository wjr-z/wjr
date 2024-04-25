#ifndef WJR_X86_MATH_SHIFT_HPP__
#define WJR_X86_MATH_SHIFT_HPP__

#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T shld(T hi, T lo, unsigned int c);

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T shrd(T lo, T hi, unsigned int c);

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(X86_SIMD)
#define WJR_HAS_BUILTIN_LSHIFT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_RSHIFT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(LSHIFT_N) || WJR_HAS_BUILTIN(RSHIFT_N)

template <bool is_constant>
WJR_INTRINSIC_INLINE auto __mm_get_shift(unsigned int c) {
    if constexpr (is_constant) {
        return c;
    } else {
        return simd_cast<unsigned int, __m128i_t>(c);
    }
}

WJR_INTRINSIC_INLINE __m128i __mm_sll_epi64(__m128i x, unsigned int c) {
    return sse::slli_epi64(x, c);
}

WJR_INTRINSIC_INLINE __m128i __mm_sll_epi64(__m128i x, __m128i c) {
    return sse::sll_epi64(x, c);
}

WJR_INTRINSIC_INLINE __m128i __mm_srl_epi64(__m128i x, unsigned int c) {
    return sse::srli_epi64(x, c);
}

WJR_INTRINSIC_INLINE __m128i __mm_srl_epi64(__m128i x, __m128i c) {
    return sse::srl_epi64(x, c);
}

#endif

#if WJR_HAS_BUILTIN(LSHIFT_N)

#define WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(index)                                      \
    do {                                                                                 \
        __m128i x1 = sse::loadu((__m128i *)(src - 3 - (index)));                         \
        x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                \
            simd_cast<__m128i_t, __m128_t>(x1), simd_cast<__m128i_t, __m128_t>(x0)));    \
                                                                                         \
        __m128i r0 = __mm_sll_epi64(x0, y);                                              \
        __m128i r1 = __mm_srl_epi64(x1, z);                                              \
                                                                                         \
        __m128i r = sse::Or(r0, r1);                                                     \
                                                                                         \
        sse::storeu((__m128i *)(dst - 2 - (index)), r);                                  \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

template <bool is_constant, typename T>
void large_builtin_lshift_n_impl(T *dst, const T *src, size_t n, unsigned int c) {
    const auto y = __mm_get_shift<is_constant>(c);
    const auto z = __mm_get_shift<is_constant>(64 - c);

    if (n & 1) {
        dst[-1] = shld(src[-1], src[-2], c);
        --src;
        --dst;
    }

    __m128i x0 = sse::set1_epi64(src[-1]);

    if (n & 2) {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(0);

        src -= 2;
        dst -= 2;
    }

    if (n & 4) {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(2);

        src -= 4;
        dst -= 4;
    }

    size_t idx = n / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return;
    }

    do {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(2);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(4);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(6);

        src -= 8;
        dst -= 8;
        --idx;
    } while (WJR_LIKELY(idx != 0));
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_lshift_n_impl(T *dst, const T *src, size_t n,
                                                unsigned int c) {
    if (WJR_UNLIKELY(n < 4)) {
        switch (n) {
        case 3: {
            dst[2] = shld(src[2], src[1], c);
            WJR_FALLTHROUGH;
        }
        case 2: {
            dst[1] = shld(src[1], src[0], c);
            WJR_FALLTHROUGH;
        }
        case 1: {
            dst[0] = shld(src[0], src[-1], c);
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return;
    }

    src += n;
    dst += n;

    if (WJR_BUILTIN_CONSTANT_P(c)) {
        return large_builtin_lshift_n_impl<true>(dst, src, n, c);
    }

    return large_builtin_lshift_n_impl<false>(dst, src, n, c);
}

#undef WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED

template <typename T>
WJR_INTRINSIC_INLINE T builtin_lshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                        T lo) {
    const T ret = src[n - 1] >> (64 - c);
    builtin_lshift_n_impl(dst + 1, src + 1, n - 1, c);
    dst[0] = shld(src[0], lo, c);
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

#define WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(index)                                      \
    do {                                                                                 \
        __m128i x1 = sse::loadu((__m128i *)(src + 1 + (index)));                         \
        x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                \
            simd_cast<__m128i_t, __m128_t>(x0), simd_cast<__m128i_t, __m128_t>(x1)));    \
                                                                                         \
        __m128i r0 = __mm_srl_epi64(x0, y);                                              \
        __m128i r1 = __mm_sll_epi64(x1, z);                                              \
                                                                                         \
        __m128i r = sse::Or(r0, r1);                                                     \
                                                                                         \
        sse::storeu((__m128i *)(dst + (index)), r);                                      \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

template <bool is_constant, typename T>
void large_builtin_rshift_n_impl(T *dst, const T *src, size_t n, unsigned int c) {
    const auto y = __mm_get_shift<is_constant>(c);
    const auto z = __mm_get_shift<is_constant>(64 - c);

    if (n & 1) {
        dst[0] = shrd(src[0], src[1], c);
        ++src;
        ++dst;
    }

    __m128i x0 = sse::set1_epi64(src[0]);

    if (n & 2) {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(0);

        src += 2;
        dst += 2;
    }

    if (n & 4) {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(2);

        src += 4;
        dst += 4;
    }

    size_t idx = n / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return;
    }

    do {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(2);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(4);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(6);

        dst += 8;
        src += 8;
        --idx;
    } while (WJR_LIKELY(idx != 0));
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_rshift_n_impl(T *dst, const T *src, size_t n,
                                                unsigned int c) {
    if (WJR_UNLIKELY(n < 4)) {
        dst += n - 3;
        src += n - 3;

        switch (n) {
        case 3: {
            dst[0] = shrd(src[0], src[1], c);
            WJR_FALLTHROUGH;
        }
        case 2: {
            dst[1] = shrd(src[1], src[2], c);
            WJR_FALLTHROUGH;
        }
        case 1: {
            dst[2] = shrd(src[2], src[3], c);
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(c)) {
        return large_builtin_rshift_n_impl<true>(dst, src, n, c);
    }

    return large_builtin_rshift_n_impl<false>(dst, src, n, c);
}

#undef WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED

template <typename T>
WJR_INTRINSIC_INLINE T builtin_rshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                        T hi) {
    const T ret = src[0] << (64 - c);
    builtin_rshift_n_impl(dst, src, n - 1, c);
    dst[n - 1] = shrd(src[n - 1], hi, c);
    return ret;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_SHIFT_HPP__