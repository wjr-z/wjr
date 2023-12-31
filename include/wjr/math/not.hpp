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

template <typename simd, typename T>
WJR_INLINE void builtin_simd_not_n(T *dst, const T *src, size_t n) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    using simd_int = typename simd::int_type;

    WJR_ASSUME(n >= simd_width);

    {
        size_t k = n % simd_width;
        builtin_unroll_not_n<simd_width - 1>(dst, src, k);

        dst += k;
        src += k;
        n -= k;
    }

    WJR_ASSUME(n % simd_width == 0);
    WJR_ASSUME(n != 0);

    auto ones = simd::ones();

#define WJR_REGISTER_NOT_N_IMPL(index)                                                   \
    do {                                                                                 \
        simd_int x = simd::loadu((simd_int *)(src + (index)));                           \
        simd_int r = simd::Xor(x, ones);                                                 \
                                                                                         \
        simd::storeu((simd_int *)(dst + (index)), r);                                    \
                                                                                         \
    } while (0)

    {
        WJR_REGISTER_NOT_N_IMPL(0);

        dst += simd_width;
        src += simd_width;
        n -= simd_width;
    }

    if (!n) {
        return;
    }

    if ((n / simd_width) & 1) {
        WJR_REGISTER_NOT_N_IMPL(0);

        dst += simd_width;
        src += simd_width;
        n -= simd_width;
    }

    if ((n / (simd_width * 2)) & 1) {
        WJR_REGISTER_NOT_N_IMPL(0);
        WJR_REGISTER_NOT_N_IMPL(simd_width);

        dst += simd_width * 2;
        src += simd_width * 2;
        n -= simd_width * 2;
    }

    if (!n) {
        return;
    }

    WJR_ASSUME(n % (simd_width * 4) == 0);

    WJR_REGISTER_NOT_N_IMPL(0);
    WJR_REGISTER_NOT_N_IMPL(simd_width);
    WJR_REGISTER_NOT_N_IMPL(simd_width * 2);
    WJR_REGISTER_NOT_N_IMPL(simd_width * 3);

    n -= simd_width * 4;

    if (WJR_UNLIKELY(!n)) {
        return;
    }

    size_t idx = 0;

    do {
        WJR_REGISTER_NOT_N_IMPL(idx);
        WJR_REGISTER_NOT_N_IMPL(idx + simd_width);
        WJR_REGISTER_NOT_N_IMPL(idx + simd_width * 2);
        WJR_REGISTER_NOT_N_IMPL(idx + simd_width * 3);

        idx += simd_width * 4;
    } while (idx != n);

    return;

#undef WJR_REGISTER_NOT_N_IMPL
}

template <typename T>
WJR_INLINE void builtin_not_n(T *dst, const T *src, size_t n) {
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t.");

#if WJR_HAS_SIMD(AVX2)
    using simd = avx;
#else
    using simd = sse;
#endif

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    constexpr auto threshold = std::max<size_t>(simd_width, 4);

    if (WJR_UNLIKELY(n < threshold)) {
        return builtin_unroll_not_n<threshold - 1>(dst, src, n);
    }

    WJR_ASSUME(n >= threshold);
    return builtin_simd_not_n<simd>(dst, src, n);
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