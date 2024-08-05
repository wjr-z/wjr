#ifndef WJR_X86_MATH_LARGE_COMPARE_IMPL_HPP__
#define WJR_X86_MATH_LARGE_COMPARE_IMPL_HPP__

#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1)
#define WJR_HAS_BUILTIN_COMPARE_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_COMPARE_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(COMPARE_N)

/**
 * @brief Use SIMD to compare two arrays of uint64_t.
 *
 */
template <typename T>
WJR_PURE int large_builtin_compare_n(const T *src0, const T *src1, size_t n) noexcept {
#define WJR_REGISTER_COMPARE_NOT_N_2(index)                                              \
    do {                                                                                 \
        const auto x = sse::loadu(src0 + (index));                                       \
        const auto y = sse::loadu(src1 + (index));                                       \
        const auto r = sse::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const sse::mask_type mask = ~sse::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            if (mask == 0xFF00) {                                                        \
                return src0[(index) + 1] < src1[(index) + 1] ? -1 : 1;                   \
            }                                                                            \
            return src0[index] < src1[index] ? -1 : 1;                                   \
        }                                                                                \
    } while (0)

#if WJR_HAS_SIMD(AVX2)
#define WJR_REGISTER_COMPARE_NOT_N_4(index)                                              \
    do {                                                                                 \
        const auto x = avx::loadu(src0 + (index));                                       \
        const auto y = avx::loadu(src1 + (index));                                       \
        const auto r = avx::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const avx::mask_type mask = ~avx::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            const auto offset = ctz(mask) / 8;                                           \
            return src0[(index) + offset] < src1[(index) + offset] ? -1 : 1;             \
        }                                                                                \
    } while (0)
#else
#define WJR_REGISTER_COMPARE_NOT_N_4(index)                                              \
    WJR_REGISTER_COMPARE_NOT_N_2(index);                                                 \
    WJR_REGISTER_COMPARE_NOT_N_2((index) + 2)
#endif

#define WJR_REGISTER_COMPARE_NOT_N_ADVANCE(index)                                        \
    src0 += index;                                                                       \
    src1 += index
#define WJR_REGISTER_COMPARE_NOT_N_RET(index) 0

    WJR_ASSUME(n > 2);

    WJR_REGISTER_X86_NORMAL_SIMD_FUNCTION(
        n, WJR_REGISTER_COMPARE_NOT_N_2, WJR_REGISTER_COMPARE_NOT_N_4, WJR_HAS_SIMD(AVX2),
        WJR_REGISTER_COMPARE_NOT_N_ADVANCE, , WJR_REGISTER_COMPARE_NOT_N_RET);

#if !WJR_HAS_SIMD(AVX2)
    do {
        const auto r0 = sse::cmpeq_epi64(sse::loadu(src0), sse::loadu(src1));
        const auto r1 = sse::cmpeq_epi64(sse::loadu(src0 + 2), sse::loadu(src1 + 2));
        const auto r2 = sse::cmpeq_epi64(sse::loadu(src0 + 4), sse::loadu(src1 + 4));
        const auto r3 = sse::cmpeq_epi64(sse::loadu(src0 + 6), sse::loadu(src1 + 6));

        const auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[1] < src1[1] ? -1 : 1;
                }
                return src0[0] < src1[0] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[3] < src1[3] ? -1 : 1;
                }
                return src0[2] < src1[2] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[5] < src1[5] ? -1 : 1;
                }
                return src0[4] < src1[4] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r3);
            if (mask == 0xFF00) {
                return src0[7] < src1[7] ? -1 : 1;
            }
            return src0[6] < src1[6] ? -1 : 1;
        }

        src0 += 8;
        src1 += 8;
        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    do {
        const auto r0 = avx::cmpeq_epi64(avx::loadu(src0), avx::loadu(src1));
        const auto r1 = avx::cmpeq_epi64(avx::loadu(src0 + 4), avx::loadu(src1 + 4));
        const auto r2 = avx::cmpeq_epi64(avx::loadu(src0 + 8), avx::loadu(src1 + 8));
        const auto r3 = avx::cmpeq_epi64(avx::loadu(src0 + 12), avx::loadu(src1 + 12));

        const auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (mask != 0) {
                const auto offset = ctz(mask) / 8;
                return src0[offset] < src1[offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                const auto offset = ctz(mask) / 8;
                return src0[offset + 4] < src1[offset + 4] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                const auto offset = ctz(mask) / 8;
                return src0[offset + 8] < src1[offset + 8] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r3);
            const auto offset = ctz(mask) / 8;
            return src0[offset + 12] < src1[offset + 12] ? -1 : 1;
        }

        src0 += 16;
        src1 += 16;
        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_COMPARE_NOT_N_RET
#undef WJR_REGISTER_COMPARE_NOT_N_ADVANCE
#undef WJR_REGISTER_COMPARE_NOT_N_4
#undef WJR_REGISTER_COMPARE_NOT_N_2
}

extern template WJR_PURE int large_builtin_compare_n<uint64_t>(const uint64_t *src0,
                                                               const uint64_t *src1,
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
WJR_PURE int large_builtin_reverse_compare_n(const T *src0, const T *src1,
                                             size_t n) noexcept {
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_2(index)                                      \
    do {                                                                                 \
        const auto x = sse::loadu(src0 + (index));                                       \
        const auto y = sse::loadu(src1 + (index));                                       \
        const auto r = sse::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const sse::mask_type mask = ~sse::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            if (mask == 0x00FF) {                                                        \
                return src0[index] < src1[index] ? -1 : 1;                               \
            }                                                                            \
            return src0[(index) + 1] < src1[(index) + 1] ? -1 : 1;                       \
        }                                                                                \
    } while (0)

#if WJR_HAS_SIMD(AVX2)
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_4(index)                                      \
    do {                                                                                 \
        const auto x = avx::loadu(src0 + (index));                                       \
        const auto y = avx::loadu(src1 + (index));                                       \
        const auto r = avx::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const avx::mask_type mask = ~avx::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            const auto offset = clz(mask) / 8;                                           \
            return src0[(index) + 3 - offset] < src1[(index) + 3 - offset] ? -1 : 1;     \
        }                                                                                \
    } while (0)
#else
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_4(index)                                      \
    WJR_REGISTER_REVERSE_COMPARE_NOT_N_2((index) + 2);                                   \
    WJR_REGISTER_REVERSE_COMPARE_NOT_N_2(index)
#endif

#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_ADVANCE(index)                                \
    src0 += index;                                                                       \
    src1 += index

#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET(index) 0

    WJR_ASSUME(n > 2);

    WJR_REGISTER_X86_NORMAL_REVERSE_SIMD_FUNCTION(
        n, WJR_REGISTER_REVERSE_COMPARE_NOT_N_2, WJR_REGISTER_REVERSE_COMPARE_NOT_N_4,
        WJR_HAS_SIMD(AVX2), WJR_REGISTER_REVERSE_COMPARE_NOT_N_ADVANCE, ,
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET);

#if !WJR_HAS_SIMD(AVX2)
    do {
        const auto r0 = sse::cmpeq_epi64(sse::loadu(src0 - 8), sse::loadu(src1 - 8));
        const auto r1 = sse::cmpeq_epi64(sse::loadu(src0 - 6), sse::loadu(src1 - 6));
        const auto r2 = sse::cmpeq_epi64(sse::loadu(src0 - 4), sse::loadu(src1 - 4));
        const auto r3 = sse::cmpeq_epi64(sse::loadu(src0 - 2), sse::loadu(src1 - 2));

        const auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[-2] < src1[-2] ? -1 : 1;
                }
                return src0[-1] < src1[-1] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[-4] < src1[-4] ? -1 : 1;
                }
                return src0[-3] < src1[-3] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[-6] < src1[-6] ? -1 : 1;
                }
                return src0[-5] < src1[-5] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r0);
            if (mask == 0x00FF) {
                return src0[-8] < src1[-8] ? -1 : 1;
            }
            return src0[-7] < src1[-7] ? -1 : 1;
        }

        src0 -= 8;
        src1 -= 8;
        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    do {
        const auto r0 = avx::cmpeq_epi64(avx::loadu(src0 - 16), avx::loadu(src1 - 16));
        const auto r1 = avx::cmpeq_epi64(avx::loadu(src0 - 12), avx::loadu(src1 - 12));
        const auto r2 = avx::cmpeq_epi64(avx::loadu(src0 - 8), avx::loadu(src1 - 8));
        const auto r3 = avx::cmpeq_epi64(avx::loadu(src0 - 4), avx::loadu(src1 - 4));

        const auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                const auto offset = clz(mask) / 8;
                return src0[-1 - offset] < src1[-1 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                const auto offset = clz(mask) / 8;
                return src0[-5 - offset] < src1[-5 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                const auto offset = clz(mask) / 8;
                return src0[-9 - offset] < src1[-9 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r0);
            const auto offset = clz(mask) / 8;
            return src0[-13 - offset] < src1[-13 - offset] ? -1 : 1;
        }

        src0 -= 16;
        src1 -= 16;
        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_ADVANCE
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_4
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_2
}

extern template WJR_PURE int
large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                          size_t n) noexcept;

#endif

} // namespace wjr

#endif // WJR_X86_MATH_LARGE_COMPARE_IMPL_HPP__