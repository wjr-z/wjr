#ifndef WJR_ARCH_X86_SIMD_SIMD_UNROLL_MACRO_HPP__
#define WJR_ARCH_X86_SIMD_SIMD_UNROLL_MACRO_HPP__

#include <wjr/preprocessor.hpp>

#define WJR_X86_SIMD_UNROLL_NOP()

#define WJR_X86_SIMD_UNROLL_FAST_4_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, RET)    \
    WJR_ASSUME(N >= 5);                                                                            \
    INIT4();                                                                                       \
    const size_t __rem = (N - 1) % 16 + 1;                                                         \
    /* probability : 75% */                                                                        \
    if (WJR_LIKELY(__rem > 4)) {                                                                   \
        /* probability : 66.7% */                                                                  \
        if (WJR_LIKELY(__rem > 8)) {                                                               \
            UNROLL4(4);                                                                            \
            if (__rem > 12) {                                                                      \
                UNROLL4(8);                                                                        \
            }                                                                                      \
        }                                                                                          \
        UNROLL4(__rem - 4);                                                                        \
        if (WJR_UNLIKELY(__rem == N)) {                                                            \
            return RET(N);                                                                         \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    INIT8();                                                                                       \
    ADVANCE(__rem);                                                                                \
    N -= __rem;

#define WJR_X86_SIMD_UNROLL_FAST_2_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, RET)    \
    WJR_ASSUME(N >= 3);                                                                            \
    if (WJR_UNLIKELY(N <= 4)) {                                                                    \
        UNROLL2(N - 2);                                                                            \
        return RET(N);                                                                             \
    }                                                                                              \
    UNROLL2(2);                                                                                    \
    WJR_X86_SIMD_UNROLL_FAST_4_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, RET)

#define WJR_X86_SIMD_UNROLL_REVERSE_FAST_4_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, \
                                                       RET)                                        \
    WJR_ASSUME(N >= 5);                                                                            \
    INIT4();                                                                                       \
    const size_t __rem = (N - 1) % 16 + 1;                                                         \
    if (WJR_LIKELY(__rem > 4)) {                                                                   \
        if (WJR_LIKELY(__rem > 8)) {                                                               \
            UNROLL4(N - 8);                                                                        \
            if (__rem > 12) {                                                                      \
                UNROLL4(N - 12);                                                                   \
            }                                                                                      \
        }                                                                                          \
        UNROLL4(N - __rem);                                                                        \
        if (WJR_UNLIKELY(__rem == N)) {                                                            \
            return RET(0);                                                                         \
        }                                                                                          \
    }                                                                                              \
    INIT8();                                                                                       \
    ADVANCE(N - __rem);                                                                            \
    N -= __rem;

#define WJR_X86_SIMD_UNROLL_REVERSE_FAST_2_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, \
                                                       RET)                                        \
    WJR_ASSUME(N >= 3);                                                                            \
    if (WJR_UNLIKELY(N <= 4)) {                                                                    \
        UNROLL2(0);                                                                                \
        return RET(0);                                                                             \
    }                                                                                              \
    UNROLL2(N - 4);                                                                                \
    WJR_X86_SIMD_UNROLL_REVERSE_FAST_4_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, RET)

#endif // WJR_ARCH_X86_SIMD_SIMD_UNROLL_MACRO_HPP__