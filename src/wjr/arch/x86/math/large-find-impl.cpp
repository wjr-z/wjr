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

#if WJR_HAS_BUILTIN(FIND_N)

template <typename T>
WJR_PURE size_t large_builtin_find_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 5
    #define WJRX_START_COUNT 4

    #define WJRX_SCALAR_I(I)                                                                       \
        if (WJR_LIKELY(src0[I] == src1[I])) {                                                      \
            return I;                                                                              \
        }

    #define WJR_XI 0

    #define WJR_REGISTER_FIND_N_SIMD(SIMD, index)                                                  \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
                                                                                                   \
            auto mask = SIMD::movemask_epi8(r);                                                    \
            if (WJR_LIKELY(mask != 0)) {                                                           \
                return (WJR_XI) + (index) + SIMD::ctz_nz(mask, T()) / 8;                           \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(I) WJR_REGISTER_FIND_N_SIMD(sse, I)

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(I) WJR_REGISTER_FIND_N_SIMD(avx, I)
    #else
        #define WJRX_UNROLL4_I(I)                                                                  \
            WJRX_UNROLL2_I(I);                                                                     \
            WJRX_UNROLL2_I(I + 2)
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src0 = src0;
    const auto *__end0 = src0 + n;

    #undef WJR_XI
    #define WJR_XI (src0 - __src0)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src0 += rem4;
        src1 += rem4;

        do {
            WJRX_UNROLL4(0);

            src0 += 4;
            src1 += 4;
        } while (WJR_LIKELY(src0 != __end0));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

        src0 += rem16;
        src1 += rem16;

        do {
            const auto r0 = simd::cmpeq_epi64(simd::loadu(src0), simd::loadu(src1));
            const auto r1 =
                simd::cmpeq_epi64(simd::loadu(src0 + type_width), simd::loadu(src1 + type_width));
            const auto r2 = simd::cmpeq_epi64(simd::loadu(src0 + type_width * 2),
                                              simd::loadu(src1 + type_width * 2));
            const auto r3 = simd::cmpeq_epi64(simd::loadu(src0 + type_width * 3),
                                              simd::loadu(src1 + type_width * 3));

            if (const auto z = simd::Or(simd::Or(r0, r1), simd::Or(r2, r3));
                WJR_UNLIKELY(!simd::test_all_zeros(z))) {
                typename simd::mask_type mask = simd::movemask_epi8(r0);
                if (mask != 0) {
                    return (WJR_XI) + simd::ctz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI) + type_width + simd::ctz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI) + type_width * 2 + simd::ctz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(z);
                return (WJR_XI) + type_width * 3 + simd::ctz_nz(mask, T()) / 8;
            }

            src0 += type_width * 4;
            src1 += type_width * 4;
        } while (WJR_LIKELY(src0 != __end0));
    }

    return __end0 - __src0;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_N_SIMD
    #undef WJR_XI
}

template size_t large_builtin_find_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                               size_t n) noexcept;

template <typename T>
WJR_PURE size_t large_builtin_find_n(const T *src, T val, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 5
    #define WJRX_START_COUNT 4

    #define WJRX_SCALAR_I(I)                                                                       \
        if (WJR_LIKELY(src[I] == val)) {                                                           \
            return I;                                                                              \
        }

    #define WJR_XI 0

    #define WJR_REGISTER_FIND_N_SIMD(SIMD, index, VAL)                                             \
        do {                                                                                       \
            const auto x = SIMD::loadu(src + (index));                                             \
            const auto r = SIMD::cmpeq_epi64(x, VAL);                                              \
                                                                                                   \
            auto mask = SIMD::movemask_epi8(r);                                                    \
            if (WJR_LIKELY(mask != 0)) {                                                           \
                return (WJR_XI) + (index) + SIMD::ctz_nz(mask, T()) / 8;                           \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(I) WJR_REGISTER_FIND_N_SIMD(sse, I, y2)

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(I) WJR_REGISTER_FIND_N_SIMD(avx, I, y4)
    #else
        #define WJRX_UNROLL4_I(I)                                                                  \
            WJRX_UNROLL2_I(I);                                                                     \
            WJRX_UNROLL2_I(I + 2)
    #endif

    #define WJRX_INIT2() const auto y2 = sse::set1(val, T());

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_INIT4() const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
    #else
        #define WJRX_INIT4()
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src = src;
    const auto *__end = src + n;

    #undef WJR_XI
    #define WJR_XI (src - __src)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src += rem4;

        do {
            WJRX_UNROLL4(0);

            src += 4;
        } while (WJR_LIKELY(src != __end));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_VAL y4
    #else
        #define WJR_VAL y2
    #endif

        src += rem16;

        do {
            const auto r0 = simd::cmpeq_epi64(simd::loadu(src), WJR_VAL);
            const auto r1 = simd::cmpeq_epi64(simd::loadu(src + type_width), WJR_VAL);
            const auto r2 = simd::cmpeq_epi64(simd::loadu(src + type_width * 2), WJR_VAL);
            const auto r3 = simd::cmpeq_epi64(simd::loadu(src + type_width * 3), WJR_VAL);

            if (const auto z = simd::Or(simd::Or(r0, r1), simd::Or(r2, r3));
                WJR_UNLIKELY(!simd::test_all_zeros(z))) {
                typename simd::mask_type mask = simd::movemask_epi8(r0);
                if (mask != 0) {
                    return (WJR_XI) + simd::ctz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI) + type_width + simd::ctz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI) + type_width * 2 + simd::ctz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(z);
                return (WJR_XI) + type_width * 3 + simd::ctz_nz(mask, T()) / 8;
            }

            src += type_width * 4;
        } while (WJR_LIKELY(src != __end));
    }

    return __end - __src;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_N_SIMD
    #undef WJR_XI
    #undef WJR_VAL
}

template size_t large_builtin_find_n<uint64_t>(const uint64_t *src, uint64_t val,
                                               size_t n) noexcept;

#endif

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
size_t large_builtin_find_not_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 3
    #define WJRX_START_COUNT 2

    #define WJRX_SCALAR_I(I)                                                                       \
        if (WJR_LIKELY(src0[I] != src1[I])) {                                                      \
            return I;                                                                              \
        }

    #define WJR_XI 0

    #define WJR_REGISTER_FIND_NOT_N_SIMD(SIMD, index)                                              \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (WJR_XI) + (index) + SIMD::ctz_nz(mask, T()) / 8;                           \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(I) WJR_REGISTER_FIND_NOT_N_SIMD(sse, I);

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(I) WJR_REGISTER_FIND_NOT_N_SIMD(avx, I)
    #else
        #define WJRX_UNROLL4_I(I)                                                                  \
            WJRX_UNROLL2_I(I);                                                                     \
            WJRX_UNROLL2_I((I) + 2)
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src0 = src0;
    const auto *__end0 = src0 + n;

    #undef WJR_XI
    #define WJR_XI (src0 - __src0)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src0 += rem4;
        src1 += rem4;

        do {
            WJRX_UNROLL4(0);

            src0 += 4;
            src1 += 4;
        } while (WJR_LIKELY(src0 != __end0));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

        src0 += rem16;
        src1 += rem16;

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
                    return (WJR_XI) + simd::ctz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI) + type_width + simd::ctz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI) + type_width * 2 + simd::ctz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(z);
                return (WJR_XI) + type_width * 3 + simd::ctz_nz(mask, T()) / 8;
            }

            src0 += type_width * 4;
            src1 += type_width * 4;
        } while (WJR_LIKELY(src0 != __end0));
    }

    return __end0 - __src0;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_NOT_N_SIMD
    #undef WJR_XI
}

template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                                   size_t n) noexcept;

template <typename T>
size_t large_builtin_find_not_n(const T *src, T val, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 3
    #define WJRX_START_COUNT 2

    #define WJRX_SCALAR_I(I)                                                                       \
        if (WJR_LIKELY(src[I] != val)) {                                                           \
            return I;                                                                              \
        }

    #define WJR_XI 0

    #define WJR_REGISTER_FIND_NOT_N_SIMD(SIMD, index, VAL)                                         \
        do {                                                                                       \
            const auto x = SIMD::loadu(src + (index));                                             \
            const auto r = SIMD::cmpeq_epi64(x, VAL);                                              \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (WJR_XI) + (index) + SIMD::ctz_nz(mask, T()) / 8;                           \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(I) WJR_REGISTER_FIND_NOT_N_SIMD(sse, I, y2);

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(I) WJR_REGISTER_FIND_NOT_N_SIMD(avx, I, y4)
    #else
        #define WJRX_UNROLL4_I(I)                                                                  \
            WJRX_UNROLL2_I(I);                                                                     \
            WJRX_UNROLL2_I((I) + 2)
    #endif

    #define WJRX_INIT2() const auto y2 = sse::set1(val, T());

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_INIT4() const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src = src;
    const auto *__end = src + n;

    #undef WJR_XI
    #define WJR_XI (src - __src)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src += rem4;

        do {
            WJRX_UNROLL4(0);
            src += 4;
        } while (WJR_LIKELY(src != __end));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        src += rem16;

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
                    return (WJR_XI) + simd::ctz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI) + type_width + simd::ctz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI) + type_width * 2 + simd::ctz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(z);
                return (WJR_XI) + type_width * 3 + simd::ctz_nz(mask, T()) / 8;
            }

            src += type_width * 4;
        } while (WJR_LIKELY(src != __end));
    }

    return __end - __src;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_VAL
    #undef WJR_REGISTER_FIND_NOT_N_SIMD
    #undef WJR_XI
}

template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                   size_t n) noexcept;

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)

template <typename T>
WJR_PURE size_t large_builtin_reverse_find_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 3
    #define WJRX_START_COUNT 2
    #define WJRX_REVERSE 1

    #define WJR_XI 0

    #define WJRX_SCALAR_I(I)                                                                       \
        if (src0[I] == src1[I]) {                                                                  \
            return I + 1;                                                                          \
        }

    #define WJR_REGISTER_FIND_N_SIMD(SIMD, WIDTH, index)                                           \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
                                                                                                   \
            auto mask = SIMD::movemask_epi8(r);                                                    \
            if (WJR_LIKELY(mask != 0)) {                                                           \
                return (WJR_XI) + (index) + (WIDTH) - SIMD::clz_nz(mask, T()) / 8;                 \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(I) WJR_REGISTER_FIND_N_SIMD(sse, 2, I)

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(I) WJR_REGISTER_FIND_N_SIMD(avx, 4, I)
    #else
        #define WJRX_UNROLL4_I(I)                                                                  \
            WJRX_UNROLL2_I((I) + 2);                                                               \
            WJRX_UNROLL2_I(I);
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src0 = src0;

    #undef WJR_XI
    #define WJR_XI (src0 - __src0)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src0 += n - rem4;
        src1 += n - rem4;

        do {
            WJRX_UNROLL4_I(-4);

            src0 -= 4;
            src1 -= 4;
        } while (WJR_LIKELY(src0 != __src0));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

        src0 += n - rem16;
        src1 += n - rem16;

        do {
            const auto r0 = simd::cmpeq_epi64(simd::loadu(src0 - type_width * 4),
                                              simd::loadu(src1 - type_width * 4));
            const auto r1 = simd::cmpeq_epi64(simd::loadu(src0 - type_width * 3),
                                              simd::loadu(src1 - type_width * 3));
            const auto r2 = simd::cmpeq_epi64(simd::loadu(src0 - type_width * 2),
                                              simd::loadu(src1 - type_width * 2));
            const auto r3 =
                simd::cmpeq_epi64(simd::loadu(src0 - type_width), simd::loadu(src1 - type_width));

            if (const auto z = simd::Or(simd::Or(r0, r1), simd::Or(r2, r3));
                WJR_UNLIKELY(!simd::test_all_zeros(z))) {
                typename simd::mask_type mask = simd::movemask_epi8(r3);
                if (mask != 0) {
                    return (WJR_XI)-simd::clz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI)-type_width - simd::clz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI)-type_width * 2 - simd::clz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(z);
                return (WJR_XI)-type_width * 3 - simd::clz_nz(mask, T()) / 8;
            }

            src0 -= type_width * 4;
            src1 -= type_width * 4;
        } while (WJR_LIKELY(src0 != __src0));
    }

    return 0;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_N_SIMD
    #undef WJR_XI
}

template size_t large_builtin_reverse_find_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                                       size_t n) noexcept;

template <typename T>
WJR_PURE size_t large_builtin_reverse_find_n(const T *src, T val, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 3
    #define WJRX_START_COUNT 2
    #define WJRX_REVERSE 1

    #define WJR_XI 0

    #define WJRX_SCALAR_I(I)                                                                       \
        if (src[I] == val) {                                                                       \
            return I + 1;                                                                          \
        }

    #define WJR_REGISTER_REVERSE_FIND_N_SIMD(SIMD, WIDTH, index, VAL)                              \
        do {                                                                                       \
            const auto x = SIMD::loadu(src + (index));                                             \
            const auto r = SIMD::cmpeq_epi64(x, VAL);                                              \
                                                                                                   \
            const auto mask = SIMD::movemask_epi8(r);                                              \
            if (WJR_LIKELY(mask != 0)) {                                                           \
                return (WJR_XI) + (index) + (WIDTH) - SIMD::clz_nz(mask, T()) / 8;                 \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(index) WJR_REGISTER_REVERSE_FIND_N_SIMD(sse, 2, index, y2)

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(index) WJR_REGISTER_REVERSE_FIND_N_SIMD(avx, 4, index, y4)
    #else
        #define WJRX_UNROLL4_I(index)                                                              \
            WJRX_UNROLL2_I((index) + 2);                                                           \
            WJRX_UNROLL2_I(index)
    #endif

    #define WJRX_INIT2() const auto y2 = sse::set1(val, T());

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_INIT4() const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src = src;

    #undef WJR_XI
    #define WJR_XI (src - __src)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src += n - rem4;

        do {
            WJRX_UNROLL4_I(-4);

            src -= 4;
        } while (WJR_LIKELY(src != __src));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_VAL y4
    #else
        #define WJR_VAL y2
    #endif

        src += n - rem16;

        do {
            const auto r0 = simd::cmpeq_epi64(simd::loadu(src - type_width * 4), WJR_VAL);
            const auto r1 = simd::cmpeq_epi64(simd::loadu(src - type_width * 3), WJR_VAL);
            const auto r2 = simd::cmpeq_epi64(simd::loadu(src - type_width * 2), WJR_VAL);
            const auto r3 = simd::cmpeq_epi64(simd::loadu(src - type_width), WJR_VAL);

            if (const auto z = simd::Or(simd::Or(r0, r1), simd::Or(r2, r3));
                WJR_UNLIKELY(!simd::test_all_zeros(z))) {
                typename simd::mask_type mask = simd::movemask_epi8(r3);
                if (mask != 0) {
                    return (WJR_XI)-simd::clz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI)-type_width - simd::clz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI)-type_width * 2 - simd::clz_nz(mask, T()) / 8;
                }

                mask = simd::movemask_epi8(z);
                return (WJR_XI)-type_width * 3 - simd::clz_nz(mask, T()) / 8;
            }

            src -= type_width * 4;
        } while (WJR_LIKELY(src != __src));
    }

    return 0;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_N_SIMD
    #undef WJR_XI
    #undef WJR_VAL
}

template size_t large_builtin_reverse_find_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                       size_t n) noexcept;

#endif

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 3
    #define WJRX_START_COUNT 2
    #define WJRX_REVERSE 1

    #define WJR_XI 0

    #define WJRX_SCALAR_I(I)                                                                       \
        if (src0[I] != src1[I]) {                                                                  \
            return I + 1;                                                                          \
        }

    #define WJR_REGISTER_FIND_NOT_N_SIMD(SIMD, WIDTH, index)                                       \
        do {                                                                                       \
            const auto x = SIMD::loadu(src0 + (index));                                            \
            const auto y = SIMD::loadu(src1 + (index));                                            \
            const auto r = SIMD::cmpeq_epi64(x, y);                                                \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (WJR_XI) + (index) + (WIDTH) - SIMD::clz_nz(mask, T()) / 8;                 \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(I) WJR_REGISTER_FIND_NOT_N_SIMD(sse, 2, I)

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(I) WJR_REGISTER_FIND_NOT_N_SIMD(avx, 4, I)
    #else
        #define WJRX_UNROLL4_I(I)                                                                  \
            WJRX_UNROLL2_I((I) + 2);                                                               \
            WJRX_UNROLL2_I(I);
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src0 = src0;

    #undef WJR_XI
    #define WJR_XI (src0 - __src0)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src0 += n - rem4;
        src1 += n - rem4;

        do {
            WJRX_UNROLL4_I(-4);

            src0 -= 4;
            src1 -= 4;
        } while (WJR_LIKELY(src0 != __src0));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

        src0 += n - rem16;
        src1 += n - rem16;

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
                    return (WJR_XI)-simd::clz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI)-type_width - simd::clz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI)-type_width * 2 - simd::clz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(z);
                return (WJR_XI)-type_width * 3 - simd::clz_nz(mask, T()) / 8;
            }

            src0 -= type_width * 4;
            src1 -= type_width * 4;
        } while (WJR_LIKELY(src0 != __src0));
    }

    return 0;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_NOT_N_SIMD
    #undef WJR_XI
}

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src0,
                                                           const uint64_t *src1, size_t n) noexcept;

template <typename T>
size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) noexcept {
    #define WJRX_ALLOW_OVERLAP 1
    #define WJRX_COUNT n
    #define WJRX_MIN_COUNT 3
    #define WJRX_START_COUNT 2
    #define WJRX_REVERSE 1

    #define WJR_XI 0

    #define WJRX_SCALAR_I(I)                                                                       \
        if (src[I] != val) {                                                                       \
            return I + 1;                                                                          \
        }

    #define WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD(SIMD, WIDTH, index, VAL)                          \
        do {                                                                                       \
            const auto x = SIMD::loadu(src + (index));                                             \
            const auto r = SIMD::cmpeq_epi64(x, VAL);                                              \
            if (!SIMD::test_all_ones(r)) {                                                         \
                const SIMD::mask_type mask = ~SIMD::movemask_epi8(r);                              \
                return (WJR_XI) + (index) + (WIDTH) - SIMD::clz_nz(mask, T()) / 8;                 \
            }                                                                                      \
        } while (0)

    #define WJRX_UNROLL2_I(index) WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD(sse, 2, index, y2)

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_UNROLL4_I(index) WJR_REGISTER_REVERSE_FIND_NOT_N_SIMD(avx, 4, index, y4)
    #else
        #define WJRX_UNROLL4_I(index)                                                              \
            WJRX_UNROLL2_I((index) + 2);                                                           \
            WJRX_UNROLL2_I(index)
    #endif

    #define WJRX_INIT2() const auto y2 = sse::set1(val, T());

    #if WJR_HAS_SIMD(AVX2)
        #define WJRX_INIT4() const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
    #endif

    #define WJRX_RETURN_I(I) I

    #include <wjr/arch/x86/simd/detail/unroll_header.hpp>

    #include <wjr/arch/x86/simd/detail/unroll_prefix4.hpp>

    const auto *__src = src;

    #undef WJR_XI
    #define WJR_XI (src - __src)

    // small size, use unroll4, 36 = 4 + 16 + 16
    if (n <= 36) {
        src += n - rem4;

        do {
            WJRX_UNROLL4_I(-4);

            src -= 4;
        } while (WJR_LIKELY(src != __src));
    } else {
    #include <wjr/arch/x86/simd/detail/unroll_prefix16.hpp>

        using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
        constexpr auto type_width = simd::width() / std::numeric_limits<T>::digits;

    #if WJR_HAS_SIMD(AVX2)
        #define WJR_VAL y4
    #else
        #define WJR_VAL y2
    #endif

        src += n - rem16;

        do {
            const auto r0 = simd::cmpeq_epi64(simd::loadu(src - type_width * 4), WJR_VAL);
            const auto r1 = simd::cmpeq_epi64(simd::loadu(src - type_width * 3), WJR_VAL);
            const auto r2 = simd::cmpeq_epi64(simd::loadu(src - type_width * 2), WJR_VAL);
            const auto r3 = simd::cmpeq_epi64(simd::loadu(src - type_width), WJR_VAL);

            if (const auto z = simd::And(simd::And(r0, r1), simd::And(r2, r3));
                WJR_UNLIKELY(!simd::test_all_ones(z))) {
                typename simd::mask_type mask = ~simd::movemask_epi8(r3);
                if (mask != 0) {
                    return (WJR_XI)-simd::clz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r2);
                if (mask != 0) {
                    return (WJR_XI)-type_width - simd::clz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(r1);
                if (mask != 0) {
                    return (WJR_XI)-type_width * 2 - simd::clz_nz(mask, T()) / 8;
                }

                mask = ~simd::movemask_epi8(z);
                return (WJR_XI)-type_width * 3 - simd::clz_nz(mask, T()) / 8;
            }

            src -= type_width * 4;
        } while (WJR_LIKELY(src != __src));
    }

    return 0;

    #include <wjr/arch/x86/simd/detail/unroll_footer.hpp>

    #undef WJR_REGISTER_FIND_NOT_N_SIMD
    #undef WJR_XI
    #undef WJR_VAL
}

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                           size_t n) noexcept;

#endif
} // namespace wjr