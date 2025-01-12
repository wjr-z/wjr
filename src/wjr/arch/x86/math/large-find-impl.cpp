/**
 * @file large-find-impl.cpp
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

#include <wjr/arch/x86/math/large-find-impl.hpp>
#include <wjr/arch/x86/simd/simd_unroll_macro.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
size_t large_builtin_find_not_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJR_REGISTER_FIND_NOT_N_SIMD(SIMD, index)                                              \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (index) + SIMD::ctz_nz(mask, T()) / 8;                                      \
            }                                                                                      \
        } while (false)

    #define WJR_REGISTER_FIND_NOT_N_2(index) WJR_REGISTER_FIND_NOT_N_SIMD(sse, index)

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_FIND_NOT_N_4(index) WJR_REGISTER_FIND_NOT_N_SIMD(avx, index)
    #else
        #define WJR_REGISTER_FIND_NOT_N_4(index)                                                   \
            WJR_REGISTER_FIND_NOT_N_2(index);                                                      \
            WJR_REGISTER_FIND_NOT_N_2((index) + 2)
    #endif

    #define WJR_REGISTER_FIND_NOT_N_ADVNCE(index)                                                  \
        src0 += index;                                                                             \
        src1 += index

    #define WJR_REGISTER_FIND_NOT_N_INIT8() const auto __src0 = src0

    #define WJR_REGISTER_FIND_NOT_N_RET(index) index

    WJR_X86_SIMD_UNROLL_FAST_2_OVERLAPABLE(
        n, WJR_REGISTER_FIND_NOT_N_2, WJR_REGISTER_FIND_NOT_N_4, WJR_REGISTER_FIND_NOT_N_ADVNCE,
        WJR_X86_SIMD_UNROLL_NOP, WJR_REGISTER_FIND_NOT_N_INIT8, WJR_REGISTER_FIND_NOT_N_RET);

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
            typename simd::mask_type mask = ~simd::movemask_epi8(r0);
            if (mask != 0) {
                return (src0 - __src0) + simd::ctz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r1);
            if (mask != 0) {
                return (src0 - __src0) + type_width + simd::ctz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r2);
            if (mask != 0) {
                return (src0 - __src0) + type_width * 2 + simd::ctz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(z);
            return (src0 - __src0) + type_width * 3 + simd::ctz_nz(mask, T()) / 8;
        }

        src0 += type_width * 4;
        src1 += type_width * 4;
        n -= type_width * 4;
    } while (WJR_LIKELY(n != 0));

    return src0 - __src0;

    #undef WJR_REGISTER_FIND_NOT_N_RET
    #undef WJR_REGISTER_FIND_NOT_N_INIT8
    #undef WJR_REGISTER_FIND_NOT_N_ADVNCE
    #undef WJR_REGISTER_FIND_NOT_N_4
    #undef WJR_REGISTER_FIND_NOT_N_2
    #undef WJR_REGISTER_FIND_NOT_N_SIMD
}

template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                                   size_t n) noexcept;

template <typename T>
size_t large_builtin_find_not_n(const T *src, T val, size_t n) noexcept {
    #define WJR_REGISTER_FIND_NOT_N_SIMD(SIMD, index, VAL)                                         \
        do {                                                                                       \
            const auto x = SIMD::loadu(src + (index));                                             \
            const auto r = SIMD::cmpeq_epi64(x, VAL);                                              \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (index) + SIMD::ctz_nz(mask, T()) / 8;                                      \
            }                                                                                      \
        } while (false)

    #define WJR_REGISTER_FIND_NOT_N_2(index) WJR_REGISTER_FIND_NOT_N_SIMD(sse, index, y2)

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_FIND_NOT_N_4(index) WJR_REGISTER_FIND_NOT_N_SIMD(avx, index, y4)
    #else
        #define WJR_REGISTER_FIND_NOT_N_4(index)                                                   \
            WJR_REGISTER_FIND_NOT_N_2(index);                                                      \
            WJR_REGISTER_FIND_NOT_N_2((index) + 2)
    #endif

    #define WJR_REGISTER_FIND_NOT_N_ADVANCE(index) src += index

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_FIND_NOT_N_INIT4() const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
    #else
        #define WJR_REGISTER_FIND_NOT_N_INIT4()
    #endif

    #define WJR_REGISTER_FIND_NOT_N_INIT8() const auto __src = src

    #define WJR_REGISTER_FIND_NOT_N_RET(index) index

    const auto y2 = sse::set1(val, T());

    WJR_X86_SIMD_UNROLL_FAST_2_OVERLAPABLE(
        n, WJR_REGISTER_FIND_NOT_N_2, WJR_REGISTER_FIND_NOT_N_4, WJR_REGISTER_FIND_NOT_N_ADVANCE,
        WJR_REGISTER_FIND_NOT_N_INIT4, WJR_REGISTER_FIND_NOT_N_INIT8, WJR_REGISTER_FIND_NOT_N_RET);

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_VAL y4
    #else
        #define WJR_VAL y2
    #endif

    do {
        const auto r0 = simd::cmpeq_epi64(simd::loadu(src), WJR_VAL);
        const auto r1 = simd::cmpeq_epi64(simd::loadu(src + type_width), WJR_VAL);
        const auto r2 = simd::cmpeq_epi64(simd::loadu(src + type_width * 2), WJR_VAL);
        const auto r3 = simd::cmpeq_epi64(simd::loadu(src + type_width * 3), WJR_VAL);

        if (const auto z = simd::And(simd::And(r0, r1), simd::And(r2, r3));
            WJR_UNLIKELY(!simd::test_all_ones(z))) {
            typename simd::mask_type mask = ~simd::movemask_epi8(r0);
            if (mask != 0) {
                return (src - __src) + simd::ctz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r1);
            if (mask != 0) {
                return (src - __src) + type_width + simd::ctz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r2);
            if (mask != 0) {
                return (src - __src) + type_width * 2 + simd::ctz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(z);
            return (src - __src) + type_width * 3 + simd::ctz_nz(mask, T()) / 8;
        }

        src += type_width * 4;
        n -= type_width * 4;
    } while (WJR_LIKELY(n != 0));

    return src - __src;

    #undef WJR_VAL
    #undef WJR_REGISTER_FIND_NOT_N_RET
    #undef WJR_REGISTER_FIND_NOT_N_INIT4
    #undef WJR_REGISTER_FIND_NOT_N_INIT8
    #undef WJR_REGISTER_FIND_NOT_N_ADVANCE
    #undef WJR_REGISTER_FIND_NOT_N_4
    #undef WJR_REGISTER_FIND_NOT_N_2
    #undef WJR_REGISTER_FIND_NOT_N_SIMD
}

template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                   size_t n) noexcept;

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJR_REGISTER_FIND_NOT_N_SIMD(SIMD, WIDTH, index)                                       \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (index) + (WIDTH) - SIMD::clz_nz(mask, T()) / 8;                            \
            }                                                                                      \
        } while (false)

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_2(index) WJR_REGISTER_FIND_NOT_N_SIMD(sse, 2, index)

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index) WJR_REGISTER_FIND_NOT_N_SIMD(avx, 4, index)
    #else
        #define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                           \
            WJR_REGISTER_REVERSE_FIND_NOT_N_2((index) + 2);                                        \
            WJR_REGISTER_REVERSE_FIND_NOT_N_2(index);
    #endif

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE(index)                                         \
        src0 += index;                                                                             \
        src1 += index

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_RET(index) 0

    WJR_X86_SIMD_UNROLL_REVERSE_FAST_2_OVERLAPABLE(
        n, WJR_REGISTER_REVERSE_FIND_NOT_N_2, WJR_REGISTER_REVERSE_FIND_NOT_N_4,
        WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE, WJR_X86_SIMD_UNROLL_NOP, WJR_X86_SIMD_UNROLL_NOP,
        WJR_REGISTER_REVERSE_FIND_NOT_N_RET);

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
            typename simd::mask_type mask = ~simd::movemask_epi8(r3);
            if (mask != 0) {
                return n - simd::clz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r2);
            if (mask != 0) {
                return n - type_width - simd::clz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r1);
            if (mask != 0) {
                return n - type_width * 2 - simd::clz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(z);
            return n - type_width * 3 - simd::clz_nz(mask, T()) / 8;
        }

        src0 -= type_width * 4;
        src1 -= type_width * 4;
        n -= type_width * 4;
    } while (WJR_LIKELY(n != 0));

    return 0;

    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_4
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_2
    #undef WJR_REGISTER_FIND_NOT_N_SIMD
}

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src0,
                                                           const uint64_t *src1, size_t n) noexcept;

template <typename T>
size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) noexcept {
    #define WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD(SIMD, WIDTH, index, VAL)                          \
        do {                                                                                       \
            const auto x = SIMD::loadu(src + (index));                                             \
            const auto r = SIMD::cmpeq_epi64(x, VAL);                                              \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (index) + (WIDTH) - SIMD::clz_nz(mask, T()) / 8;                            \
            }                                                                                      \
        } while (false)

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_2(index)                                               \
        WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD(sse, 2, index, y2)

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                           \
            WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD(avx, 4, index, y4)
    #else
        #define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                           \
            WJR_REGISTER_REVERSE_FIND_NOT_N_2((index) + 2);                                        \
            WJR_REGISTER_REVERSE_FIND_NOT_N_2(index)
    #endif

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE(index) src += index

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_REGISTER_REVERSE_FIND_NOT_N_INIT4()                                            \
            const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
    #else
        #define WJR_REGISTER_REVERSE_FIND_NOT_N_INIT4()
    #endif

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_RET(index) 0

    const auto y2 = sse::set1(val, T());

    WJR_X86_SIMD_UNROLL_REVERSE_FAST_2_OVERLAPABLE(
        n, WJR_REGISTER_REVERSE_FIND_NOT_N_2, WJR_REGISTER_REVERSE_FIND_NOT_N_4,
        WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE, WJR_REGISTER_REVERSE_FIND_NOT_N_INIT4,
        WJR_X86_SIMD_UNROLL_NOP, WJR_REGISTER_REVERSE_FIND_NOT_N_RET);

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_VAL y4
    #else
        #define WJR_VAL y2
    #endif

    do {
        const auto r0 = simd::cmpeq_epi64(simd::loadu(src - type_width * 4), WJR_VAL);
        const auto r1 = simd::cmpeq_epi64(simd::loadu(src - type_width * 3), WJR_VAL);
        const auto r2 = simd::cmpeq_epi64(simd::loadu(src - type_width * 2), WJR_VAL);
        const auto r3 = simd::cmpeq_epi64(simd::loadu(src - type_width), WJR_VAL);

        if (const auto z = simd::And(simd::And(r0, r1), simd::And(r2, r3));
            WJR_UNLIKELY(!simd::test_all_ones(z))) {
            typename simd::mask_type mask = ~simd::movemask_epi8(r3);
            if (mask != 0) {
                return n - simd::clz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r2);
            if (mask != 0) {
                return n - type_width - simd::clz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(r1);
            if (mask != 0) {
                return n - type_width * 2 - simd::clz_nz(mask, T()) / 8;
            }

            mask = ~simd::movemask_epi8(z);
            return n - type_width * 3 - simd::clz_nz(mask, T()) / 8;
        }

        src -= type_width * 4;
        n -= type_width * 4;
    } while (WJR_LIKELY(n != 0));

    return 0;

    #undef WJR_VAL
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_INIT4
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_4
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_2
    #undef WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD
}

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                           size_t n) noexcept;

#endif
} // namespace wjr