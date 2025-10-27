/**
 * @file shift.cpp
 * @author wjr
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @todo More optimization for some arch. Maybe only use SSE on AMD.
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <wjr/arch/x86/math/shift.hpp>
#include <wjr/arch/x86/simd/simd.hpp>
#include <wjr/math/shift.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(LSHIFT_N) || WJR_HAS_BUILTIN(RSHIFT_N)

namespace {
template <typename simd>
struct simd_shift;

template <>
struct simd_shift<sse> {
    template <bool is_constant>
    WJR_INTRINSIC_INLINE static auto get_shift(unsigned int cl) noexcept {
        if constexpr (is_constant) {
            return cl;
        } else {
            return simd_cast<unsigned int, _m128i_t>(cl);
        }
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i x, unsigned int cl) noexcept {
        return sse::slli_epi64(x, cl);
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i x, __m128i cl) noexcept {
        return sse::sll_epi64(x, cl);
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i x, unsigned int cl) noexcept {
        return sse::srli_epi64(x, cl);
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i x, __m128i cl) noexcept {
        return sse::srl_epi64(x, cl);
    }

    WJR_INTRINSIC_INLINE static __m128i mergel(__m128i lo, __m128i hi) noexcept {
        return sse::template alignr<8>(hi, lo);
    }

    WJR_INTRINSIC_INLINE static __m128i merger(__m128i lo, __m128i hi) noexcept {
        return sse::template alignr<8>(hi, lo);
    }
};

    #if WJR_HAS_SIMD(AVX2)

template <>
struct simd_shift<avx> {
    template <bool is_constant>
    WJR_INTRINSIC_INLINE static auto get_shift(unsigned int cl) noexcept {
        if constexpr (is_constant) {
            return cl;
        } else {
            return broadcast<uint64_t, _m256i_t>(cl);
        }
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i x, unsigned int cl) noexcept {
        return avx::slli_epi64(x, cl);
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i x, __m256i cl) noexcept {
        return avx::sllv_epi64(x, cl);
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i x, unsigned int cl) noexcept {
        return avx::srli_epi64(x, cl);
    }

    /// @private
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i x, __m256i cl) noexcept {
        return avx::srlv_epi64(x, cl);
    }

    WJR_INTRINSIC_INLINE static __m256i mergel(__m256i lo, __m256i hi) noexcept {
        return _mm256_permute4x64_epi64(_mm256_blend_epi32(lo, hi, 0b0000'0011), 0b00'11'10'01);
    }

    WJR_INTRINSIC_INLINE static __m256i merger(__m256i lo, __m256i hi) noexcept {
        return _mm256_permute4x64_epi64(_mm256_blend_epi32(hi, lo, 0b1100'0000), 0b10'01'00'11);
    }
};

    #endif

} // namespace

#endif

#if WJR_HAS_BUILTIN(LSHIFT_N)

template <bool is_constant, typename T>
void large_builtin_lshift_n_impl(T *dst, const T *src, size_t n, unsigned int cl) noexcept {
    #define WJR_REGISTER_LSHIFT_N_IMPL(index, LAST, NOW)                                           \
        do {                                                                                       \
            LAST = simd_shift<simd>::mergel(NOW, LAST);                                            \
            auto r0 = simd_shift<simd>::sll(LAST, y);                                              \
            auto r1 = simd_shift<simd>::srl(NOW, z);                                               \
            LAST = simd::Or(r0, r1);                                                               \
        } while (0)

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    WJR_ASSUME(n >= shift_detail::small_shift_n_threshold);

    unsigned int tcl = 64 - cl;
    if (size_t idx = n & (type_width - 1); idx) {
    #if WJR_HAS_SIMD(AVX2)
        static_assert(type_width == 4);
        do {
            dst[-1] = shld(src[-1], src[-2], cl);
            if (idx == 1)
                break;
            dst[-2] = shld(src[-2], src[-3], cl);
            if (idx == 2)
                break;
            dst[-3] = shld(src[-3], src[-4], cl);
        } while (0);
        dst -= idx;
        src -= idx;
    #else
        dst[-1] = shld(src[-1], src[-2], cl);
        --dst;
        --src;
    #endif
    }

    const auto y = simd_shift<simd>::template get_shift<is_constant>(cl);
    const auto z = simd_shift<simd>::template get_shift<is_constant>(tcl);
    typename simd::int_type x0 = simd_cast<T, typename simd::int_tag_type>(src[-1]);

    if (n & type_width) {
        const auto x1 = simd::loadu(src - (type_width + 1));
        WJR_REGISTER_LSHIFT_N_IMPL(0, x0, x1);
        simd::storeu(dst - type_width, x0);
        x0 = x1;

        dst -= type_width;
        src -= type_width;
    }

    if (n & (type_width * 2)) {
        auto x1 = simd::loadu(src - (type_width + 1));
        const auto x2 = simd::loadu(src - (type_width + 1) - (type_width));
        WJR_REGISTER_LSHIFT_N_IMPL(0, x0, x1);
        WJR_REGISTER_LSHIFT_N_IMPL(type_width, x1, x2);
        simd::storeu(dst - type_width, x0);
        simd::storeu(dst - type_width * 2, x1);
        x0 = x2;

        dst -= type_width * 2;
        src -= type_width * 2;
    }

    size_t idx = n / (type_width * 4);
    if (WJR_UNLIKELY(idx == 0)) {
        return;
    }

    // todo : unroll
    do {
        auto x1 = simd::loadu(src - (type_width + 1));
        auto x2 = simd::loadu(src - (type_width + 1) - (type_width));
        auto x3 = simd::loadu(src - (type_width + 1) - (type_width * 2));
        const auto x4 = simd::loadu(src - (type_width + 1) - (type_width * 3));
        WJR_REGISTER_LSHIFT_N_IMPL(0, x0, x1);
        WJR_REGISTER_LSHIFT_N_IMPL(type_width, x1, x2);
        WJR_REGISTER_LSHIFT_N_IMPL(type_width * 2, x2, x3);
        WJR_REGISTER_LSHIFT_N_IMPL(type_width * 3, x3, x4);
        simd::storeu(dst - type_width, x0);
        simd::storeu(dst - type_width * 2, x1);
        simd::storeu(dst - type_width * 3, x2);
        simd::storeu(dst - type_width * 4, x3);
        x0 = x4;

        dst -= type_width * 4;
        src -= type_width * 4;
        --idx;
    } while (WJR_LIKELY(idx != 0));

    #undef WJR_REGISTER_LSHIFT_N_IMPL
}

template void large_builtin_lshift_n_impl<false, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                           size_t n, unsigned int cl) noexcept;
template void large_builtin_lshift_n_impl<true, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                          size_t n, unsigned int cl) noexcept;

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

template <bool is_constant, typename T>
WJR_ALL_NONNULL void large_builtin_rshift_n_impl(T *dst, const T *src, size_t n,
                                                 unsigned int cl) noexcept {
    #define WJR_REGISTER_RSHIFT_N_IMPL(index, LAST, NOW)                                           \
        do {                                                                                       \
            LAST = simd_shift<simd>::merger(LAST, NOW);                                            \
            auto r0 = simd_shift<simd>::srl(LAST, y);                                              \
            auto r1 = simd_shift<simd>::sll(NOW, z);                                               \
            LAST = simd::Or(r0, r1);                                                               \
        } while (0)

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    WJR_ASSUME(n >= shift_detail::small_shift_n_threshold);

    unsigned int tcl = 64 - cl;
    if (size_t idx = n & (type_width - 1); idx) {
    #if WJR_HAS_SIMD(AVX2)
        static_assert(type_width == 4);
        do {
            dst[0] = shrd(src[0], src[1], cl);
            if (idx == 1)
                break;
            dst[1] = shrd(src[1], src[2], cl);
            if (idx == 2)
                break;
            dst[2] = shrd(src[2], src[3], cl);
        } while (0);
        dst += idx;
        src += idx;
    #else
        dst[0] = shrd(src[0], src[1], cl);
        ++dst;
        ++src;
    #endif
    }

    const auto y = simd_shift<simd>::template get_shift<is_constant>(cl);
    const auto z = simd_shift<simd>::template get_shift<is_constant>(tcl);
    typename simd::int_type x0 = broadcast<T, typename simd::int_tag_type>(src[0]);

    if (n & type_width) {
        const auto x1 = simd::loadu(src + 1);
        WJR_REGISTER_RSHIFT_N_IMPL(0, x0, x1);
        simd::storeu(dst, x0);
        x0 = x1;

        dst += type_width;
        src += type_width;
    }

    if (n & (type_width * 2)) {
        auto x1 = simd::loadu(src + 1);
        const auto x2 = simd::loadu(src + 1 + type_width);
        WJR_REGISTER_RSHIFT_N_IMPL(0, x0, x1);
        WJR_REGISTER_RSHIFT_N_IMPL(type_width, x1, x2);
        simd::storeu(dst, x0);
        simd::storeu(dst + type_width, x1);
        x0 = x2;

        dst += type_width * 2;
        src += type_width * 2;
    }

    size_t idx = n / (type_width * 4);
    if (WJR_UNLIKELY(idx == 0)) {
        return;
    }

    // todo : unroll
    do {
        auto x1 = simd::loadu(src + 1);
        auto x2 = simd::loadu(src + 1 + (type_width));
        auto x3 = simd::loadu(src + 1 + (type_width * 2));
        const auto x4 = simd::loadu(src + 1 + (type_width * 3));
        WJR_REGISTER_RSHIFT_N_IMPL(0, x0, x1);
        WJR_REGISTER_RSHIFT_N_IMPL(type_width, x1, x2);
        WJR_REGISTER_RSHIFT_N_IMPL(type_width * 2, x2, x3);
        WJR_REGISTER_RSHIFT_N_IMPL(type_width * 3, x3, x4);
        simd::storeu(dst, x0);
        simd::storeu(dst + type_width, x1);
        simd::storeu(dst + type_width * 2, x2);
        simd::storeu(dst + type_width * 3, x3);
        x0 = x4;

        dst += type_width * 4;
        src += type_width * 4;
        --idx;
    } while (WJR_LIKELY(idx != 0));

    #undef WJR_REGISTER_RSHIFT_N_IMPL
}

template void large_builtin_rshift_n_impl<false, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                           size_t n, unsigned int cl) noexcept;
template void large_builtin_rshift_n_impl<true, uint64_t>(uint64_t *dst, const uint64_t *src,
                                                          size_t n, unsigned int cl) noexcept;

#endif

} // namespace wjr