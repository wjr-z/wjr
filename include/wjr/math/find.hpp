#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
WJR_INLINE_CONSTEXPR size_t fallback_find_not(const T *src, size_t n, T val) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            break;
        }
    }

    return idx;
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_FIND_NOT_VAL WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_NOT_VAL)

template <typename T>
WJR_INLINE size_t builtin_SSE4_1_find_not(const T *src, const size_t n, T val) {
    WJR_ASSUME(n >= 4);

    size_t p = n;

    if (p & 1) {
        if (src[0] != val) {
            return 0;
        }

        ++src;
        --p;
    }

    __m128i y = _mm_set1_epi64x(val);

#define WJR_REGISTER_FIND_NOT_IMPL(index)                                                \
    do {                                                                                 \
        __m128i x = _mm_loadu_si128((__m128i *)(src + index));                           \
        __m128i r = _mm_cmpeq_epi64(x, y);                                               \
        uint16_t mask = _mm_movemask_epi8(r);                                            \
        if (mask != 0xFFFF) {                                                            \
            return (n - p) + index + (mask == 0x00FF);                                   \
        }                                                                                \
    } while (0)

    if (p <= 14) {
        do {
            WJR_REGISTER_FIND_NOT_IMPL(0);

            src += 2;
            p -= 2;
        } while (p);

        return n;
    }

    WJR_REGISTER_FIND_NOT_IMPL(0);
    WJR_REGISTER_FIND_NOT_IMPL(2);
    WJR_REGISTER_FIND_NOT_IMPL(4);
    WJR_REGISTER_FIND_NOT_IMPL(6);

    if (p & 2) {
        WJR_REGISTER_FIND_NOT_IMPL(8);
        src += 10;
        p -= 10;
    } else {
        src -= 8;
        p -= 8;
    }

    WJR_ASSUME(p % 4 == 0);

    do {
        WJR_REGISTER_FIND_NOT_IMPL(0);
        WJR_REGISTER_FIND_NOT_IMPL(2);

        src += 4;
        p -= 4;
    } while (p);

    return n;

#undef WJR_REGISTER_FIND_NOT_IMPL
}

template <typename T>
WJR_INLINE size_t builtin_find_not(const T *src, size_t n, T val) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (n <= 7) {
        return fallback_find_not(src, n, val);
    }

    // n >= 8

    size_t idx = fallback_find_not(src, 4, val);

    if (WJR_LIKELY(idx != 4)) {
        return idx;
    }

    return 4 + builtin_SSE4_1_find_not(src - 4, n - 4, val);
}

#endif

template <typename T>
WJR_INLINE_CONSTEXPR size_t find_not(const T *src, size_t n, type_identity_t<T> val) {
#if WJR_HAS_BUILTIN(FIND_NOT_VAL)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not(src, n, val);
        }

        return builtin_find_not(src, n, val);
    } else {
        return fallback_find_not(src, n, val);
    }
#else
    return fallback_find_not(src, n, val);
#endif
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__