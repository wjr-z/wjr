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
WJR_INLINE void builtin_not_n(T *dst, const T *src, size_t n) {

    if (WJR_UNLIKELY(n < 5)) {
        return builtin_unroll_not_n<4>(dst, src, n);
    }

    if (n & 1) {
        dst[0] = ~src[0];

        ++dst;
        ++src;
        --n;
    }

    auto ones = sse::ones();

#define WJR_REGISTER_NOT_N_IMPL(index)                                                   \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::Xor(x, ones);                                                      \
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

    if (n == 0) {
        return;
    }

    size_t idx = 0;

    do {
        WJR_REGISTER_NOT_N_IMPL(idx);
        WJR_REGISTER_NOT_N_IMPL(idx + 2);

        idx += 4;
    } while (idx != n);

#undef WJR_REGISTER_NOT_N_IMPL
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR void not_n(T *dst, const T *src, size_t n) {
#if WJR_HAS_BUILTIN(NOT_N)
    if (is_constant_evaluated()) {
        return fallback_not_n(dst, src, n);
    }

    return builtin_not_n(dst, src, n);
#else
    return fallback_not_n(dst, src, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_NOT_HPP__