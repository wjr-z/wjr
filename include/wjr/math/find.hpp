#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/simd/simd.hpp>
#include <wjr/unroll.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not(const T *src, size_t n, T val) {
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

    if (p <= 22) {
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

    switch (p & 6) {
    case 6: {
        WJR_REGISTER_FIND_NOT_IMPL(8);
        WJR_REGISTER_FIND_NOT_IMPL(10);
        WJR_REGISTER_FIND_NOT_IMPL(12);

        src += 14;
        p -= 14;
        break;
    }
    case 4: {
        WJR_REGISTER_FIND_NOT_IMPL(8);
        WJR_REGISTER_FIND_NOT_IMPL(10);

        src += 12;
        p -= 12;
        break;
    }
    case 2: {
        WJR_REGISTER_FIND_NOT_IMPL(8);

        src += 10;
        p -= 10;
        break;
    }
    case 0: {
        src += 8;
        p -= 8;
        break;
    }
    default: {
        WJR_UNREACHABLE;
        break;
    }
    }

    WJR_ASSUME(p % 8 == 0);

    do {
        WJR_REGISTER_FIND_NOT_IMPL(0);
        WJR_REGISTER_FIND_NOT_IMPL(2);
        WJR_REGISTER_FIND_NOT_IMPL(4);
        WJR_REGISTER_FIND_NOT_IMPL(6);

        src += 8;
        p -= 8;
    } while (p);

    return n;

#undef WJR_REGISTER_FIND_NOT_IMPL
}

template <size_t unroll, typename T>
WJR_INTRINSIC_INLINE size_t builtin_unroll_find_not(const T *src, T val) {
    auto fn = [src, val](auto ic) -> std::optional<size_t> {
        constexpr size_t idx = decltype(ic)::value;
        if (WJR_LIKELY(src[idx] != val)) {
            return idx;
        }

        return std::nullopt;
    };

    auto idx = unroll_call<unroll>(fn);
    if (idx.has_value()) {
        return idx.value();
    }

    return unroll;
}

template <size_t unroll, typename T>
WJR_INTRINSIC_INLINE size_t builtin_unroll_find_not(const T *src, size_t n, T val) {
    auto fn = [src, n, val](auto ic) -> std::optional<size_t> {
        constexpr size_t idx = decltype(ic)::value;

        if (idx == n || WJR_LIKELY(src[idx] != val)) {
            return idx;
        }

        return std::nullopt;
    };

    auto idx = unroll_call<unroll>(fn);
    if (idx.has_value()) {
        return idx.value();
    }

    return unroll;
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not(const T *src, size_t n, T val) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (WJR_UNLIKELY(n < 6)) {
        return builtin_unroll_find_not<5>(src, n, val);
    }

    // n >= 6

    size_t idx = builtin_unroll_find_not<2>(src, val);

    if (WJR_LIKELY(idx != 2)) {
        return idx;
    }

    return 2 + builtin_SSE4_1_find_not(src + 2, n - 2, val);
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t find_not(const T *src, size_t n, type_identity_t<T> val) {
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