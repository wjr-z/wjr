/**
 * @file large-compare-impl.cpp
 * @author wjr
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @todo Support uint8_t, uint16_t, uint32_t.
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <wjr/arch/x86/math/large-compare-impl.hpp>
#include <wjr/arch/x86/simd/simd_unroll_macro.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(COMPARE_N)

/**
 * @brief Use SIMD to compare two arrays of uint64_t.
 *
 */
template <typename T>
int large_builtin_compare_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJR_REGISTER_COMPARE_NOT_N_SIMD(SIMD, index)                                           \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                const auto offset = SIMD::ctz_nz(mask, T()) / 8;                                   \
                return src0[(index) + offset] < src1[(index) + offset] ? -1 : 1;                   \
            }                                                                                      \
        } while (0)

    #define WJR_REGISTER_COMPARE_NOT_N_2(index) WJR_REGISTER_COMPARE_NOT_N_SIMD(sse, index)

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_COMPARE_NOT_N_4(index) WJR_REGISTER_COMPARE_NOT_N_SIMD(avx, index)
    #else
        #define WJR_REGISTER_COMPARE_NOT_N_4(index)                                                \
            WJR_REGISTER_COMPARE_NOT_N_2(index);                                                   \
            WJR_REGISTER_COMPARE_NOT_N_2((index) + 2)
    #endif

    #define WJR_REGISTER_COMPARE_NOT_N_ADVANCE(index)                                              \
        src0 += index;                                                                             \
        src1 += index
    #define WJR_REGISTER_COMPARE_NOT_N_RET(index) 0

    WJR_ASSUME(n > 2);

    WJR_X86_SIMD_UNROLL_FAST_2_OVERLAPABLE(
        n, WJR_REGISTER_COMPARE_NOT_N_2, WJR_REGISTER_COMPARE_NOT_N_4,
        WJR_REGISTER_COMPARE_NOT_N_ADVANCE, WJR_X86_SIMD_UNROLL_NOP, WJR_X86_SIMD_UNROLL_NOP,
        WJR_REGISTER_COMPARE_NOT_N_RET);

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    do {
        const auto r0 = simd::cmpeq_epi64(simd::loadu(src0), simd::loadu(src1));
        const auto r1 =
            simd::cmpeq_epi64(simd::loadu(src0 + type_width), simd::loadu(src1 + type_width));
        const auto r2 = simd::cmpeq_epi64(simd::loadu(src0 + type_width * 2),
                                          simd::loadu(src1 + type_width * 2));
        const auto r3 = simd::cmpeq_epi64(simd::loadu(src0 + type_width * 3),
                                          simd::loadu(src1 + type_width * 3));

        if (const auto z = simd::And(simd::And(r0, r1), simd::And(r2, r3));
            WJR_UNLIKELY(!simd::test_all_ones(z))) {
            simd::mask_type mask = ~simd::movemask_epi8(r0);
            if (mask != 0) {
                const auto offset = simd::ctz_nz(mask, T()) / 8;
                return src0[offset] < src1[offset] ? -1 : 1;
            }

            mask = ~simd::movemask_epi8(r1);
            if (mask != 0) {
                const auto offset = simd::ctz_nz(mask, T()) / 8;
                return src0[offset + type_width] < src1[offset + type_width] ? -1 : 1;
            }

            mask = ~simd::movemask_epi8(r2);
            if (mask != 0) {
                const auto offset = simd::ctz_nz(mask, T()) / 8;
                return src0[offset + type_width * 2] < src1[offset + type_width * 2] ? -1 : 1;
            }

            mask = ~simd::movemask_epi8(z);
            const auto offset = simd::ctz_nz(mask, T()) / 8;
            return src0[offset + type_width * 3] < src1[offset + type_width * 3] ? -1 : 1;
        }

        src0 += type_width * 4;
        src1 += type_width * 4;
        n -= type_width * 4;
    } while (WJR_LIKELY(n != 0));

    return 0;

    #undef WJR_REGISTER_COMPARE_NOT_N_RET
    #undef WJR_REGISTER_COMPARE_NOT_N_ADVANCE
    #undef WJR_REGISTER_COMPARE_NOT_N_4
    #undef WJR_REGISTER_COMPARE_NOT_N_2
    #undef WJR_REGISTER_COMPARE_NOT_N_SIMD
}

template int large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                               size_t n) noexcept;

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

/**
 * @brief Use SIMD to compare two arrays of uint64_t in reverse order.
 *
 * @details @ref large_builtin_compare_n in reverse order.
 *
 */
template <typename T>
int large_builtin_reverse_compare_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJR_REGISTER_REVERSE_COMPARE_NOT_N_SIMD(SIMD, WIDTH, index)                            \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                const auto offset = SIMD::clz_nz(mask, T()) / 8;                                   \
                return src0[(index) + (WIDTH) - 1 - offset] < src1[(index) + (WIDTH) - 1 - offset] \
                           ? -1                                                                    \
                           : 1;                                                                    \
            }                                                                                      \
        } while (0)

    #define WJR_REGISTER_REVERSE_COMPARE_NOT_N_2(index)                                            \
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_SIMD(sse, 2, index)

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_REVERSE_COMPARE_NOT_N_4(index)                                        \
            WJR_REGISTER_REVERSE_COMPARE_NOT_N_SIMD(avx, 4, index)
    #else
        #define WJR_REGISTER_REVERSE_COMPARE_NOT_N_4(index)                                        \
            WJR_REGISTER_REVERSE_COMPARE_NOT_N_2((index) + 2);                                     \
            WJR_REGISTER_REVERSE_COMPARE_NOT_N_2(index)
    #endif

    #define WJR_REGISTER_REVERSE_COMPARE_NOT_N_ADVANCE(index)                                      \
        src0 += index;                                                                             \
        src1 += index

    #define WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET(index) 0

    WJR_ASSUME(n > 2);

    WJR_X86_SIMD_UNROLL_REVERSE_FAST_2_OVERLAPABLE(
        n, WJR_REGISTER_REVERSE_COMPARE_NOT_N_2, WJR_REGISTER_REVERSE_COMPARE_NOT_N_4,
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_ADVANCE, WJR_X86_SIMD_UNROLL_NOP,
        WJR_X86_SIMD_UNROLL_NOP, WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET);

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    do {
        const auto r0 = simd::cmpeq_epi64(simd::loadu(src0 - type_width * 4),
                                          simd::loadu(src1 - type_width * 4));
        const auto r1 = simd::cmpeq_epi64(simd::loadu(src0 - type_width * 3),
                                          simd::loadu(src1 - type_width * 3));
        const auto r2 = simd::cmpeq_epi64(simd::loadu(src0 - type_width * 2),
                                          simd::loadu(src1 - type_width * 2));
        const auto r3 =
            simd::cmpeq_epi64(simd::loadu(src0 - type_width), simd::loadu(src1 - type_width));

        if (const auto z = simd::And(simd::And(r0, r1), simd::And(r2, r3));
            WJR_UNLIKELY(!simd::test_all_ones(z))) {
            simd::mask_type mask = ~simd::movemask_epi8(r3);
            if (mask != 0) {
                const auto offset = simd::clz_nz(mask, T()) / 8;
                return src0[-offset - 1] < src1[-offset - 1] ? -1 : 1;
            }

            mask = ~simd::movemask_epi8(r2);
            if (mask != 0) {
                const auto offset = simd::clz_nz(mask, T()) / 8;
                return src0[-offset - 1 - type_width] < src1[-offset - 1 - type_width] ? -1 : 1;
            }

            mask = ~simd::movemask_epi8(r1);
            if (mask != 0) {
                const auto offset = simd::clz_nz(mask, T()) / 8;
                return src0[-offset - 1 - type_width * 2] < src1[-offset - 1 - type_width * 2] ? -1
                                                                                               : 1;
            }

            mask = ~simd::movemask_epi8(z);
            const auto offset = simd::clz_nz(mask, T()) / 8;
            return src0[-offset - 1 - type_width * 3] < src1[-offset - 1 - type_width * 3] ? -1 : 1;
        }

        src0 -= type_width * 4;
        src1 -= type_width * 4;
        n -= type_width * 4;
    } while (WJR_LIKELY(n != 0));

    return 0;

    #undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET
    #undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_ADVANCE
    #undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_4
    #undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_2
    #undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_SIMD
}

template int large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                                       size_t n) noexcept;

#endif

} // namespace wjr