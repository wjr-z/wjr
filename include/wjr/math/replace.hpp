#ifndef WJR_MATH_REPLACE_HPP__
#define WJR_MATH_REPLACE_HPP__

#include <cstring>

#include <wjr/math/broadcast.hpp>
#include <wjr/math/find.hpp>

namespace wjr {

template <typename T>
WJR_INLINE_CONSTEXPR size_t fallback_replace_val_nomemset(T *dst, const T *src, size_t n,
                                                          T from, T to) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != from) {
            break;
        }

        dst[idx] = to;
    }

    return idx;
}

template <typename T>
WJR_INLINE size_t fallback_replace_val_memset(T *dst, const T *src, size_t n, T from,
                                              uint8_t to) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != from) {
            break;
        }
    }

    memset(dst, to, sizeof(T) * idx);
    return idx;
}

template <typename T>
WJR_INLINE_CONSTEXPR size_t fallback_replace_val(T *dst, const T *src, size_t n, T from,
                                                 T to) {
    if (is_constant_evaluated() || (is_constant_p(n <= 8) && n <= 8)) {
        return fallback_replace_val_nomemset(dst, src, n, from, to);
    }

    uint8_t set = to;

    if (broadcast<T, uint8_t>(set) == to) {
        return fallback_replace_val_memset(dst, src, n, from, set);
    }

    return fallback_replace_val_nomemset(dst, src, n, from, to);
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_REPLACE_VAL WJR_HAS_DEF
#define WJR_HAS_BUILTIN_SSE4_1_REPLACE_VAL WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(REPLACE_VAL)

template <typename T>
WJR_INLINE size_t builtin_SSE4_1_replace_val(T *dst, const T *src, size_t n, T from,
                                             T to) {
    size_t idx = 0;
    __m128i y = _mm_set1_epi64x(from);
    __m128i z = _mm_set1_epi64x(to);

    do {
        __m128i x = _mm_loadu_si128((__m128i *)(src + idx));
        __m128i r = _mm_cmpeq_epi64(x, y);
        uint16_t mask = _mm_movemask_epi8(r);
        if (mask != 0xFFFF) {
            if (mask != 0x00FF) {
                return idx;
            }

            dst[idx] = to;
            return idx + 1;
        }

        _mm_storeu_si128((__m128i *)(dst + idx), z);

        idx += 2;
    } while (idx + 2 <= n);

    if ((n & 1) && src[idx] == from) {
        dst[idx] = to;
        ++idx;
    }

    return idx;
}

template <typename T>
WJR_INLINE size_t builtin_replace_val(T *dst, const T *src, size_t n, T from, T to) {
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t.");

    if (n <= 7) {
        return fallback_replace_val(dst, src, n, from, to);
    }

    // n >= 8

    size_t idx = fallback_replace_val(dst, src, 4, from, to);

    if (WJR_LIKELY(idx != 4)) {
        return idx;
    }

    return 4 + builtin_SSE4_1_replace_val(dst + 4, src + 4, n - 4, from, to);
}

#endif

template <typename T>
WJR_INLINE_CONSTEXPR size_t replace_val(T *dst, const T *src, size_t n,
                                        type_identity_t<T> from, type_identity_t<T> to) {
    if (from == to && src == dst) {
        return find_not_val(src, n, from);
    }

#if WJR_HAS_BUILTIN(REPLACE_VAL)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_replace_val(dst, src, n, from, to);
        }

        return builtin_replace_val(dst, src, n, from, to);
    } else {
        return fallback_replace_val(dst, src, n, from, to);
    }
#else
    return fallback_replace_val(dst, src, n, from, to);
#endif
}

} // namespace wjr

#endif // WJR_MATH_REPLACE_HPP__