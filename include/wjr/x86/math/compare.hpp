#ifndef WJR_X86_MATH_COMPARE_HPP__
#define WJR_X86_MATH_COMPARE_HPP__

#include <wjr/math/ctz.hpp>
#include <wjr/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_COMPARE_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_COMPARE_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(COMPARE_N)

template <typename T>
WJR_COLD int large_builtin_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_COMPARE_NOT_N(index)                                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        sse::mask_type mask = ~sse::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            if (mask == 0xFF00) {                                                        \
                return src0[(index) + 1] < src1[(index) + 1] ? -1 : 1;                   \
            }                                                                            \
            return src0[(index)] < src1[(index)] ? -1 : 1;                               \
        }                                                                                \
    } while (0)
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

    switch (n & 7) {
    case 1: { // 8
        WJR_REGISTER_COMPARE_NOT_N(1);
        WJR_REGISTER_COMPARE_NOT_N(3);
#if !WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_COMPARE_NOT_N(5);
        WJR_REGISTER_COMPARE_NOT_N(7);
#else
        WJR_REGISTER_COMPARE_NOT_N_AVX(5);
#endif

        src0 += 9;
        src1 += 9;
        break;
    }
    case 2: { // 1
        if (WJR_LIKELY(src0[1] != src1[1])) {
            return src0[1] < src1[1] ? -1 : 1;
        }

        src0 += 2;
        src1 += 2;
        break;
    }
    case 3: { // 2
        WJR_REGISTER_COMPARE_NOT_N(1);

        src0 += 3;
        src1 += 3;
        break;
    }
    case 4: { // 2 + 1
        if (WJR_LIKELY(src0[1] != src1[1])) {
            return src0[1] < src1[1] ? -1 : 1;
        }

        WJR_REGISTER_COMPARE_NOT_N(2);

        src0 += 4;
        src1 += 4;
        break;
    }
    case 5: { // 2 * 2
        WJR_REGISTER_COMPARE_NOT_N(1);
        WJR_REGISTER_COMPARE_NOT_N(3);

        src0 += 5;
        src1 += 5;
        break;
    }
    case 6: { // 2 * 2 + 1
        if (WJR_LIKELY(src0[1] != src1[1])) {
            return src0[1] < src1[1] ? -1 : 1;
        }

        WJR_REGISTER_COMPARE_NOT_N(2);
        WJR_REGISTER_COMPARE_NOT_N(4);

        src0 += 6;
        src1 += 6;
        break;
    }
    case 7: { // 2 * 3
        WJR_REGISTER_COMPARE_NOT_N(1);
#if !WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_COMPARE_NOT_N(3);
        WJR_REGISTER_COMPARE_NOT_N(5);
#else
        WJR_REGISTER_COMPARE_NOT_N_AVX(3);
#endif

        src0 += 7;
        src1 += 7;
        break;
    }
    case 0: { // 2 * 3 + 1
        if (WJR_LIKELY(src0[1] != src1[1])) {
            return src0[1] < src1[1] ? -1 : 1;
        }

        WJR_REGISTER_COMPARE_NOT_N(2);
#if !WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_COMPARE_NOT_N(4);
        WJR_REGISTER_COMPARE_NOT_N(6);
#else
        WJR_REGISTER_COMPARE_NOT_N_AVX(4);
#endif

        src0 += 8;
        src1 += 8;
        break;
    }
    }

    size_t idx = (n - 2) / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)

    do {
        auto x0 = sse::loadu((__m128i *)(src0));
        auto x1 = sse::loadu((__m128i *)(src0 + 2));
        auto x2 = sse::loadu((__m128i *)(src0 + 4));
        auto x3 = sse::loadu((__m128i *)(src0 + 6));
        auto y0 = sse::loadu((__m128i *)(src1));
        auto y1 = sse::loadu((__m128i *)(src1 + 2));
        auto y2 = sse::loadu((__m128i *)(src1 + 4));
        auto y3 = sse::loadu((__m128i *)(src1 + 6));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto z0 = sse::And(r0, r2);
        auto r3 = sse::cmpeq_epi64(x3, y3);
        auto z1 = sse::And(r1, r3);
        z0 = sse::And(z0, z1);

        if (WJR_UNLIKELY(!sse::test_all_ones(z0))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (WJR_UNLIKELY(mask != 0)) {
                if (mask == 0xFF00) {
                    return src0[1] < src1[1] ? -1 : 1;
                }
                return src0[0] < src1[0] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (WJR_UNLIKELY(mask != 0)) {
                if (mask == 0xFF00) {
                    return src0[3] < src1[3] ? -1 : 1;
                }
                return src0[2] < src1[2] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (WJR_UNLIKELY(mask != 0)) {
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
        --idx;
    } while (WJR_LIKELY(idx != 0));

    return 0;
#else

    if (idx & 1) {
        WJR_REGISTER_COMPARE_NOT_N_AVX(0);
        WJR_REGISTER_COMPARE_NOT_N_AVX(4);

        src0 += 8;
        src1 += 8;

        if (WJR_UNLIKELY(idx == 1)) {
            return 0;
        }
    }

    idx /= 2;

    do {
        auto x0 = avx::loadu((__m256i *)(src0));
        auto x1 = avx::loadu((__m256i *)(src0 + 4));
        auto x2 = avx::loadu((__m256i *)(src0 + 8));
        auto x3 = avx::loadu((__m256i *)(src0 + 12));
        auto y0 = avx::loadu((__m256i *)(src1));
        auto y1 = avx::loadu((__m256i *)(src1 + 4));
        auto y2 = avx::loadu((__m256i *)(src1 + 8));
        auto y3 = avx::loadu((__m256i *)(src1 + 12));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto z0 = avx::And(r0, r2);
        auto r3 = avx::cmpeq_epi64(x3, y3);
        auto z1 = avx::And(r1, r3);
        z0 = avx::And(z0, z1);

        if (WJR_UNLIKELY(!avx::test_all_ones(z0))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[offset] < src1[offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[offset + 4] < src1[offset + 4] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[offset + 8] < src1[offset + 8] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r3);
            auto offset = ctz(mask) / 8;
            return src0[offset + 12] < src1[offset + 12] ? -1 : 1;
        }

        src0 += 16;
        src1 += 16;
        --idx;
    } while (WJR_LIKELY(idx != 0));

    return 0;
#endif

#undef WJR_REGISTER_COMPARE_NOT_N_AVX
#undef WJR_REGISTER_COMPARE_NOT_N
}

template <typename T>
WJR_INTRINSIC_INLINE int builtin_compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    // Quickly check the first one. There is a high probability that the comparison will
    // end in the first place
    if (WJR_LIKELY(src0[0] != src1[0])) {
        return src0[0] < src1[0] ? -1 : 1;
    }

    if (WJR_UNLIKELY(n < 4)) {

        do {
            if (n == 1) {
                break;
            }

            if (WJR_LIKELY(src0[1] != src1[1])) {
                return src0[1] < src1[1] ? -1 : 1;
            }

            if (n == 2) {
                break;
            }

            if (WJR_LIKELY(src0[2] != src1[2])) {
                return src0[2] < src1[2] ? -1 : 1;
            }

        } while (0);

        return 0;
    }

    return large_builtin_compare_n(src0, src1, n);
}

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

template <typename T>
WJR_COLD int large_builtin_reverse_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N(index)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        sse::mask_type mask = ~sse::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            if (mask == 0x00FF) {                                                        \
                return src0[-2 - (index)] < src1[-2 - (index)] ? -1 : 1;                 \
            }                                                                            \
            return src0[-1 - (index)] < src1[-1 - (index)] ? -1 : 1;                     \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(index)                                    \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 - (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 - (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = ~avx::movemask_epi8(r);                                              \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            auto offset = clz(mask) / 8;                                                 \
            return src0[-1 - (index)-offset] < src1[-1 - (index)-offset] ? -1 : 1;       \
        }                                                                                \
    } while (0)

    src0 += n;
    src1 += n;

    switch (n & 7) {
    case 1: { // 8
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(1);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(3);
#if !WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(5);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(7);
#else
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(5);
#endif

        src0 -= 9;
        src1 -= 9;
        break;
    }
    case 2: { // 1
        if (WJR_LIKELY(src0[-2] != src1[-2])) {
            return src0[-2] < src1[-2] ? -1 : 1;
        }

        src0 -= 2;
        src1 -= 2;
        break;
    }
    case 3: { // 2
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(1);

        src0 -= 3;
        src1 -= 3;
        break;
    }
    case 4: { // 2 + 1
        if (WJR_LIKELY(src0[-2] != src1[-2])) {
            return src0[-2] < src1[-2] ? -1 : 1;
        }

        WJR_REGISTER_REVERSE_COMPARE_NOT_N(2);

        src0 -= 4;
        src1 -= 4;
        break;
    }
    case 5: { // 2 * 2
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(1);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(3);

        src0 -= 5;
        src1 -= 5;
        break;
    }
    case 6: { // 2 * 2 + 1
        if (WJR_LIKELY(src0[-2] != src1[-2])) {
            return src0[-2] < src1[-2] ? -1 : 1;
        }

        WJR_REGISTER_REVERSE_COMPARE_NOT_N(2);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(4);

        src0 -= 6;
        src1 -= 6;
        break;
    }
    case 7: { // 2 * 3
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(1);
#if !WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(3);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(5);
#else
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(3);
#endif

        src0 -= 7;
        src1 -= 7;
        break;
    }
    case 0: { // 2 * 3 + 1
        if (WJR_LIKELY(src0[-2] != src1[-2])) {
            return src0[-2] < src1[-2] ? -1 : 1;
        }

        WJR_REGISTER_REVERSE_COMPARE_NOT_N(2);
#if !WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(4);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(6);
#else
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(4);
#endif

        src0 -= 8;
        src1 -= 8;
        break;
    }
    }

    size_t idx = (n - 2) / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)

    do {
        auto x0 = sse::loadu((__m128i *)(src0 - 8));
        auto x1 = sse::loadu((__m128i *)(src0 - 6));
        auto x2 = sse::loadu((__m128i *)(src0 - 4));
        auto x3 = sse::loadu((__m128i *)(src0 - 2));
        auto y0 = sse::loadu((__m128i *)(src1 - 8));
        auto y1 = sse::loadu((__m128i *)(src1 - 6));
        auto y2 = sse::loadu((__m128i *)(src1 - 4));
        auto y3 = sse::loadu((__m128i *)(src1 - 2));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto z0 = sse::And(r0, r2);
        auto r3 = sse::cmpeq_epi64(x3, y3);
        auto z1 = sse::And(r1, r3);
        z0 = sse::And(z0, z1);

        if (WJR_UNLIKELY(!sse::test_all_ones(z0))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (WJR_UNLIKELY(mask != 0)) {
                if (mask == 0x00FF) {
                    return src0[-2] < src1[-2] ? -1 : 1;
                }
                return src0[-1] < src1[-1] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (WJR_UNLIKELY(mask != 0)) {
                if (mask == 0x00FF) {
                    return src0[-4] < src1[-4] ? -1 : 1;
                }
                return src0[-3] < src1[-3] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (WJR_UNLIKELY(mask != 0)) {
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
        --idx;
    } while (WJR_LIKELY(idx != 0));

    return 0;
#else

    if (idx & 1) {
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(0);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(4);

        src0 -= 8;
        src1 -= 8;

        if (WJR_UNLIKELY(idx == 1)) {
            return 0;
        }
    }

    idx /= 2;

    do {
        auto x0 = avx::loadu((__m256i *)(src0 - 16));
        auto x1 = avx::loadu((__m256i *)(src0 - 12));
        auto x2 = avx::loadu((__m256i *)(src0 - 8));
        auto x3 = avx::loadu((__m256i *)(src0 - 4));
        auto y0 = avx::loadu((__m256i *)(src1 - 16));
        auto y1 = avx::loadu((__m256i *)(src1 - 12));
        auto y2 = avx::loadu((__m256i *)(src1 - 8));
        auto y3 = avx::loadu((__m256i *)(src1 - 4));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto z0 = avx::And(r0, r2);
        auto r3 = avx::cmpeq_epi64(x3, y3);
        auto z1 = avx::And(r1, r3);
        z0 = avx::And(z0, z1);

        if (WJR_UNLIKELY(!avx::test_all_ones(z0))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = clz(mask) / 8;
                return src0[-1 - offset] < src1[-1 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = clz(mask) / 8;
                return src0[-5 - offset] < src1[-5 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = clz(mask) / 8;
                return src0[-9 - offset] < src1[-9 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r0);
            auto offset = clz(mask) / 8;
            return src0[-13 - offset] < src1[-13 - offset] ? -1 : 1;
        }

        src0 -= 16;
        src1 -= 16;
        --idx;
    } while (WJR_LIKELY(idx != 0));

    return 0;
#endif

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N
}

template <typename T>
WJR_INTRINSIC_INLINE int builtin_reverse_compare_n(const T *src0, const T *src1,
                                                   size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return src0[n - 1] < src1[n - 1] ? -1 : 1;
    }

    if (WJR_UNLIKELY(n < 4)) {
        do {
            if (n == 1) {
                break;
            }

            if (WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
                return src0[n - 2] < src1[n - 2] ? -1 : 1;
            }

            if (n == 2) {
                break;
            }

            if (WJR_LIKELY(src0[n - 3] != src1[n - 3])) {
                return src0[n - 3] < src1[n - 3] ? -1 : 1;
            }

        } while (0);

        return 0;
    }

    return large_builtin_reverse_compare_n(src0, src1, n);
}

#endif

// __uint128_t has certain bugs in GCC 13.2, resulting in low performance
#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN___ASM_LESS_128 WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_LESS_EQUAL_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_128)

WJR_INTRINSIC_INLINE bool __asm_less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                         uint64_t hi1) {
    bool ret;
    asm("cmp{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(b)
        : WJR_ASM_CCOUT(b)(ret), [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)

WJR_INTRINSIC_INLINE bool __asm_less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                               uint64_t hi1) {
    bool ret;
    asm("cmp{q %[lo0], %[lo1]| %[lo1], %[lo0]}\n\t"
        "sbb{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(ae)
        : WJR_ASM_CCOUT(ae)(ret), [lo1] "+&r"(lo1), [hi1] "+r"(hi1)
        : [lo0] "r"(lo0), [hi0] "r"(hi0)
        : "cc");
    return ret;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_COMPARE_HPP__