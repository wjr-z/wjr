#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
WJR_INLINE_CONSTEXPR size_t fallback_find_not_val(const T *src, size_t n, T val) {
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
WJR_INLINE size_t builtin_SSE4_1_find_not_val(const T *src, size_t n, T val) {
    WJR_ASSERT(n >= 2);
    WJR_ASSUME(n >= 2);

    size_t idx = 0;

    __m128i y = _mm_set1_epi64x(val);

    do {
        __m128i x = _mm_loadu_si128((__m128i *)(src + idx));
        __m128i r = _mm_cmpeq_epi64(x, y);
        uint16_t mask = _mm_movemask_epi8(r);
        if (mask != 0xFFFF) {
            return idx + (mask == 0x00FF);
        }

        idx += 2;
    } while (idx + 2 <= n);

    if ((n & 1) && src[idx] == val) {
        ++idx;
    }

    return idx;
}

template <typename T>
WJR_INLINE size_t builtin_find_not_val(const T *src, size_t n, T val) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (n <= 7) {
        return fallback_find_not_val(src, n, val);
    }

    // n >= 8

    size_t idx = fallback_find_not_val(src, 4, val);

    if (WJR_LIKELY(idx != 4)) {
        return idx;
    }

    return 4 + builtin_SSE4_1_find_not_val(src - 4, n - 4, val);
}

#endif

template <typename T>
WJR_INLINE_CONSTEXPR size_t find_not_val(const T *src, size_t n, type_identity_t<T> val) {
#if WJR_HAS_BUILTIN(FIND_NOT_VAL)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_val(src, n, val);
        }

        return builtin_find_not_val(src, n, val);
    } else {
        return fallback_find_not_val(src, n, val);
    }
#else
    return fallback_find_not_val(src, n, val);
#endif
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__