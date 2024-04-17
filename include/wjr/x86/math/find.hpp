#ifndef WJR_X86_MATH_FIND_HPP__
#define WJR_X86_MATH_FIND_HPP__

#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FIND_NOT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_N)

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_N_AVX(index)                                                   \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 + (index)));                                \
        auto y = avx::loadu((__m256i *)(src1 + (index)));                                \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = avx::movemask_epi8(r);                                     \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src0 + (rem - 2)));
        auto y1 = sse::loadu((__m128i *)(src1 + (rem - 2)));
        auto y0 = sse::loadu((__m128i *)(src1 + (rem - 4)));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return (rem - 4) + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            return (rem - 2) + (mask == 0xFF00);
        }
#else
        WJR_REGISTER_FIND_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
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

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r3);
            return rem + 6 + (mask == 0xFF00);
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_N_AVX(rem);
        WJR_REGISTER_FIND_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + rem));
        auto x2 = avx::loadu((__m256i *)(src0 + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src0 + rem + 12));
        auto x1 = avx::loadu((__m256i *)(src0 + rem + 4));
        auto y0 = avx::loadu((__m256i *)(src1 + rem));
        auto y1 = avx::loadu((__m256i *)(src1 + rem + 4));
        auto y2 = avx::loadu((__m256i *)(src1 + rem + 8));
        auto y3 = avx::loadu((__m256i *)(src1 + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0 || src0[0] == src1[0])) {
        return 0;
    }

    if (n == 1 || WJR_UNLIKELY(src0[1] == src1[1])) {
        return 1;
    }

    if (n == 2 || WJR_UNLIKELY(src0[2] == src1[2])) {
        return 2;
    }

    if (n == 3 || WJR_UNLIKELY(src0[3] == src1[3])) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_n(src0, src1, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_N_AVX(index)                                                   \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src + (index)));                                 \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = avx::movemask_epi8(r);                                               \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem - 4 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            return rem - 2 + (mask == 0xFF00);
        }
#else
        WJR_REGISTER_FIND_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + rem));
        auto x1 = sse::loadu((__m128i *)(src + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r3);
            return rem + 6 + (mask == 0xFF00);
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_N_AVX(rem);
        WJR_REGISTER_FIND_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + rem));
        auto x1 = avx::loadu((__m256i *)(src + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0 || src[0] == val)) {
        return 0;
    }

    if (n == 1 || WJR_UNLIKELY(src[1] == val)) {
        return 1;
    }

    if (n == 2 || WJR_UNLIKELY(src[2] == val)) {
        return 2;
    }

    if (n == 3 || WJR_UNLIKELY(src[3] == val)) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_n(src, val, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_N)

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_find_not_n(const T *src0, const T *src1,
                                                  size_t n) {
#define WJR_REGISTER_FIND_NOT_N_AVX(index)                                               \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 + (index)));                                \
        auto y = avx::loadu((__m256i *)(src1 + (index)));                                \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
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
                return rem - 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            return rem - 2 + ctz(mask) / 8;
        }
#else
        WJR_REGISTER_FIND_NOT_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
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
                return rem + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r3);
            return rem + 6 + ctz(mask) / 8;
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_NOT_N_AVX(rem);
        WJR_REGISTER_FIND_NOT_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
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
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_NOT_N_AVX
}

extern template WJR_PURE WJR_COLD size_t
large_builtin_find_not_n<uint64_t>(const uint64_t *src0, const uint64_t *src1, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src0[0] != src1[0])) {
        return 0;
    }

    if (n == 1 || WJR_LIKELY(src0[1] != src1[1])) {
        return 1;
    }

    if (n == 2 || WJR_LIKELY(src0[2] != src1[2])) {
        return 2;
    }

    if (n == 3 || WJR_LIKELY(src0[3] != src1[3])) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_not_n(src0, src1, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_AVX(index)                                               \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src + (index)));                                 \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = ~avx::movemask_epi8(r);                                              \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem - 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            return rem - 2 + ctz(mask) / 8;
        }
#else
        WJR_REGISTER_FIND_NOT_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + rem));
        auto x1 = sse::loadu((__m128i *)(src + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r3);
            return rem + 6 + ctz(mask) / 8;
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_NOT_N_AVX(rem);
        WJR_REGISTER_FIND_NOT_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + rem));
        auto x1 = avx::loadu((__m256i *)(src + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_NOT_N_AVX
}

extern template WJR_PURE WJR_COLD size_t
large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src[0] != val)) {
        return 0;
    }

    if (n == 1 || WJR_LIKELY(src[1] != val)) {
        return 1;
    }

    if (n == 2 || WJR_LIKELY(src[2] != val)) {
        return 2;
    }

    if (n == 3 || WJR_LIKELY(src[3] != val)) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_not_n(src, val, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_reverse_find_n(const T *src0, const T *src1,
                                                      size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 + (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 + (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = avx::movemask_epi8(r);                                     \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
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

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
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

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

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

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src0, const T *src1,
                                                   size_t n) {
    if (WJR_UNLIKELY(n == 0 || src0[n - 1] == src1[n - 1])) {
        return n;
    }

    if (n == 1 || WJR_UNLIKELY(src0[n - 2] == src1[n - 2])) {
        return n - 1;
    }

    if (n == 2 || WJR_UNLIKELY(src0[n - 3] == src1[n - 3])) {
        return n - 2;
    }

    if (n == 3 || WJR_UNLIKELY(src0[n - 4] == src1[n - 4])) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_n(src0, src1, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_reverse_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src - 4 + (index)));                             \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = avx::movemask_epi8(r);                                     \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + n + 2));
        auto x1 = sse::loadu((__m128i *)(src + n));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + n - 8));
        auto x1 = sse::loadu((__m128i *)(src + n - 6));
        auto x2 = sse::loadu((__m128i *)(src + n - 4));
        auto x3 = sse::loadu((__m128i *)(src + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + n - 16));
        auto x1 = avx::loadu((__m256i *)(src + n - 12));
        auto x2 = avx::loadu((__m256i *)(src + n - 8));
        auto x3 = avx::loadu((__m256i *)(src + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0 || src[n - 1] == val)) {
        return n;
    }

    if (n == 1 || WJR_UNLIKELY(src[n - 2] == val)) {
        return n - 1;
    }

    if (n == 2 || WJR_UNLIKELY(src[n - 3] == val)) {
        return n - 2;
    }

    if (n == 3 || WJR_UNLIKELY(src[n - 4] == val)) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_n(src, val, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                          size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 + (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 + (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
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
                return n + 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
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
                return n - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

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
                return n - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

extern template WJR_PURE WJR_COLD size_t large_builtin_reverse_find_not_n<uint64_t>(
    const uint64_t *src0, const uint64_t *src1, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                       size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return n;
    }

    if (n == 1 || WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
        return n - 1;
    }

    if (n == 2 || WJR_LIKELY(src0[n - 3] != src1[n - 3])) {
        return n - 2;
    }

    if (n == 3 || WJR_LIKELY(src0[n - 4] != src1[n - 4])) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_not_n(src0, src1, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

template <typename T>
WJR_PURE WJR_COLD size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src - 4 + (index)));                             \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + n + 2));
        auto x1 = sse::loadu((__m128i *)(src + n));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + n - 8));
        auto x1 = sse::loadu((__m128i *)(src + n - 6));
        auto x2 = sse::loadu((__m128i *)(src + n - 4));
        auto x3 = sse::loadu((__m128i *)(src + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + n - 16));
        auto x1 = avx::loadu((__m256i *)(src + n - 12));
        auto x2 = avx::loadu((__m256i *)(src + n - 8));
        auto x3 = avx::loadu((__m256i *)(src + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

extern template WJR_PURE WJR_COLD size_t
large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src, uint64_t val, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src[n - 1] != val)) {
        return n;
    }

    if (n == 1 || WJR_LIKELY(src[n - 2] != val)) {
        return n - 1;
    }

    if (n == 2 || WJR_LIKELY(src[n - 3] != val)) {
        return n - 2;
    }

    if (n == 3 || WJR_LIKELY(src[n - 4] != val)) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_not_n(src, val, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

} // namespace wjr

#endif // WJR_X86_MATH_FIND_HPP__