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
WJR_PURE WJR_COLD int large_builtin_compare_n(const T *src0, const T *src1,
                                              size_t n) noexcept {
#define WJR_REGISTER_COMPARE_NOT_N_AVX(index)                                            \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 + (index)));                                \
        auto y = avx::loadu((__m256i *)(src1 + (index)));                                \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            auto offset = ctz(mask) / 8;                                                 \
            return src0[(index) + offset] < src1[(index) + offset] ? -1 : 1;             \
        }                                                                                \
    } while (0)

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src0 + (rem - 2)));
        auto y0 = sse::loadu((__m128i *)(src1 + (rem - 4)));
        auto y1 = sse::loadu((__m128i *)(src1 + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[(rem - 4) + 1] < src1[(rem - 4) + 1] ? -1 : 1;
                }
                return src0[(rem - 4)] < src1[(rem - 4)] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask == 0xFF00) {
                return src0[(rem - 2) + 1] < src1[(rem - 2) + 1] ? -1 : 1;
            }
            return src0[(rem - 2)] < src1[(rem - 2)] ? -1 : 1;
        }
#else
        WJR_REGISTER_COMPARE_NOT_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + rem));
        auto x1 = sse::loadu((__m128i *)(src0 + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src0 + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src0 + rem + 6));
        auto y0 = sse::loadu((__m128i *)(src1 + rem));
        auto y1 = sse::loadu((__m128i *)(src1 + rem + 2));
        auto y2 = sse::loadu((__m128i *)(src1 + rem + 4));
        auto y3 = sse::loadu((__m128i *)(src1 + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[rem + 1] < src1[rem + 1] ? -1 : 1;
                }
                return src0[rem] < src1[rem] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[rem + 3] < src1[rem + 3] ? -1 : 1;
                }
                return src0[rem + 2] < src1[rem + 2] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[rem + 5] < src1[rem + 5] ? -1 : 1;
                }
                return src0[rem + 4] < src1[rem + 4] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r3);
            if (mask == 0xFF00) {
                return src0[rem + 7] < src1[rem + 7] ? -1 : 1;
            }
            return src0[rem + 6] < src1[rem + 6] ? -1 : 1;
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_COMPARE_NOT_N_AVX(rem);
        WJR_REGISTER_COMPARE_NOT_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + rem));
        auto x1 = avx::loadu((__m256i *)(src0 + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src0 + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src0 + rem + 12));
        auto y0 = avx::loadu((__m256i *)(src1 + rem));
        auto y1 = avx::loadu((__m256i *)(src1 + rem + 4));
        auto y2 = avx::loadu((__m256i *)(src1 + rem + 8));
        auto y3 = avx::loadu((__m256i *)(src1 + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[rem + offset] < src1[rem + offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[rem + offset + 4] < src1[rem + offset + 4] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[rem + offset + 8] < src1[rem + offset + 8] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r3);
            auto offset = ctz(mask) / 8;
            return src0[rem + offset + 12] < src1[rem + offset + 12] ? -1 : 1;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return 0;

#undef WJR_REGISTER_COMPARE_NOT_N_AVX
}

extern template WJR_PURE WJR_COLD int
large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                  size_t n) noexcept;

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

/**
 * @brief Use SIMD to compare two arrays of uint64_t in reverse order.
 *
 * @detail @ref large_builtin_compare_n in reverse order.
 *
 */
template <typename T>
WJR_PURE WJR_COLD int large_builtin_reverse_compare_n(const T *src0, const T *src1,
                                                      size_t n) noexcept {
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(index)                                    \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 + (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 + (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            auto offset = clz(mask) / 8;                                                 \
            return src0[(index)-1 - offset] < src1[(index)-1 - offset] ? -1 : 1;         \
        }                                                                                \
    } while (0)

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + n + 2));
        auto x1 = sse::loadu((__m128i *)(src0 + n));
        auto y0 = sse::loadu((__m128i *)(src1 + n + 2));
        auto y1 = sse::loadu((__m128i *)(src1 + n));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n + 2] < src1[n + 2] ? -1 : 1;
                }
                return src0[n + 3] < src1[n + 3] ? -1 : 1;
            }
            mask = ~sse::movemask_epi8(r1);
            if (mask == 0x00FF) {
                return src0[n] < src1[n] ? -1 : 1;
            }
            return src0[n + 1] < src1[n + 1] ? -1 : 1;
        }
#else
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + n - 8));
        auto x1 = sse::loadu((__m128i *)(src0 + n - 6));
        auto x2 = sse::loadu((__m128i *)(src0 + n - 4));
        auto x3 = sse::loadu((__m128i *)(src0 + n - 2));
        auto y0 = sse::loadu((__m128i *)(src1 + n - 8));
        auto y1 = sse::loadu((__m128i *)(src1 + n - 6));
        auto y2 = sse::loadu((__m128i *)(src1 + n - 4));
        auto y3 = sse::loadu((__m128i *)(src1 + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n - 2] < src1[n - 2] ? -1 : 1;
                }
                return src0[n - 1] < src1[n - 1] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n - 4] < src1[n - 4] ? -1 : 1;
                }
                return src0[n - 3] < src1[n - 3] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n - 6] < src1[n - 6] ? -1 : 1;
                }
                return src0[n - 5] < src1[n - 5] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r0);
            if (mask == 0x00FF) {
                return src0[n - 8] < src1[n - 8] ? -1 : 1;
            }
            return src0[n - 7] < src1[n - 7] ? -1 : 1;
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if (n & 8) {
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(n);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + n - 16));
        auto x1 = avx::loadu((__m256i *)(src0 + n - 12));
        auto x2 = avx::loadu((__m256i *)(src0 + n - 8));
        auto x3 = avx::loadu((__m256i *)(src0 + n - 4));
        auto y0 = avx::loadu((__m256i *)(src1 + n - 16));
        auto y1 = avx::loadu((__m256i *)(src1 + n - 12));
        auto y2 = avx::loadu((__m256i *)(src1 + n - 8));
        auto y3 = avx::loadu((__m256i *)(src1 + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                auto offset = clz(mask) / 8;
                return src0[n - 1 - offset] < src1[n - 1 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                auto offset = clz(mask) / 8;
                return src0[n - 5 - offset] < src1[n - 5 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                auto offset = clz(mask) / 8;
                return src0[n - 9 - offset] < src1[n - 9 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r0);
            auto offset = clz(mask) / 8;
            return src0[n - 13 - offset] < src1[n - 13 - offset] ? -1 : 1;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX
}

extern template WJR_PURE WJR_COLD int
large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                          size_t n) noexcept;

#endif

} // namespace wjr

#endif // WJR_X86_MATH_LARGE_COMPARE_IMPL_HPP__