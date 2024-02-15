#ifndef WJR_X86_SHIFT_HPP__
#define WJR_X86_SHIFT_HPP__

#include <wjr/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T shld(T hi, T lo, unsigned int c);

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T shrd(T lo, T hi, unsigned int c);

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_LSHIFT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_RSHIFT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(LSHIFT_N)

#define WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(index)                                   \
    do {                                                                                 \
        __m128i x1 = sse::loadu((__m128i *)(src - 3 - (index)));                         \
        x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                \
            simd_cast<__m128i_t, __m128_t>(x1), simd_cast<__m128i_t, __m128_t>(x0)));    \
                                                                                         \
        __m128i r0 = sse::sll_epi64(x0, y);                                              \
        __m128i r1 = sse::srl_epi64(x1, z);                                              \
                                                                                         \
        __m128i r = sse::Or(r0, r1);                                                     \
                                                                                         \
        sse::storeu((__m128i *)(dst - 2 - (index)), r);                                  \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)
#define WJR_REGISTER_LSHIFT_N_IMPL_I2(index)                                             \
    __m128i y = simd_cast<uint32_t, __m128i_t>(c);                                       \
    __m128i z = simd_cast<uint32_t, __m128i_t>(64 - c);                                  \
    __m128i x0 = sse::set1_epi64(src[-1 - (index)]);

template <typename T>
void builtin_lshift_n_impl(T *dst, const T *src, size_t n, unsigned int c) {
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
        }

        return;
    }

    dst += n;
    src += n;

    size_t idx = 0;

    if (n & 1) {
        dst[-1] = shld(src[-1], src[-2], c);

        idx += 1;
    }

    WJR_REGISTER_LSHIFT_N_IMPL_I2(idx);

    if (n & 2) {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx);

        idx += 2;
    }

    if (n & 4) {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx + 2);

        idx += 4;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return;
    }

    WJR_ASSUME((n - idx) % 8 == 0);

    do {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx + 2);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx + 4);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2(idx + 6);

        idx += 8;
    } while (idx != n);
}

#undef WJR_REGISTER_LSHIFT_N_IMPL_I2
#undef WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED_L2

template <typename T>
T builtin_lshift_n(T *dst, const T *src, size_t n, unsigned int c, T lo) {
    T ret = src[n - 1] >> (64 - c);
    builtin_lshift_n_impl(dst + 1, src + 1, n - 1, c);
    dst[0] = shld(src[0], lo, c);
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

#define WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(index)                                   \
    do {                                                                                 \
        __m128i x1 = sse::loadu((__m128i *)(src + 1 + (index)));                         \
        x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                \
            simd_cast<__m128i_t, __m128_t>(x0), simd_cast<__m128i_t, __m128_t>(x1)));    \
                                                                                         \
        __m128i r0 = sse::srl_epi64(x0, y);                                              \
        __m128i r1 = sse::sll_epi64(x1, z);                                              \
                                                                                         \
        __m128i r = sse::Or(r0, r1);                                                     \
                                                                                         \
        sse::storeu((__m128i *)(dst + (index)), r);                                      \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)
#define WJR_REGISTER_RSHIFT_N_IMPL_I2(index)                                             \
    __m128i y = simd_cast<uint32_t, __m128i_t>(c);                                       \
    __m128i z = simd_cast<uint32_t, __m128i_t>(64 - c);                                  \
    __m128i x0 = sse::set1_epi64(src[(index)]);

template <typename T>
void builtin_rshift_n_impl(T *dst, const T *src, size_t n, unsigned int c) {
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
        }

        return;
    }

    size_t idx = 0;

    if (n & 1) {
        dst[0] = shrd(src[0], src[1], c);

        idx += 1;
    }

    WJR_REGISTER_RSHIFT_N_IMPL_I2(idx);

    if (n & 2) {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx);

        idx += 2;
    }

    if (n & 4) {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx + 2);

        idx += 4;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return;
    }

    WJR_ASSUME((n - idx) % 8 == 0);

    do {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx + 2);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx + 4);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2(idx + 6);

        idx += 8;
    } while (idx != n);
}

#undef WJR_REGISTER_RSHIFT_N_IMPL_I2
#undef WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED_L2

template <typename T>
T builtin_rshift_n(T *dst, const T *src, size_t n, unsigned int c, T hi) {
    T ret = src[0] << (64 - c);
    builtin_rshift_n_impl(dst, src, n - 1, c);
    dst[n - 1] = shrd(src[n - 1], hi, c);
    return ret;
}

#endif

} // namespace wjr

#endif // WJR_X86_SHIFT_HPP__