#ifndef WJR_MATH_NOT_HPP__
#define WJR_MATH_NOT_HPP__

#include <wjr/simd/simd.hpp>
#include <wjr/unroll.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_not_n(T *dst, const T *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(NOT_N)

template <size_t unroll, typename T>
WJR_INTRINSIC_INLINE void builtin_unroll_not_n(T *dst, const T *src, size_t n) {
    (void)unroll_call<unroll>([dst, src, n](auto ic) -> std::optional<empty> {
        constexpr auto idx = decltype(ic)::value;

        if (idx == n) {
            return empty{};
        }

        dst[idx] = ~src[idx];
        return std::nullopt;
    });
}

template <typename T>
WJR_INLINE void builtin_simd_not_n(T *dst, const T *src, size_t n) {
    WJR_ASSUME(n >= 2);

    {
        size_t k = n % 2;
        builtin_unroll_not_n<1>(dst, src, k);

        dst += k;
        src += k;
        n -= k;
    }

    WJR_ASSUME(n % 2 == 0);
    WJR_ASSUME(n != 0);

    auto ones = sse::ones();

#define WJR_REGISTER_NOT_N_IMPL(index)                                                   \
    do {                                                                                 \
        __m128i x = sse::loadu((__m128i *)(src + (index)));                              \
        __m128i r = sse::Xor(x, ones);                                                   \
                                                                                         \
        sse::storeu((__m128i *)(dst + (index)), r);                                      \
                                                                                         \
    } while (0)

    if (n & 2) {
        WJR_REGISTER_NOT_N_IMPL(0);

        dst += 2;
        src += 2;
        n -= 2;
    }

    if (n & 4) {
        WJR_REGISTER_NOT_N_IMPL(0);
        WJR_REGISTER_NOT_N_IMPL(2);

        dst += 4;
        src += 4;
        n -= 4;
    }

    if (!n) {
        return;
    }

    WJR_ASSUME(n % 8 == 0);

    size_t idx = 0;

    do {
        WJR_REGISTER_NOT_N_IMPL(idx);
        WJR_REGISTER_NOT_N_IMPL(idx + 2);
        WJR_REGISTER_NOT_N_IMPL(idx + 4);
        WJR_REGISTER_NOT_N_IMPL(idx + 6);

        idx += 8;
    } while (idx != n);

#undef WJR_REGISTER_NOT_N_IMPL
}

template <typename T>
WJR_INLINE void builtin_not_n(T *dst, const T *src, size_t n) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (WJR_UNLIKELY(n < 4)) {
        return builtin_unroll_not_n<3>(dst, src, n);
    }

    return builtin_simd_not_n(dst, src, n);
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR void not_n(T *dst, const T *src, size_t n) {
#if WJR_HAS_BUILTIN(NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return builtin_not_n(dst, src, n);
        }

        return builtin_not_n(dst, src, n);
    } else {
        return builtin_not_n(dst, src, n);
    }
#else
    return fallback_not_n(dst, src, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_NOT_HPP__