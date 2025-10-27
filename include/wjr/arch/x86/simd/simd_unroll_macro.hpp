#ifndef WJR_ARCH_X86_SIMD_SIMD_UNROLL_MACRO_HPP__
#define WJR_ARCH_X86_SIMD_SIMD_UNROLL_MACRO_HPP__

#include <wjr/preprocessor.hpp>

#define WJR_X86_SIMD_UNROLL_NOP()

#define WJR_X86_SIMD_UNROLL_FAST_4_OVERLAPABLE(N, UNROLL2, UNROLL4, ADVANCE, INIT4, INIT8, RET)    \
    WJR_ASSUME(N >= 5);                                                                            \
    INIT4();                                                                                       \
    const size_t _rem = (N - 1) % 16 + 1;                                                          \
    /* probability : 75% */                                                                        \
    if (WJR_LIKELY(_rem > 4)) {                                                                    \
        /* probability : 66.7% */                                                                  \
        if (WJR_LIKELY(_rem > 8)) {                                                                \
            UNROLL4(4);                                                                            \
            if (_rem > 12) {                                                                       \
                UNROLL4(8);                                                                        \
            }                                                                                      \
        }                                                                                          \
        UNROLL4(_rem - 4);                                                                         \
        if (WJR_UNLIKELY(_rem == N)) {                                                             \
            return RET(N);                                                                         \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    INIT8();                                                                                       \
    ADVANCE(_rem);                                                                                 \
    N -= _rem;

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
    const size_t _rem = (N - 1) % 16 + 1;                                                          \
    if (WJR_LIKELY(_rem > 4)) {                                                                    \
        if (WJR_LIKELY(_rem > 8)) {                                                                \
            UNROLL4(N - 8);                                                                        \
            if (_rem > 12) {                                                                       \
                UNROLL4(N - 12);                                                                   \
            }                                                                                      \
        }                                                                                          \
        UNROLL4(N - _rem);                                                                         \
        if (WJR_UNLIKELY(_rem == N)) {                                                             \
            return RET(0);                                                                         \
        }                                                                                          \
    }                                                                                              \
    INIT8();                                                                                       \
    ADVANCE(N - _rem);                                                                             \
    N -= _rem;

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