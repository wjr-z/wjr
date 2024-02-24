#ifndef WJR_X86_COMPARE_HPP__
#define WJR_X86_COMPARE_HPP__

#include <wjr/gen/fast_1_2_8.hpp>
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

#define WJR_REGISTER_COMPARE_NOT_N(index, expect)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_EXPECT(mask != sse::mask(), expect)) {                                   \
            if (mask == 0x00FF) {                                                        \
                return src0[(index) + 1] < src1[(index) + 1] ? -1 : 1;                   \
            }                                                                            \
            return src0[(index)] < src1[(index)] ? -1 : 1;                               \
        }                                                                                \
    } while (0)

template <typename T>
WJR_COLD int large_builtin_compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_LIKELY(n & 1)) {
        ++src0;
        ++src1;
    } else {
        if (WJR_LIKELY(src0[1] != src1[1])) {
            return src0[1] < src1[1] ? -1 : 1;
        }

        src0 += 2;
        src1 += 2;
    }

    --n;

    if (n & 2) {
        WJR_REGISTER_COMPARE_NOT_N(0, true);

        src0 += 2;
        src1 += 2;
    }

    if (n & 4) {
        WJR_REGISTER_COMPARE_NOT_N(0, true);
        WJR_REGISTER_COMPARE_NOT_N(2, true);

        src0 += 4;
        src1 += 4;
    }

    size_t idx = n / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    do {
        WJR_REGISTER_COMPARE_NOT_N(0, false);
        WJR_REGISTER_COMPARE_NOT_N(2, false);
        WJR_REGISTER_COMPARE_NOT_N(4, false);
        WJR_REGISTER_COMPARE_NOT_N(6, false);

        src0 += 8;
        src1 += 8;
        --idx;
    } while (WJR_LIKELY(idx != 0));

    return 0;
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
        src0 += n - 2;
        src1 += n - 2;
        
        switch (n) {
        case 3: {
            if (WJR_LIKELY(src0[0] != src1[0])) {
                return src0[0] < src1[0] ? -1 : 1;
            }

            WJR_FALLTHROUGH;
        }
        case 2: {
            if (WJR_LIKELY(src0[1] != src1[1])) {
                return src0[1] < src1[1] ? -1 : 1;
            }

            WJR_FALLTHROUGH;
        }
        case 1: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return 0;
    }

    return large_builtin_compare_n(src0, src1, n);
}

#undef WJR_REGISTER_COMPARE_NOT_N

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

#define WJR_REGISTER_REVERSE_COMPARE_NOT_N(index, expect)                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_EXPECT(mask != sse::mask(), expect)) {                                   \
            if (mask == 0xFF00) {                                                        \
                return src0[-2 - (index)] < src1[-2 - (index)] ? -1 : 1;                 \
            }                                                                            \
            return src0[-1 - (index)] < src1[-1 - (index)] ? -1 : 1;                     \
        }                                                                                \
    } while (0)

template <typename T>
WJR_COLD int large_builtin_reverse_compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_LIKELY(n & 1)) {
        src0 += n - 1;
        src1 += n - 1;
    } else {
        if (WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
            return src0[n - 2] < src1[n - 2] ? -1 : 1;
        }

        src0 += n - 2;
        src1 += n - 2;
    }

    --n;

    if (n & 2) {
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(0, true);

        src0 -= 2;
        src1 -= 2;
    }

    if (n & 4) {
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(0, true);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(2, true);

        src0 -= 4;
        src1 -= 4;
    }

    size_t idx = n / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    do {
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(0, false);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(2, false);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(4, false);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N(6, false);

        src0 -= 8;
        src1 -= 8;
        --idx;
    } while (WJR_LIKELY(idx != 0));

    return 0;
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
        switch (n) {
        case 3: {
            if (WJR_LIKELY(src0[1] != src1[1])) {
                return src0[1] < src1[1] ? -1 : 1;
            }

            WJR_FALLTHROUGH;
        }
        case 2: {
            if (WJR_LIKELY(src0[0] != src1[0])) {
                return src0[0] < src1[0] ? -1 : 1;
            }

            WJR_FALLTHROUGH;
        }
        case 1: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return 0;
    }

    return large_builtin_reverse_compare_n(src0, src1, n);
}

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N

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
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
        "setb %b[ret]"
        : [ret] "=r"(ret), [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc", "memory");
    return ret;
}
#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)
WJR_INTRINSIC_INLINE bool __asm_less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                               uint64_t hi1) {
    bool ret;
    asm("cmp{q %[lo0], %[lo1]| %[lo1], %[lo0]}\n\t"
        "sbb{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t"
        "setae %b[ret]"
        : [ret] "=r"(ret), [lo1] "+&r"(lo1), [hi1] "+r"(hi1)
        : [lo0] "r"(lo0), [hi0] "r"(hi0)
        : "cc", "memory");
    return ret;
}
#endif

} // namespace wjr

#endif // WJR_X86_COMPARE_HPP__