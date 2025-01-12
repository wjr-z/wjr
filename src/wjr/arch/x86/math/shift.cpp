#include <wjr/arch/x86/math/shift.hpp>
#include <wjr/math/shift.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(LSHIFT_N) || WJR_HAS_BUILTIN(RSHIFT_N)

/// @private
template <bool is_constant>
WJR_INTRINSIC_INLINE auto __mm_get_shift(unsigned int c) noexcept {
    if constexpr (is_constant) {
        return c;
    } else {
        return simd_cast<unsigned int, __m128i_t>(c);
    }
}

/// @private
WJR_INTRINSIC_INLINE __m128i __mm_sll_epi64(__m128i x, unsigned int c) noexcept {
    return sse::slli_epi64(x, c);
}

/// @private
WJR_INTRINSIC_INLINE __m128i __mm_sll_epi64(__m128i x, __m128i c) noexcept {
    return sse::sll_epi64(x, c);
}

/// @private
WJR_INTRINSIC_INLINE __m128i __mm_srl_epi64(__m128i x, unsigned int c) noexcept {
    return sse::srli_epi64(x, c);
}

/// @private
WJR_INTRINSIC_INLINE __m128i __mm_srl_epi64(__m128i x, __m128i c) noexcept {
    return sse::srl_epi64(x, c);
}

#endif

#if WJR_HAS_BUILTIN(LSHIFT_N)

    #define WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(index)                                            \
        do {                                                                                       \
            __m128i x1 = sse::loadu(src - 3 - (index));                                            \
            x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<_MM_SHUFFLE(1, 0, 3, 2)>( \
                simd_cast<__m128i_t, __m128_t>(x1), simd_cast<__m128i_t, __m128_t>(x0)));          \
                                                                                                   \
            __m128i r0 = __mm_sll_epi64(x0, y);                                                    \
            __m128i r1 = __mm_srl_epi64(x1, z);                                                    \
                                                                                                   \
            __m128i r = sse::Or(r0, r1);                                                           \
                                                                                                   \
            sse::storeu(dst - 2 - (index), r);                                                     \
                                                                                                   \
            x0 = x1;                                                                               \
        } while (false)

template <bool is_constant, typename T>
WJR_ALL_NONNULL void large_builtin_lshift_n_impl(T *dst, const T *src, size_t n,
                                                 unsigned int c) noexcept {
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

template void large_builtin_lshift_n_impl<false, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                           size_t n, unsigned int c) noexcept;
template void large_builtin_lshift_n_impl<true, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                          size_t n, unsigned int c) noexcept;

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

    #define WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(index)                                            \
        do {                                                                                       \
            __m128i x1 = sse::loadu(src + 1 + (index));                                            \
            x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                      \
                simd_cast<__m128i_t, __m128_t>(x0), simd_cast<__m128i_t, __m128_t>(x1)));          \
                                                                                                   \
            __m128i r0 = __mm_srl_epi64(x0, y);                                                    \
            __m128i r1 = __mm_sll_epi64(x1, z);                                                    \
                                                                                                   \
            __m128i r = sse::Or(r0, r1);                                                           \
                                                                                                   \
            sse::storeu(dst + (index), r);                                                         \
                                                                                                   \
            x0 = x1;                                                                               \
        } while (false)

template <bool is_constant, typename T>
WJR_ALL_NONNULL void large_builtin_rshift_n_impl(T *dst, const T *src, size_t n,
                                                 unsigned int c) noexcept {
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

template void large_builtin_rshift_n_impl<false, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                           size_t n, unsigned int c) noexcept;
template void large_builtin_rshift_n_impl<true, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                          size_t n, unsigned int c) noexcept;

    #undef WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED

#endif

} // namespace wjr